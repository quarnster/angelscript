/**

\page doc_compile_lib Compile the library

In the sdk/angelscript/projects directory you'll find project files for many of the popular compilers. 
However, these project files are not always up to date with the latest version of the library. If you get 
any compiler or linker errors please make sure the project file include all the files in the 
sdk/angelscript/source directory, and that the project settings are set according to this article.

If you don't find a project file for your compiler, you can easily create your own project by adding
all the files in the sdk/angelscript/source directory, and configuring the project apropriately. If you
have a new compiler/target that hasn't been used with AngelScript before, you may need to edit the 
as_config.h file to make sure the library is compiled properly.

\see \ref doc_compile_platforms 



\section doc_compile_lib_1 Set compile time options

The code tries to contain compiler differences in as few places as possible. The header as_config.h 
was created for that purpose. There you will find some \#defines that allow different compilers 
to work. You'll probably not have to change this file, but if you're using a compiler not 
previously used and you're getting compiler errors it might be worth it to take a look at 
this file.

There are also a couple of other \#defines used in the code to alter the compilation. When 
compiling the library you might want to define ANGELSCRIPT_EXPORT so that library functions 
are exported. If you include the library source code directly in your application project you 
shouldn't have to define this flag.</p>

If AS_DEPRECATED is defined then some backwards compatibility is maintained, this can help 
you do the upgrade to the latest version a little more smoothly. There is no guarantee that 
the backwards compatibility will be maintained though so try to remove use of deprecated functions
as soon as possible.



\section doc_compile_lib_2 Linking with the library

There are four ways of compiling and linking with AngelScript in order to use it. I 
recommend linking with a static library. Note that all four ways are interchangable with 
only a small change in your code, i.e a defined flag before including the header file, and 
possibly a routine for manually loading the dll. The rest of your code should look exactly 
the same for each of the alternatives.



\subsection doc_compile_lib_2_1 1. Include library source files in project

You can take the source files for AngelScript and include them directly in your own 
project. The advantage of this is that you can be sure that the same compiler options are 
used for the library and the host applications, e.g. multi-threaded or single-threaded CRT. 
The disadvantage is that your project will be poluted with the library files.

The files that need to use the library should include the %angelscript.h header with no 
need for any special settings.

\code
// Include the library interface
#include "angelscript.h"

// ... Start using the library
\endcode



\subsection doc_compile_lib_2_2 2. Compile a static library and link into project

The most recommended way is to compile a static library that your project will link with. 
When compiling the static library you have to make sure that the correct compiler settings 
are used so that you don't get conflicts in linkage with the CRT functions. This happens 
if you for example compile the library with dynamically linked multi-threaded CRT and your
application with statically linked single-threaded CRT. (For Visual C++ you'll find these 
settings under Project -> Settings -> C/C++ -> Category: Code Generation)

To use the library you only need to include the %angelscript.h header file.

\code
// Include the library interface
#include "angelscript.h"

// ... Start using the library
\endcode



\subsection doc_compile_lib_2_3 3. Compile a dynamically loaded library with an import library

With Microsoft Visual C++ it is possible to compile a dynamically loaded library with an 
import library. The import library will then take care of the work needed to load the dll 
and bind the functions. A possible disadvantage of this method is that you are not able to 
give any user-friendly error messages in case loading the library fails.

To use the library you'll have to define ANGELSCRIPT_DLL_LIBRARY_IMPORT before including 
the %angelscript.h header file.

\code
// Include the library interface
#define ANGELSCRIPT_DLL_LIBRARY_IMPORT
#include "angelscript.h"

// ... Start using the library
\endcode




\subsection doc_compile_lib_2_4 4. Load the dynamically loaded library manually

If you want to use a dll, e.g. to share code between applications, I recommend loading the 
library manually as you can treat any failures to load or bind functions graciously.

To use manually loaded dll, you should define ANGELSCRIPT_DLL_MANUAL_IMPORT before including 
the %angelscript.h header file. This will insure that the header file doesn't declare the 
function prototypes, as you will most likely want to use these names for the function pointers.

\code
// Include the library interface
#define ANGELSCRIPT_DLL_MANUAL_IMPORT
#include "angelscript.h"

// Declare the function pointers
typedef asIScriptEngine * AS_CALL t_asCreateScriptEngine(int);
t_asCreateScriptEngine *asCreateScriptEngine = 0;

// ... Declare the rest of the functions

// Load the dll and bind the functions (error handling left out for clarity)
HMODULE dll = LoadLibrary("angelscript.dll");
asCreateScriptEngine = (t_asCreateScriptEngine*)GetProcAddress(dll, "_asCreateScriptEngine");

// ... Bind the other functions

// ... Start using the library
\endcode


\section doc_compile_platforms Considerations for specific platforms

As mentioned before, for most platforms the compilation of the library is as easy as including all 
source files and compiling them. However, on some platforms specific actions needs to be performed
to compile the library correctly.

\subsection doc_compile_win64 Windows 64bit

The MSVC compiler doesn't support inline assembler for the x86 64bit CPU family. To support this platform a
separate assembler file has been created: as_callfunc_x64_msvc_asm.asm.

To compile this file it is necessary to configure a custom build command with the following:

\code
ml64.exe /c  /nologo /Fo$(OutDir)\as_callfunc_x64_msvc_asm.obj /W3 /Zi /Ta $(InputDir)\$(InputFileName)
\endcode

\subsection doc_compile_msvc_sdk Microsoft Visual C++

While AngelScript doesn't use Microsoft's language extensions you may still face trouble compiling the library
if you disable the language extensions. This is because Microsoft's own SDK may have code that relies on the 
language extensions, e.g. in version 6.0a you might get compiler errors due to the existance of $ in the macro
definitions in the specstrings.h header from the Platform SDK. This particular problem was fixed by Microsoft 
in version 6.1 of their SDK, but there may be others so it might just easier to leave the language extensions
turned on.

\subsection doc_compile_gnuc GNUC based compilers

In order to properly intergrate with C++ without the need for wrappers AngelScript uses a lot of pointer casts.
Unfortunately it is not possible to always guarantee strict aliasing because of this, so on GNUC based compilers
it is necessary to disable compiler optimizations that assume strict aliasing.

Use the following compiler argument to disable this:

\code
-fno-strict-aliasing
\endcode

\subsection doc_compile_pocketpc Pocket PC with ARM CPU

The MSVC compiler doesn't support inline assembler for the ARM CPU, so a separate assembler file has been 
written with this code: as_callfunc_arm_msvc.asm.

In order to compile this file properly it is necessary to configure a custom build command with the following:

\code
armasm -g $(InputPath)
\endcode

\subsection doc_compile_marmalade Marmalade

Marmalade is a cross platform SDK created with mobile devices in mind. It functions by abstracting the underlying OS
with its own C runtime library even though it uses the common C++ compilers, e.g. MSVC on Windows, and GNUC on Linux and Mac.

Unfortunately Marmalade doesn't provide a specific pre-processor define that can be used to automatically detect that 
the library is being compiled for use with Marmalade. So in this case it is necessary to manually define the word 
MARMALADE in the project settings or makefile to allow AngelScript to detect this.




*/
