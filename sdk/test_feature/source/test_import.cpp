//
// Tests importing functions from other modules
//
// Test author: Andreas Jonsson
//

#include "utils.h"

namespace TestImport
{

#define TESTNAME "TestImport"




static const char *script1 =
"import void Test() from \"DynamicModule\";   \n"
"void main()                                  \n"
"{                                            \n"
"  Test();                                    \n"
"}                                            \n";

static const char *script2 =
"void Test()            \n"
"{                      \n"
"  number = 1234567890; \n"
"}                      \n";

bool Test()
{
	bool fail = false;

	int number = 0;

 	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	engine->RegisterGlobalProperty("int number", &number);

	COutStream out;
	engine->AddScriptSection(0, TESTNAME ":1", script1, strlen(script1), 0);
	engine->Build(0, &out);

	engine->AddScriptSection("DynamicModule", TESTNAME ":2", script2, strlen(script2), 0);
	engine->Build("DynamicModule", &out);

	// Bind all functions that the module imports
	engine->BindAllImportedFunctions(0);

	engine->ExecuteString(0, "main()", &out);

	engine->Release();

	if( number != 1234567890 )
	{
		printf("%s: Failed to set the number as expected\n", TESTNAME);
		fail = true;
	}

	// Success
	return fail;
}

} // namespace

