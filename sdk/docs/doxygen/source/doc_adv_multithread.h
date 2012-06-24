/**

\page doc_adv_multithread Multithreading

AngelScript supports multithreading, though not yet on all platforms. You can determine if multithreading 
is supported on your platform by calling the \ref asGetLibraryOptions function and checking the returned 
string for <code>"AS_NO_THREADS"</code>. If the identifier is in the returned string then multithreading is 
not supported.

Even if you don't want or can't use multithreading, you can still write applications that execute 
\ref doc_adv_concurrent "multiple scripts simultaneously". 

\section doc_adv_multithread_1 Things to think about with a multithreaded environment

 - If you plan on creating engines in multiple threads, the application should call \ref asPrepareMultithread()
   from the main thread before the first engine is created, and \ref asUnprepareMultithread() after the last engine
   has been created. This is to avoid race conditions when the engine creates the internal shared thread manager.
   If only one script engine is used, then it is not necessary to do go through these extra steps.

 - Always call \ref asThreadCleanup before terminating a thread that accesses the script engine. If this
   is not done, the memory allocated specifically for that thread will be lost until the script engine
   is freed.

 - Multiple threads may execute scripts in separate contexts. The contexts may execute scripts from the 
   same module, but if the module has global variables you need to make sure the scripts perform proper
   access control so that they do not get corrupted, if multiple threads try to update them simultaneously.

 - The engine will only allow one thread to build scripts at any one time, since this is something that 
   changes the internal state of the engine and cannot safely be done in multiple threads simultaneously.
   
 - Do not execute the \ref doc_gc "garbage collector" while other script threads are executed, as the 
   garbage collector may be traversing the reference graph while the objects are modified by the 
   other threads.

 - Resources that are shared by script modules such as registered properties and objects must be protected 
   by the application for simultaneous access, as the script engine doesn't do this automatically. A read-write
   lock is provided by the library that can be used for this. See \ref asAcquireExclusiveLock(), 
   \ref asReleaseExclusiveLock(), \ref asAcquireSharedLock(), and \ref asReleaseSharedLock().

 - Many of the add-ons are not thread safe. Either create your own or make sure you review the add-ons you 
   wish to use to guarantee thread safety.

\section doc_adv_fibers Fibers

AngelScript can be used with fibers as well. However, as fibers are not real threads you need to be careful
if multiple fibers need to execute scripts. AngelScript keeps track of active contexts by pushing and popping
the context pointers on a stack. With fibers you can easily corrupt this stack if you do not pay attention 
to where and when script executions are performed.

Try to avoid switching fibers when a script is executing, instead you should \ref asIScriptContext::Suspend "suspend" the current script 
execution and only switch to the other fiber when the context's \ref asIScriptContext::Execute "Execute()" method returns. If you really 
must switch fibers while executing a script, then make sure the second fiber initiates and concludes its own
script execution before switching back to the original fiber.



*/
