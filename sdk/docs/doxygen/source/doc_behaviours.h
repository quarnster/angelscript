/**

\page doc_behaviours Type behaviours

\todo Complete this page with more examples





\section mathop Math operators

 - \ref asBEHAVE_ADD
 - \ref asBEHAVE_SUBTRACT
 - \ref asBEHAVE_MULTIPLY
 - \ref asBEHAVE_DIVIDE
 - \ref asBEHAVE_MODULO
 - \ref asBEHAVE_NEGATE
 
All of these, with exception to asBEHAVE_NEGATE, should be registered with two 
parameters using \ref asIScriptEngine::RegisterGlobalBehaviour. asBEHAVE_NEGATE should be 
registered with no parameters using \ref asIScriptEngine::RegisterObjectBehaviour.





\section compop Comparison operators

 - \ref asBEHAVE_EQUAL
 - \ref asBEHAVE_NOTEQUAL
 - \ref asBEHAVE_LESSTHAN,
 - \ref asBEHAVE_GREATERTHAN,
 - \ref asBEHAVE_LEQUAL,
 - \ref asBEHAVE_GEQUAL
 
These should be registered with two parameters using 
\ref asIScriptEngine::RegisterGlobalBehaviour. All of them should return a bool type.





\section bitop Bitwise operators

 - \ref asBEHAVE_BIT_OR
 - \ref asBEHAVE_BIT_AND
 - \ref asBEHAVE_BIT_XOR
 - \ref asBEHAVE_BIT_SLL
 - \ref asBEHAVE_BIT_SRL
 - \ref asBEHAVE_BIT_SRA
 
These should be registered with two parameters using
\ref asIScriptEngine::RegisterGlobalBehaviour.





\section assignop Assignment operators

 - \ref asBEHAVE_ASSIGNMENT
 - \ref asBEHAVE_ADD_ASSIGN
 - \ref asBEHAVE_SUB_ASSIGN
 - \ref asBEHAVE_MUL_ASSIGN
 - \ref asBEHAVE_DIV_ASSIGN
 - \ref asBEHAVE_MOD_ASSIGN
 - \ref asBEHAVE_OR_ASSIGN
 - \ref asBEHAVE_AND_ASSIGN
 - \ref asBEHAVE_XOR_ASSIGN
 - \ref asBEHAVE_SLL_ASSIGN
 - \ref asBEHAVE_SRL_ASSIGN
 - \ref asBEHAVE_SRA_ASSIGN

All of these should be registered with one parameter using \ref asIScriptEngine::RegisterObjectBehaviour.
Preferably the functions should return a reference to the object itself.

\section idxop Index operator

 - \ref asBEHAVE_INDEX
 
This behaviour should be registered with one parameter using \ref asIScriptEngine::RegisterObjectBehaviour.





\section castop Cast operators

 - \ref asBEHAVE_VALUE_CAST
 - \ref asBEHAVE_REF_CAST
 
asBEHAVE_VALUE_CAST must be registered without parameters using \ref asIScriptEngine::RegisterObjectBehaviour. 
The return type can be any type, except bool and void.

asBEHAVE_REF_CAST must be registered with one parameter using \ref asIScriptEngine::RegisterGlobalBehaviour. 
The parameter must be an object handle, as must the return type.

\code
// Example REF_CAST behaviour
B* castAtoB(A* a)
{
    B* b = dynamic_cast<B*>(a);
    if( b == 0 )
    {
        // Since the cast couldn't be made, we need to release the handle we received
        a->release();
    }
    return b;
}

// Example registration of the behaviour
r = engine->RegisterGlobalBehaviour(asBEHAVE_REF_CAST, "B@ f(A@)", asFUNCTION(castAToB), asCALL_CDECL); assert( r >= 0 );
\endcode

\see asBEHAVE_CONSTRUCT and asBEHAVE_FACTORY in \ref memmgmt as well for an alternative value cast operator.





\section memmgmt Memory management

 - \ref asBEHAVE_CONSTRUCT
 - \ref asBEHAVE_DESTRUCT
 - \ref asBEHAVE_FACTORY
 - \ref asBEHAVE_ADDREF
 - \ref asBEHAVE_RELEASE
 
These must be registered using \ref asIScriptEngine::RegisterObjectBehaviour. asBEHAVE_CONSTRUCT and 
asBEHAVE_FACTORY may take parameters for object initialization, but the others shouldn't use parameters.
 
\see \ref doc_register_type for more information on how to register types.





\section gcmgmt Garbage collection

 - \ref asBEHAVE_GETREFCOUNT
 - \ref asBEHAVE_SETGCFLAG
 - \ref asBEHAVE_GETGCFLAG
 - \ref asBEHAVE_ENUMREFS
 - \ref asBEHAVE_RELEASEREFS

These behaviours are exclusive for objects that have been registered with the flag \ref asOBJ_GC. All of them
should be registered using \ref asIScriptEngine::RegisterObjectBehaviour.

\see \ref doc_gc_object for more information on using these.

*/