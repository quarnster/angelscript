//
// Tests importing functions from other modules
//
// Test author: Andreas Jonsson
//

#include <vector>
#include "utils.h"

namespace TestSaveLoad
{

#define TESTNAME "TestSaveLoad"


class CBytecodeStream : public asIBinaryStream
{
public:
	CBytecodeStream() {wpointer = 0;rpointer = 0;}

	void Write(const void *ptr, asUINT size) {if( size == 0 ) return; buffer.resize(buffer.size() + size); memcpy(&buffer[wpointer], ptr, size); wpointer += size;}
	void Read(void *ptr, asUINT size) {memcpy(ptr, &buffer[rpointer], size); rpointer += size;}

	int rpointer;
	int wpointer;
	std::vector<asBYTE> buffer;
};


static const char *script1 =
"import void Test() from \"DynamicModule\";   \n"
"OBJ g_obj;                                   \n"
"A @gHandle;                                  \n"
"void main()                                  \n"
"{                                            \n"
"  Test();                                    \n"
"  TestStruct();                              \n"
"  TestArray();                               \n"
"}                                            \n"
"void TestObj(OBJ &out obj)                   \n"
"{                                            \n"
"}                                            \n"
"void TestStruct()                            \n"
"{                                            \n"
"  A a;                                       \n"
"  a.a = 2;                                   \n"
"  A@ b = @a;                                 \n"
"}                                            \n"
"void TestArray()                             \n"
"{                                            \n"
"  A[] c(3);                                  \n"
"  int[] d(2);                                \n"
"  A[]@[] e(1);                               \n"
"  @e[0] = @c;                                \n"
"}                                            \n"
"class A                                      \n"
"{                                            \n"
"  int a;                                     \n"
"};                                           \n"
"void TestHandle(string @str)                 \n"
"{                                            \n"
"}                                            \n"
"interface MyIntf                             \n"
"{                                            \n"
"  void test();                               \n"
"}                                            \n"
"class MyClass : MyIntf                       \n"
"{                                            \n"
"  void test() {number = 1241;}               \n"
"}                                            \n";

static const char *script2 =
"void Test()                               \n"
"{                                         \n"
"  int[] a(3);                             \n"
"  a[0] = 23;                              \n"
"  a[1] = 13;                              \n"
"  a[2] = 34;                              \n"
"  if( a[0] + a[1] + a[2] == 23+13+34 )    \n"
"    number = 1234567890;                  \n"
"}                                         \n";

bool fail = false;
int number = 0;
COutStream out;
asIScriptEngine *ConfigureEngine()
{
	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
	RegisterScriptString_Generic(engine);
	engine->RegisterGlobalProperty("int number", &number);
	engine->RegisterObjectType("OBJ", sizeof(int), asOBJ_PRIMITIVE);

	return engine;
}

void TestScripts(asIScriptEngine *engine)
{
	int r;

	// Bind the imported functions
	r = engine->BindAllImportedFunctions(0); assert( r >= 0 );

	// Verify if handles are properly resolved
	int funcID = engine->GetFunctionIDByDecl(0, "void TestHandle(string @)");
	if( funcID < 0 ) 
	{
		printf("%s: Failed to identify function with handle\n", TESTNAME);
		fail = true;
	}

	engine->ExecuteString(0, "main()");

	if( number != 1234567890 )
	{
		printf("%s: Failed to set the number as expected\n", TESTNAME);
		fail = true;
	}

	// Call an interface method on a class that implements the interface
	int typeId = engine->GetTypeIdByDecl(0, "MyClass");
	asIScriptStruct *obj = (asIScriptStruct*)engine->CreateScriptObject(typeId);

	int intfTypeId = engine->GetTypeIdByDecl(0, "MyIntf");
	int funcId = engine->GetMethodIDByDecl(intfTypeId, "void test()");
	asIScriptContext *ctx = engine->CreateContext();
	r = ctx->Prepare(funcId);
	if( r < 0 ) fail = true;
	ctx->SetObject(obj);
	ctx->Execute();
	if( r != asEXECUTION_FINISHED )
		fail = true;

	if( ctx ) ctx->Release();
	if( obj ) obj->Release();

	if( number != 1241 )
	{
		printf("%s: Interface method failed\n", TESTNAME);
		fail = true;
	}
}

bool Test()
{
 	asIScriptEngine *engine = ConfigureEngine();

	engine->AddScriptSection(0, TESTNAME ":1", script1, strlen(script1), 0);
	engine->Build(0);

	engine->AddScriptSection("DynamicModule", TESTNAME ":2", script2, strlen(script2), 0);
	engine->Build("DynamicModule");

	TestScripts(engine);

	// Save the compiled byte code
	CBytecodeStream stream;
	engine->SaveByteCode(0, &stream);

//	engine->Release();
//	engine = ConfigureEngine();

	// Load the compiled byte code into the same module
	engine->LoadByteCode(0, &stream);

	engine->AddScriptSection("DynamicModule", TESTNAME ":2", script2, strlen(script2), 0);
	engine->Build("DynamicModule");

	TestScripts(engine);

	engine->Release();

	// Success
	return fail;
}

} // namespace

