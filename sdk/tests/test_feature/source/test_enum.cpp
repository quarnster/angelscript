#include "utils.h"

namespace TestEnum
{

#define TESTNAME		"TestEnum"
#define TESTMODULE		"TestEnum"


// Script for testing attributes
static const char *const script =
"enum TEST2_ENUM  									\n"
"{													\n"
"	TEST_1 = -1,									\n"
"	TEST1 = 1,										\n"
"	TEST2,											\n"
"	TEST1200 = 300 * 4,								\n"
"	TEST1201 = TEST1200 + 1,						\n"
"	TEST1202,										\n"
"	TEST1203										\n"
"}													\n"
"													\n"
"TEST2_ENUM Test1()									\n"
"{													\n"
"   output(TEST_1);                                 \n"
"   output(TEST1);                                  \n"
"   output(TEST2);                                  \n"
"   output(TEST1200);                               \n"
"   output(TEST1201);                               \n"
"   output(TEST1202);                               \n"
"   output(TEST1203);                               \n"
"	return TEST2;									\n"
"}													\n";

enum TEST_ENUM
{
	ENUM1 = 1,
	ENUM2 = ENUM1*10,
	ENUM3
};

std::string buffer;
static void scriptOutput(int val1)
{
	char buf[256];
	sprintf(buf, "%d\n", val1);
	buffer += buf;
}

static bool TestEnum()
{
	asIScriptEngine   *engine;
	COutStream		   out;
	CBufferedOutStream bout;
	int		 		   r;
	bool               fail = false;

 	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	r = engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);

	// Register the enum value
	r = engine->RegisterEnum("TEST_ENUM"); assert(r >= 0);
	r = engine->RegisterEnumValue("TEST_ENUM", "ENUM1", ENUM1); assert(r >= 0);
	r = engine->RegisterEnumValue("TEST_ENUM", "ENUM2", ENUM2); assert(r >= 0);
	r = engine->RegisterEnumValue("TEST_ENUM", "ENUM3", ENUM3); assert(r >= 0);

	r = engine->RegisterGlobalFunction("void output(int val1)", asFUNCTION(scriptOutput), asCALL_CDECL); assert(r >= 0);

	// Test using the registered enum values
	r = engine->ExecuteString(0, "output(ENUM1); output(ENUM2)");
	if( r != asEXECUTION_FINISHED ) 
		fail = true;
	if( buffer != "1\n10\n" )
		fail = true;

	// Test script that declare an enum
	// enum value can be given as expression of constants
	// enum can be implicitly cast to number
	buffer = "";
	r = engine->AddScriptSection(NULL, NULL, script, strlen(script), 0);
	r = engine->Build(NULL);
	if( r < 0 ) 
		fail = true;

	r = engine->ExecuteString(NULL, "Test1()");
	if( r != asEXECUTION_FINISHED ) 
		fail = true;
	if( buffer != "-1\n1\n2\n1200\n1201\n1202\n1203\n" )
		fail = true;

	// Registered enums are literal constants
	// variable of enum type can be implictly cast to primitive
	buffer = "";
	r = engine->ExecuteString(0, "TEST_ENUM e = ENUM1; switch( e ) { case ENUM1: output(e); }");
	if( r != asEXECUTION_FINISHED )
		fail = true;
	if( buffer != "1\n" )
		fail = true;

	// Script declared enums behave the same
	buffer = "";
	r = engine->ExecuteString(0, "TEST2_ENUM e = TEST_1; switch( e ) {case TEST_1: output(e); }");
	if( r != asEXECUTION_FINISHED )
		fail = true;
	if( buffer != "-1\n" )
		fail = true;

	// enum values can't be declared with expressions including subsequent values
	bout.buffer = "";
	r = engine->SetMessageCallback(asMETHOD(CBufferedOutStream,Callback), &bout, asCALL_THISCALL);
	const char *script2 = "enum TEST_ERR { ERR1 = ERR2, ERR2 }";
	r = engine->AddScriptSection("error", "error", script2, strlen(script2));
	r = engine->Build("error");
	if( r >= 0 )
		fail = true;
	if( bout.buffer != "error (1, 22) : Info    : Compiling TEST_ERR ERR1\n"
					   "error (1, 24) : Error   : Use of uninitialized global variable 'ERR2'.\n"
					   "error (1, 1) : Info    : Compiling TEST_ERR ERR2\n"
                       "error (1, 1) : Error   : Use of uninitialized global variable 'ERR1'.\n" )
	{
		printf(bout.buffer.c_str());
		fail = true;
	}

	// enum type name can't be overloaded with variable name in another scope
	bout.buffer = "";
	r = engine->ExecuteString(0, "int TEST_ENUM = 999");
	if( r >= 0  )
		fail = true;
	if( bout.buffer != "ExecuteString (1, 5) : Error   : Illegal variable name 'TEST_ENUM'.\n" )
	{
		printf(bout.buffer.c_str());
		fail = true;
	}
	r = engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);

	// enum value name can be overloaded with variable name in another scope
	buffer = "";
	r = engine->ExecuteString(0, "int ENUM1 = 999; output(ENUM1)");
	if( r != asEXECUTION_FINISHED )
		fail = true;
	if( buffer != "999\n" )
		fail = true;

	// number cannot be implicitly cast to enum type
	bout.buffer = "";
	r = engine->SetMessageCallback(asMETHOD(CBufferedOutStream,Callback), &bout, asCALL_THISCALL);
	r = engine->ExecuteString(0, "TEST_ENUM val = 1");
	if( r >= 0 )
		fail = true;
	r = engine->ExecuteString(0, "float f = 1.2f; TEST_ENUM val = f");
	if( r >= 0 )
		fail = true;
	if( bout.buffer != "ExecuteString (1, 17) : Error   : Can't implicitly convert from 'uint' to 'TEST_ENUM'.\n"
                       "ExecuteString (1, 33) : Error   : Can't implicitly convert from 'float' to 'TEST_ENUM'.\n" )
	{
		printf(bout.buffer.c_str());
		fail = true;
	}
	r = engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);

	// constant number can be explicitly cast to enum type 
	r = engine->ExecuteString(0, "TEST_ENUM val = TEST_ENUM(1)");
	if( r != asEXECUTION_FINISHED )
		fail = true;
	r = engine->ExecuteString(0, "TEST_ENUM val = cast<TEST_ENUM>(1)");
	if( r != asEXECUTION_FINISHED )
		fail = true;

	// primitive value can be explicitly cast to enum type
	r = engine->ExecuteString(0, "float f = 1.2f; TEST_ENUM val = TEST_ENUM(f)");
	if( r != asEXECUTION_FINISHED )
		fail = true;
	r = engine->ExecuteString(0, "float f = 1.2f; TEST_ENUM val = cast<TEST_ENUM>(f)");
	if( r != asEXECUTION_FINISHED )
		fail = true;

	// math operator with enums
	buffer = "";
	r = engine->ExecuteString(0, "int a = ENUM2 * 10; output(a); output(ENUM2 + ENUM1)");
	if( r != asEXECUTION_FINISHED )
		fail = true;
	if( buffer != "100\n11\n" )
		fail = true;

	// comparison operator with enums
	buffer = "";
	r = engine->ExecuteString(0, "if( ENUM2 > ENUM1 ) output(1);");
	if( r != asEXECUTION_FINISHED )
		fail = true;
	if( buffer != "1\n" )
		fail = true;

	// bitwise operators with enums
	buffer = "";
	r = engine->ExecuteString(0, "output( ENUM2 << ENUM1 )");
	if( r != asEXECUTION_FINISHED )
		fail = true;
	if( buffer != "20\n" )
		fail = true;

	// circular reference between enum and global variable are 
	// allowed if they can be resolved
	const char *script3 = 
	"enum TEST_EN                \n"
	"{                           \n"
	"  EN1,                      \n"
	"  EN2 = gvar,               \n"
	"  EN3,                      \n"
	"}                           \n"
	"const int gvar = EN1 + 10;  \n";
	engine->AddScriptSection("err", "error", script3, strlen(script3));
	r = engine->Build("err");
	if( r < 0 )
		fail = true;
	buffer = "";
	r = engine->ExecuteString("err", "output(EN2); output(EN3)");
	if( r != asEXECUTION_FINISHED )
		fail = true;
	if( buffer != "10\n11\n" )
		fail = true;
	
	// TEST: functions can be overloaded for parameters with enum type

	engine->Release();

	// Success
	return fail;
}


bool Test()
{
	return TestEnum();
}



} // namespace
