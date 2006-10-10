#include "utils.h"

void PrintException(asIScriptContext *ctx)
{
	asIScriptEngine *engine = ctx->GetEngine();
	int funcID = ctx->GetExceptionFunction();
	printf("func: %s\n", engine->GetFunctionDeclaration(funcID));
	printf("modl: %s\n", engine->GetFunctionModule(funcID));
	printf("sect: %s\n", engine->GetFunctionSection(funcID));
	printf("line: %d\n", ctx->GetExceptionLineNumber());
	printf("desc: %s\n", ctx->GetExceptionString());
}

void Assert(asIScriptGeneric *gen)
{
	bool expr = gen->GetArgDWord(0) ? true : false;
	if( !expr )
	{
		printf("--- Assert failed ---\n");
		asIScriptContext *ctx = asGetActiveContext();
		if( ctx )
		{
			asIScriptEngine *engine = ctx->GetEngine();
			int funcID = ctx->GetCurrentFunction();
			printf("func: %s\n", engine->GetFunctionDeclaration(funcID));
			printf("mdle: %s\n", engine->GetFunctionModule(funcID));
			printf("sect: %s\n", engine->GetFunctionSection(funcID));
			printf("line: %d\n", ctx->GetCurrentLineNumber());
			ctx->SetException("Assert failed");
			printf("---------------------\n");
		}
	}
}
