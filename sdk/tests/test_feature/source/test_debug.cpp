
#include <stdarg.h>
#include "utils.h"

namespace TestDebug
{

#define TESTNAME "TestDebug"




static const char *script1 =
"import void Test2() from \"Module2\";  \n"
"void main()                            \n"
"{                                      \n"
"  int a = 1;                           \n"
"  string s = \"text\";                 \n"
"  Test1();                             \n" // 5
"  Test2();                             \n" // 6
"}                                      \n"
"void Test1()                           \n"
"{                                      \n" // 9
"  int d = 4;                           \n"
"}                                      \n";

static const char *script2 =
"void Test2()           \n"
"{                      \n"
"  int b = 2;           \n"
"  Test3();             \n" // 4
"}                      \n"
"void Test3()           \n"
"{                      \n"
"  int c = 3;           \n"
"  int[] a;             \n" // 9
"  a[0] = 0;            \n" // 10
"}                      \n";


std::string printBuffer;

static const char *correct =
"Module1:void main():4,3\n"
//" int a = -842150451\n"
//" string s = <null>\n"
"Module1:void main():5,3\n"
//" int a = 1\n"
//" string s = <null>\n"
"Module1:void main():6,3\n"
//" int a = 1\n"
//" string s = 'text'\n"
" Module1:void Test1():11,3\n"
//" int d = 6179008\n"
" Module1:void Test1():12,2\n"
//" int d = 4\n"
"Module1:void main():7,3\n"
//" int a = 1\n"
//" string s = 'text'\n"
" Module2:void Test2():3,3\n"
//" int b = 4\n"
" Module2:void Test2():4,3\n"
//" int b = 2\n"
"  Module2:void Test3():8,3\n"
//" int c = -842150451\n"
"  Module2:void Test3():9,3\n"
//" int c = 3\n"
"  Module2:void Test3():10,3\n"
//" int c = 3\n"
"--- exception ---\n"
"desc: Out of range\n"
"func: void Test3()\n"
"modl: Module2\n"
"sect: TestDebug:2\n"
"line: 10,3\n"
" int c = 3\n"
"--- call stack ---\n"
"Module1:void main():8,2\n"
" int a = 1\n"
" string s = 'text'\n"
"Module2:void Test2():5,2\n"
" int b = 2\n";

static const char *correctWithoutLineCues =
"--- exception ---\n"
"desc: Out of range\n"
"func: void Test3()\n"
"modl: Module2\n"
"sect: TestDebug:2\n"
"line: 8,3\n"
"--- call stack ---\n"
"Module1:void main():6,2\n"
"Module2:void Test2():4,2\n";


void print(const char *format, ...)
{
	char buf[256];
	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	printBuffer += buf;
}

void PrintVariables(asIScriptContext *ctx, int stackLevel);

void LineCallback(asIScriptContext *ctx, void *param)
{
	asIScriptEngine *engine = ctx->GetEngine();
	int funcID = ctx->GetCurrentFunction();
	int col;
	int line = ctx->GetCurrentLineNumber(&col);
	int indent = ctx->GetCallstackSize();
	for( int n = 0; n < indent; n++ )
		print(" ");
	print("%s:%s:%d,%d\n", engine->GetFunctionModule(funcID),
	                    engine->GetFunctionDeclaration(funcID),
	                    line, col);

//	PrintVariables(ctx, -1);
}

void PrintVariables(asIScriptContext *ctx, int stackLevel)
{
	int numVars = ctx->GetVarCount(stackLevel);
	asIScriptEngine *engine = ctx->GetEngine();
	for( int n = 0; n < numVars; n++ )
	{
		int typeId = ctx->GetVarTypeId(n, stackLevel); 
		void *varPointer = ctx->GetVarPointer(n, stackLevel);
		if( typeId == engine->GetTypeIdByDecl(0, "int") )
		{
			print(" %s = %d\n", ctx->GetVarDeclaration(n, 0, stackLevel), *(int*)varPointer);
		}
		else if( typeId == engine->GetTypeIdByDecl(0, "string") )
		{
			asCScriptString *str = *(asCScriptString**)varPointer;
			if( str )
				print(" %s = '%s'\n", ctx->GetVarDeclaration(n, 0, stackLevel), str->buffer.c_str());
			else
				print(" %s = <null>\n", ctx->GetVarDeclaration(n, 0, stackLevel));
		}
	}
}

void ExceptionCallback(asIScriptContext *ctx, void *param)
{
	asIScriptEngine *engine = ctx->GetEngine();
	int funcID = ctx->GetExceptionFunction();
	print("--- exception ---\n");
	print("desc: %s\n", ctx->GetExceptionString());
	print("func: %s\n", engine->GetFunctionDeclaration(funcID));
	print("modl: %s\n", engine->GetFunctionModule(funcID));
	print("sect: %s\n", engine->GetFunctionSection(funcID));
	int col, line = ctx->GetExceptionLineNumber(&col);
	print("line: %d,%d\n", line, col);

	// Print the variables in the current function
	PrintVariables(ctx, -1);

	// Show the call stack with the variables
	print("--- call stack ---\n");
	for( int n = 0; n < ctx->GetCallstackSize(); n++ )
	{
		funcID = ctx->GetCallstackFunction(n);
		line = ctx->GetCallstackLineNumber(n,&col);
		print("%s:%s:%d,%d\n", engine->GetFunctionModule(funcID),
		                       engine->GetFunctionDeclaration(funcID),
							   line, col);
		PrintVariables(ctx, n);
	}
}

bool Test()
{
	bool fail = false;

	int number = 0;

 	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	RegisterScriptString_Generic(engine);
	engine->RegisterGlobalProperty("int number", &number);

	COutStream out;
	engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
	engine->AddScriptSection("Module1", TESTNAME ":1", script1, strlen(script1), 0);
	engine->Build("Module1");

	engine->AddScriptSection("Module2", TESTNAME ":2", script2, strlen(script2), 0);
	engine->Build("Module2");

	// Bind all functions that the module imports
	engine->BindAllImportedFunctions("Module1");

	asIScriptContext *ctx =	engine->CreateContext();
	ctx->SetLineCallback(asFUNCTION(LineCallback), 0, asCALL_CDECL);
	ctx->SetExceptionCallback(asFUNCTION(ExceptionCallback), 0, asCALL_CDECL);
	ctx->Prepare(engine->GetFunctionIDByDecl("Module1", "void main()"));
	ctx->Execute();
	ctx->Release();
	engine->Release();

	if( printBuffer != correct &&
		printBuffer != correctWithoutLineCues )
	{
		fail = true;
		printf(printBuffer.c_str());
		printf("%s: failed\n", TESTNAME);
	}

	// Success
	return fail;
}

} // namespace

