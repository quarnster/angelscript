//
// asCScriptString
//
// This class is used to pass strings between the application and the script engine.
// It is basically a container for the normal std::string, with the addition of a
// reference counter so that the script can use object handles to hold the type.
//
// Because the class is reference counted it cannot be stored locally in the
// application functions, nor be received or returned by value. Instead it should
// be manipulated through pointers or references.
//
// Note, because the internal buffer is placed at the beginning of the class
// structure it is infact possible to receive this type as a reference or pointer
// to a normal std::string where the reference counter doesn't have to be manipulated.
//

#ifndef SCRIPTSTRING_H
#define SCRIPTSTRING_H

#include <angelscript.h>
#include <string>

BEGIN_AS_NAMESPACE

class asCScriptString
{
public:
	asCScriptString();
	asCScriptString(const asCScriptString &other);
	asCScriptString(const char *s);
	asCScriptString(const std::string &s);

	void AddRef();
	void Release();

	asCScriptString &operator=(const asCScriptString &other);
	asCScriptString &operator+=(const asCScriptString &other);

	std::string buffer;

protected:
	~asCScriptString();
	int refCount;
};

// This function will determine the configuration of the engine
// and use one of the two functions below to register the string type
void RegisterScriptString(asIScriptEngine *engine);

// Call this function to register the string type
// using native calling conventions
void RegisterScriptString_Native(asIScriptEngine *engine);

// Use this one instead if native calling conventions
// are not supported on the target platform
void RegisterScriptString_Generic(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
