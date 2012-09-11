/**

\page doc_adv_precompile Pre-compiled byte code

Sometimes it may be useful to use pre-compiled bytecode instead of re-building the 
scripts every time a script is loaded. This can for example reduce load times, or 
if scripts are transferred over the net, it can reduce the transfer time for large scripts.

To save already compiled code you should call the \ref asIScriptModule::SaveByteCode "SaveByteCode" 
method on the \ref asIScriptModule, passing in a pointer to the object that implements the \ref
asIBinaryStream interface. The script module will then push all the bytecode and related data onto
this object with the \ref asIBinaryStream::Write "Write" method.

When loading the pre-compiled bytecode you should first get an instance of the \ref asIScriptModule,
just as if compiling a new script. Instead of calling AddScriptSection followed by Build you will
call the \ref asIScriptModule::LoadByteCode "LoadByteCode" method, passing it a pointer to the binary 
stream object. The script module will then pull all the bytecode and related data from that object with 
the \ref asIBinaryStream::Read "Read" method to reconstruct the script module.

Here's a very simple example of how to implement the \ref asIBinaryStream interface:

\code
class CBytecodeStream : public asIBinaryStream
{
public:
  CBytecodeStream(FILE *fp) : f(fp) {}

  void Write(const void *ptr, asUINT size) 
  {
    if( size == 0 ) return; 
    fwrite(ptr, size, 1, f); 
  }
  void Read(void *ptr, asUINT size) 
  { 
    if( size == 0 ) return; 
    fread(ptr, size, 1, f); 
  }

protected:
  FILE *f;
};
\endcode


\see \ref doc_samples_asbuild

\section doc_adv_precompile_1 Things to remember

 - All the objects, methods, properties, etc, used when compiling the bytecode must also be registered
   when loading the precompiled bytecode, otherwise the load will fail when not finding the correct entity.

 - The script engine doesn't perform validation of the pre-compiled bytecode, so the application is
   responsible for authenticating the source. If this authentication isn't performed by the application
   you'll have potential security risks in your application as the bytecode may have been manually 
   manipulated to perform otherwise illegal tasks.

 - If the application that compiles the script code is separate from the application that will execute
   them, then you may register the functions and methods with null pointers so you don't need to 
   actually implement all of them. Object properties must be registered with different offsets so that
   they can be differentiated by the bytecode loader. You should also set the engine property \ref asEP_INIT_GLOBAL_VARS_AFTER_BUILD 
   to false with \ref asIScriptEngine::SetEngineProperty, so the script engine doesn't attempt to initialize 
   the global variables after the script has been built.

 - The saved bytecode is almost fully platform independent. The following is properly handled, pointer size,
   size of registered types, offsets of registered properties, CPU endianess, etc. Some things that are not
   handled include difference in size of primitive types (e.g. bool on older Mac PPC platforms) and CPU floating point
   representation (e.g. IEEE 754 versus non-IEEE 754).

 - If the application is not going to compile scripts from source code, but only load pre-compiled scripts,
   then it may be beneficial to compile the library with the AS_NO_COMPILER define so as to reduce the size
   of the executable.

 - To reduce the size of the saved bytecode you can strip the debug information from the code. This will skip
   saving information that is not necessary for executing the scripts, such as the name of script sections,
   line numbers, and the name and type of local variables. 




*/
