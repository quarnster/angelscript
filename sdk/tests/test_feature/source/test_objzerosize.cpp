#include "utils.h"

namespace TestObjZeroSize
{

#define TESTNAME "TestObjZeroSize"

class CObject
{
public:
	CObject() {val = 0;refCount = 1;}
	~CObject() {}
	void AddRef() {refCount++;}
	void Release() {if( --refCount == 0 ) delete this;}
	void Set(int v) {val = v;}
	int Get() {return val;}
	int val;
	int refCount;
};

void Construct(CObject *o)
{
	new(o) CObject();
}

void Destruct(CObject *o)
{
	o->~CObject();
}

CObject obj;

CObject *CreateObject()
{
	CObject *obj = new CObject();
	
	// The constructor already initialized the reference counter with 1

	return obj;
}

bool Test()
{
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
	{
		printf("%s: Skipped due to AS_MAX_PORTABILITY\n", TESTNAME);
		return false;
	}
	bool fail = false;
	int r;

 	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	engine->RegisterGlobalFunction("void Assert(bool)", asFUNCTION(Assert), asCALL_GENERIC);

	engine->RegisterObjectType("Object", 0, asOBJ_REF);
	engine->RegisterObjectBehaviour("Object", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Construct), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Object", asBEHAVE_ADDREF, "void f()", asMETHOD(CObject, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("Object", asBEHAVE_RELEASE, "void f()", asMETHOD(CObject, Release), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void Set(int)", asMETHOD(CObject, Set), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "int Get()", asMETHOD(CObject, Get), asCALL_THISCALL);
	engine->RegisterObjectProperty("Object", "int val", offsetof(CObject, val));

	engine->RegisterGlobalProperty("Object obj", &obj);
	engine->RegisterGlobalFunction("Object @CreateObject()", asFUNCTION(CreateObject), asCALL_CDECL);

	COutStream out;
	CBufferedOutStream bout;

	engine->SetMessageCallback(asMETHOD(CBufferedOutStream,Callback), &bout, asCALL_THISCALL);
	r = engine->ExecuteString(0, "Object obj;");
	if( r >= 0 || bout.buffer != "ExecuteString (1, 8) : Error   : Data type can't be 'Object'\n" )
	{
		printf("%s: Didn't fail to compile as expected\n", TESTNAME);
		fail = true;
	}

	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
	r = engine->ExecuteString(0, "Object @obj;");
	if( r < 0 )
	{
		printf("%s: Failed to compile\n", TESTNAME);
		fail = true;
	}

	r = engine->ExecuteString(0, "Object@ obj = @CreateObject();");
	if( r < 0 )
	{
		printf("%s: Failed to compile\n", TESTNAME);
		fail = true;
	}

	r = engine->ExecuteString(0, "CreateObject();");
	if( r < 0 )
	{
		printf("%s: Failed to compile\n", TESTNAME);
		fail = true;
	}

	r = engine->ExecuteString(0, "Object@ obj = @CreateObject(); @obj = @CreateObject();");
	if( r < 0 )
	{
		printf("%s: Failed to compile\n", TESTNAME);
		fail = true;
	}

	bout.buffer = "";
	engine->SetMessageCallback(asMETHOD(CBufferedOutStream,Callback), &bout, asCALL_THISCALL);
	r = engine->ExecuteString(0, "Object@ obj = @CreateObject(); obj = CreateObject();");
	if( r >= 0 || bout.buffer != "ExecuteString (1, 36) : Error   : There is no copy operator for this type available.\n" )
	{
		printf("%s: Didn't fail to compile as expected\n", TESTNAME);
		fail = true;
	}

	bout.buffer = "";
	r = engine->ExecuteString(0, "@CreateObject() = @CreateObject();");
	if( r >= 0 || bout.buffer != "ExecuteString (1, 1) : Error   : Reference is temporary\n" )
	{
		printf("%s: Didn't fail to compile as expected\n", TESTNAME);
		fail = true;
	}

	bout.buffer = "";
	r = engine->ExecuteString(0, "CreateObject() = CreateObject();");
	if( r >= 0 || bout.buffer != "ExecuteString (1, 1) : Error   : Reference is temporary\n" )
	{
		printf("%s: Didn't fail to compile as expected\n", TESTNAME);
		fail = true;
	}

	// Test object with zero size as member of script class
	const char *script = "  \n\
	 class myclass          \n\
	 {                      \n\
	   Object obj;          \n\
	 }                      \n";
	engine->AddScriptSection(0, "script", script, strlen(script));
	bout.buffer = "";
	r = engine->Build(0);
	if( r >= 0 || bout.buffer != "script (4, 5) : Error   : Data type can't be 'Object'\n" )
	{
		printf("%s: Didn't fail to compile as expected\n", TESTNAME);
		fail = true;
	}

	engine->Release();

	// Success
	return fail;
}

} // namespace

