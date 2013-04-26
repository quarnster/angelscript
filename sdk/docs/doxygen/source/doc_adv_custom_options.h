/**

\page doc_adv_custom_options Custom options

Most applications have different needs and AngelScript is meant to be as flexible 
as possible to suit everybody. For this reason there are several options for 
customizing the script engine for the best fit.







\section doc_adv_custom_options_reg_types Registerable types

Even types that one might normally expect to be built-in to a script language, e.g.
string, array, and other container classes, are \ref doc_register_type "registered by the application". 
This is to allow the application to provide the exact implementation desired. 

Of course, should the application developer want to use a premade implementation the 
\ref doc_addon_script "add-ons provide just that".










\section doc_adv_custom_options_lang_mod Language modifications

The engine method \ref asIScriptEngine::SetEngineProperty "SetEngineProperty" permits making 
runtime choices about several different behaviours in the engine.

The following modify the script language in one way or the other:

\ref asEP_ALLOW_UNSAFE_REFERENCES
 
By turning on unsafe references you allow in-out references to be used for primitives and value types too. 
Normally this will work fine, just as it does in ordinary languages such as C++, but know that it is 
quite possible to write scripts that will cause memory invasions or crashes if the references are not properly
guarded. With this option turned on you cannot consider the scripts to be sand-boxed any longer.
 
\ref asEP_USE_CHARACTER_LITERALS, \ref asEP_ALLOW_MULTILINE_STRINGS, \ref asEP_SCRIPT_SCANNER, \ref asEP_STRING_ENCODING

These options are used to determine how strings are treated by the compiler. The details are described in \ref doc_strings.
 
\ref asEP_ALLOW_IMPLICIT_HANDLE_TYPES
 
This option is experimental. By turning it on script classes can be declared to always be treated as handles by declaring the 
class with @ before the name of the class. When this is done all variables of that type will be handles, and the assignment
operator will always perform a handle assignment.
 
\ref asEP_REQUIRE_ENUM_SCOPE
 
With this option all enum values must be prefixed with the enum type using the scope operator to qualify.
 
\ref asEP_PROPERTY_ACCESSOR_MODE
 
Some application writers do not want to use \ref doc_script_class_prop "virtual property accessors", or perhaps only 
wish to use them for registered types. By setting this engine property to 0 the use of property accessors is turned off
completely, and setting it to 1 only registered types can be allowed to use them.  
 
\ref asEP_DISALLOW_GLOBAL_VARS

The application can disable the ability to declare global variables in scripts completely. This can be useful in such cases
when the application wish to have full control of what can and cannot be stored by a script.  
 
\ref asEP_ALWAYS_IMPL_DEFAULT_CONSTRUCT

If this flag is set to true, the compiler will always provide a default constructor for clasaes even if it hasn't been implemented
by the script. Normally this option is not recommended, because if a script class provides a non-default constructor but not the 
default constructor it is most likely because it is desired that the class should always be initialized with the non-default constructor.





\section doc_adv_custom_options_engine Engine behaviours

There are also several options in \ref asIScriptEngine::SetEngineProperty "SetEngineProperty" for modifying the way the engine behaves.

\ref asEP_OPTIMIZE_BYTECODE

Normally this option is only used for testing the library, but should you find that the compilation time takes too long, then
it may be of interest to turn off the bytecode optimization pass by setting this option to false. 
 
\ref asEP_COPY_SCRIPT_SECTIONS
 
If wan't to spare some dynamic memory and the script sections passed to the engine is already stored somewhere in memory then you
can turn off this options. If you do you'll need to be careful not to modify or deallocate the script sections until module has been built.
 
\ref asEP_MAX_STACK_SIZE

The script context's stack grows dynamically as needed during executions. By default there is no limit to how large it may grow, but
if you wish to set this limit you can do with this option. The limit is given in bytes, but it is only an approximate limit. Setting the 
limit to 1MB doesn't mean the stack will grow exactly to 1MB, but you can be certain that it will not grow much beyond that.

Whenever a context attempts to grow the stack more than it is allowed, it will abort the execution and return a 'stack overflow' script 
exception.
 
\ref asEP_BUILD_WITHOUT_LINE_CUES
 
This option can be used to speed up the execution of scripts a bit. It is best described in \ref doc_finetuning_3.
 
\ref asEP_INIT_GLOBAL_VARS_AFTER_BUILD
 
By default the global variables in the script modules are initialized immediately after building the scripts, but sometimes this is not 
desired. For example, if you debug the initialization of the variables, or if you're building an offline compiler, or even if you want
to add extra validation of the variables to make sure the script doesn't declare any variables you don't want it to.

\see \ref doc_adv_precompile
 
\ref asEP_INCLUDE_JIT_INSTRUCTIONS

In order for JIT compilation to work properly it needs to have some hints in the bytecode so it can know where a switch from 
bytecode execution to JIT compiled execution shall be done. By default this option is turned off as it would just add an overhead
to the bytecode, but when using JIT compilation this overhead will be compensated by the faster execution with direct machine code.

\see \ref doc_adv_jit

\ref asEP_EXPAND_DEF_ARRAY_TO_TMPL
 
This option is to determine how the \ref asIScriptEngine::RegisterDefaultArrayType "default array type" is displayed in compiler 
messages and function declarations, e.g. int[] or array<int>. 
 
\ref asEP_AUTO_GARBAGE_COLLECT

By default AngelScript's incremental garbage collector is executed as scripts are invoked by the application. This does add 
a slight overhead to the execution, which is normally tolerable but in some applications the CPU cycles may be needed for other
things. In this case the automatic execution of the garbage collector can be turned off to hold off on the execution until the
CPU can be spared.

\see \ref doc_gc
 
\ref asEP_COMPILER_WARNINGS

Compiler warnings can be turned off or treated as errors by setting this engine property.







*/
