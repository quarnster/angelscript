#include "utils.h"
using std::string;

#define TESTNAME "TestGlobalVar"
static const char *script1 = "float global = func() * g_f * 2.0f;";
static const char *script2 = "float global = 1.0f;";

static void func(asIScriptGeneric *gen)
{
	gen->SetReturnFloat(3.0f);
}

static float cnst = 2.0f;
static std::string g_str = "test";

static const char *script3 =
"float f = 2;                 \n"
"string str = \"test\";       \n"
"void TestGlobalVar()         \n"
"{                            \n"
"  float a = f + g_f;         \n"
"  string s = str + g_str;    \n"
"  g_f = a;                   \n"
"  f = a;                     \n"
"  g_str = s;                 \n"
"  str = s;                   \n"
"}                            \n";

static const char *script4 =
"const double gca=12;   \n"
"const double gcb=5;    \n"
"const double gcc=35.2; \n"
"const double gcd=4;    \n"
"double a=12;   \n"
"double b=5;    \n"
"double c=35.2; \n"
"double d=4;    \n"
"void test()          \n"
"{                    \n"
"  print(gca+\"\\n\");  \n"
"  print(gcb+\"\\n\");  \n"
"  print(gcc+\"\\n\");  \n"
"  print(gcd+\"\\n\");  \n"
"  print(a+\"\\n\");  \n"
"  print(b+\"\\n\");  \n"
"  print(c+\"\\n\");  \n"
"  print(d+\"\\n\");  \n"
"const double lca=12;   \n"
"const double lcb=5;    \n"
"const double lcc=35.2; \n"
"const double lcd=4;    \n"
"double la=12;   \n"
"double lb=5;    \n"
"double lc=35.2; \n"
"double ld=4;    \n"
"  print(lca+\"\\n\");  \n"
"  print(lcb+\"\\n\");  \n"
"  print(lcc+\"\\n\");  \n"
"  print(lcd+\"\\n\");  \n"
"  print(la+\"\\n\");  \n"
"  print(lb+\"\\n\");  \n"
"  print(lc+\"\\n\");  \n"
"  print(ld+\"\\n\");  \n"
"}                    \n";

static const char *script5 =
"uint OFLAG_BSP = uint(1024);";

void print(asIScriptGeneric *gen)
{
	std::string s = ((asCScriptString*)gen->GetArgAddress(0))->buffer;

	if( s != "12\n" && 
		s != "5\n" &&
		s != "35.2\n" &&
		s != "4\n" )
		printf("Error....\n");

//	printf(s.c_str());
}

bool TestGlobalVar()
{
	bool ret = false;

	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	RegisterScriptString_Generic(engine);

	engine->RegisterGlobalFunction("float func()", asFUNCTION(func), asCALL_GENERIC);
	engine->RegisterGlobalProperty("float g_f", &cnst);
	engine->RegisterGlobalProperty("string g_str", &g_str);
	engine->RegisterGlobalFunction("void print(string &in)", asFUNCTION(print), asCALL_GENERIC);

	COutStream out;
	engine->AddScriptSection("a", TESTNAME, script1, strlen(script1), 0);
	// This should fail, since we are trying to call a function in the initialization
	if( engine->Build("a") >= 0 )
	{
		printf("%s: build erronously returned success\n", TESTNAME);
		ret = true;
	}

	engine->AddScriptSection("a", "script", script2, strlen(script2), 0);
	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
	if( engine->Build("a") < 0 )
	{
		printf("%s: build failed\n", TESTNAME);
		ret = true;
	}

	engine->AddScriptSection("a", "script", script3, strlen(script3), 0);
	if( engine->Build("a") < 0 )
	{
		printf("%s: build failed\n", TESTNAME);
		ret = true;
	}

	engine->ExecuteString("a", "TestGlobalVar()");

	float *f = (float*)engine->GetGlobalVarPointer(engine->GetGlobalVarIDByDecl("a", "float f"));
	string *str = (string*)engine->GetGlobalVarPointer(engine->GetGlobalVarIDByDecl("a", "string str"));

	float fv = *f; UNUSED_VAR(fv);
	string strv = *str;

	engine->ResetModule("a");

	f = (float*)engine->GetGlobalVarPointer(engine->GetGlobalVarIDByDecl("a", "float f"));
	str = (string*)engine->GetGlobalVarPointer(engine->GetGlobalVarIDByDecl("a", "string str"));

	if( !CompareDouble(*f, 2) || *str != "test" )
	{
		printf("%s: Failed to reset the module\n", TESTNAME);
		ret = true;
	}

	engine->AddScriptSection("a", "script", script4, strlen(script4), 0, false);
	if( engine->Build("a") < 0 )
	{
		printf("%s: build failed\n", TESTNAME);
		ret = true;
	}

	int c = engine->GetGlobalVarCount("a");
	if( c != 8 ) ret = true;
	double d;
	d = *(double*)engine->GetGlobalVarPointer(engine->GetGlobalVarIDByIndex("a", 0)); 
	if( !CompareDouble(d, 12) ) ret = true;
	d = *(double*)engine->GetGlobalVarPointer(engine->GetGlobalVarIDByIndex("a", 1)); 
	if( !CompareDouble(d, 5) ) ret = true;
	d = *(double*)engine->GetGlobalVarPointer(engine->GetGlobalVarIDByIndex("a", 2)); 
	if( !CompareDouble(d, 35.2) ) ret = true;
	d = *(double*)engine->GetGlobalVarPointer(engine->GetGlobalVarIDByIndex("a", 3)); 
	if( !CompareDouble(d, 4) ) ret = true;
	
	engine->ExecuteString("a", "test()");

	engine->Release();

	//--------------------

	asIScriptArray *gPacketData = 0;
	unsigned int gPacketLength = 0;
	int r;

	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	r = engine->RegisterGlobalProperty("uint gPacketLength", &gPacketLength); assert( r >= 0 );
	r = engine->RegisterGlobalProperty("uint8[] @gPacketData", &gPacketData); assert( r >= 0 );
	engine->Release();

	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	r = engine->RegisterGlobalProperty("uint8[] @gPacketData", &gPacketData); assert( r >= 0 );
	r = engine->RegisterGlobalProperty("uint gPacketLength", &gPacketLength); assert( r >= 0 );
	engine->Release();

	//-----------------------
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
	engine->AddScriptSection(0, "script", script5, strlen(script5), 0, false);
	r = engine->Build(0); 
	if( r < 0 )
		ret = true;
	engine->Release();

	return ret;
}

