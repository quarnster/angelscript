#include <assert.h>
#include <string.h>
#include "scriptdictionary.h"

BEGIN_AS_NAMESPACE

using namespace std;

//--------------------------------------------------------------------------
// CScriptDictionary implementation

CScriptDictionary::CScriptDictionary(asIScriptEngine *engine)
{
    // We start with one reference
    refCount = 1;

    // Keep a reference to the engine for as long as we live
    this->engine = engine;
    engine->AddRef();
}

CScriptDictionary::~CScriptDictionary()
{
    // Delete all keys and values
    DeleteAll();

    // Release the engine reference
    if( engine ) engine->Release();
}

void CScriptDictionary::AddRef()
{
    refCount++;
}

void CScriptDictionary::Release()
{
    if( --refCount == 0 )
        delete this;
}

CScriptDictionary &CScriptDictionary::operator =(const CScriptDictionary & /*other*/)
{
    // Do nothing

    return *this;
}

void CScriptDictionary::Set(string &key, void *value, int typeId)
{
	valueStruct valStruct = {{0},0};
	valStruct.typeId = typeId;
	if( typeId & asTYPEID_OBJHANDLE )
	{
		// We're receiving a reference to the handle, so we need to dereference it
		valStruct.valueObj = *(void**)value;
		engine->AddRefScriptObject(valStruct.valueObj, typeId);
	}
	else if( typeId & asTYPEID_MASK_OBJECT )
	{
		// Create a copy of the object
		// We need to dereference the reference, as we receive a pointer to a pointer to the object
		valStruct.valueObj = engine->CreateScriptObjectCopy(*(void**)value, typeId);
	}
	else
	{
		// Copy the primitive value
		// We receive a pointer to the value.
		int size = engine->GetSizeOfPrimitiveType(typeId);
		memcpy(&valStruct.valueInt, value, size);
	}

    map<string, valueStruct>::iterator it;
    it = dict.find(key);
    if( it != dict.end() )
    {
        FreeValue(it->second);

        // Insert the new value
        it->second = valStruct;
    }
    else
    {
        dict.insert(map<string, valueStruct>::value_type(key, valStruct));
    }
}

void CScriptDictionary::Set(string &key, asINT64 &value)
{
	int typeId = engine->GetTypeIdByDecl(0, "int64");
	Set(key, &value, typeId);
}

void CScriptDictionary::Set(string &key, double &value)
{
	int typeId = engine->GetTypeIdByDecl(0, "double");
	Set(key, &value, typeId);
}

// Returns true if the value was successfully retrieved
bool CScriptDictionary::Get(string &key, void *value, int typeId)
{
    map<string, valueStruct>::iterator it;
    it = dict.find(key);
    if( it != dict.end() )
    {
        // Return the value
		if( typeId & asTYPEID_OBJHANDLE )
		{
			// A handle can be retrieved if the stored type is a handle of same or compatible type
			// or if the stored type is an object that implements the interface that the handle refer to.
			if( (it->second.typeId & asTYPEID_MASK_OBJECT) && 
				engine->IsHandleCompatibleWithObject(it->second.valueObj, it->second.typeId, typeId) )
			{
				engine->AddRefScriptObject(it->second.valueObj, it->second.typeId);
				*(void**)value = it->second.valueObj;

				return true;
			}
		}
		else if( typeId & asTYPEID_MASK_OBJECT )
		{
			// Verify that the copy can be made
			bool isCompatible = false;
			if( it->second.typeId == typeId )
				isCompatible = true;

			// Copy the object into the given reference
			if( isCompatible )
			{
				engine->CopyScriptObject(*(void**)value, it->second.valueObj, typeId);

				return true;
			}
		}
		else
		{
			if( it->second.typeId == typeId )
			{
				int size = engine->GetSizeOfPrimitiveType(typeId);
				memcpy(value, &it->second.valueInt, size);
				return true;
			}

			const char *type = engine->GetTypeDeclaration(typeId);

			// We know all numbers are stored as either int64 or double, since we register overloaded functions for those
			int intTypeId = engine->GetTypeIdByDecl(0, "int64");
			int fltTypeId = engine->GetTypeIdByDecl(0, "double");
			if( it->second.typeId == intTypeId && typeId == fltTypeId )
			{
				*(double*)value = double(it->second.valueInt);
				return true;
			}
			else if( it->second.typeId == fltTypeId && typeId == intTypeId )
			{
				*(asINT64*)value = asINT64(it->second.valueFlt);
				return true;
			}
		}
    }

    // AngelScript has already initialized the value with a default value,
    // so we don't have to do anything if we don't find the element, or if 
	// the element is incompatible with the requested type.

	return false;
}

bool CScriptDictionary::Get(string &key, asINT64 &value)
{
	int typeId = engine->GetTypeIdByDecl(0, "int64");
	return Get(key, &value, typeId);
}

bool CScriptDictionary::Get(string &key, double &value)
{
	int typeId = engine->GetTypeIdByDecl(0, "double");
	return Get(key, &value, typeId);
}

bool CScriptDictionary::Exists(string &key)
{
    map<string, valueStruct>::iterator it;
    it = dict.find(key);
    if( it != dict.end() )
    {
        return true;
    }

    return false;
}

void CScriptDictionary::Delete(string &key)
{
    map<string, valueStruct>::iterator it;
    it = dict.find(key);
    if( it != dict.end() )
    {
        FreeValue(it->second);

        dict.erase(it);
    }
}

void CScriptDictionary::DeleteAll()
{
    map<string, valueStruct>::iterator it;
    for( it = dict.begin(); it != dict.end(); it++ )
    {
        FreeValue(it->second);
    }

    dict.clear();
}

void CScriptDictionary::FreeValue(valueStruct &value)
{
    // If it is a handle or a ref counted object, call release
	if( value.typeId & asTYPEID_MASK_OBJECT )
	{
		// Let the engine release the object
		engine->ReleaseScriptObject(value.valueObj, value.typeId);
		value.valueObj = 0;
		value.typeId = 0;
	}

    // For primitives, there's nothing to do
}

//--------------------------------------------------------------------------
// Custom memory management for the class

// This function allocates memory for the string object
static void *ScriptDictionaryAlloc(int)
{
	return new char[sizeof(CScriptDictionary)];
}

// This function deallocates the memory for the string object
static void ScriptDictionaryFree(void *p)
{
	assert( p );
	delete[] (char*)p;
}

//--------------------------------------------------------------------------
// Generic wrappers

void ScriptDictionaryConstruct_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    new(dict) CScriptDictionary(gen->GetEngine());
}

void ScriptDictionaryAddRef_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    dict->AddRef();
}

void ScriptDictionaryRelease_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    dict->Release();
}

void ScriptDictionarySet_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    string *key = *(string**)gen->GetArgPointer(0);
    void *ref = *(void**)gen->GetArgPointer(1);
    int typeId = gen->GetArgTypeId(1);
    dict->Set(*key, ref, typeId);
}

void ScriptDictionarySetInt_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    string *key = *(string**)gen->GetArgPointer(0);
    void *ref = *(void**)gen->GetArgPointer(1);
    dict->Set(*key, *(asINT64*)ref);
}

void ScriptDictionarySetFlt_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    string *key = *(string**)gen->GetArgPointer(0);
    void *ref = *(void**)gen->GetArgPointer(1);
    dict->Set(*key, *(double*)ref);
}

void ScriptDictionaryGet_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    string *key = *(string**)gen->GetArgPointer(0);
    void *ref = *(void**)gen->GetArgPointer(1);
    int typeId = gen->GetArgTypeId(1);
    *(bool*)gen->GetReturnPointer() = dict->Get(*key, ref, typeId);
}

void ScriptDictionaryGetInt_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    string *key = *(string**)gen->GetArgPointer(0);
    void *ref = *(void**)gen->GetArgPointer(1);
    *(bool*)gen->GetReturnPointer() = dict->Get(*key, *(asINT64*)ref);
}

void ScriptDictionaryGetFlt_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    string *key = *(string**)gen->GetArgPointer(0);
    void *ref = *(void**)gen->GetArgPointer(1);
    *(bool*)gen->GetReturnPointer() = dict->Get(*key, *(double*)ref);
}

void ScriptDictionaryExists_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    string *key = *(string**)gen->GetArgPointer(0);
    bool ret = dict->Exists(*key);
    *(bool*)gen->GetReturnPointer() = ret;
}

void ScriptDictionaryDelete_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    string *key = *(string**)gen->GetArgPointer(0);
    dict->Delete(*key);
}

void ScriptDictionaryDeleteAll_Generic(asIScriptGeneric *gen)
{
    CScriptDictionary *dict = (CScriptDictionary*)gen->GetObject();
    dict->DeleteAll();
}

//--------------------------------------------------------------------------
// Register the type

void RegisterScriptDictionary(asIScriptEngine *engine)
{
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
		RegisterScriptDictionary_Generic(engine);
	else
		RegisterScriptDictionary_Native(engine);
}

void RegisterScriptDictionary_Native(asIScriptEngine *engine)
{
	int r;

    r = engine->RegisterObjectType("dictionary", sizeof(CScriptDictionary), asOBJ_CLASS_CDA); assert( r >= 0 );
	// Use the generic interface to construct the object since we need the engine pointer, we could also have retrieved the engine pointer from the active context
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ScriptDictionaryConstruct_Generic), asCALL_GENERIC); assert( r>= 0 );
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptDictionary,AddRef), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptDictionary,Release), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_ALLOC, "dictionary &f(uint)", asFUNCTION(ScriptDictionaryAlloc), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_FREE, "void f(dictionary &in)", asFUNCTION(ScriptDictionaryFree), asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("dictionary", "void set(const string &in, ?&in)", asMETHODPR(CScriptDictionary,Set,(string&,void*,int),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "bool get(const string &in, ?&out)", asMETHODPR(CScriptDictionary,Get,(string&,void*,int),bool), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("dictionary", "void set(const string &in, int64&in)", asMETHODPR(CScriptDictionary,Set,(string&,asINT64&),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "bool get(const string &in, int64&out)", asMETHODPR(CScriptDictionary,Get,(string&,asINT64&),bool), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("dictionary", "void set(const string &in, double&in)", asMETHODPR(CScriptDictionary,Set,(string&,double&),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "bool get(const string &in, double&out)", asMETHODPR(CScriptDictionary,Get,(string&,double&),bool), asCALL_THISCALL); assert( r >= 0 );
    
	r = engine->RegisterObjectMethod("dictionary", "bool exists(const string &in)", asMETHOD(CScriptDictionary,Exists), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "void delete(const string &in)", asMETHOD(CScriptDictionary,Delete), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "void deleteAll()", asMETHOD(CScriptDictionary,DeleteAll), asCALL_THISCALL); assert( r >= 0 );
}

void RegisterScriptDictionary_Generic(asIScriptEngine *engine)
{
    int r;

    r = engine->RegisterObjectType("dictionary", sizeof(CScriptDictionary), asOBJ_CLASS_CDA); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ScriptDictionaryConstruct_Generic), asCALL_GENERIC); assert( r>= 0 );
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_ADDREF, "void f()", asFUNCTION(ScriptDictionaryAddRef_Generic), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_RELEASE, "void f()", asFUNCTION(ScriptDictionaryRelease_Generic), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_ALLOC, "dictionary &f(uint)", asFUNCTION(ScriptDictionaryAlloc), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("dictionary", asBEHAVE_FREE, "void f(dictionary &in)", asFUNCTION(ScriptDictionaryFree), asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("dictionary", "void set(const string &in, ?&in)", asFUNCTION(ScriptDictionarySet_Generic), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "bool get(const string &in, ?&out)", asFUNCTION(ScriptDictionaryGet_Generic), asCALL_GENERIC); assert( r >= 0 );
    
    r = engine->RegisterObjectMethod("dictionary", "void set(const string &in, int64&in)", asFUNCTION(ScriptDictionarySetInt_Generic), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "bool get(const string &in, int64&out)", asFUNCTION(ScriptDictionaryGetInt_Generic), asCALL_GENERIC); assert( r >= 0 );

    r = engine->RegisterObjectMethod("dictionary", "void set(const string &in, double&in)", asFUNCTION(ScriptDictionarySetFlt_Generic), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "bool get(const string &in, double&out)", asFUNCTION(ScriptDictionaryGetFlt_Generic), asCALL_GENERIC); assert( r >= 0 );

	r = engine->RegisterObjectMethod("dictionary", "bool exists(const string &in)", asFUNCTION(ScriptDictionaryExists_Generic), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "void delete(const string &in)", asFUNCTION(ScriptDictionaryDelete_Generic), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectMethod("dictionary", "void deleteAll()", asFUNCTION(ScriptDictionaryDeleteAll_Generic), asCALL_GENERIC); assert( r >= 0 );
}

END_AS_NAMESPACE


