
#include "utils.h"

namespace TestGarbageCollect
{

int called = 0;
std::string buf;
void PrintString_Generic(asIScriptGeneric *gen)
{
	std::string *str = (std::string*)gen->GetArgAddress(0);
	UNUSED_VAR(str);
	buf += *str;
//	printf("%s",str->c_str());
	called++;
}




class CFoo 
{ 
public:     
	CFoo() : m_Ref(1) { m_pObject = 0; }
	~CFoo() { if( m_pObject ) m_pObject->Release(); }
	void SetScriptObject(asIScriptObject* _pObject) { m_pObject = _pObject; }
	void AddRef() { m_Ref++; }
	void Release() { if( --m_Ref == 0 ) { delete this; } }
	static CFoo* CreateObject() { return new CFoo; }
private:
	asIScriptObject* m_pObject;
	asUINT m_Ref;
};


void Yield()
{
	asIScriptContext *ctx = asGetActiveContext();
	if( ctx )
		ctx->Suspend();
}

void Exit()
{
	asIScriptContext *ctx = asGetActiveContext();
	if( ctx )
		ctx->Abort();
}

void GCLineCallback(asIScriptContext *ctx)
{
	asIScriptEngine *engine = ctx->GetEngine();
	engine->GarbageCollect(asGC_ONE_STEP | asGC_DESTROY_GARBAGE | asGC_DETECT_GARBAGE);
}

bool Test()
{
	bool fail = false;

    // Create the script engine
    asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
 
    // Register Function
    RegisterScriptString(engine);
    engine->RegisterGlobalFunction("void Print(string &in)", asFUNCTION(PrintString_Generic), asCALL_GENERIC);
 
    // Compile
    asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
    mod->AddScriptSection("script", 
        "class Obj{};"
        "class Hoge"
        "{"
        "    Hoge(){ Print('ctor\\n'); }"
        "    ~Hoge(){ Print('dtor\\n'); }"
        "    Obj@ obj;"
        "};"
        "void main()"
        "{"
        "    Hoge hoge;"
        "};"
        , 0);
    mod->Build();
 
    // Context Create
    asIScriptContext *ctx = engine->CreateContext();
 
    // Loop
    for( asUINT n = 0; n < 3; n++ )
    {
        // Execute
        //printf("----- execute\n");
        ctx->Prepare(mod->GetFunctionByDecl("void main()"));
        ctx->Execute();
 
        // GC
        const int GC_STEP_COUNT_PER_FRAME = 100;
        for ( int i = 0; i < GC_STEP_COUNT_PER_FRAME; ++i )
        {
            engine->GarbageCollect(asGC_ONE_STEP);
        }
        
        // Check status
        {
            asUINT currentSize = asUINT();
            asUINT totalDestroyed = asUINT();
            asUINT totalDetected = asUINT();
            engine->GetGCStatistics(&currentSize , &totalDestroyed , &totalDetected );
			if( currentSize    != 0 ||
				totalDestroyed != n+1 ||
				totalDetected  != 0 )
				TEST_FAILED;
            //printf("(%lu,%lu,%lu)\n" , currentSize , totalDestroyed , totalDetected );
        }
    }

    // Release 
    ctx->Release();
    engine->Release();

	// Test problem reported by Poly�k Istv�n
	{	
		COutStream out;
		int r;

		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream, Callback), &out, asCALL_THISCALL);

		mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection(0, 
			"class C1 \n"
			"{ \n"
			"    C1 () {} \n"
			"    ~C1 () { close(); } \n"
			"    void close () {} \n"
			"} \n"
			"class C2 \n"
			"{ \n"
			"    C2 (C1 @ c1) { @c1_ = @c1; } \n"
			"    ~C2 () { close(); } \n"
			"    void close () { @c1_ = null; } \n"
			"    private C1 @ c1_; \n"
			"} \n"
			"void f1 ()  \n"
			"{  \n"
			"    C1 c1; \n"
			"    C2 c2(c1); \n"
			"    c2.close(); \n"
			"} \n"
			"void main () \n"
			"{ \n"
			"    const uint c = 5; \n"
			"    for ( uint i = 0; i < c; ++i ) \n"
			"        f1(); \n"
			"} \n" );
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		r = ExecuteString(engine, "main()", mod);
		if( r != asEXECUTION_FINISHED )
			TEST_FAILED;

		engine->Release();
	}

	// Test 
	{	
		COutStream out;
		int r;

		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream, Callback), &out, asCALL_THISCALL);
		RegisterScriptArray(engine, true);

		mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection(0, 
			"interface ITest\n"
			"{\n"
			"}\n"
			"class Test : ITest\n"
			"{\n"
			"	ITest@[] arr;\n"
			"	void Set(ITest@ e)\n"
			"	{\n"
			"		arr.resize(1);\n"
			"		@arr[0]=e;\n"
			"	}\n"
			"}\n"
			"void main()\n"
			"{\n"
			"	Test@ t=Test();\n"
			"	t.Set(t);\n"
			"}\n");
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		asUINT currentSize;
		engine->GetGCStatistics(&currentSize);
		
		r = ExecuteString(engine, "main()", mod);
		if( r != asEXECUTION_FINISHED )
			TEST_FAILED;

		engine->GetGCStatistics(&currentSize);

		engine->Release();
	}

	// Test attempted access of global variable after it has been destroyed
	{	
		COutStream out;
		int r;

		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream, Callback), &out, asCALL_THISCALL);
		RegisterScriptArray(engine, true);
		RegisterStdString(engine);
		engine->RegisterGlobalFunction("void Log(const string &in)", asFUNCTION(PrintString_Generic), asCALL_GENERIC);

		mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection(0, 
			"class Big \n"
			"{ \n"
			"  Big() \n"
			"  { \n"
			"    Log('Big instance created\\n'); \n"
			"  } \n"
			"  ~Big() \n"
			"  { \n"
			"    Log('Big instance being destroyed\\n'); \n"
			"  } \n"
			"  void exec() \n"
			"  { \n"
			"    Log('executed\\n'); \n"
			"  } \n"
			"} \n"
			"Big big; \n" // Global object
			"class SomeClass \n"
			"{ \n"
			"  SomeClass@ handle; \n" // Make sure SomeClass is garbage collected
			"  SomeClass() {} \n"
			"  ~SomeClass() \n"
			"  { \n"
			"    Log('Before attempting access to global var\\n'); \n"
			"    big.exec(); \n" // As the module has already been destroyed, the global variable won't exist anymore, thus raising a null pointer exception here
			"    Log('SomeClass instance being destroyed\\n'); \n" // This won't be called
			"  } \n"
			"} \n"
			"SomeClass @something; \n" 
			"void test_main() \n"
			"{ \n"
			"  @something = @SomeClass(); \n" // Instanciate the object. It will only be destroyed by the GC
			"} \n");
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		r = ExecuteString(engine, "test_main()", mod);
		if( r != asEXECUTION_FINISHED )
			TEST_FAILED;

		// The global variables in the module will be destroyed first. The objects in the GC that 
		// tries to access them should throw exception, but should not cause the application to crash
		called = 0;
		buf = "";
		engine->Release();
		if( called != 2 )
			TEST_FAILED;
		if( buf != "Big instance being destroyed\n"
		           "Before attempting access to global var\n" )
		{
			printf("%s", buf.c_str());
			TEST_FAILED;
		}
	}

	{
		const char *script =
			"const int number_of_instances=50; \n"
			"int destroyed_instances=0; \n"
			"class dummy \n"
			"{ \n"
			"  dummy@ other_dummy; \n"
			"  dummy() \n"
			"  { \n"
			"    @other_dummy=this; \n"
			"  } \n"
			"  ~dummy() \n"
			"  { \n"
			"    destroyed_instances+=1; \n"
			"    if(destroyed_instances==number_of_instances) \n"
			"    { \n"
			"      print('Destroying last class' + 'The last class instance is now being destroyed after having existed for ' + 0.0f + ' milliseconds.'); \n"
			"      exit(); \n"
			"    } \n"
			"  } \n"
			"} \n"
			"void main() \n"
			"{ \n"
			"  generate_garbage(); \n"
			"  while( destroyed_instances < number_of_instances ) \n"
			"  { \n"
			"    yield(); \n"
			"  } \n"
			"} \n"
			"void generate_garbage() \n"
			"{ \n"
			"  dummy[] dummy_list(number_of_instances); \n"
			"} \n";

		COutStream out;
		int r;

		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream, Callback), &out, asCALL_THISCALL);
		RegisterScriptArray(engine, true);
		RegisterStdString(engine);
		engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(PrintString_Generic), asCALL_GENERIC);
#ifndef AS_MAX_PORTABILITY
		engine->RegisterGlobalFunction("void yield()", asFUNCTION(Yield), asCALL_CDECL);
		engine->RegisterGlobalFunction("void exit()", asFUNCTION(Exit), asCALL_CDECL);
#else
		engine->RegisterGlobalFunction("void yield()", WRAP_FN(Yield), asCALL_GENERIC);
		engine->RegisterGlobalFunction("void exit()", WRAP_FN(Exit), asCALL_GENERIC);
#endif

		mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection(0, script);
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		// Make sure the garbage is collected properly when calling it step-by-step from line callback
		// As the classes have a destructor, the garbage collector will also be invoked recursively when
		// calling the destructor.
		engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, false);
		asIScriptContext *ctx = engine->CreateContext();
		ctx->SetLineCallback(asFUNCTION(GCLineCallback), 0, asCALL_CDECL);
		ctx->Prepare(mod->GetFunctionByName("main"));
		while( ctx->Execute() == asEXECUTION_SUSPENDED ) {};
		ctx->Release();

		// Make sure the automatic garbage collection can also free up the memory properly
		mod->ResetGlobalVars();
		engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, true);
		ctx = engine->CreateContext();
		ctx->Prepare(mod->GetFunctionByName("main"));
		while( ctx->Execute() == asEXECUTION_SUSPENDED ) {};
		ctx->Release();

		engine->Release();
	}

	// This test is to validate that object types are not destroyed while there are live
	// objects in the garbage collector, even if the module has been destroyed.
	{
		COutStream out;
		int r;
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream, Callback), &out, asCALL_THISCALL);

		asIScriptModule *mod = engine->GetModule("test", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("test",
			"enum TEST { ETEST } \n"
			"class A { TEST e; A @a; } \n");
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		int enumTypeId = mod->GetTypeIdByDecl("TEST");
		if( enumTypeId < 0 )
			TEST_FAILED;
		int count = engine->GetEnumValueCount(enumTypeId);
		if( count != 1 )
			TEST_FAILED;

		int typeId = mod->GetTypeIdByDecl("A");
		asIScriptObject *obj = reinterpret_cast<asIScriptObject*>(engine->CreateScriptObject(typeId));

		engine->DiscardModule("test");
		engine->GarbageCollect();

		// The object is still alive so the enumType should still be valid
		count = engine->GetEnumValueCount(enumTypeId);
		if( count != 1 )
			TEST_FAILED;

		obj->Release();
		
		engine->Release();
	}

	// Test circular reference between script object and template instance type
	{
		COutStream out;
		int r;
		asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream, Callback), &out, asCALL_THISCALL);

		RegisterScriptArray(engine, false);

		asIScriptModule *mod = engine->GetModule("test", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("test",
			"class A { array<array<A@>> a; } \n");
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		engine->Release();
	}

/*
	{
		// This test forces a memory leak due to not registering the GC behaviours for the CFoo class
		COutStream out;
		int r;

		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream, Callback), &out, asCALL_THISCALL);

		engine->RegisterInterface("IMyInterface"); 
		engine->RegisterObjectType("CFoo", sizeof(CFoo), asOBJ_REF); 
		engine->RegisterObjectBehaviour("CFoo", asBEHAVE_ADDREF, "void f()", asMETHOD(CFoo, AddRef), asCALL_THISCALL); 
		engine->RegisterObjectBehaviour("CFoo", asBEHAVE_RELEASE, "void f()", asMETHOD(CFoo, Release), asCALL_THISCALL); 
		engine->RegisterObjectBehaviour("CFoo", asBEHAVE_FACTORY, "CFoo@ f()", asFUNCTION(&CFoo::CreateObject), asCALL_CDECL);       
		engine->RegisterObjectMethod("CFoo", "void SetObject(IMyInterface@)", asMETHOD(CFoo, SetScriptObject), asCALL_THISCALL); 

		const char *script = 
			"CBar test; \n"
			"class CBase : IMyInterface \n"
			"{ \n"
			"  IMyInterface@ m_dummy; \n" // Comment only this and everything is ok
			"} \n"
			"class CBar : CBase \n"
			"{ \n"
			"  CBar() \n"
			"  { \n"
			"    m_foo.SetObject(this); \n" // Comment only this and everything is ok
			"  } \n"
			"  CFoo m_foo; \n"
			"}; ";

		asIScriptModule *mod = engine->GetModule("test", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("test", script);
		r = mod->Build();

		engine->Release();
	}
*/
	return fail;
}


} // namespace

