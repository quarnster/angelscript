#include "utils.h"

namespace TestInterface
{

static const char * const TESTNAME = "TestInterface";

// Test implementing multiple interfaces
// Test implicit conversion from class to interface
// Test calling method of interface handle from script
// Register interface from application
static const char *script1 =
"interface myintf                                \n"
"{                                               \n"
"   void test();                                 \n"
"}                                               \n"
"class myclass : myintf, intf2, appintf          \n"
"{                                               \n"
"   myclass() {this.str = \"test\";}             \n"
"   void test() {Assert(this.str == \"test\");}  \n"
"   int func2(const string &in i)                \n"
"   {                                            \n"
"      Assert(this.str == i);                    \n"
"      return 0;                                 \n"
"   }                                            \n"
"   string str;                                  \n"
"}                                               \n"
"interface intf2                                 \n"
"{                                               \n"
"   int func2(const string &in);                 \n"
"}                                               \n"
"void test()                                     \n"
"{                                               \n"
"   myclass a;                                   \n"
"   myintf@ b = a;                               \n"
"   intf2@ c;                                    \n"
"   @c = a;                                      \n"
"   a.func2(\"test\");                           \n"
"   c.func2(\"test\");                           \n"
"   test(a);                                     \n"
"}                                               \n"
"void test(appintf@i)                            \n"
"{                                               \n"
"   i.test();                                    \n"
"}                                               \n";

// Test class that don't implement all functions of the interface.
// Test instanciating an interface. Shouldn't work.
// Test that classes don't implement the same interface twice
// Try copying an interface variable to another. Shouldn't work.
// Test implicit conversion from class to interface that is not being implemented. Should give compiler error
// Test implicit conversion from interface to class. Should give compiler error.
static const char *script2 = 
"interface intf             \n"
"{                          \n"
"    void test();           \n"
"}                          \n"
"class myclass : intf, intf \n"
"{                          \n"
"}                          \n"
"interface nointf {}        \n"
"void test(intf &i)         \n"
"{                          \n"
"   intf a;                 \n"
"   intf@ b, c;             \n"
"   b = c;                  \n"
"   myclass d;              \n"
"   nointf@ e = d;          \n"
"   myclass@f = b;          \n"
"}                          \n";

// TODO: Test explicit conversion from interface to class. Should give null value if not the right class.

bool Test2();

bool Test()
{
	bool fail = false;

	if( !fail ) fail = Test2();

	int r;

 	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	COutStream out;
	CBufferedOutStream bout;
	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);

	RegisterScriptString_Generic(engine);

	engine->RegisterGlobalFunction("void Assert(bool)", asFUNCTION(Assert), asCALL_GENERIC);

	// Register an interface from the application
	r = engine->RegisterInterface("appintf"); assert( r >= 0 );
	r = engine->RegisterInterfaceMethod("appintf", "void test()"); assert( r >= 0 );

	// Test working example
	asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
	mod->AddScriptSection(TESTNAME, script1, strlen(script1), 0);
	r = mod->Build();
	if( r < 0 ) TEST_FAILED;

	r = ExecuteString(engine, "test()", mod);
	if( r != asEXECUTION_FINISHED ) TEST_FAILED;

	// Test calling the interface method from the application
	int typeId = engine->GetModule(0)->GetTypeIdByDecl("myclass");
	asIScriptObject *obj = (asIScriptObject*)engine->CreateScriptObject(typeId);

	int intfTypeId = engine->GetModule(0)->GetTypeIdByDecl("myintf");
	asIObjectType *type = engine->GetObjectTypeById(intfTypeId);
	asIScriptFunction *func = type->GetMethodByDecl("void test()");
	asIScriptContext *ctx = engine->CreateContext();
	r = ctx->Prepare(func);
	if( r < 0 ) TEST_FAILED;
	ctx->SetObject(obj);
	ctx->Execute();
	if( r != asEXECUTION_FINISHED )
		TEST_FAILED;

	intfTypeId = engine->GetTypeIdByDecl("appintf");
	type = engine->GetObjectTypeById(intfTypeId);
	func = type->GetMethodByDecl("void test()");

	r = ctx->Prepare(func);
	if( r < 0 ) TEST_FAILED;
	ctx->SetObject(obj);
	ctx->Execute();
	if( r != asEXECUTION_FINISHED )
		TEST_FAILED;

	if( ctx ) ctx->Release();
	if( obj ) obj->Release();

	// Test class that don't implement all functions of the interface.
	// Test instanciating an interface. Shouldn't work.
	// Test that classes don't implement the same interface twice
	// Try copying an interface variable to another. Shouldn't work.
	// Test implicit conversion from class to interface that is not being implemented. Should give compiler error
	// Test implicit conversion from interface to class. Should give compiler error.
	engine->SetMessageCallback(asMETHOD(CBufferedOutStream,Callback), &bout, asCALL_THISCALL);
	mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
	mod->AddScriptSection(TESTNAME, script2, strlen(script2), 0);
	r = mod->Build();
	if( r >= 0 ) TEST_FAILED;
	if( bout.buffer != "TestInterface (5, 23) : Warning : The interface 'intf' is already implemented\n"
					   "TestInterface (5, 7) : Error   : Missing implementation of 'void intf::test()'\n"
					   "TestInterface (9, 1) : Info    : Compiling void test(intf&inout)\n"
					   "TestInterface (11, 9) : Error   : Data type can't be 'intf'\n"
					   "TestInterface (13, 6) : Error   : There is no copy operator for the type 'intf' available.\n"
					   "TestInterface (15, 16) : Error   : Can't implicitly convert from 'myclass&' to 'nointf@&'.\n"
					   "TestInterface (16, 16) : Error   : Can't implicitly convert from 'intf@&' to 'myclass@&'.\n" )
	{
		printf("%s", bout.buffer.c_str());
		TEST_FAILED;
	}

	engine->Release();

	// Test cast for both temporary handle and non-temporary handle
	{
		const char *script = 
			"interface ScriptLogic {} \n"
			"class PlayerLogic : ScriptLogic {} \n"
			"ScriptLogic @getScriptObject() { return PlayerLogic(); } \n";

		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
		RegisterStdString(engine);
		mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection(TESTNAME, script);
		r = mod->Build();
		if( r < 0 ) TEST_FAILED;

		// Non-temporary handle
		r = ExecuteString(engine, "ScriptLogic @c = getScriptObject(); cast<PlayerLogic>(c);", mod);
		if( r != asEXECUTION_FINISHED )
			TEST_FAILED;

		// Temporary handle
		r = ExecuteString(engine, "cast<PlayerLogic>(getScriptObject());", mod);
		if( r != asEXECUTION_FINISHED )
			TEST_FAILED;

		engine->Release();			
	}

	// It should be possible to inherit the implementation of an interface method
	{
		const char *script = 
			"interface I { void method(); } \n"
			"class B { void method() {} } \n"
			"class D : B, I {} \n"
			"D d; \n";

		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
		mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection(TESTNAME, script);
		r = mod->Build();
		if( r < 0 ) TEST_FAILED;

		engine->Release();			
	}

	// Allow script declared interfaces to inherit from other interfaces
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(CBufferedOutStream,Callback), &bout, asCALL_THISCALL);
		mod = engine->GetModule(0, asGM_ALWAYS_CREATE);

		const char *script = 
			"interface A { void a(); } \n"
			"interface B : A { void b(); } \n"
			"class C : B {} \n"; // Must implement both a() and b()

		bout.buffer = "";
		mod->AddScriptSection(TESTNAME, script);
		r = mod->Build();
		if( r >= 0 ) TEST_FAILED;
		if( bout.buffer != "TestInterface (3, 7) : Error   : Missing implementation of 'void B::b()'\n"
		                   "TestInterface (3, 7) : Error   : Missing implementation of 'void A::a()'\n" )
		{
			printf("%s", bout.buffer.c_str());
			TEST_FAILED;
		}

		// Don't allow shared interface to implement non-shared interface
		script = 
			"interface A {} \n"
			"shared interface B : A {} \n";

		bout.buffer = "";
		mod->AddScriptSection(TESTNAME, script);
		r = mod->Build();
		if( r >= 0 ) TEST_FAILED;
		if( bout.buffer != "TestInterface (2, 22) : Error   : Shared type cannot implement non-shared interface 'A'\n" )
		{
			printf("%s", bout.buffer.c_str());
			TEST_FAILED;
		}


		engine->Release();
	}

	// Success
	return fail;
}



bool Test2()
{
	bool fail = false;
	int r;
	COutStream out;
	asIScriptEngine *engine;
	asIScriptModule *mod;

	// An interface that is declared equally in two different modules should receive the same type id
	// As of release 2.23.0, the interface must be explicitly marked as shared
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);

		const char *script = "shared interface Simple { void function(int); }";
		mod = engine->GetModule("a", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script, strlen(script));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		mod = engine->GetModule("b", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script, strlen(script));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		int typeA = engine->GetModule("a")->GetTypeIdByDecl("Simple");
		int typeB = engine->GetModule("b")->GetTypeIdByDecl("Simple");

		if( typeA != typeB )
			TEST_FAILED;

		// Test recompiling a module
		mod = engine->GetModule("a", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script, strlen(script));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		typeA = engine->GetModule("a")->GetTypeIdByDecl("Simple");
		if( typeA != typeB )
			TEST_FAILED;

		// Test interface that references itself
		const char *script1 = "shared interface A { A@ f(); }";
		mod = engine->GetModule("a", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script1, strlen(script1));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		mod = engine->GetModule("b", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script1, strlen(script1));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		int typeAA = engine->GetModule("a")->GetTypeIdByDecl("A");
		int typeBA = engine->GetModule("b")->GetTypeIdByDecl("A");

		if( typeAA != typeBA )
			TEST_FAILED;

		engine->Release();
	}

	// Test with more complex interfaces
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);

		const char *script2 = "shared interface A { B@ f(); } \n"
			                  "shared interface B { A@ f1(); C@ f2(); } \n"
							  "shared interface C { A@ f(); } \n";
		mod = engine->GetModule("a", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script2, strlen(script2));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		mod = engine->GetModule("b", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script2, strlen(script2));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		int typeAA = engine->GetModule("a")->GetTypeIdByDecl("A");
		int typeAB = engine->GetModule("a")->GetTypeIdByDecl("B");
		int typeAC = engine->GetModule("a")->GetTypeIdByDecl("C");
		
		int typeBA = engine->GetModule("b")->GetTypeIdByDecl("A");
		int typeBB = engine->GetModule("b")->GetTypeIdByDecl("B");
		int typeBC = engine->GetModule("b")->GetTypeIdByDecl("C");

		if( typeAA != typeBA ||
			typeAB != typeBB ||
			typeAC != typeBC )
			TEST_FAILED;

		engine->Release();
	}

	// Test interfaces that are not equal
	{
		CBufferedOutStream bout;
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(CBufferedOutStream,Callback), &bout, asCALL_THISCALL);

		const char *script3 = "shared interface A { B@ f(); } shared interface B { int f(); }";
		const char *script4 = "shared interface A { B@ f(); } shared interface B { float f(); }";

		mod = engine->GetModule("a", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script3, strlen(script3));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		// Shared interfaces won't allow compiling two interfaces with the same name but different methods
		mod = engine->GetModule("b", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script", script4, strlen(script4));
		r = mod->Build();
		if( r >= 0 )
			TEST_FAILED;

		if( bout.buffer != "script (1, 53) : Error   : Shared type doesn't match the original declaration in other module\n" )
		{
			printf("%s", bout.buffer.c_str());
			TEST_FAILED;
		}

		engine->Release();
	}

	// Interfaces that uses the interfaces that are substituted must be updated
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);

		const char *script5 = "shared interface A { float f(); }";
		const char *script6 = "shared interface B { A@ f(); }";
		mod = engine->GetModule("a", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script5", script5, strlen(script5));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		mod = engine->GetModule("b", asGM_ALWAYS_CREATE);
		mod->AddScriptSection("script5", script5, strlen(script5));
		mod->AddScriptSection("script6", script6, strlen(script6));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		int typeBA = engine->GetModule("b")->GetTypeIdByDecl("A@");
		int typeBB = engine->GetModule("b")->GetTypeIdByDecl("B");
		asIObjectType *objType = engine->GetObjectTypeById(typeBB);
		asIScriptFunction *func = objType->GetMethodByIndex(0);
		if( func->GetReturnTypeId() != typeBA )
			TEST_FAILED;

		engine->Release();
	}

	// This must work for pre-compiled byte code as well, i.e. when loading the byte code 
	// the interface ids must be resolved in the same way it is for compiled scripts
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);

		mod = engine->GetModule("a", asGM_ALWAYS_CREATE);
		const char *script1 = "shared interface A { A@ f(); }";
		mod->AddScriptSection("script", script1, strlen(script1));
		r = mod->Build();
		if( r < 0 )
			TEST_FAILED;

		CBytecodeStream stream(__FILE__"1");
		asIScriptModule *module = engine->GetModule("a");
		module->SaveByteCode(&stream);
		module = engine->GetModule("b", asGM_CREATE_IF_NOT_EXISTS);
		r = module->LoadByteCode(&stream);
		if( r < 0 )
			TEST_FAILED;

		int typeAA = engine->GetModule("a")->GetTypeIdByDecl("A");
		int typeBA = engine->GetModule("b")->GetTypeIdByDecl("A");

		if( typeAA != typeBA )
			TEST_FAILED;

		// TODO: The interfaces should be equal if they use enums declared in the 
		// scripts as well (we don't bother checking the enum values)

		engine->Release();
	}

	return fail;
}

} // namespace

