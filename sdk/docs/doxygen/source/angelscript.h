/*
   AngelCode Scripting Library
   Copyright (c) 2003-2008 Andreas Jonsson

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any
   purpose, including commercial applications, and to alter it and
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.

   The original version of this library can be located at:
   http://www.angelcode.com/angelscript/

   Andreas Jonsson
   andreas@angelcode.com
*/


//
// angelscript.h
//
// The script engine interface
//

//! \mainpage
//!
//! This is the reference documentation for the AngelScript application programming interface.
//!
//!  - \subpage funcs \n
//!  - \subpage class \n
//!  - \subpage const \n


//! \file angelscript.h
//! \brief The API definition for AngelScript.
//!
//! This header file describes the complete application programming interface for AngelScript.

#ifndef ANGELSCRIPT_H
#define ANGELSCRIPT_H

#include <stddef.h>

#ifdef AS_USE_NAMESPACE
 #define BEGIN_AS_NAMESPACE namespace AngelScript {
 #define END_AS_NAMESPACE }
#else
 #define BEGIN_AS_NAMESPACE
 #define END_AS_NAMESPACE
#endif

BEGIN_AS_NAMESPACE

// AngelScript version

//! The library version.
#define ANGELSCRIPT_VERSION        21200
#define ANGELSCRIPT_VERSION_MAJOR  2
#define ANGELSCRIPT_VERSION_MINOR  12
#define ANGELSCRIPT_VERSION_BUILD  0
#define ANGELSCRIPT_VERSION_STRING "2.12.0 WIP"

// Data types

class asIScriptEngine;
class asIScriptContext;
class asIScriptGeneric;
class asIScriptStruct;
class asIScriptArray;
class asIObjectType;
class asIScriptFunction;
class asIBinaryStream;

enum asEMsgType;
enum asEContextState;
enum asEExecStrFlags;
enum asEEngineProp;
enum asECallConvTypes;
enum asETypeIdFlags;
enum asEBehaviours;

//! \typedef asBYTE
//! \brief 8 bit unsigned integer

//! \typedef asWORD
//! \brief 16 bit unsigned integer

//! \typedef asDWORD
//! \brief 32 bit unsigned integer

//! \typedef asQWORD
//! \brief 64 bit unsigned integer

//! \typedef asUINT
//! \brief 32 bit unsigned integer

//! \typedef asINT64
//! \brief 64 bit integer

//! \typedef asPWORD
//! \brief Unsigned integer with the size of a pointer.

//
// asBYTE  =  8 bits
// asWORD  = 16 bits
// asDWORD = 32 bits
// asQWORD = 64 bits
// asPWORD = size of pointer
//
typedef unsigned char  asBYTE;
typedef unsigned short asWORD;
typedef unsigned int   asUINT;
typedef size_t         asPWORD;
#ifdef __LP64__
    typedef unsigned int  asDWORD;
    typedef unsigned long asQWORD;
    typedef long asINT64;
#else
    typedef unsigned long asDWORD;
  #if defined(__GNUC__) || defined(__MWERKS__)
    typedef unsigned long long asQWORD;
    typedef long long asINT64;
  #else
    typedef unsigned __int64 asQWORD;
    typedef __int64 asINT64;
  #endif
#endif

typedef void (*asFUNCTION_t)();
typedef void (*asGENFUNC_t)(asIScriptGeneric *);

//! The function signature for the custom memory allocation function
typedef void *(*asALLOCFUNC_t)(size_t);
//! The function signature for the custom memory deallocation function
typedef void (*asFREEFUNC_t)(void *);

//! \ingroup funcs
//! \brief Returns an asSFuncPtr representing the function specified by the name
#define asFUNCTION(f) asFunctionPtr(f)
//! \ingroup funcs
//! \brief Returns an asSFuncPtr representing the function specified by the name, parameter list, and return type
#define asFUNCTIONPR(f,p,r) asFunctionPtr((void (*)())((r (*)p)(f)))

#ifndef AS_NO_CLASS_METHODS

class asCUnknownClass;
typedef void (asCUnknownClass::*asMETHOD_t)();

//! \brief Represents a function or method pointer.
struct asSFuncPtr
{
	union
	{
		char dummy[24]; // largest known class method pointer
		struct {asMETHOD_t   mthd; char dummy[24-sizeof(asMETHOD_t)];} m;
		struct {asFUNCTION_t func; char dummy[24-sizeof(asFUNCTION_t)];} f;
	} ptr;
	asBYTE flag; // 1 = generic, 2 = global func, 3 = method
};

//! \ingroup funcs
//! \brief Returns an asSFuncPtr representing the class method specified by class and method name.
#define asMETHOD(c,m) asSMethodPtr<sizeof(void (c::*)())>::Convert((void (c::*)())(&c::m))
//! \ingroup funcs
//! \brief Returns an asSFuncPtr representing the class method specified by class, method name, parameter list, return type.
#define asMETHODPR(c,m,p,r) asSMethodPtr<sizeof(void (c::*)())>::Convert((r (c::*)p)(&c::m))

#else // Class methods are disabled

struct asSFuncPtr
{
	union
	{
		char dummy[24]; // largest known class method pointer
		struct {asFUNCTION_t func; char dummy[24-sizeof(asFUNCTION_t)];} f;
	} ptr;
	asBYTE flag; // 1 = generic, 2 = global func
};

#endif

//! \brief Represents a compiler message
struct asSMessageInfo
{
	//! The script section where the message is raised
	const char *section;
	//! The row number
	int         row;
	//! The column
	int         col;
	//! The type of message
	asEMsgType  type;
	//! The message text
	const char *message;
};

#ifdef AS_C_INTERFACE
typedef void (*asBINARYREADFUNC_t)(void *ptr, asUINT size, void *param);
typedef void (*asBINARYWRITEFUNC_t)(const void *ptr, asUINT size, void *param);
#endif

// API functions

// ANGELSCRIPT_EXPORT is defined when compiling the dll or lib
// ANGELSCRIPT_DLL_LIBRARY_IMPORT is defined when dynamically linking to the
// dll through the link lib automatically generated by MSVC++
// ANGELSCRIPT_DLL_MANUAL_IMPORT is defined when manually loading the dll
// Don't define anything when linking statically to the lib

//! \def AS_API
//! \brief A define that specifies how the function should be imported

#ifdef WIN32
  #ifdef ANGELSCRIPT_EXPORT
    #define AS_API __declspec(dllexport)
  #elif defined ANGELSCRIPT_DLL_LIBRARY_IMPORT
    #define AS_API __declspec(dllimport)
  #else // statically linked library
    #define AS_API
  #endif
#else
  #define AS_API
#endif

#ifndef ANGELSCRIPT_DLL_MANUAL_IMPORT
extern "C"
{
    //! \defgroup funcs Functions
    //!@{

	// Engine
	//! \brief Creates the script engine.
    //!
    //! \param[in] version The library version. Should always be \ref ANGELSCRIPT_VERSION.
    //! \return A pointer to the script engine interface.
    //!
    //! Call this function to create a new script engine. When you're done with the
    //! script engine, i.e. after you've executed all your scripts, you should call
    //! \ref asIScriptEngine::Release "Release" on the pointer to free the engine object.
	AS_API asIScriptEngine * asCreateScriptEngine(asDWORD version);
	//! \brief Returns the version of the compiled library.
    //!
    //! \return A null terminated string with the library version.
    //!
    //! The returned string can be used for presenting the library version in a log file, or in the GUI.
	AS_API const char * asGetLibraryVersion();
	//! \brief Returns the options used to compile the library.
    //!
    //! \return A null terminated string with indicators that identify the options
    //!         used to compile the script library.
    //!
    //! This can be used to identify at run-time different ways to configure the engine.
    //! For example, if the returned string contain the identifier AS_MAX_PORTABILITY then
    //! functions and methods must be registered with the \ref asCALL_GENERIC calling convention.
	AS_API const char * asGetLibraryOptions();

	// Context
	//! \brief Returns the currently active context.
    //!
    //! \return A pointer to the currently executing context, or null if no context is executing.
    //!
    //! This function is most useful for registered functions, as it will allow them to obtain
    //! a pointer to the context that is calling the function, and through that get the engine,
    //! or custom user data.
    //!
    //! If the script library is compiled with multithread support, this function will return
    //! the context that is currently active in the thread that is being executed. It will thus
    //! work even if there are multiple threads executing scripts at the same time.
	AS_API asIScriptContext * asGetActiveContext();

	// Thread support
	//! \brief Cleans up memory allocated for the current thread.
	//!
    //! \return A negative value on error.
    //! \retval asCONTEXT_ACTIVE A context is still active.
    //!
    //! Call this method before terminating a thread that has
	//! accessed the engine to clean up memory allocated for that thread.
	//!
	//! It's not necessary to call this if only a single thread accesses the engine.
	AS_API int asThreadCleanup();

	// Memory management
	//! \brief Set the memory management functions that AngelScript should use.
	//!
    //! \param[in] allocFunc The function that will be used to allocate memory.
    //! \param[in] freeFunc The function that will be used to free the memory.
    //! \return A negative value on error.
    //!
    //! Call this method to register the global memory allocation and deallocation
	//! functions that AngelScript should use for memory management. This function
	//! Should be called before \ref asCreateScriptEngine.
	//!
	//! If not called, AngelScript will use the malloc and free functions from the
	//! standard C library.
	AS_API int asSetGlobalMemoryFunctions(asALLOCFUNC_t allocFunc, asFREEFUNC_t freeFunc);

	//! \brief Remove previously registered memory management functions.
    //!
    //! \return A negative value on error.
    //!
    //! Call this method to restore the default memory management functions.
	AS_API int asResetGlobalMemoryFunctions();

    //!@}

#ifdef AS_C_INTERFACE
	AS_API int               asEngine_AddRef(asIScriptEngine *e);
	AS_API int               asEngine_Release(asIScriptEngine *e);
	AS_API int               asEngine_SetEngineProperty(asIScriptEngine *e, asDWORD property, asPWORD value);
	AS_API asPWORD           asEngine_GetEngineProperty(asIScriptEngine *e, asDWORD property);
	AS_API int               asEngine_SetMessageCallback(asIScriptEngine *e, asFUNCTION_t callback, void *obj, asDWORD callConv);
	AS_API int               asEngine_ClearMessageCallback(asIScriptEngine *e);
	AS_API int               asEngine_RegisterObjectType(asIScriptEngine *e, const char *name, int byteSize, asDWORD flags);
	AS_API int               asEngine_RegisterObjectProperty(asIScriptEngine *e, const char *obj, const char *declaration, int byteOffset);
	AS_API int               asEngine_RegisterObjectMethod(asIScriptEngine *e, const char *obj, const char *declaration, asFUNCTION_t funcPointer, asDWORD callConv);
	AS_API int               asEngine_RegisterObjectBehaviour(asIScriptEngine *e, const char *datatype, asEBehaviours behaviour, const char *declaration, asFUNCTION_t funcPointer, asDWORD callConv);
	AS_API int               asEngine_RegisterGlobalProperty(asIScriptEngine *e, const char *declaration, void *pointer);
	AS_API int               asEngine_RegisterGlobalFunction(asIScriptEngine *e, const char *declaration, asFUNCTION_t funcPointer, asDWORD callConv);
	AS_API int               asEngine_RegisterGlobalBehaviour(asIScriptEngine *e, asDWORD behaviour, const char *declaration, asFUNCTION_t funcPointer, asDWORD callConv);
	AS_API int               asEngine_RegisterInterface(asIScriptEngine *e, const char *name);
	AS_API int               asEngine_RegisterInterfaceMethod(asIScriptEngine *e, const char *intf, const char *declaration);
	AS_API int               asEngine_RegisterEnum(asIScriptEngine *e, const char *type);
	AS_API int               asEngine_RegisterEnumValue(asIScriptEngine *e, const char *type, const char *name, int value);
	AS_API int               asEngine_RegisterTypedef(asIScriptEngine *e, const char *type, const char *decl);
	AS_API int               asEngine_RegisterStringFactory(asIScriptEngine *e, const char *datatype, asFUNCTION_t factoryFunc, asDWORD callConv);
	AS_API int               asEngine_BeginConfigGroup(asIScriptEngine *e, const char *groupName);
	AS_API int               asEngine_EndConfigGroup(asIScriptEngine *e);
	AS_API int               asEngine_RemoveConfigGroup(asIScriptEngine *e, const char *groupName);
	AS_API int               asEngine_SetConfigGroupModuleAccess(asIScriptEngine *e, const char *groupName, const char *module, bool hasAccess);
	AS_API int               asEngine_AddScriptSection(asIScriptEngine *e, const char *module, const char *name, const char *code, int codeLength, int lineOffset = 0);
	AS_API int               asEngine_Build(asIScriptEngine *e, const char *module);
	AS_API int               asEngine_Discard(asIScriptEngine *e, const char *module);
	AS_API int               asEngine_GetFunctionCount(asIScriptEngine *e, const char *module);
	AS_API int               asEngine_GetFunctionIDByIndex(asIScriptEngine *e, const char *module, int index);
	AS_API int               asEngine_GetFunctionIDByName(asIScriptEngine *e, const char *module, const char *name);
	AS_API int               asEngine_GetFunctionIDByDecl(asIScriptEngine *e, const char *module, const char *decl);
	AS_API const char *      asEngine_GetFunctionDeclaration(asIScriptEngine *e, int funcID, int *length = 0);
	AS_API const char *      asEngine_GetFunctionName(asIScriptEngine *e, int funcID, int *length = 0);
	AS_API const char *      asEngine_GetFunctionModule(asIScriptEngine *e, int funcID, int *length = 0);
	AS_API const char *      asEngine_GetFunctionSection(asIScriptEngine *e, int funcID, int *length = 0);
	AS_API const asIScriptFunction *asEngine_GetFunctionDescriptorByIndex(asIScriptEngine *e, const char *module, int index);
	AS_API int               asEngine_GetMethodCount(asIScriptEngine *e, int typeId);
	AS_API int               asEngine_GetMethodIDByIndex(asIScriptEngine *e, int typeId, int index);
	AS_API int               asEngine_GetMethodIDByName(asIScriptEngine *e, int typeId, const char *name);
	AS_API int               asEngine_GetMethodIDByDecl(asIScriptEngine *e, int typeId, const char *decl);
	AS_API const asIScriptFunction *asEngine_GetMethodDescriptorByIndex(asIScriptEngine *e, int typeId, int index);
	AS_API int               asEngine_GetGlobalVarCount(asIScriptEngine *e, const char *module);
	AS_API int               asEngine_GetGlobalVarIDByIndex(asIScriptEngine *e, const char *module, int index);
	AS_API int               asEngine_GetGlobalVarIDByName(asIScriptEngine *e, const char *module, const char *name);
	AS_API int               asEngine_GetGlobalVarIDByDecl(asIScriptEngine *e, const char *module, const char *decl);
	AS_API const char *      asEngine_GetGlobalVarDeclaration(asIScriptEngine *e, int gvarID, int *length = 0);
	AS_API const char *      asEngine_GetGlobalVarName(asIScriptEngine *e, int gvarID, int *length = 0);
	AS_API void *            asEngine_GetGlobalVarPointer(asIScriptEngine *e, int gvarID);
	AS_API int               asEngine_GetImportedFunctionCount(asIScriptEngine *e, const char *module);
	AS_API int               asEngine_GetImportedFunctionIndexByDecl(asIScriptEngine *e, const char *module, const char *decl);
	AS_API const char *      asEngine_GetImportedFunctionDeclaration(asIScriptEngine *e, const char *module, int importIndex, int *length = 0);
	AS_API const char *      asEngine_GetImportedFunctionSourceModule(asIScriptEngine *e, const char *module, int importIndex, int *length = 0);
	AS_API int               asEngine_BindImportedFunction(asIScriptEngine *e, const char *module, int importIndex, int funcID);
	AS_API int               asEngine_UnbindImportedFunction(asIScriptEngine *e, const char *module, int importIndex);
	AS_API int               asEngine_BindAllImportedFunctions(asIScriptEngine *e, const char *module);
	AS_API int               asEngine_UnbindAllImportedFunctions(asIScriptEngine *e, const char *module);
	AS_API int               asEngine_GetTypeIdByDecl(asIScriptEngine *e, const char *module, const char *decl);
	AS_API const char *      asEngine_GetTypeDeclaration(asIScriptEngine *e, int typeId, int *length = 0);
	AS_API int               asEngine_GetSizeOfPrimitiveType(asIScriptEngine *e, int typeId);
	AS_API asIObjectType *   asEngine_GetObjectTypeById(asIScriptEngine *e, int typeId);
	AS_API asIObjectType *   asEngine_GetObjectTypeByIndex(asIScriptEngine *e, asUINT index);
	AS_API int               asEngine_GetObjectTypeCount(asIScriptEngine *e);
	AS_API int               asEngine_SetDefaultContextStackSize(asIScriptEngine *e, asUINT initial, asUINT maximum);
	AS_API asIScriptContext *asEngine_CreateContext(asIScriptEngine *e);
	AS_API void *            asEngine_CreateScriptObject(asIScriptEngine *e, int typeId);
	AS_API void *            asEngine_CreateScriptObjectCopy(asIScriptEngine *e, void *obj, int typeId);
	AS_API void              asEngine_CopyScriptObject(asIScriptEngine *e, void *dstObj, void *srcObj, int typeId);
	AS_API void              asEngine_ReleaseScriptObject(asIScriptEngine *e, void *obj, int typeId);
	AS_API void              asEngine_AddRefScriptObject(asIScriptEngine *e, void *obj, int typeId);
	AS_API bool              asEngine_IsHandleCompatibleWithObject(asIScriptEngine *e, void *obj, int objTypeId, int handleTypeId);
	AS_API int               asEngine_CompareScriptObjects(asIScriptEngine *e, bool &result, int behaviour, void *leftObj, void *rightObj, int typeId);
	AS_API int               asEngine_ExecuteString(asIScriptEngine *e, const char *module, const char *script, asIScriptContext **ctx, asDWORD flags);
	AS_API int               asEngine_GarbageCollect(asIScriptEngine *e, bool doFullCycle = true);
	AS_API int               asEngine_GetObjectsInGarbageCollectorCount(asIScriptEngine *e);
	AS_API void              asEngine_NotifyGarbageCollectorOfNewObject(asIScriptEngine *e, void *obj, int typeId);
	AS_API void              asEngine_GCEnumCallback(asIScriptEngine *e, void *obj);
	AS_API int               asEngine_SaveByteCode(asIScriptEngine *e, const char *module, asBINARYWRITEFUNC_t outFunc, void *outParam);
	AS_API int               asEngine_LoadByteCode(asIScriptEngine *e, const char *module, asBINARYREADFUNC_t inFunc, void *inParam);

	AS_API int              asContext_AddRef(asIScriptContext *c);
	AS_API int              asContext_Release(asIScriptContext *c);
	AS_API asIScriptEngine *asContext_GetEngine(asIScriptContext *c);
	AS_API int              asContext_GetState(asIScriptContext *c);
	AS_API int              asContext_Prepare(asIScriptContext *c, int funcID);
	AS_API int              asContext_SetArgByte(asIScriptContext *c, asUINT arg, asBYTE value);
	AS_API int              asContext_SetArgWord(asIScriptContext *c, asUINT arg, asWORD value);
	AS_API int              asContext_SetArgDWord(asIScriptContext *c, asUINT arg, asDWORD value);
	AS_API int              asContext_SetArgQWord(asIScriptContext *c, asUINT arg, asQWORD value);
	AS_API int              asContext_SetArgFloat(asIScriptContext *c, asUINT arg, float value);
	AS_API int              asContext_SetArgDouble(asIScriptContext *c, asUINT arg, double value);
	AS_API int              asContext_SetArgAddress(asIScriptContext *c, asUINT arg, void *addr);
	AS_API int              asContext_SetArgObject(asIScriptContext *c, asUINT arg, void *obj);
	AS_API void *           asContext_GetArgPointer(asIScriptContext *c, asUINT arg);
	AS_API int              asContext_SetObject(asIScriptContext *c, void *obj);
	AS_API asBYTE           asContext_GetReturnByte(asIScriptContext *c);
	AS_API asWORD           asContext_GetReturnWord(asIScriptContext *c);
	AS_API asDWORD          asContext_GetReturnDWord(asIScriptContext *c);
	AS_API asQWORD          asContext_GetReturnQWord(asIScriptContext *c);
	AS_API float            asContext_GetReturnFloat(asIScriptContext *c);
	AS_API double           asContext_GetReturnDouble(asIScriptContext *c);
	AS_API void *           asContext_GetReturnAddress(asIScriptContext *c);
	AS_API void *           asContext_GetReturnObject(asIScriptContext *c);
	AS_API void *           asContext_GetReturnPointer(asIScriptContext *c);
	AS_API int              asContext_Execute(asIScriptContext *c);
	AS_API int              asContext_Abort(asIScriptContext *c);
	AS_API int              asContext_Suspend(asIScriptContext *c);
	AS_API int              asContext_GetCurrentLineNumber(asIScriptContext *c, int *column = 0);
	AS_API int              asContext_GetCurrentFunction(asIScriptContext *c);
	AS_API int              asContext_SetException(asIScriptContext *c, const char *string);
	AS_API int              asContext_GetExceptionLineNumber(asIScriptContext *c, int *column = 0);
	AS_API int              asContext_GetExceptionFunction(asIScriptContext *c);
	AS_API const char *     asContext_GetExceptionString(asIScriptContext *c, int *length = 0);
	AS_API int              asContext_SetLineCallback(asIScriptContext *c, asSFuncPtr callback, void *obj, int callConv);
	AS_API void             asContext_ClearLineCallback(asIScriptContext *c);
	AS_API int              asContext_SetExceptionCallback(asIScriptContext *c, asSFuncPtr callback, void *obj, int callConv);
	AS_API void             asContext_ClearExceptionCallback(asIScriptContext *c);
	AS_API int              asContext_GetCallstackSize(asIScriptContext *c);
	AS_API int              asContext_GetCallstackFunction(asIScriptContext *c, int index);
	AS_API int              asContext_GetCallstackLineNumber(asIScriptContext *c, int index, int *column = 0);
	AS_API int              asContext_GetVarCount(asIScriptContext *c, int stackLevel = 0);
	AS_API const char *     asContext_GetVarName(asIScriptContext *c, int varIndex, int *length = 0, int stackLevel = 0);
	AS_API const char *     asContext_GetVarDeclaration(asIScriptContext *c, int varIndex, int *length = 0, int stackLevel = 0);
	AS_API int              asContext_GetVarTypeId(asIScriptContext *c, int varIndex, int stackLevel = -1);
	AS_API void *           asContext_GetVarPointer(asIScriptContext *c, int varIndex, int stackLevel = 0);
	AS_API int              asContext_GetThisTypeId(asIScriptContext *c, int stackLevel = -1);
	AS_API void *           asContext_GetThisPointer(asIScriptContext *c, int stackLevel = -1);
	AS_API void *           asContext_SetUserData(asIScriptContext *c, void *data);
	AS_API void *           asContext_GetUserData(asIScriptContext *c);

	AS_API asIScriptEngine *asGeneric_GetEngine(asIScriptGeneric *g);
	AS_API int              asGeneric_GetFunctionId(asIScriptGeneric *g);
	AS_API void *           asGeneric_GetObject(asIScriptGeneric *g);
	AS_API int              asGeneric_GetObjectTypeId(asIScriptGeneric *g);
	AS_API int              asGeneric_GetArgCount(asIScriptGeneric *g);
	AS_API asBYTE           asGeneric_GetArgByte(asIScriptGeneric *g, asUINT arg);
	AS_API asWORD           asGeneric_GetArgWord(asIScriptGeneric *g, asUINT arg);
	AS_API asDWORD          asGeneric_GetArgDWord(asIScriptGeneric *g, asUINT arg);
	AS_API asQWORD          asGeneric_GetArgQWord(asIScriptGeneric *g, asUINT arg);
	AS_API float            asGeneric_GetArgFloat(asIScriptGeneric *g, asUINT arg);
	AS_API double           asGeneric_GetArgDouble(asIScriptGeneric *g, asUINT arg);
	AS_API void *           asGeneric_GetArgAddress(asIScriptGeneric *g, asUINT arg);
	AS_API void *           asGeneric_GetArgObject(asIScriptGeneric *g, asUINT arg);
	AS_API void *           asGeneric_GetArgPointer(asIScriptGeneric *g, asUINT arg);
	AS_API int              asGeneric_GetArgTypeId(asIScriptGeneric *g, asUINT arg);
	AS_API int              asGeneric_SetReturnByte(asIScriptGeneric *g, asBYTE val);
	AS_API int              asGeneric_SetReturnWord(asIScriptGeneric *g, asWORD val);
	AS_API int              asGeneric_SetReturnDWord(asIScriptGeneric *g, asDWORD val);
	AS_API int              asGeneric_SetReturnQWord(asIScriptGeneric *g, asQWORD val);
	AS_API int              asGeneric_SetReturnFloat(asIScriptGeneric *g, float val);
	AS_API int              asGeneric_SetReturnDouble(asIScriptGeneric *g, double val);
	AS_API int              asGeneric_SetReturnAddress(asIScriptGeneric *g, void *addr);
	AS_API int              asGeneric_SetReturnObject(asIScriptGeneric *g, void *obj);
	AS_API void *           asGeneric_GetReturnPointer(asIScriptGeneric *g);
	AS_API int              asGeneric_GetReturnTypeId(asIScriptGeneric *g);

	AS_API int            asStruct_AddRef(asIScriptStruct *s);
	AS_API int            asStruct_Release(asIScriptStruct *s);
	AS_API int            asStruct_GetStructTypeId(asIScriptStruct *s);
	AS_API asIObjectType *asStruct_GetObjectType(asIScriptStruct *s);
	AS_API int            asStruct_GetPropertyCount(asIScriptStruct *s);
	AS_API int            asStruct_GetPropertyTypeId(asIScriptStruct *s, asUINT prop);
	AS_API const char *   asStruct_GetPropertyName(asIScriptStruct *s, asUINT prop);
	AS_API void *         asStruct_GetPropertyPointer(asIScriptStruct *s, asUINT prop);
	AS_API int            asStruct_CopyFrom(asIScriptStruct *s, asIScriptStruct *other);

	AS_API int    asArray_AddRef(asIScriptArray *a);
	AS_API int    asArray_Release(asIScriptArray *a);
	AS_API int    asArray_GetArrayTypeId(asIScriptArray *a);
	AS_API int    asArray_GetElementTypeId(asIScriptArray *a);
	AS_API asUINT asArray_GetElementCount(asIScriptArray *a);
	AS_API void * asArray_GetElementPointer(asIScriptArray *a, asUINT index);
	AS_API void   asArray_Resize(asIScriptArray *a, asUINT size);
	AS_API int    asArray_CopyFrom(asIScriptArray *a, asIScriptArray *other);

	AS_API asIScriptEngine *    asObjectType_GetEngine(const asIObjectType *o);
	AS_API const char *         asObjectType_GetName(const asIObjectType *o);
	AS_API const asIObjectType *asObjectType_GetSubType(const asIObjectType *o);
	AS_API int                  asObjectType_GetInterfaceCount(const asIObjectType *o);
	AS_API const asIObjectType *asObjectType_GetInterface(const asIObjectType *o, asUINT index);
	AS_API bool                 asObjectType_IsInterface(const asIObjectType *o);

	AS_API const char          *asScriptFunction_GetModuleName(const asIScriptFunction *f);
	AS_API const asIObjectType *asScriptFunction_GetObjectType(const asIScriptFunction *f);
	AS_API const char          *asScriptFunction_GetObjectName(const asIScriptFunction *f);
	AS_API const char          *asScriptFunction_GetFunctionName(const asIScriptFunction *f);
	AS_API bool                 asScriptFunction_IsClassMethod(const asIScriptFunction *f);
	AS_API bool                 asScriptFunction_IsInterfaceMethod(const asIScriptFunction *f);

#endif // AS_C_INTERFACE
}
#endif // ANGELSCRIPT_DLL_MANUAL_IMPORT

// Interface declarations

//! \defgroup class Interfaces
//!@{

//! \brief The engine interface
class asIScriptEngine
{
public:
	// Memory management
	//! \brief Increase reference counter.
    //!
    //! \return The number of references to this object.
    //!
    //! Call this method when storing an additional reference to the object.
    //! Remember that the first reference that is received from \ref asCreateScriptEngine
    //! is already accounted for.
	virtual int AddRef() = 0;
	//! \brief Decrease reference counter.
    //!
    //! \return The number of references to this object.
    //!
    //! Call this method when you will no longer use the references that you own.
	virtual int Release() = 0;

	// Engine configuration
	//! \brief Dynamically change some engine properties.
    //!
    //! \param[in] property One of the \ref asEEngineProp values.
    //! \param[in] value The new value of the property.
    //! \return Negative value on error.
    //! \retval asINVALID_ARG Invalid property.
    //!
    //! With this method you can change the way the script engine works in some regards.
	virtual int     SetEngineProperty(asEEngineProp property, asPWORD value) = 0;
	//! \brief Retrieve current engine property settings.
    //!
    //! \param[in] property One of the \ref asEEngineProp values.
    //! \return The value of the property, or 0 if it is an invalid property.
    //!
    //! Calling this method lets you determine the current value of the engine properties.
	virtual asPWORD GetEngineProperty(asEEngineProp property) = 0;

	//! \brief Sets a message callback that will receive compiler messages.
	//!
	//! \param[in] callback A function or class method pointer.
	//! \param[in] obj      The object for methods, or an optional parameter for functions.
	//! \param[in] callConv The calling convention.
	//! \return A negative value for an error.
	//! \retval asINVALID_ARG   One of the arguments is incorrect, e.g. obj is null for a class method.
	//! \retval asNOT_SUPPORTED The arguments are not supported, e.g. asCALL_GENERIC.
	//!
	//! This method sets the callback routine that will receive compiler messages.
	//! The callback routine can be either a class method, e.g:
	//! \code
	//! void MyClass::MessageCallback(const asSMessageInfo *msg);
	//! r = engine->SetMessageCallback(asMETHOD(MyClass,MessageCallback), &obj, asCALL_THISCALL);
	//! \endcode
	//! or a global function, e.g:
	//! \code
	//! void MessageCallback(const asSMessageInfo *msg, void *param);
	//! r = engine->SetMessageCallback(asFUNCTION(MessageCallback), param, asCALL_CDECL);
	//! \endcode
	//! It is recommended to register the message callback routine right after creating the engine,
	//! as some of the registration functions can provide useful information to better explain errors.
	virtual int SetMessageCallback(const asSFuncPtr &callback, void *obj, asDWORD callConv) = 0;

	//! \brief Clears the registered message callback routine.
    //!
    //! \return A negative value on error.
    //!
    //! Call this method to remove the message callback.
	virtual int ClearMessageCallback() = 0;

	//! \brief Registers a new object type.
    //!
    //! \param[in] name The name of the type.
    //! \param[in] byteSize The size of the type in bytes. Only necessary for value types.
    //! \param[in] flags One or more of the asEObjTypeFlags.
    //! \return A negative value on error.
    //! \retval asINVALID_ARG The flags are invalid.
    //! \retval asINVALID_NAME The name is invalid.
    //! \retval asALREADY_REGISTERED Another type of the same name already exists.
    //! \retval asNAME_TAKEN The name conflicts with other symbol names.
    //! \retval asLOWER_ARRAY_DIMENSION_NOT_REGISTERED When registering an array type the array element must be a primitive or a registered type.
    //! \retval asINVALID_TYPE The array type was not properly formed.
    //! \retval asNOT_SUPPORTED The array type is not supported, or already in use preventing it from being overloaded.
    //!
    //! Use this method to register new types that should be available to the scripts.
    //! Reference types, which have their memory managed by the application, should be registered with \ref asOBJ_REF.
    //! Value types, which have their memory managed by the engine, should be registered with \ref asOBJ_VALUE.
    //!
    //! \see \ref doc_register_type
	virtual int RegisterObjectType(const char *name, int byteSize, asDWORD flags) = 0;
	//! \brief Registers a property for the object type.
    //!
    //! \param[in] obj The name of the type.
    //! \param[in] declaration The property declaration in script syntax.
    //! \param[in] byteOffset The offset into the memory block where this property is found.
    //! \return A negative value on error.
    //! \retval asWRONG_CONFIG_GROUP The object type was registered in a different configuration group.
    //! \retval asINVALID_OBJECT The \a obj does not specify an object type.
    //! \retval asINVALID_TYPE The \a obj parameter has invalid syntax.
    //! \retval asINVALID_DECLARATION The \a declaration is invalid.
    //! \retval asNAME_TAKEN The name conflicts with other members.
    //!
    //! Use this method to register a member property of a class. The property must
    //! be local to the object, i.e. not a global variable or a static member. The
    //! easiest way to get the offset of the property is to use the offsetof macro
    //! from stddef.h.
    //!
    //! \code
    //! struct MyType {float prop;};
    //! r = engine->RegisterObjectProperty("MyType", "float prop", offsetof(MyType, prop)));
    //! \endcode
	virtual int RegisterObjectProperty(const char *obj, const char *declaration, int byteOffset) = 0;
	//! \brief Registers a method for the object type.
    //!
    //! \param[in] obj The name of the type.
    //! \param[in] declaration The declaration of the method in script syntax.
    //! \param[in] funcPointer The method or function pointer.
    //! \param[in] callConv The calling convention for the method or function.
    //! \return A negative value on error, or the function id if successful.
    //! \retval asWRONG_CONFIG_GROUP The object type was registered in a different configuration group.
    //! \retval asNOT_SUPPORTED The calling convention is not supported.
    //! \retval asINVALID_TYPE The \a obj parameter is not a valid object name.
    //! \retval asINVALID_DECLARATION The \a declaration is invalid.
    //! \retval asNAME_TAKEN The name conflicts with other members.
    //! \retval asWRONG_CALLING_CONV The function's calling convention isn't compatible with \a callConv.
    //!
    //! Use this method to register a member method for the type. The method
    //! that is registered may be an actual class method, or a global function
    //! that takes the object pointer as either the first or last parameter. Or
    //! it may be a global function implemented with the generic calling convention.
    //!
    //! \see \ref doc_register_func
	virtual int RegisterObjectMethod(const char *obj, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv) = 0;
	//! \brief Registers a behaviour for the object type.
    //!
    //! \param[in] obj The name of the type.
    //! \param[in] behaviour One of the object behaviours from \ref asEBehaviours.
    //! \param[in] declaration The declaration of the method in script syntax.
    //! \param[in] funcPointer The method or function pointer.
    //! \param[in] callConv The calling convention for the method or function.
    //! \return A negative value on error, or the function id is successful.
    //! \retval asWRONG_CONFIG_GROUP The object type was registered in a different configuration group.
    //! \retval asINVALID_ARG \a obj is not set, or a global behaviour is given in \a behaviour.
    //! \retval asWRONG_CALLING_CONV The function's calling convention isn't compatible with \a callConv.
    //! \retval asNOT_SUPPORTED The calling convention or the behaviour signature is not supported.
    //! \retval asINVALID_TYPE The \a obj parameter is not a valid object name.
    //! \retval asINVALID_DECLARATION The \a declaration is invalid.
    //! \retval asILLEGAL_BEHAVIOUR_FOR_TYPE The \a behaviour is not allowed for this type.
    //! \retval asALREADY_REGISTERED The behaviour is already registered with the same signature.
    //!
    //! Use this method to register behaviour functions that will be called by
    //! the virtual machine to perform certain operations, such as memory management,
    //! math operations, comparisons, etc.
    //!
    //! \see \ref doc_register_func, \ref doc_behaviours
	virtual int RegisterObjectBehaviour(const char *obj, asEBehaviours behaviour, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv) = 0;

	//! \brief Registers a global property.
    //!
    //! \param[in] declaration The declaration of the global property in script syntax.
    //! \param[in] pointer The address of the property that will be used to access the property value.
    //! \return A negative value on error.
    //!
    //! Use this method to register a global property that the scripts will be
    //! able to access as global variables. The property may optionally be registered
    //! as const, if the scripts shouldn't be allowed to modify it.
    //!
    //! When registering the property, the application must pass the address to
    //! the actual value. The application must also make sure that this address
    //! remains valid throughout the life time of this registration, i.e. until
    //! the engine is released or the dynamic configuration group is removed.
    //!
    //! \todo List error codes
	virtual int RegisterGlobalProperty(const char *declaration, void *pointer) = 0;
	//! \brief Registers a global function.
    //!
    //! \param[in] declaration The declaration of the global function in script syntax.
    //! \param[in] funcPointer The function pointer.
    //! \param[in] callConv The calling convention for the function.
    //! \return A negative value on error.
    //!
    //! \todo Describe method, List error codes
    //! \see \ref doc_register_func
	virtual int RegisterGlobalFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv) = 0;
	//! \brief Registers a global behaviour, e.g. operators.
    //!
    //! \param[in] behaviour The global behaviour.
    //! \param[in] declaration The declaration of the behaviour function in script syntax.
    //! \param[in] funcPointer The function pointer.
    //! \param[in] callConv The calling convention for the function.
    //! \return A negative value on error.
    //!
    //! \todo Describe method, List error codes
    //! \see \ref doc_register_func
	virtual int RegisterGlobalBehaviour(asDWORD behaviour, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv) = 0;

	//! \brief Registers an interface.
    //!
    //! \param[in] name The name of the interface.
    //! \return A negative value on error.
    //!
    //! \todo Describe method, List error codes
	virtual int RegisterInterface(const char *name) = 0;
	//! \brief Registers an interface method.
    //!
    //! \param[in] intf The name of the interface.
    //! \param[in] declaration The method declaration.
    //! \return A negative value on error.
    //!
    //! \todo List the error codes, Describe method
	virtual int RegisterInterfaceMethod(const char *intf, const char *declaration) = 0;

	//! \brief Registers an enum type.
    //!
    //! \param[in] type The name of the enum type.
    //! \return A negative value on error.
    //!
    //! \todo Describe method, List error codes
	virtual int RegisterEnum(const char *type) = 0;
	//! \brief Registers an enum value.
    //!
    //! \param[in] type The name of the enum type.
    //! \param[in] name The name of the enum value.
    //! \param[in] value The integer value of the enum value.
    //! \return A negative value on error.
    //!
    //! \todo Describe method, List error codes
	virtual int RegisterEnumValue(const char *type, const char *name, int value) = 0;

	//! \brief Registers a typedef.
    //!
    //! \param[in] type The name of the new typedef
    //! \param[in] decl The datatype that the typedef represents
    //! \return A negative value on error.
    //!
    //! \todo Describe method, List error codes
	virtual int RegisterTypedef(const char *type, const char *decl) = 0;

	//! \brief Registers the string factory.
    //!
    //! \param[in] datatype The datatype that the string factory returns
    //! \param[in] factoryFunc The pointer to the factory function
    //! \param[in] callConv The calling convention of the factory function
    //! \return A negative value on error.
    //!
    //! \todo Describe method, List error codes
	virtual int RegisterStringFactory(const char *datatype, const asSFuncPtr &factoryFunc, asDWORD callConv) = 0;

	//! \brief Starts a new dynamic configuration group.
    //!
    //! \param[in] groupName The name of the configuration group
    //! \return A negative value on error
    //!
    //! \todo Describe method, List error codes
	virtual int BeginConfigGroup(const char *groupName) = 0;
	//! \brief Ends the configuration group.
    //!
    //! \return A negative value on error
    //!
    //! \todo Describe method, List error codes
	virtual int EndConfigGroup() = 0;
	//! \brief Removes a previously registered configuration group.
    //!
    //! \param[in] groupName The name of the configuration group
    //! \return A negative value on error
    //!
    //! \todo Describe method, List error codes
	virtual int RemoveConfigGroup(const char *groupName) = 0;
	//! \brief Tell AngelScript which modules have access to which configuration groups.
    //!
    //! \param[in] groupName The name of the configuration group
    //! \param[in] module The module name
    //! \param[in] hasAccess Whether the module has access or not to the group members
    //! \return A negative value on error
    //!
    //! \todo Describe method, List error codes
	virtual int SetConfigGroupModuleAccess(const char *groupName, const char *module, bool hasAccess) = 0;

	// Script modules
	//! \brief Add a script section for the next build.
    //!
    //! \param[in] module The name of the module
    //! \param[in] name The name of the script section
    //! \param[in] code The script code buffer
    //! \param[in] codeLength The length of the script code
    //! \param[in] lineOffset An offset that will be added to compiler message line numbers
    //! \return A negative value on error
    //!
    //! \todo Describe method, List error codes
	virtual int AddScriptSection(const char *module, const char *name, const char *code, size_t codeLength, int lineOffset = 0) = 0;
	//! \brief Build the previously added script sections.
    //!
    //! \param[in] module The name of the module
    //! \return A negative value on error
    //!
    //! \todo Describe method, List error codes
	virtual int Build(const char *module) = 0;
	//! \brief Discard a compiled module.
    //!
    //! \param[in] module The name of the module
    //! \return A negative value on error
    //!
    //! \todo Describe method, List error codes
    virtual int Discard(const char *module) = 0;
	//! \brief Reset the global variables of a module.
    //!
    //! \param[in] module The name of the module
    //! \return A negative value on error
    //!
    //! \todo Describe method, List error codes
	virtual int ResetModule(const char *module) = 0;

	// Script functions
	//! \brief Returns the number of global functions in the module.
    //! \param[in] module The name of the module.
    //! \return A negative value on error, or the number of global functions in this module.
    //! \todo Describe method, List error codes
	virtual int GetFunctionCount(const char *module) = 0;
	//! \brief Returns the function id by index.
    //! \param[in] module The name of the module.
    //! \param[in] index The index of the function.
    //! \return A negative value on error, or the function id.
    //! \todo Describe method, List error codes
	virtual int GetFunctionIDByIndex(const char *module, int index) = 0;
	//! \brief Returns the function id by name.
    //! \param[in] module The name of the module.
    //! \param[in] name The name of the function.
    //! \return A negative value on error, or the function id.
    //! \todo Describe method, List error codes
	virtual int GetFunctionIDByName(const char *module, const char *name) = 0;
	//! \brief Returns the function id by declaration.
    //! \param[in] module The name of the module.
    //! \param[in] decl The function signature.
    //! \return A negative value on error, or the function id.
    //! \todo Describe method, List error codes
	virtual int GetFunctionIDByDecl(const char *module, const char *decl) = 0;
	//! \brief Returns the function declaration.
    //! \param[in] funcId The function id.
    //! \param[out] length A pointer to the variable that will receive the length of the returned string.
    //! \return A null terminated string with the function declaration, or null if not found.
    //! \todo Describe method
	virtual const char *GetFunctionDeclaration(int funcId, int *length = 0) = 0;
	//! \brief Returns the function name.
    //! \param[in] funcId The function id.
    //! \param[out] length A pointer to the variable that will receive the length of the returned string.
    //! \return A null terminated string with the function name, or null if not found.
    //! \todo Describe method
	virtual const char *GetFunctionName(int funcId, int *length = 0) = 0;
	//! \brief Returns the module where the function was implemented.
    //! \param[in] funcId The function id.
    //! \param[out] length A pointer to the variable that will receive the length of the returned string.
    //! \return A null terminated string with the module name, or null if not found.
    //! \todo Describe method
	virtual const char *GetFunctionModule(int funcId, int *length = 0) = 0;
	//! \brief Returns the section where the function was implemented.
    //! \param[in] funcId The function id.
    //! \param[out] length A pointer to the variable that will receive the length of the returned string.
    //! \return A null terminated string with the section name, or null if not found.
    //! \todo Describe method
	virtual const char *GetFunctionSection(int funcId, int *length = 0) = 0;
	//! \brief Returns the function descriptor for the script function
    //! \param[in] module The module name.
    //! \param[in] index The index of the function.
    //! \return A pointer to the function description interface, or null if not found.
    //! \todo Describe method
	virtual const asIScriptFunction *GetFunctionDescriptorByIndex(const char *module, int index) = 0;

	//! \brief Returns the number of methods for the object type.
    //! \param[in] typeId The object type id.
    //! \return A negative value on error, or the number of methods for this object.
    //! \todo Describe method, List error codes
	virtual int GetMethodCount(int typeId) = 0;
	//! \brief Returns the method id by index.
    //! \param[in] typeId The object type id.
    //! \param[in] index The index of the method.
    //! \return A negative value on error, or the method id.
    //! \todo Describe method
	virtual int GetMethodIDByIndex(int typeId, int index) = 0;
	//! \brief Returns the method id by name.
    //! \param[in] typeId The object type id.
    //! \param[in] name The name of the method.
    //! \return A negative value on error, or the method id.
    //! \todo Describe method, List error codes
	virtual int GetMethodIDByName(int typeId, const char *name) = 0;
	//! \brief Returns the method id by declaration.
    //! \param[in] typeId The object type id.
    //! \param[in] decl The method signature.
    //! \return A negative value on error, or the method id.
    //! \todo Describe method, List error codes
	virtual int GetMethodIDByDecl(int typeId, const char *decl) = 0;
	//! \brief Returns the function descriptor for the script method
    //! \param[in] typeId The object type id.
    //! \param[in] index The index of the method.
    //! \return A pointer to the method description interface, or null if not found.
    //! \todo Describe method
	virtual const asIScriptFunction *GetMethodDescriptorByIndex(int typeId, int index) = 0;

	// Script global variables
	//! \brief Returns the number of global variables in the module.
    //! \param[in] module The name of the module.
    //! \return A negative value on error, or the number of global variables.
    //! \todo Describe method, List error codes
	virtual int GetGlobalVarCount(const char *module) = 0;
	//! \brief Returns the global variable id by index.
    //! \param[in] module The name of the module.
    //! \param[in] index The index of the global variable.
    //! \return A negative value on error, or the global variable id.
    //! \todo Describe method, List error codes
	virtual int GetGlobalVarIDByIndex(const char *module, int index) = 0;
	//! \brief Returns the global variable id by name.
    //! \param[in] module The name of the module.
    //! \param[in] name The name of the global variable.
    //! \return A negative value on error, or the global variable id.
    //! \todo Describe method, List error codes
	virtual int GetGlobalVarIDByName(const char *module, const char *name) = 0;
	//! \brief Returns the global variable id by declaration.
    //! \param[in] module The name of the module.
    //! \param[in] decl The global variable declaration.
    //! \return A negative value on error, or the global variable id.
    //! \todo Describe method, List error codes
	virtual int GetGlobalVarIDByDecl(const char *module, const char *decl) = 0;
	//! \brief Returns the global variable declaration.
    //! \param[in] gvarID The global variable id.
    //! \param[out] length The length of the returned string.
    //! \return A null terminated string with the variable declaration, or null if not found.
    //! \todo Describe method
	virtual const char *GetGlobalVarDeclaration(int gvarID, int *length = 0) = 0;
	//! \brief Returns the global variable name.
    //! \param[in] gvarID The global variable id.
    //! \param[out] length The length of the returned string.
    //! \return A null terminated string with the variable name, or null if not found.
    //! \todo Describe method
	virtual const char *GetGlobalVarName(int gvarID, int *length = 0) = 0;
	//! \brief Returns the pointer to the global variable.
    //! \param[in] gvarID The global variable id.
    //! \return A pointer to the global variable, or null if not found.
    //! \todo Describe method
	virtual void *GetGlobalVarPointer(int gvarID) = 0;

	// Dynamic binding between modules
	//! \brief Returns the number of functions declared for import.
    //! \param[in] module The name of the module.
    //! \return A negative value on error, or the number of imported functions.
    //! \todo Describe method, List error codes
	virtual int GetImportedFunctionCount(const char *module) = 0;
	//! \brief Returns the imported function index by declaration.
    //! \param[in] module The name of the module.
    //! \param[in] decl The function declaration of the imported function.
    //! \return A negative value on error, or the index of the imported function.
    //! \todo Describe method, List error codes
	virtual int GetImportedFunctionIndexByDecl(const char *module, const char *decl) = 0;
	//! \brief Returns the imported function declaration.
    //! \param[in] module The name of the module.
    //! \param[in] importIndex The index of the imported function.
    //! \param[out] length The length of the returned string.
    //! \return A null terminated string with the function declaration, or null if not found.
    //! \todo Describe method
	virtual const char *GetImportedFunctionDeclaration(const char *module, int importIndex, int *length = 0) = 0;
	//! \brief Returns the declared imported function source module.
    //! \param[in] module The name of the module.
    //! \param[in] importIndex The index of the imported function.
    //! \param[out] length The length of the returned string.
    //! \return A null terminated string with the name of the source module, or null if not found.
    //! \todo Describe method
	virtual const char *GetImportedFunctionSourceModule(const char *module, int importIndex, int *length = 0) = 0;
	//! \brief Binds an imported function to the function from another module.
    //! \param[in] module The name of the module.
    //! \param[in] importIndex The index of the imported function.
    //! \param[in] funcId The function id of the function that will be bound to the imported function.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int BindImportedFunction(const char *module, int importIndex, int funcId) = 0;
	//! \brief Unbinds an imported function.
    //! \param[in] module The name of the module.
    //! \param[in] importIndex The index of the imported function.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int UnbindImportedFunction(const char *module, int importIndex) = 0;

	//! \brief Binds all imported functions in a module, by searching their equivalents in the declared source modules.
    //! \param[in] module The name of the module.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int BindAllImportedFunctions(const char *module) = 0;
	//! \brief Unbinds all imported functions.
    //! \param[in] module The name of the module.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int UnbindAllImportedFunctions(const char *module) = 0;

	// Type identification
	//! \brief Returns a type id by declaration.
    //! \param[in] module The name of the module.
    //! \param[in] decl The declaration of the type.
    //! \return A negative value on error, or the type id of the type.
    //! \todo Describe method, List error codes
	virtual int GetTypeIdByDecl(const char *module, const char *decl) = 0;
	//! \brief Returns a type declaration.
    //! \param[in] typeId The type id of the type.
    //! \param[out] length The length of the returned string.
    //! \return A null terminated string with the type declaration, or null if not found.
    //! \todo Describe method
	virtual const char *GetTypeDeclaration(int typeId, int *length = 0) = 0;
	//! \brief Returns the size of a primitive type.
    //! \param[in] typeId The type id of the type.
    //! \return A negative value on error, or the size of the type in bytes.
    //! \todo Describe method, List error codes
	virtual int GetSizeOfPrimitiveType(int typeId) = 0;
	//! \brief Returns the object type interface for type.
    //! \param[in] typeId The type id of the type.
    //! \return The object type interface for the type, or null if not found.
    //! \todo Describe method
	virtual asIObjectType *GetObjectTypeById(int typeId) = 0;
	//! \brief Returns the object type interface by index.
    //! \param[in] index The index of the type.
    //! \return The object type interface for the type, or null if not found.
    //! \todo Describe method
	virtual asIObjectType *GetObjectTypeByIndex(asUINT index) = 0;
	//! \brief Returns the number of object types.
    //! \return A negative value on error, or the number of object types known to the engine.
    //! \todo Describe method, List error codes
	virtual int GetObjectTypeCount() = 0;

	// Script execution
	//! \brief Sets the default context stack size.
    //! \param[in] initial The smallest stack size
    //! \param[in] maximum The largest stack size
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int SetDefaultContextStackSize(asUINT initial, asUINT maximum) = 0;
	//! \brief Creates a new script context.
    //! \return A pointer to the new script context.
    //! \todo Describe method
	virtual asIScriptContext *CreateContext() = 0;
	//! \brief Creates a script object defined by its type id.
    //! \param[in] typeId The type id of the object to create.
    //! \return A pointer to the new object if successful, or null if not.
    //! \todo Describe method
	virtual void *CreateScriptObject(int typeId) = 0;
	//! \brief Creates a copy of a script object.
    //! \param[in] obj A pointer to the source object.
    //! \param[in] typeId The type id of the object.
    //! \return A pointer to the new object if successful, or null if not.
    //! \todo Describe method
	virtual void *CreateScriptObjectCopy(void *obj, int typeId) = 0;
	//! \brief Copy one script object to another.
    //! \param[in] dstObj A pointer to the destination object.
    //! \param[in] srcObj A pointer to the source object.
    //! \param[in] typeId The type id of the objects.
    //! \todo Describe method
	virtual void CopyScriptObject(void *dstObj, void *srcObj, int typeId) = 0;
	//! \brief Release the script object pointer.
    //! \param[in] obj A pointer to the object.
    //! \param[in] typeId The type id of the object.
    //! \todo Describe method
	virtual void ReleaseScriptObject(void *obj, int typeId) = 0;
	//! \brief Increase the reference counter for the script object.
    //! \param[in] obj A pointer to the object.
    //! \param[in] typeId The type id of the object.
    //! \todo Describe method
	virtual void AddRefScriptObject(void *obj, int typeId) = 0;
	//! \brief Returns true if the object referenced by a handle compatible with the specified type.
    //! \param[in] obj A pointer to the object.
    //! \param[in] objTypeId The type id of the object.
    //! \param[in] handleTypeId The type id of the handle.
    //! \return Returns true if the handle type is compatible with the object type.
    //! \todo Describe method
	virtual bool IsHandleCompatibleWithObject(void *obj, int objTypeId, int handleTypeId) = 0;
	//! \brief Performs a comparison of two objects using the specified operator behaviour.
    //! \param[out] result The result of the comparison
    //! \param[in] behaviour One of the comparison behaviours from \ref asEBehaviours.
    //! \param[in] leftObj A pointer to the left object.
    //! \param[in] rightObj A pointer to the right object.
    //! \param[in] typeId The type id of the objects.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int CompareScriptObjects(bool &result, int behaviour, void *leftObj, void *rightObj, int typeId) = 0;

	// String interpretation
	//! \brief Compiles and executes script statements within the context of a module.
    //! \param[in] module The name of the module in which the string should be executed.
    //! \param[in] script The script string that will be executed.
    //! \param[in,out] ctx Either pass in your own context or receive the default context, depending on the flags parameter.
    //! \param[in] flags A combination of the values from \ref asEExecStrFlags.
    //! \return One of \ref asEContextState values, or a negative value on compiler error.
    //! \todo Describe method, List error codes
	virtual int ExecuteString(const char *module, const char *script, asIScriptContext **ctx = 0, asDWORD flags = 0) = 0;

	// Garbage collection
	//! \brief Perform garbage collection.
    //! \param[in] doFullCycle Set to true if a full cycle should be done, or false if only an iterative step should be done.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int GarbageCollect(bool doFullCycle = true) = 0;
	//! \brief Returns the number of objects currently referenced by the garbage collector.
    //! \return The number of objects currently known by the garbage collector.
    //! \todo Describe method
	virtual int GetObjectsInGarbageCollectorCount() = 0;
	//! \brief Notify the garbage collector of a new object that needs to be managed.
    //! \param[in] obj A pointer to the newly created object.
    //! \param[in] typeId The type id of the object.
    //! \todo Describe method
	virtual void NotifyGarbageCollectorOfNewObject(void *obj, int typeId) = 0;
	//! \brief Used by the garbage collector to enumerate all references held by an object.
    //! \param[in] obj A pointer to the references object.
    //! \todo Describe method
	virtual void GCEnumCallback(void *obj) = 0;

	// Bytecode Saving/Loading
	//! \brief Save compiled bytecode to a binary stream.
    //!
    //! \param[in] module The name of the module.
    //! \param[in] out The output stream.
    //! \return A negative value on error.
    //! \retval asNO_MODULE The module couldn't be found.
    //! \retval asINVALID_ARG The stream object wasn't specified.
    //!
    //! This method is used to save pre-compiled byte code to disk or memory, for a later restoral.
    //! The application must implement an object that inherits from \ref asIBinaryStream to provide
    //! the necessary stream operations.
    //!
    //! The pre-compiled byte code is currently not platform independent, so you need to make
    //! sure the byte code is compiled on a platform that is compatible with the one that will load it.
	virtual int SaveByteCode(const char *module, asIBinaryStream *out) = 0;
	//! \brief Load pre-compiled bytecode from a binary stream.
    //!
    //! \param[in] module The name of the module.
    //! \param[in] in The input stream.
    //! \return A negative value on error.
    //! \retval asNO_MODULE The module couldn't be found.
    //! \retval asINVALID_ARG The stream object wasn't specified.
    //!
    //! This method is used to load pre-compiled byte code from disk or memory. The application must
    //! implement an object that inherits from \ref asIBinaryStream to provide the necessary stream operations.
    //!
    //! It is expected that the application performs the necessary validations to make sure the
    //! pre-compiled byte code is from a trusted source. The application should also make sure the
    //! pre-compiled byte code is compatible with the current engine configuration, i.e. that the
    //! engine has been configured in the same way as when the byte code was first compiled.
	virtual int LoadByteCode(const char *module, asIBinaryStream *in) = 0;

protected:
	virtual ~asIScriptEngine() {}
};

//! \brief The interface to the virtual machine
class asIScriptContext
{
public:
	// Memory management
	//! \brief Increase reference counter.
    //!
    //! \return The number of references to this object.
    //!
    //! Call this method when storing an additional reference to the object.
    //! Remember that the first reference that is received from \ref asIScriptEngine::CreateContext
    //! is already accounted for.
	virtual int AddRef() = 0;
	//! \brief Decrease reference counter.
    //!
    //! \return The number of references to this object.
    //!
    //! Call this method when you will no longer use the references that you own.
	virtual int Release() = 0;

	// Engine
	//! \brief Returns a pointer to the engine.
    //! \return A pointer to the engine.
    //! \todo Describe method
	virtual asIScriptEngine *GetEngine() = 0;

	// Script context
	//! \brief Returns the state of the context.
    //! \return The current state of the context.
    //! \todo Describe method
	virtual asEContextState GetState() = 0;

    //! \brief Prepares the context for execution of the function identified by funcId.
    //! \param[in] funcId The id of the function/method that will be executed.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
    //! \see \ref doc_call_script_func
	virtual int Prepare(int funcId) = 0;
	//! \brief Frees resources held by the context.
    //! \return A negative value on error.
    //! This function is usually not necessary to call.
    //! \todo Describe method, List error codes
	virtual int Unprepare() = 0;

	//! \brief Sets an 8-bit argument value.
    //! \param[in] arg The argument index.
    //! \param[in] value The value of the argument.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetArgByte(asUINT arg, asBYTE value) = 0;
	//! \brief Sets a 16-bit argument value.
    //! \param[in] arg The argument index.
    //! \param[in] value The value of the argument.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetArgWord(asUINT arg, asWORD value) = 0;
	//! \brief Sets a 32-bit integer argument value.
    //! \param[in] arg The argument index.
    //! \param[in] value The value of the argument.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetArgDWord(asUINT arg, asDWORD value) = 0;
	//! \brief Sets a 64-bit integer argument value.
    //! \param[in] arg The argument index.
    //! \param[in] value The value of the argument.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetArgQWord(asUINT arg, asQWORD value) = 0;
	//! \brief Sets a float argument value.
    //! \param[in] arg The argument index.
    //! \param[in] value The value of the argument.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetArgFloat(asUINT arg, float value) = 0;
	//! \brief Sets a double argument value.
    //! \param[in] arg The argument index.
    //! \param[in] value The value of the argument.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetArgDouble(asUINT arg, double value) = 0;
	//! \brief Sets the address of a reference or handle argument.
    //! \param[in] arg The argument index.
    //! \param[in] addr The address that should be passed in the argument.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetArgAddress(asUINT arg, void *addr) = 0;
	//! \brief Sets the object argument value.
    //! \param[in] arg The argument index.
    //! \param[in] obj A pointer to the object.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetArgObject(asUINT arg, void *obj) = 0;
	//! \brief Returns a pointer to the argument for assignment.
    //! \param[in] arg The argument index.
    //! \return A pointer to the argument on the stack.
    //! \todo Describe method
	virtual void *GetArgPointer(asUINT arg) = 0;

	//! \brief Sets the object for a class method call.
    //! \param[in] obj A pointer to the object.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetObject(void *obj) = 0;

	//! \brief Returns the 8-bit return value.
    //! \return The value returned from the script function, or 0 on error.
    //! \todo Describe method
	virtual asBYTE  GetReturnByte() = 0;
	//! \brief Returns the 16-bit return value.
    //! \return The value returned from the script function, or 0 on error.
    //! \todo Describe method
	virtual asWORD  GetReturnWord() = 0;
	//! \brief Returns the 32-bit return value.
    //! \return The value returned from the script function, or 0 on error.
    //! \todo Describe method
	virtual asDWORD GetReturnDWord() = 0;
	//! \brief Returns the 64-bit return value.
    //! \return The value returned from the script function, or 0 on error.
    //! \todo Describe method
	virtual asQWORD GetReturnQWord() = 0;
	//! \brief Returns the float return value.
    //! \return The value returned from the script function, or 0 on error.
    //! \todo Describe method
	virtual float   GetReturnFloat() = 0;
	//! \brief Returns the double return value.
    //! \return The value returned from the script function, or 0 on error.
    //! \todo Describe method
	virtual double  GetReturnDouble() = 0;
	//! \brief Returns the address for a reference or handle return type.
    //! \return The value returned from the script function, or 0 on error.
    //! \todo Describe method
	virtual void   *GetReturnAddress() = 0;
	//! \brief Return a pointer to the returned object.
    //! \return The address returned from the script function, or 0 on error.
    //! \todo Describe method
	virtual void   *GetReturnObject() = 0;
	//! \brief Returns a pointer to the returned value independent of type.
    //! \return A pointer to the object returned from the script function, or 0 on error.
    //! \todo Describe method
	virtual void   *GetReturnPointer() = 0;

	//! \brief Executes the prepared function.
    //! \return A negative value on error, or one of \ref asEContextState.
    //! \todo Describe method, List error codes
    //! \see \ref doc_call_script_func
	virtual int Execute() = 0;
	//! \brief Aborts the execution.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int Abort() = 0;
	//! \brief Suspends the execution, which can then be resumed by calling Execute again.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
    //! \see \ref doc_call_script_func
	virtual int Suspend() = 0;

	//! \brief Get the current line number that is being executed.
    //! \param[out] column The variable will receive the column number.
    //! \return The current line number.
    //! \todo Describe method
	virtual int GetCurrentLineNumber(int *column = 0) = 0;
	//! \brief Get the current function that is being executed.
    //! \return The function id of the current function.
    //! \todo Describe method
	virtual int GetCurrentFunction() = 0;

	// Exception handling
 	//! \brief Sets an exception, which aborts the execution.
    //! \param[in] string A string that describes the exception that occurred.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int SetException(const char *string) = 0;
	//! \brief Returns the line number where the exception occurred.
    //! \param[out] column The variable will receive the column number.
    //! \return The line number where the exception occurred.
    //! \todo Describe method
	virtual int GetExceptionLineNumber(int *column = 0) = 0;
	//! \brief Returns the function id of the function where the exception occurred.
    //! \return The function id where the exception occurred.
    //! \todo Describe method
	virtual int GetExceptionFunction() = 0;
	//! \brief Returns the exception string text.
    //! \param[out] length The length of the returned string.
    //! \return A string describing the exception that occurred.
    //! \todo Describe method
	virtual const char *GetExceptionString(int *length = 0) = 0;

	//! \brief Sets a line callback function. The function will be called for each executed script statement.
    //! \param[in] callback The callback function/method that should be called for each script line executed.
    //! \param[in] obj The object pointer on which the callback is called.
    //! \param[in] callConv The calling convention of the callback function/method.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int  SetLineCallback(asSFuncPtr callback, void *obj, int callConv) = 0;
	//! \brief Removes a previously registered callback.
    //! Removes a previously registered callback.
	virtual void ClearLineCallback() = 0;
	//! \brief Sets an exception callback function. The function will be called if a script exception occurs.
    //! \param[in] callback The callback function/method that should be called upon an exception.
    //! \param[in] obj The object pointer on which the callback is called.
    //! \param[in] callConv The calling convention of the callback function/method.
    //! \return A negative value on error.
    //! \todo Describe method, List error codes
	virtual int  SetExceptionCallback(asSFuncPtr callback, void *obj, int callConv) = 0;
	//! \brief Removes a previously registered callback.
    //! Removes a previously registered callback.
	virtual void ClearExceptionCallback() = 0;

	//! \brief Returns the size of the callstack, i.e. the number of functions that have yet to complete.
    //! \return The number of functions on the call stack.
    //! \todo Describe method
	virtual int GetCallstackSize() = 0;
	//! \brief Returns the function id at the specified callstack level.
    //! \param[in] index The index on the call stack.
    //! \return The function id on the call stack referred to by the index.
    //! \todo Describe method
	virtual int GetCallstackFunction(int index) = 0;
	//! \brief Returns the line number at the specified callstack level.
    //! \param[in] index The index on the call stack.
    //! \param[out] column The variable will receive the column number.
    //! \return The line number for the call stack level referred to by the index.
    //! \todo Describe method
	virtual int GetCallstackLineNumber(int index, int *column = 0) = 0;

	//! \brief Returns the number of local variables at the specified callstack level.
    //! \param[in] stackLevel The index on the call stack.
    //! \return The number of variables in the function on the call stack level.
    //! \todo Describe method
	virtual int         GetVarCount(int stackLevel = -1) = 0;
	//! \brief Returns the name of local variable at the specified callstack level.
    //! \param[in] varIndex The index of the variable.
    //! \param[out] length The length of the returned string.
    //! \param[in] stackLevel The index on the call stack.
    //! \return A null terminated string with the name of the variable.
    //! \todo Describe method
	virtual const char *GetVarName(int varIndex, int *length = 0, int stackLevel = -1) = 0;
	//! \brief Returns the declaration of a local variable at the specified callstack level.
    //! \param[in] varIndex The index of the variable.
    //! \param[out] length The length of the returned string.
    //! \param[in] stackLevel The index on the call stack.
    //! \return A null terminated string with the declaration of the variable.
    //! \todo Describe method
	virtual const char *GetVarDeclaration(int varIndex, int *length = 0, int stackLevel = -1) = 0;
	//! \brief Returns the type id of a local variable at the specified callstack level.
    //! \param[in] varIndex The index of the variable.
    //! \param[in] stackLevel The index on the call stack.
    //! \return The type id of the variable.
    //! \todo Describe method
	virtual int         GetVarTypeId(int varIndex, int stackLevel = -1) = 0;
	//! \brief Returns a pointer to a local variable at the specified callstack level.
    //! \param[in] varIndex The index of the variable.
    //! \param[in] stackLevel The index on the call stack.
    //! \return A pointer to the variable.
    //! \todo Describe method
	virtual void       *GetVarPointer(int varIndex, int stackLevel = -1) = 0;
	//! \brief Returns the type id of the object, if a class method is being executed.
    //! \param[in] stackLevel The index on the call stack.
    //! \return Returns the type id of the object if it is a class method.
    //! \todo Describe method
	virtual int         GetThisTypeId(int stackLevel = -1) = 0;
	//! \brief Returns a pointer to the object, if a class method is being executed.
    //! \param[in] stackLevel The index on the call stack.
    //! \return Returns a pointer to the object if it is a class method.
    //! \todo Describe method
	virtual void       *GetThisPointer(int stackLevel = -1) = 0;

	//! \brief Register the memory address of some user data.
    //! \param[in] data A pointer to the user data.
    //! \return The previous pointer stored in the context.
    //! \todo Describe method
	virtual void *SetUserData(void *data) = 0;
	//! \brief Returns the address of the previously registered user data.
    //! \return The pointer to the user data.
	virtual void *GetUserData() = 0;

protected:
	virtual ~asIScriptContext() {}
};

//! \brief The interface for the generic calling convention
class asIScriptGeneric
{
public:
    //! \brief Returns a pointer to the script engine.
    //! \return A pointer to the engine.
	virtual asIScriptEngine *GetEngine() = 0;

    //! \brief Returns the function id of the called function.
    //! \return The function id of the function being called.
	virtual int     GetFunctionId() = 0;

    //! \brief Returns the object pointer if this is a class method, or null if it not.
    //! \return A pointer to the object, if this is a method.
	virtual void   *GetObject() = 0;
    //! \brief Returns the type id of the object if this is a class method.
    //! \return The type id of the object if this is a method.
	virtual int     GetObjectTypeId() = 0;

    //! \brief Returns the number of arguments.
    //! \return The number of arguments to the function.
	virtual int     GetArgCount() = 0;
    //! \brief Returns the value of an 8-bit argument.
    //! \param[in] arg The argument index.
    //! \return The argument value.
    //! \todo Describe method
	virtual asBYTE  GetArgByte(asUINT arg) = 0;
    //! \brief Returns the value of a 16-bit argument.
    //! \param[in] arg The argument index.
    //! \return The argument value.
    //! \todo Describe method
	virtual asWORD  GetArgWord(asUINT arg) = 0;
    //! \brief Returns the value of a 32-bit integer argument.
    //! \param[in] arg The argument index.
    //! \return The argument value.
    //! \todo Describe method
	virtual asDWORD GetArgDWord(asUINT arg) = 0;
    //! \brief Returns the value of a 64-bit integer argument.
    //! \param[in] arg The argument index.
    //! \return The argument value.
    //! \todo Describe method
	virtual asQWORD GetArgQWord(asUINT arg) = 0;
    //! \brief Returns the value of a float argument.
    //! \param[in] arg The argument index.
    //! \return The argument value.
    //! \todo Describe method
	virtual float   GetArgFloat(asUINT arg) = 0;
    //! \brief Returns the value of a double argument.
    //! \param[in] arg The argument index.
    //! \return The argument value.
    //! \todo Describe method
	virtual double  GetArgDouble(asUINT arg) = 0;
    //! \brief Returns the address held in a reference or handle argument.
    //! \param[in] arg The argument index.
    //! \return The argument value, which is an address.
    //! \todo Describe method
	virtual void   *GetArgAddress(asUINT arg) = 0;
    //! \brief Returns a pointer to the object in a object argument.
    //! \param[in] arg The argument index.
    //! \return A pointer to the object argument.
    //! \todo Describe method
	virtual void   *GetArgObject(asUINT arg) = 0;
    //! \brief Returns a pointer to the argument value.
    //! \param[in] arg The argument index.
    //! \return A pointer to the argument value.
    //! \todo Describe method
	virtual void   *GetArgPointer(asUINT arg) = 0;
    //! \brief Returns the type id of the argument.
    //! \param[in] arg The argument index.
    //! \return The type id of the argument.
	virtual int     GetArgTypeId(asUINT arg) = 0;

    //! \brief Sets the 8-bit return value.
    //! \param[in] val The return value.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int     SetReturnByte(asBYTE val) = 0;
    //! \brief Sets the 16-bit return value.
    //! \param[in] val The return value.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int     SetReturnWord(asWORD val) = 0;
    //! \brief Sets the 32-bit integer return value.
    //! \param[in] val The return value.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int     SetReturnDWord(asDWORD val) = 0;
    //! \brief Sets the 64-bit integer return value.
    //! \param[in] val The return value.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int     SetReturnQWord(asQWORD val) = 0;
    //! \brief Sets the float return value.
    //! \param[in] val The return value.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int     SetReturnFloat(float val) = 0;
    //! \brief Sets the double return value.
    //! \param[in] val The return value.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int     SetReturnDouble(double val) = 0;
    //! \brief Sets the address return value when the return is a reference or handle.
    //! \param[in] addr The return value, which is an address.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int     SetReturnAddress(void *addr) = 0;
    //! \brief Sets the object return value.
    //! \param[in] obj A pointer to the object return value.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int     SetReturnObject(void *obj) = 0;
    //! \brief Gets the pointer to the return value so it can be assigned a value.
    //! \return A pointer to the return value.
    //! \todo Describe method
	virtual void   *GetReturnPointer() = 0;
    //! \brief Gets the type id of the return value.
    //! \return The type id of the return value.
	virtual int     GetReturnTypeId() = 0;

protected:
	virtual ~asIScriptGeneric() {}
};

//! \brief The interface for a script class or interface
class asIScriptStruct
{
public:
	// Memory management
	//! \brief Increase reference counter.
    //!
    //! \return The number of references to this object.
    //!
    //! Call this method when storing an additional reference to the object.
	virtual int AddRef() = 0;
	//! \brief Decrease reference counter.
    //!
    //! \return The number of references to this object.
    //!
    //! Call this method when you will no longer use the references that you own.
	virtual int Release() = 0;

	// Struct type
	//! \brief Returns the type id of the object.
    //! \return The type id of the script object.
	virtual int GetStructTypeId() = 0;

    //! \brief Returns the object type interface for the object.
    //! \return The object type interface of the script object.
	virtual asIObjectType *GetObjectType() = 0;

	// Struct properties
	//! \brief Returns the number of properties that the object contains.
    //! \return The number of member properties of the script object.
	virtual int GetPropertyCount() = 0;

    //! \brief Returns the type id of the property referenced by prop.
    //! \param[in] prop The property index.
    //! \return The type id of the member property.
	virtual int GetPropertyTypeId(asUINT prop) = 0;

    //! \brief Returns the name of the property referenced by prop.
    //! \param[in] prop The property index.
    //! \return A null terminated string with the property name.
	virtual const char *GetPropertyName(asUINT prop) = 0;

    //! \brief Returns a pointer to the property referenced by prop.
    //! \param[in] prop The property index.
    //! \return A pointer to the property value.
    //! \todo Describe method
	virtual void *GetPropertyPointer(asUINT prop) = 0;

    //! \brief Copies the content from another object of the same type.
    //! \param[in] other A pointer to the source object.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int CopyFrom(asIScriptStruct *other) = 0;

protected:
	virtual ~asIScriptStruct() {}
};

//! \brief The interface for a script array object
class asIScriptArray
{
public:
	// Memory management
	//! \brief Increase reference counter.
    //!
    //! \return The number of references to this object.
    //!
    //! Call this method when storing an additional reference to the object.
	virtual int AddRef() = 0;
	//! \brief Decrease reference counter.
    //!
    //! \return The number of references to this object.
    //!
    //! Call this method when you will no longer use the references that you own.
	virtual int Release() = 0;

	// Array type
	//! \brief Returns the type id of the array object.
    //! \return The type id of the array object.
	virtual int GetArrayTypeId() = 0;

	// Elements
	//! \brief Returns the type id of the contained elements.
    //! \return The type id of the array elements.
	virtual int    GetElementTypeId() = 0;

    //! \brief Returns the size of the array.
    //! \return The number of elements in the array.
	virtual asUINT GetElementCount() = 0;

    //! \brief Returns a pointer to the element referenced by index.
    //! \param[in] index The element index.
    //! \return A pointer to the element value.
    //! \todo Describe method
	virtual void * GetElementPointer(asUINT index) = 0;

    //! \brief Resizes the array.
    //! \param[in] size The new size of the array.
    //! \todo Describe method
	virtual void   Resize(asUINT size) = 0;

    //! \brief Copies the elements from another array, overwriting the current content.
    //! \param[in] other A pointer to the source array.
    //! \return A negative value on error.
    //! \todo Describe method
	virtual int    CopyFrom(asIScriptArray *other) = 0;

protected:
	virtual ~asIScriptArray() {}
};

//! \brief The interface for an object type
class asIObjectType
{
public:
	//! \brief Returns a pointer to the script engine.
    //! \return A pointer to the engine.
	virtual asIScriptEngine *GetEngine() const = 0;

	//! \brief Returns a temporary pointer to the name of the datatype.
    //! \return A null terminated string with the name of the object type.
	virtual const char *GetName() const = 0;

	//! \brief Returns a temporary pointer to the type associated with this descriptor.
    //! \return A pointer to the sub type.
	virtual const asIObjectType *GetSubType() const = 0;

	//! \brief Returns the number of interfaces implemented.
    //! \return The number of interfaces implemented by this type.
	virtual int GetInterfaceCount() const = 0;

	//! \brief Returns a temporary pointer to the specified interface or null if none are found.
    //! \param[in] index The interface index.
    //! \return A pointer to the interface type.
	virtual const asIObjectType *GetInterface(asUINT index) const = 0;

    //! \brief Returns true if the type is an interface.
    //! \return True if the type is an interface.
	virtual bool IsInterface() const = 0;

protected:
	virtual ~asIObjectType() {}
};

//! \brief The interface for a script function description
class asIScriptFunction
{
public:
	//! \brief Returns the name of the module where the function was implemented
    //! \return A null terminated string with the module name.
	virtual const char          *GetModuleName() const = 0;
	//! \brief Returns the object type for class or interface method
    //! \return A pointer to the object type interface if this is a method.
	virtual const asIObjectType *GetObjectType() const = 0;
	//! \brief Returns the name of the object for class or interface methods
    //! \return A null terminated string with the name of the object type if this a method.
	virtual const char          *GetObjectName() const = 0;
	//! \brief Returns the name of the function or method
    //! \return A null terminated string with the name of the function.
	virtual const char          *GetFunctionName() const = 0;
	//! \brief Returns true if it is a class method
    //! \return True if this a class method.
	virtual bool                 IsClassMethod() const = 0;
	//! \brief Returns true if it is an interface method
    //! \return True if this is an interface method.
	virtual bool                 IsInterfaceMethod() const = 0;

protected:
	virtual ~asIScriptFunction() {};
};

//! \brief A binary stream interface.
//!
//! This interface is used when storing compiled bytecode to disk or memory, and then loading it into the engine again.
//!
//! \see asIScriptEngine::SaveByteCode, asIScriptEngine::LoadByteCode
class asIBinaryStream
{
public:
    //! \brief Read size bytes from the stream into the memory pointed to by ptr.
    //!
    //! \param[out] ptr A pointer to the buffer that will receive the data.
    //! \param[in] size The number of bytes to read.
    //!
    //! Read \a size bytes from the data stream into the memory pointed to by \a ptr.
	virtual void Read(void *ptr, asUINT size) = 0;
    //! \brief Write size bytes to the stream from the memory pointed to by ptr.
    //!
    //! \param[in] ptr A pointer to the buffer that the data should written from.
    //! \param[in] size The number of bytes to write.
    //!
    //! Write \a size bytes to the data stream from the memory pointed to by \a ptr.
	virtual void Write(const void *ptr, asUINT size) = 0;

public:
	virtual ~asIBinaryStream() {}
};

//!@}

//! \defgroup const Constants
//!@{

// Enumerations and constants

// Engine properties
//! Engine properties
enum asEEngineProp
{
	//! Allow unsafe references. Default: false.
	asEP_ALLOW_UNSAFE_REFERENCES = 1,
	//! Optimize byte code. Default: true.
	asEP_OPTIMIZE_BYTECODE       = 2,
	//! Copy script section memory. Default: true.
	asEP_COPY_SCRIPT_SECTIONS    = 3,
};

// Calling conventions
//! Calling conventions
enum asECallConvTypes
{
	//! A cdecl function.
	asCALL_CDECL            = 0,
	//! A stdcall function.
	asCALL_STDCALL          = 1,
	//! A thiscall class method.
	asCALL_THISCALL         = 2,
	//! A cdecl function that takes the object pointer as the last parameter.
	asCALL_CDECL_OBJLAST    = 3,
	//! A cdecl function that takes the object pointer as the first parameter.
	asCALL_CDECL_OBJFIRST   = 4,
	//! A function using the generic calling convention.
	asCALL_GENERIC          = 5,
};

// Object type flags
//! Object type flags
enum asEObjTypeFlags
{
	//! A reference type.
	asOBJ_REF                   = 0x01,
	//! A value type.
	asOBJ_VALUE                 = 0x02,
	//! A garbage collected type. Only valid with asOBJ_REF.
	asOBJ_GC                    = 0x04,
	//! A plain-old-data type. Only valid with asOBJ_VALUE.
	asOBJ_POD                   = 0x08,
	//! This reference type doesn't allow handles to be held. Only value with asOBJ_REF.
	asOBJ_NOHANDLE              = 0x10,
	//! The life time of objects of this type are controlled by the scope of the variable. Only valid with asOBJ_REF.
	asOBJ_SCOPED                = 0x20,
	//! The C++ type is a class type. Only valid with asOBJ_VALUE.
	asOBJ_APP_CLASS             = 0x100,
	//! The C++ class has an explicit constructor. Only valid with asOBJ_VALUE.
	asOBJ_APP_CLASS_CONSTRUCTOR = 0x200,
	//! The C++ class has an explicit destructor. Only valid with asOBJ_VALUE.
	asOBJ_APP_CLASS_DESTRUCTOR  = 0x400,
	//! The C++ class has an explicit assignment operator. Only valid with asOBJ_VALUE.
	asOBJ_APP_CLASS_ASSIGNMENT  = 0x800,
	asOBJ_APP_CLASS_C           = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR),
	asOBJ_APP_CLASS_CD          = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_DESTRUCTOR),
	asOBJ_APP_CLASS_CA          = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_ASSIGNMENT),
	asOBJ_APP_CLASS_CDA         = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_DESTRUCTOR + asOBJ_APP_CLASS_ASSIGNMENT),
	asOBJ_APP_CLASS_D           = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_DESTRUCTOR),
	asOBJ_APP_CLASS_A           = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_ASSIGNMENT),
	asOBJ_APP_CLASS_DA          = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_DESTRUCTOR + asOBJ_APP_CLASS_ASSIGNMENT),
	//! The C++ type is a primitive type. Only valid with asOBJ_VALUE.
	asOBJ_APP_PRIMITIVE         = 0x1000,
	//! The C++ type is a float or double. Only valid with asOBJ_VALUE.
	asOBJ_APP_FLOAT             = 0x2000,
	asOBJ_MASK_VALID_FLAGS      = 0x3F3F,
};

// Behaviours
//! Behaviours
enum asEBehaviours
{
	// Value object memory management
	//! \brief (Object) Constructor
	asBEHAVE_CONSTRUCT,
	//! \brief (Object) Destructor
	asBEHAVE_DESTRUCT,

	// Reference object memory management
	//! \brief (Object) Factory
	asBEHAVE_FACTORY,
	//! \brief (Object) AddRef
	asBEHAVE_ADDREF,
	//! \brief (Object) Release
	asBEHAVE_RELEASE,

	// Object operators
	//! \brief (Object) Value cast operator
	asBEHAVE_VALUE_CAST,
	//! \brief (Object) operator []
	asBEHAVE_INDEX,
	//! \brief (Object) operator - (Unary negate)
	asBEHAVE_NEGATE,

	// Assignment operators
	asBEHAVE_FIRST_ASSIGN,
	 //! \brief (Object) operator =
	 asBEHAVE_ASSIGNMENT = asBEHAVE_FIRST_ASSIGN,
	 //! \brief (Object) operator +=
	 asBEHAVE_ADD_ASSIGN,
	 //! \brief (Object) operator -=
	 asBEHAVE_SUB_ASSIGN,
	 //! \brief (Object) operator *=
	 asBEHAVE_MUL_ASSIGN,
	 //! \brief (Object) operator /=
	 asBEHAVE_DIV_ASSIGN,
	 //! \brief (Object) operator %=
	 asBEHAVE_MOD_ASSIGN,
	 //! \brief (Object) operator |=
	 asBEHAVE_OR_ASSIGN,
	 //! \brief (Object) operator &=
	 asBEHAVE_AND_ASSIGN,
	 //! \brief (Object) operator ^=
	 asBEHAVE_XOR_ASSIGN,
	 //! \brief (Object) operator <<=
	 asBEHAVE_SLL_ASSIGN,
	 //! \brief (Object) operator >>= (Logical right shift)
	 asBEHAVE_SRL_ASSIGN,
	 //! \brief (Object) operator >>>= (Arithmetic right shift)
	 asBEHAVE_SRA_ASSIGN,
	asBEHAVE_LAST_ASSIGN = asBEHAVE_SRA_ASSIGN,

	// Global operators
	asBEHAVE_FIRST_DUAL,
	 //! \brief (Global) operator +
	 asBEHAVE_ADD = asBEHAVE_FIRST_DUAL,
	 //! \brief (Global) operator -
	 asBEHAVE_SUBTRACT,
	 //! \brief (Global) operator *
	 asBEHAVE_MULTIPLY,
	 //! \brief (Global) operator /
	 asBEHAVE_DIVIDE,
	 //! \brief (Global) operator %
	 asBEHAVE_MODULO,
	 //! \brief (Global) operator ==
	 asBEHAVE_EQUAL,
	 //! \brief (Global) operator !=
	 asBEHAVE_NOTEQUAL,
	 //! \brief (Global) operator <
	 asBEHAVE_LESSTHAN,
	 //! \brief (Global) operator >
	 asBEHAVE_GREATERTHAN,
	 //! \brief (Global) operator <=
	 asBEHAVE_LEQUAL,
	 //! \brief (Global) operator >=
	 asBEHAVE_GEQUAL,
	 //! \brief (Global) operator ||
	 asBEHAVE_LOGIC_OR,
	 //! \brief (Global) operator &&
	 asBEHAVE_LOGIC_AND,
	 //! \brief (Global) operator |
	 asBEHAVE_BIT_OR,
	 //! \brief (Global) operator &
	 asBEHAVE_BIT_AND,
	 //! \brief (Global) operator ^
	 asBEHAVE_BIT_XOR,
	 //! \brief (Global) operator <<
	 asBEHAVE_BIT_SLL,
	 //! \brief (Global) operator >> (Logical right shift)
	 asBEHAVE_BIT_SRL,
	 //! \brief (Global) operator >>> (Arithmetic right shift)
	 asBEHAVE_BIT_SRA,
	asBEHAVE_LAST_DUAL = asBEHAVE_BIT_SRA,

	// Garbage collection behaviours
	asBEHAVE_FIRST_GC,
	//! \brief (Object, GC) Get reference count
	 asBEHAVE_GETREFCOUNT = asBEHAVE_FIRST_GC,
	 //! \brief (Object, GC) Set GC flag
	 asBEHAVE_SETGCFLAG,
	 //! \brief (Object, GC) Get GC flag
	 asBEHAVE_GETGCFLAG,
	 //! \brief (Object, GC) Enumerate held references
	 asBEHAVE_ENUMREFS,
	 //! \brief (Object, GC) Release all references
	 asBEHAVE_RELEASEREFS,
	asBEHAVE_LAST_GC = asBEHAVE_RELEASEREFS,
};

// Return codes
//! Return codes
enum asERetCodes
{
	//! Success
	asSUCCESS                              =  0,
	//! Failure
	asERROR                                = -1,
	//! The context is active
	asCONTEXT_ACTIVE                       = -2,
	//! The context is not finished
	asCONTEXT_NOT_FINISHED                 = -3,
	//! The context is not prepared
	asCONTEXT_NOT_PREPARED                 = -4,
	//! Invalid argument
	asINVALID_ARG                          = -5,
	//! The function was not found
	asNO_FUNCTION                          = -6,
	//! Not supported
	asNOT_SUPPORTED                        = -7,
	//! Invalid name
	asINVALID_NAME                         = -8,
	//! The name is already taken
	asNAME_TAKEN                           = -9,
	//! Invalid declaration
	asINVALID_DECLARATION                  = -10,
	//! Invalid object
	asINVALID_OBJECT                       = -11,
	//! Invalid type
	asINVALID_TYPE                         = -12,
	//! Already registered
	asALREADY_REGISTERED                   = -13,
	//! Multiple matching functions
	asMULTIPLE_FUNCTIONS                   = -14,
	//! The module was not found
	asNO_MODULE                            = -15,
	//! The global variable was not found
	asNO_GLOBAL_VAR                        = -16,
	//! Invalid configuration
	asINVALID_CONFIGURATION                = -17,
	//! Invalid interface
	asINVALID_INTERFACE                    = -18,
	//! All imported functions couldn't be bound
	asCANT_BIND_ALL_FUNCTIONS              = -19,
	//! The array sub type has not been registered yet
	asLOWER_ARRAY_DIMENSION_NOT_REGISTERED = -20,
	//! Wrong configuration group
	asWRONG_CONFIG_GROUP                   = -21,
	//! The configuration group is in use
	asCONFIG_GROUP_IS_IN_USE               = -22,
	//! Illegal behaviour for the type
	asILLEGAL_BEHAVIOUR_FOR_TYPE           = -23,
	//! The specified calling convention doesn't match the function/method pointer
	asWRONG_CALLING_CONV                   = -24,
};

// Context states

//! \brief Context states.
enum asEContextState
{
	//! The context has successfully completed the execution.
    asEXECUTION_FINISHED      = 0,
    //! The execution is suspended and can be resumed.
    asEXECUTION_SUSPENDED     = 1,
    //! The execution was aborted by the application.
    asEXECUTION_ABORTED       = 2,
    //! The execution was terminated by an unhandled script exception.
    asEXECUTION_EXCEPTION     = 3,
    //! The context has been prepared for a new execution.
    asEXECUTION_PREPARED      = 4,
    //! The context is not initialized.
    asEXECUTION_UNINITIALIZED = 5,
    //! The context is currently executing a function call.
    asEXECUTION_ACTIVE        = 6,
    //! The context has encountered an error and must be reinitialized.
    asEXECUTION_ERROR         = 7,
};

// ExecuteString flags

//! \brief ExecuteString flags.
enum asEExecStrFlags
{
	//! Only prepare the context
	asEXECSTRING_ONLY_PREPARE	= 1,
	//! Use the pre-allocated context
	asEXECSTRING_USE_MY_CONTEXT = 2,
};

// Message types

//! \brief Compiler message types.
enum asEMsgType
{
	//! The message is an error.
    asMSGTYPE_ERROR       = 0,
    //! The message is a warning.
    asMSGTYPE_WARNING     = 1,
    //! The message is informational only.
    asMSGTYPE_INFORMATION = 2,
};

// Prepare flags
const int asPREPARE_PREVIOUS = -1;

// Config groups
const char * const asALL_MODULES = (const char * const)-1;

// Type id flags
enum asETypeIdFlags
{
	asTYPEID_OBJHANDLE      = 0x40000000,
	asTYPEID_HANDLETOCONST  = 0x20000000,
	asTYPEID_MASK_OBJECT    = 0x1C000000,
	asTYPEID_APPOBJECT      = 0x04000000,
	asTYPEID_SCRIPTSTRUCT   = 0x0C000000,
	asTYPEID_SCRIPTARRAY    = 0x10000000,
	asTYPEID_MASK_SEQNBR    = 0x03FFFFFF,
};

//!@}

//-----------------------------------------------------------------
// Function pointers

// Use our own memset() and memcpy() implementations for better portability
inline void asMemClear(void *_p, int size)
{
	char *p = (char *)_p;
	const char *e = p + size;
	for( ; p < e; p++ )
		*p = 0;
}

inline void asMemCopy(void *_d, const void *_s, int size)
{
	char *d = (char *)_d;
	const char *s = (const char *)_s;
	const char *e = s + size;
	for( ; s < e; d++, s++ )
		*d = *s;
}

// Template function to capture all global functions,
// except the ones using the generic calling convention
template <class T>
inline asSFuncPtr asFunctionPtr(T func)
{
	asSFuncPtr p;
	asMemClear(&p, sizeof(p));
	p.ptr.f.func = (asFUNCTION_t)(size_t)func;

	// Mark this as a global function
	p.flag = 2;

	return p;
}

// Specialization for functions using the generic calling convention
template<>
inline asSFuncPtr asFunctionPtr<asGENFUNC_t>(asGENFUNC_t func)
{
	asSFuncPtr p;
	asMemClear(&p, sizeof(p));
	p.ptr.f.func = (asFUNCTION_t)func;

	// Mark this as a generic function
	p.flag = 1;

	return p;
}

#ifndef AS_NO_CLASS_METHODS

// Method pointers

// Declare a dummy class so that we can determine the size of a simple method pointer
class asCSimpleDummy {};
typedef void (asCSimpleDummy::*asSIMPLEMETHOD_t)();
const int SINGLE_PTR_SIZE = sizeof(asSIMPLEMETHOD_t);

// Define template
template <int N>
struct asSMethodPtr
{
	template<class M>
	static asSFuncPtr Convert(M Mthd)
	{
		// This version of the function should never be executed, nor compiled,
		// as it would mean that the size of the method pointer cannot be determined.
		// int ERROR_UnsupportedMethodPtr[-1];
		return 0;
	}
};

// Template specialization
template <>
struct asSMethodPtr<SINGLE_PTR_SIZE>
{
	template<class M>
	static asSFuncPtr Convert(M Mthd)
	{
		asSFuncPtr p;
		asMemClear(&p, sizeof(p));

		asMemCopy(&p, &Mthd, SINGLE_PTR_SIZE);

		// Mark this as a class method
		p.flag = 3;

		return p;
	}
};

#if defined(_MSC_VER) && !defined(__MWERKS__)

// MSVC and Intel uses different sizes for different class method pointers
template <>
struct asSMethodPtr<SINGLE_PTR_SIZE+1*sizeof(int)>
{
	template <class M>
	static asSFuncPtr Convert(M Mthd)
	{
		asSFuncPtr p;
		asMemClear(&p, sizeof(p));

		asMemCopy(&p, &Mthd, SINGLE_PTR_SIZE+sizeof(int));

		// Mark this as a class method
		p.flag = 3;

		return p;
	}
};

template <>
struct asSMethodPtr<SINGLE_PTR_SIZE+2*sizeof(int)>
{
	template <class M>
	static asSFuncPtr Convert(M Mthd)
	{
		// This is where a class with virtual inheritance falls

		// Since method pointers of this type doesn't have all the
		// information we need we force a compile failure for this case.
		int ERROR_VirtualInheritanceIsNotAllowedForMSVC[-1];

		// The missing information is the location of the vbase table,
		// which is only known at compile time.

		// You can get around this by forward declaring the class and
		// storing the sizeof its method pointer in a constant. Example:

		// class ClassWithVirtualInheritance;
		// const int ClassWithVirtualInheritance_workaround = sizeof(void ClassWithVirtualInheritance::*());

		// This will force the compiler to use the unknown type
		// for the class, which falls under the next case

		asSFuncPtr p;
		return p;
	}
};

template <>
struct asSMethodPtr<SINGLE_PTR_SIZE+3*sizeof(int)>
{
	template <class M>
	static asSFuncPtr Convert(M Mthd)
	{
		asSFuncPtr p;
		asMemClear(&p, sizeof(p));

		asMemCopy(&p, &Mthd, SINGLE_PTR_SIZE+3*sizeof(int));

		// Mark this as a class method
		p.flag = 3;

		return p;
	}
};

#endif

#endif // AS_NO_CLASS_METHODS

END_AS_NAMESPACE

#endif
