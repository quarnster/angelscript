#include "utils.h"
#include "../../../add_on/scripthandle/scripthandle.h"
#include "../../../add_on/scriptarray/scriptarray.h"

namespace Test_Addon_ScriptHandle
{

static const char *TESTNAME = "Test_Addon_ScriptHandle";

bool Test()
{
	bool fail = false;
	int r;
	COutStream out;
	CBufferedOutStream bout;
	asIScriptContext *ctx;
 	asIScriptEngine *engine;

	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asMETHOD(COutStream,Callback), &out, asCALL_THISCALL);
		RegisterScriptHandle(engine);
		RegisterScriptArray(engine, false);
		engine->RegisterGlobalFunction("void assert(bool)", asFUNCTION(Assert), asCALL_GENERIC);

		const char *script = "class A {} \n"
							 "class B {} \n"
							 "void main() \n"
							 "{ \n"
							 "  handle@ ra, rb; \n"
							 "  A a; B b; \n"
							 // Assignment of reference
							 "  @ra = @a; \n"
							 "  assert( ra is a ); \n" 
							 "  @rb = @b; \n"
							 // Casting to reference
							 "  A@ ha = cast<A>(ra); \n"
							 "  assert( ha !is null ); \n"
							 "  B@ hb = cast<B>(ra); \n"
							 "  assert( hb is null ); \n"
							 // Assigning null, and comparing with null
							 "  @ra = null; \n"
							 "  assert( ra is null ); \n"
							 "  func2(ra); \n"
							 // Handle assignment with explicit handle
							 "  @ra = @rb; \n"
							 "  assert( ra is b ); \n"
							 "  assert( rb is b ); \n"
							 "  assert( ra is rb ); \n"
							 // Handle assignment with implicit handle
							 "  @rb = rb; \n"
							 "  assert( rb is b ); \n"
							 "  assert( ra is rb ); \n"
							 // Function call and return value
							 "  @rb = func(rb); \n"
							 "  assert( rb is b ); \n"
							 "  assert( func(rb) is b ); \n"
							 // Arrays of handles 
							 "  array<handle@> arr(2); \n"
							 "  assert( arr[0] is null ); \n"
							 "  @arr[0] = a; \n"
							 "  @arr[1] = a; \n"
							 "  assert( arr[0] is arr[1] ); \n"
							 "  assert( arr[0] is a ); \n"
							 "} \n"
							 "handle@ func(handle@ r) { return r; } \n"
							 "void func2(handle@r) { assert( r is null ); } \n";

		asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection(TESTNAME, script);
		r = mod->Build();
		if( r < 0 )
		{
			TEST_FAILED;
			printf("%s: Failed to compile the script\n", TESTNAME);
		}

		ctx = engine->CreateContext();
		r = ExecuteString(engine, "main()", mod, ctx);
		if( r != asEXECUTION_FINISHED )
		{
			if( r == asEXECUTION_EXCEPTION )
				PrintException(ctx);

			printf("%s: Failed to execute script\n", TESTNAME);
			TEST_FAILED;
		}
		if( ctx ) ctx->Release();

		engine->Release();
	}


	// Success
	return fail;
}


} // namespace

