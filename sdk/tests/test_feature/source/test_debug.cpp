
#include <stdarg.h>
#include "utils.h"

namespace TestDebug
{

#define TESTNAME "TestDebug"




static const char *script1 =
"import void Test2() from \"Module2\";  \n"
"void main()                            \n"
"{                                      \n"
"  Test1();                             \n" // 4
"  Test2();                             \n" // 5
"}                                      \n"
"void Test1()                           \n"
"{                                      \n" // 8
"}                                      \n";

static const char *script2 =
"void Test2()           \n"
"{                      \n"
"  Test3();             \n" // 3
"}                      \n"
"void Test3()           \n"
"{                      \n"
"  int[] a;             \n" // 7
"  a[0] = 0;            \n" // 8
"}                      \n";


std::string printBuffer;
const char *correct =
"Module1:void main():4,3\n"
" Module1:void Test1():9,2\n"
"Module1:void main():5,3\n"    
" Module2:void Test2():3,3\n"
"  Module2:void Test3():7,3\n"
"  Module2:void Test3():8,3\n" 
"--- exception ---\n"
"desc: Out of range\n"
"func: void Test3()\n"
"modl: Module2\n"
"sect: TestDebug:2\n"
"line: 8,3\n"
"--- call stack ---\n"
"Module1:void main():6,2\n"
"Module2:void Test2():4,2\n";

const char *correctWithoutLineCues =
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

//	printf(buf);
	printBuffer += buf;
}

void LineCallback(asIScriptContext *ctx, void *param)
{
	asIScriptEngine *engine = ctx->GetEngine();
	int funcID = ctx->GetCurrentFunction();
	int col;
	int line = ctx->GetCurrentLineNumber(&col);
	int indent = ctx->GetCallstackSize();
	for( int n = 0; n < indent; n++ )
		print(" ");
	print("%s:%s:%d,%d\n", engine->GetModuleNameFromIndex(asMODULEIDX(funcID)),
	                    engine->GetFunctionDeclaration(funcID),
	                    line, col);
}

void ExceptionCallback(asIScriptContext *ctx, void *param)
{
	asIScriptEngine *engine = ctx->GetEngine();
	int funcID = ctx->GetExceptionFunction();
	print("--- exception ---\n");
	print("desc: %s\n", ctx->GetExceptionString());
	print("func: %s\n", engine->GetFunctionDeclaration(funcID));
	print("modl: %s\n", engine->GetModuleNameFromIndex(asMODULEIDX(funcID)));
	print("sect: %s\n", engine->GetFunctionSection(funcID));
	int col, line = ctx->GetExceptionLineNumber(&col);
	print("line: %d,%d\n", line, col);

	print("--- call stack ---\n");
	for( int n = 0; n < ctx->GetCallstackSize(); n++ )
	{
		funcID = ctx->GetCallstackFunction(n);
		line = ctx->GetCallstackLineNumber(n,&col);
		print("%s:%s:%d,%d\n", engine->GetModuleNameFromIndex(asMODULEIDX(funcID)),
		                       engine->GetFunctionDeclaration(funcID),
							   line, col);
	}
}

bool Test()
{
	bool fail = false;

	int number = 0;

 	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	engine->RegisterGlobalProperty("int number", &number);

	COutStream out;
	engine->AddScriptSection("Module1", TESTNAME ":1", script1, strlen(script1), 0);
	engine->Build("Module1", &out);

	engine->AddScriptSection("Module2", TESTNAME ":2", script2, strlen(script2), 0);
	engine->Build("Module2", &out);

	// Bind all functions that the module imports
	engine->BindAllImportedFunctions("Module1");

	asIScriptContext *ctx;
	engine->CreateContext(&ctx);
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
		printf("%s: failed\n", TESTNAME);
	}

	// Success
	return fail;
}

} // namespace

