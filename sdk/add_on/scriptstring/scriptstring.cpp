#include <assert.h>
#include <string.h> // strstr
#include "scriptstring.h"
using namespace std;

BEGIN_AS_NAMESPACE

//--------------
// constructors
//--------------

asCScriptString::asCScriptString()
{
	// Count the first reference
	refCount = 1;
}

asCScriptString::asCScriptString(const char *s)
{
	refCount = 1;
	buffer = s;
}

asCScriptString::asCScriptString(const string &s)
{
	refCount = 1;
	buffer = s;
}

asCScriptString::asCScriptString(const asCScriptString &s)
{
	refCount = 1;
	buffer = s.buffer;
}

asCScriptString::~asCScriptString()
{
	assert( refCount == 0 );
}

//--------------------
// reference counting
//--------------------

void asCScriptString::AddRef()
{
	refCount++;
}

static void StringAddRef_Generic(asIScriptGeneric *gen)
{
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	thisPointer->AddRef();
}

void asCScriptString::Release()
{
	if( --refCount == 0 )
		delete this;
}

static void StringRelease_Generic(asIScriptGeneric *gen)
{
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	thisPointer->Release();
}

//-----------------
// string = string
//-----------------

asCScriptString &asCScriptString::operator=(const asCScriptString &other)
{
	// Copy only the buffer, not the reference counter
	buffer = other.buffer;

	// Return a reference to this object
	return *this;
}

static void AssignString_Generic(asIScriptGeneric *gen)
{
	asCScriptString *a = (asCScriptString*)gen->GetArgAddress(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	*thisPointer = *a;
	gen->SetReturnAddress(thisPointer);
}

//------------------
// string += string
//------------------

asCScriptString &asCScriptString::operator+=(const asCScriptString &other)
{
	buffer += other.buffer;
	return *this;
}

static void AddAssignString_Generic(asIScriptGeneric *gen)
{
	asCScriptString *a = (asCScriptString*)gen->GetArgAddress(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	*thisPointer += *a;
	gen->SetReturnAddress(thisPointer);
}

//-----------------
// string + string
//-----------------

asCScriptString *operator+(const asCScriptString &a, const asCScriptString &b)
{
	// Return a new object as a script handle
	return new asCScriptString(a.buffer + b.buffer);
}

static void ConcatenateStrings_Generic(asIScriptGeneric *gen)
{
	asCScriptString *a = (asCScriptString*)gen->GetArgAddress(0);
	asCScriptString *b = (asCScriptString*)gen->GetArgAddress(1);
	asCScriptString *out = *a + *b;
	gen->SetReturnAddress(out);
}

//----------------
// string = value
//----------------

static asCScriptString &AssignUIntToString(unsigned int i, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%u", i);
	dest.buffer = buf;
	return dest;
}

static void AssignUIntToString_Generic(asIScriptGeneric *gen)
{
	unsigned int i = gen->GetArgDWord(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AssignUIntToString(i, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AssignIntToString(int i, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%d", i);
	dest.buffer = buf;
	return dest;
}

static void AssignIntToString_Generic(asIScriptGeneric *gen)
{
	int i = gen->GetArgDWord(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AssignIntToString(i, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AssignFloatToString(float f, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%g", f);
	dest.buffer = buf;
	return dest;
}

static void AssignFloatToString_Generic(asIScriptGeneric *gen)
{
	float f = gen->GetArgFloat(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AssignFloatToString(f, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AssignDoubleToString(double f, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%g", f);
	dest.buffer = buf;
	return dest;
}

static void AssignDoubleToString_Generic(asIScriptGeneric *gen)
{
	double f = gen->GetArgDouble(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AssignDoubleToString(f, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

//-----------------
// string += value
//-----------------

static asCScriptString &AddAssignUIntToString(unsigned int i, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%u", i);
	dest.buffer += buf;
	return dest;
}

static void AddAssignUIntToString_Generic(asIScriptGeneric *gen)
{
	unsigned int i = gen->GetArgDWord(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AddAssignUIntToString(i, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AddAssignIntToString(int i, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%d", i);
	dest.buffer += buf;
	return dest;
}

static void AddAssignIntToString_Generic(asIScriptGeneric *gen)
{
	int i = gen->GetArgDWord(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AddAssignIntToString(i, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AddAssignFloatToString(float f, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%g", f);
	dest.buffer += buf;
	return dest;
}

static void AddAssignFloatToString_Generic(asIScriptGeneric *gen)
{
	float f = gen->GetArgFloat(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AddAssignFloatToString(f, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AddAssignDoubleToString(double f, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%g", f);
	dest.buffer += buf;
	return dest;
}

static void AddAssignDoubleToString_Generic(asIScriptGeneric *gen)
{
	double f = gen->GetArgDouble(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AddAssignDoubleToString(f, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

//----------------
// string + value
//----------------

static asCScriptString *AddStringUInt(const asCScriptString &str, unsigned int i)
{
	char buf[100];
	sprintf(buf, "%u", i);
	return new asCScriptString(str.buffer + buf);
}

static void AddStringUInt_Generic(asIScriptGeneric *gen)
{
	asCScriptString *str = (asCScriptString*)gen->GetArgAddress(0);
	unsigned int i = gen->GetArgDWord(1);
	asCScriptString *out = AddStringUInt(*str, i);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddStringInt(const asCScriptString &str, int i)
{
	char buf[100];
	sprintf(buf, "%d", i);
	return new asCScriptString(str.buffer + buf);
}

static void AddStringInt_Generic(asIScriptGeneric *gen)
{
	asCScriptString *str = (asCScriptString*)gen->GetArgAddress(0);
	int i = gen->GetArgDWord(1);
	asCScriptString *out = AddStringInt(*str, i);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddStringFloat(const asCScriptString &str, float f)
{
	char buf[100];
	sprintf(buf, "%g", f);
	return new asCScriptString(str.buffer + buf);
}

static void AddStringFloat_Generic(asIScriptGeneric *gen)
{
	asCScriptString *str = (asCScriptString*)gen->GetArgAddress(0);
	float f = gen->GetArgFloat(1);
	asCScriptString *out = AddStringFloat(*str, f);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddStringDouble(const asCScriptString &str, double f)
{
	char buf[100];
	sprintf(buf, "%g", f);
	return new asCScriptString(str.buffer + buf);
}

static void AddStringDouble_Generic(asIScriptGeneric *gen)
{
	asCScriptString *str = (asCScriptString*)gen->GetArgAddress(0);
	double f = gen->GetArgDouble(1);
	asCScriptString *out = AddStringDouble(*str, f);
	gen->SetReturnAddress(out);
}

//----------------
// value + string
//----------------

static asCScriptString *AddIntString(int i, const asCScriptString &str)
{
	char buf[100];
	sprintf(buf, "%d", i);
	return new asCScriptString(buf + str.buffer);
}

static void AddIntString_Generic(asIScriptGeneric *gen)
{
	int i = gen->GetArgDWord(0);
	asCScriptString *str = (asCScriptString*)gen->GetArgAddress(1);
	asCScriptString *out = AddIntString(i, *str);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddUIntString(unsigned int i, const asCScriptString &str)
{
	char buf[100];
	sprintf(buf, "%u", i);
	return new asCScriptString(buf + str.buffer);
}

static void AddUIntString_Generic(asIScriptGeneric *gen)
{
	unsigned int i = gen->GetArgDWord(0);
	asCScriptString *str = (asCScriptString*)gen->GetArgAddress(1);
	asCScriptString *out = AddUIntString(i, *str);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddFloatString(float f, const asCScriptString &str)
{
	char buf[100];
	sprintf(buf, "%g", f);
	return new asCScriptString(buf + str.buffer);
}

static void AddFloatString_Generic(asIScriptGeneric *gen)
{
	float f = gen->GetArgFloat(0);
	asCScriptString *str = (asCScriptString*)gen->GetArgAddress(1);
	asCScriptString *out = AddFloatString(f, *str);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddDoubleString(double f, const asCScriptString &str)
{
	char buf[100];
	sprintf(buf, "%g", f);
	return new asCScriptString(buf + str.buffer);
}

static void AddDoubleString_Generic(asIScriptGeneric *gen)
{
	double f = gen->GetArgDouble(0);
	asCScriptString *str = (asCScriptString*)gen->GetArgAddress(1);
	asCScriptString *out = AddDoubleString(f, *str);
	gen->SetReturnAddress(out);
}

//----------
// string[]
//----------

static char *StringCharAt(unsigned int i, asCScriptString &str)
{
	if( i >= str.buffer.size() )
	{
		// Set a script exception
		asIScriptContext *ctx = asGetActiveContext();
		ctx->SetException("Out of range");

		// Return a null pointer
		return 0;
	}

	return &str.buffer[i];
}

static void StringCharAt_Generic(asIScriptGeneric *gen)
{
	unsigned int i = gen->GetArgDWord(0);
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	char *ch = StringCharAt(i, *str);
	gen->SetReturnAddress(ch);
}

//-----------------------
// AngelScript functions
//-----------------------

// This is the string factory that creates new strings for the script based on string literals
static asCScriptString *StringFactory(asUINT /*length*/, const char *s)
{
	return new asCScriptString(s);
}

static void StringFactory_Generic(asIScriptGeneric *gen)
{
	asUINT length = gen->GetArgDWord(0);
	const char *s = (const char*)gen->GetArgAddress(1);
	asCScriptString *str = StringFactory(length, s);
	gen->SetReturnAddress(str);
}

// This is the default string factory, that is responsible for creating empty string objects, e.g. when a variable is declared
static asCScriptString *StringDefaultFactory()
{
	// Allocate and initialize with the default constructor
	return new asCScriptString();
}

static asCScriptString *StringCopyFactory(const asCScriptString &other)
{
	// Allocate and initialize with the copy constructor
	return new asCScriptString(other);
}

static void StringDefaultFactory_Generic(asIScriptGeneric *gen)
{
	*(asCScriptString**)gen->GetReturnPointer() = StringDefaultFactory();
}

static void StringCopyFactory_Generic(asIScriptGeneric *gen)
{
	asCScriptString *other = (asCScriptString *)gen->GetArgObject(0);
	*(asCScriptString**)gen->GetReturnPointer() = StringCopyFactory(*other);
}

static void StringEqual_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a == *b;
    *(bool*)gen->GetReturnPointer() = r;
}

static void StringNotEqual_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a != *b;
    *(bool*)gen->GetReturnPointer() = r;
}

static void StringLesserOrEqual_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a <= *b;
    *(bool*)gen->GetReturnPointer() = r;
}

static void StringGreaterOrEqual_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a >= *b;
    *(bool*)gen->GetReturnPointer() = r;
}

static void StringLesser_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a < *b;
    *(bool*)gen->GetReturnPointer() = r;
}

static void StringGreater_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a > *b;
    *(bool*)gen->GetReturnPointer() = r;
}

static void StringLength_Generic(asIScriptGeneric *gen)
{
	string *s = (string*)gen->GetObject();
	size_t l = s->size();
	gen->SetReturnDWord((asUINT)l);
}

// This is where we register the string type
void RegisterScriptString_Native(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("string", sizeof(asCScriptString), asOBJ_REF); assert( r >= 0 );

	// Register the object operator overloads
	// Note: We don't have to register the destructor, since the object uses reference counting
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_FACTORY,    "string @f()",                 asFUNCTION(StringDefaultFactory), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_FACTORY,    "string @f(const string &in)", asFUNCTION(StringCopyFactory), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADDREF,     "void f()",                    asMETHOD(asCScriptString,AddRef), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_RELEASE,    "void f()",                    asMETHOD(asCScriptString,Release), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(const string &in)", asMETHODPR(asCScriptString, operator =, (const asCScriptString&), asCScriptString&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(const string &in)", asMETHODPR(asCScriptString, operator+=, (const asCScriptString&), asCScriptString&), asCALL_THISCALL); assert( r >= 0 );

	// Register the factory to return a handle to a new string
	// Note: We must register the string factory after the basic behaviours,
	// otherwise the library will not allow the use of object handles for this type
	r = engine->RegisterStringFactory("string@", asFUNCTION(StringFactory), asCALL_CDECL); assert( r >= 0 );

	// Register the global operator overloads
	// Note: We can use std::string's methods directly because the
	// internal std::string is placed at the beginning of the class
	r = engine->RegisterGlobalBehaviour(asBEHAVE_EQUAL,       "bool f(const string &in, const string &in)",    asFUNCTIONPR(operator==, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_NOTEQUAL,    "bool f(const string &in, const string &in)",    asFUNCTIONPR(operator!=, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_LEQUAL,      "bool f(const string &in, const string &in)",    asFUNCTIONPR(operator<=, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_GEQUAL,      "bool f(const string &in, const string &in)",    asFUNCTIONPR(operator>=, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_LESSTHAN,    "bool f(const string &in, const string &in)",    asFUNCTIONPR(operator <, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_GREATERTHAN, "bool f(const string &in, const string &in)",    asFUNCTIONPR(operator >, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, const string &in)", asFUNCTIONPR(operator +, (const asCScriptString &, const asCScriptString &), asCScriptString*), asCALL_CDECL); assert( r >= 0 );

	// Register the index operator, both as a mutator and as an inspector
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_INDEX, "uint8 &f(uint)", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_INDEX, "const uint8 &f(uint) const", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("string", "uint length() const", asMETHOD(string,size), asCALL_THISCALL); assert( r >= 0 );

	// TODO: Add explicit type conversion via constructor and value cast
	
	// Automatic conversion from values
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(double)", asFUNCTION(AssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(double)", asFUNCTION(AddAssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, double)", asFUNCTION(AddStringDouble), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(double, const string &in)", asFUNCTION(AddDoubleString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(float)", asFUNCTION(AssignFloatToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(float)", asFUNCTION(AddAssignFloatToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, float)", asFUNCTION(AddStringFloat), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(float, const string &in)", asFUNCTION(AddFloatString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(int)", asFUNCTION(AssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(int)", asFUNCTION(AddAssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, int)", asFUNCTION(AddStringInt), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(int, const string &in)", asFUNCTION(AddIntString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(uint)", asFUNCTION(AssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(uint)", asFUNCTION(AddAssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, uint)", asFUNCTION(AddStringUInt), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(uint, const string &in)", asFUNCTION(AddUIntString), asCALL_CDECL); assert( r >= 0 );
}

void RegisterScriptString_Generic(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("string", sizeof(asCScriptString), asOBJ_REF); assert( r >= 0 );

	// Register the object operator overloads
	// Note: We don't have to register the destructor, since the object uses reference counting
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_FACTORY,    "string @f()",                 asFUNCTION(StringDefaultFactory_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_FACTORY,    "string @f(const string &in)", asFUNCTION(StringCopyFactory_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADDREF,     "void f()",                    asFUNCTION(StringAddRef_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_RELEASE,    "void f()",                    asFUNCTION(StringRelease_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(const string &in)", asFUNCTION(AssignString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(const string &in)", asFUNCTION(AddAssignString_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the factory to return a handle to a new string
	// Note: We must register the string factory after the basic behaviours,
	// otherwise the library will not allow the use of object handles for this type
	r = engine->RegisterStringFactory("string@", asFUNCTION(StringFactory_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the global operator overloads
	// Note: We can use std::string's methods directly because the
	// internal std::string is placed at the beginning of the class
	r = engine->RegisterGlobalBehaviour(asBEHAVE_EQUAL,       "bool f(const string &in, const string &in)",    asFUNCTION(StringEqual_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_NOTEQUAL,    "bool f(const string &in, const string &in)",    asFUNCTION(StringNotEqual_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_LEQUAL,      "bool f(const string &in, const string &in)",    asFUNCTION(StringLesserOrEqual_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_GEQUAL,      "bool f(const string &in, const string &in)",    asFUNCTION(StringGreaterOrEqual_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_LESSTHAN,    "bool f(const string &in, const string &in)",    asFUNCTION(StringLesser_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_GREATERTHAN, "bool f(const string &in, const string &in)",    asFUNCTION(StringGreater_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, const string &in)", asFUNCTION(ConcatenateStrings_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the index operator, both as a mutator and as an inspector
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_INDEX, "uint8 &f(uint)", asFUNCTION(StringCharAt_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_INDEX, "const uint8 &f(uint) const", asFUNCTION(StringCharAt_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("string", "uint length() const", asFUNCTION(StringLength_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Automatic conversion from values
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(double)", asFUNCTION(AssignDoubleToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(double)", asFUNCTION(AddAssignDoubleToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, double)", asFUNCTION(AddStringDouble_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(double, const string &in)", asFUNCTION(AddDoubleString_Generic), asCALL_GENERIC); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(float)", asFUNCTION(AssignFloatToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(float)", asFUNCTION(AddAssignFloatToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, float)", asFUNCTION(AddStringFloat_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(float, const string &in)", asFUNCTION(AddFloatString_Generic), asCALL_GENERIC); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(int)", asFUNCTION(AssignIntToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(int)", asFUNCTION(AddAssignIntToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, int)", asFUNCTION(AddStringInt_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(int, const string &in)", asFUNCTION(AddIntString_Generic), asCALL_GENERIC); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(uint)", asFUNCTION(AssignUIntToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(uint)", asFUNCTION(AddAssignUIntToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(const string &in, uint)", asFUNCTION(AddStringUInt_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string@ f(uint, const string &in)", asFUNCTION(AddUIntString_Generic), asCALL_GENERIC); assert( r >= 0 );
}

void RegisterScriptString(asIScriptEngine *engine)
{
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
		RegisterScriptString_Generic(engine);
	else
		RegisterScriptString_Native(engine);
}

END_AS_NAMESPACE


