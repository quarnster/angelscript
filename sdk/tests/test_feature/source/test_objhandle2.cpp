#include "utils.h"

namespace TestObjHandle2
{

#define TESTNAME "TestObjHandle2"

static const char *script1 =
"void TestObjHandle()                   \n"
"{                                      \n"
"   refclass@ b = @getRefClass();       \n"
"   Assert(b.id == 0xdeadc0de);         \n"
"   refclass@ c = @getRefClass(@b);     \n"
"   Assert(@c == @b);                   \n"
"}                                      \n";

class CRefClass
{
public:
	CRefClass() 
	{
		id = 0xdeadc0de;
//		asIScriptContext *ctx = asGetActiveContext(); 
//		printf("ln:%d ", ctx->GetCurrentLineNumber()); 
//		printf("Construct(%X)\n",this); 
		refCount = 1;
	}
	~CRefClass() 
	{
//		asIScriptContext *ctx = asGetActiveContext(); 
//		printf("ln:%d ", ctx->GetCurrentLineNumber()); 
//		printf("Destruct(%X)\n",this);
	}
	int AddRef() 
	{
//		asIScriptContext *ctx = asGetActiveContext(); 
//		printf("ln:%d ", ctx->GetCurrentLineNumber()); 
//		printf("AddRef(%X)\n",this); 
		return ++refCount;
	}
	int Release() 
	{
//		asIScriptContext *ctx = asGetActiveContext(); 
//		printf("ln:%d ", ctx->GetCurrentLineNumber()); 
//		printf("Release(%X)\n",this); 
		int r = --refCount; 
		if( refCount == 0 ) delete this; 
		return r;
	}
	int refCount;
	int id;
};

CRefClass c;
CRefClass *getRefClass() 
{
//	asIScriptContext *ctx = asGetActiveContext(); 
//	printf("ln:%d ", ctx->GetCurrentLineNumber()); 
//	printf("getRefClass() = %X\n", &c); 

	// Must add the reference before returning it
	c.AddRef();
	return &c;
}

CRefClass *getRefClass(CRefClass *obj)
{
	assert(obj != 0);
	return obj;
}

bool Test()
{
	bool fail = false;
	int r;

 	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	RegisterScriptString(engine);

	r = engine->RegisterObjectType("refclass", sizeof(CRefClass), asOBJ_CLASS_CDA); assert(r >= 0);
	r = engine->RegisterObjectProperty("refclass", "int id", offsetof(CRefClass, id));
	r = engine->RegisterObjectBehaviour("refclass", asBEHAVE_ADDREF, "void f()", asMETHOD(CRefClass, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("refclass", asBEHAVE_RELEASE, "void f()", asMETHOD(CRefClass, Release), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterGlobalFunction("refclass @getRefClass()", asFUNCTIONPR(getRefClass,(),CRefClass*), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalFunction("refclass @getRefClass(refclass@)", asFUNCTIONPR(getRefClass,(CRefClass*),CRefClass*), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterGlobalFunction("void Assert(bool)", asFUNCTION(Assert), asCALL_CDECL); assert( r >= 0 );

	COutStream out;

	engine->AddScriptSection(0, TESTNAME, script1, strlen(script1), 0);
	r = engine->Build(0, &out);
	if( r < 0 )
	{
		fail = true;
		printf("%s: Failed to compile the script\n", TESTNAME);
	}
	asIScriptContext *ctx;
	r = engine->ExecuteString(0, "TestObjHandle()", 0, &ctx);

	if( r != asEXECUTION_FINISHED )
	{
		fail = true;
		printf("%s: Execution failed\n", TESTNAME);
	}
	if( ctx ) ctx->Release();


	engine->Release();

	// Verify that the compiler doesn't allow the use of handles if addref/release aren't registered
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	engine->RegisterObjectType("type", 0, asOBJ_PRIMITIVE);
	engine->RegisterGlobalFunction("type @func()", asFUNCTION(0), asCALL_CDECL);
	engine->Release();

	// Success
	return fail;
}

} // namespace

