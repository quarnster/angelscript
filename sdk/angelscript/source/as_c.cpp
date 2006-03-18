/*
   AngelCode Scripting Library
   Copyright (c) 2003-2005 Andreas J�nsson

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

   Andreas J�nsson
   andreas@angelcode.com
*/


//
// as_c.cpp
//
// A C interface to the library 
//

#include "as_config.h"

#ifdef AS_C_INTERFACE

class asCOutputStreamC : public asIOutputStream
{
public:
	asCOutputStreamC(asOUTPUTFUNC_t func, void *param) {this->func = func; this->param = param;}

	void Write(const char *text) { func(text, param); }

	asOUTPUTFUNC_t func;
	void          *param;
};

class asCBinaryStreamC : public asIBinaryStream
{
public:
	asCBinaryStreamC(asBINARYFUNC_t write, asBINARYFUNC_t read, void *param) {this->write = write; this->read = read; this->param = param;}

	void Write(void *ptr, int size) { write(ptr, size, param); }
	void Read(void *ptr, int size) { read(ptr, size, param); }

	asBINARYFUNC_t read;
	asBINARYFUNC_t write;
	void          *param;
};

int         asEngine_AddRef(asIScriptEngine *e)                                                                                                                                { return e->AddRef(); }
int         asEngine_Release(asIScriptEngine *e)                                                                                                                               { return e->Release(); }
int         asEngine_RegisterObjectType(asIScriptEngine *e, const char *name, int byteSize, asDWORD flags)                                                                     { return e->RegisterObjectType(name, byteSize, flags); }
int         asEngine_RegisterObjectProperty(asIScriptEngine *e, const char *obj, const char *declaration, int byteOffset)                                                      { return e->RegisterObjectProperty(obj, declaration, byteOffset); }
int         asEngine_RegisterObjectMethod(asIScriptEngine *e, const char *obj, const char *declaration, asFUNCTION_t funcPointer, asDWORD callConv)                            { return e->RegisterObjectMethod(obj, declaration, asFUNCTION(funcPointer), callConv); }
int         asEngine_RegisterObjectBehaviour(asIScriptEngine *e, const char *datatype, asDWORD behaviour, const char *declaration, asFUNCTION_t funcPointer, asDWORD callConv) { return e->RegisterObjectBehaviour(datatype, behaviour, declaration, asFUNCTION(funcPointer), callConv); }
int         asEngine_RegisterGlobalProperty(asIScriptEngine *e, const char *declaration, void *pointer)																		   { return e->RegisterGlobalProperty(declaration, pointer); }
int         asEngine_RegisterGlobalFunction(asIScriptEngine *e, const char *declaration, asFUNCTION_t funcPointer, asDWORD callConv)                                           { return e->RegisterGlobalFunction(declaration, asFUNCTION(funcPointer), callConv); }
int         asEngine_RegisterGlobalBehaviour(asIScriptEngine *e, asDWORD behaviour, const char *declaration, asFUNCTION_t funcPointer, asDWORD callConv)                       { return e->RegisterGlobalBehaviour(behaviour, declaration, asFUNCTION(funcPointer), callConv); }
int         asEngine_RegisterStringFactory(asIScriptEngine *e, const char *datatype, asFUNCTION_t factoryFunc, asDWORD callConv)                                               { return e->RegisterStringFactory(datatype, asFUNCTION(factoryFunc), callConv); }
int         asEngine_AddScriptSection(asIScriptEngine *e, const char *module, const char *name, const char *code, int codeLength, int lineOffset, bool makeCopy)               { return e->AddScriptSection(module, name, code, codeLength, lineOffset, makeCopy); }
int         asEngine_Build(asIScriptEngine *e, const char *module, asOUTPUTFUNC_t outFunc, void *outParam)                                                                     { asCOutputStreamC out(outFunc, outParam); return e->Build(module, &out); }
int         asEngine_Discard(asIScriptEngine *e, const char *module)                                                                                                           { return e->Discard(module); }
int         asEngine_GetModuleIndex(asIScriptEngine *e, const char *module)                                                                                                    { return e->GetModuleIndex(module); }
const char *asEngine_GetModuleNameFromIndex(asIScriptEngine *e, int index, int *length)                                                                                        { return e->GetModuleNameFromIndex(index, length); }
int         asEngine_GetFunctionCount(asIScriptEngine *e, const char *module)                                                                                                  { return e->GetFunctionCount(module); }
int         asEngine_GetFunctionIDByIndex(asIScriptEngine *e, const char *module, int index)                                                                                   { return e->GetFunctionIDByIndex(module, index); }
int         asEngine_GetFunctionIDByName(asIScriptEngine *e, const char *module, const char *name)                                                                             { return e->GetFunctionIDByName(module, name); }
int         asEngine_GetFunctionIDByDecl(asIScriptEngine *e, const char *module, const char *decl)                                                                             { return e->GetFunctionIDByDecl(module, decl); }
const char *asEngine_GetFunctionDeclaration(asIScriptEngine *e, int funcID, int *length)                                                                                       { return e->GetFunctionDeclaration(funcID, length); }
const char *asEngine_GetFunctionName(asIScriptEngine *e, int funcID, int *length)                                                                                              { return e->GetFunctionName(funcID, length); }
const char *asEngine_GetFunctionSection(asIScriptEngine *e, int funcID, int *length)                                                                                           { return e->GetFunctionSection(funcID, length); }
int         asEngine_GetGlobalVarCount(asIScriptEngine *e, const char *module)                                                                                                 { return e->GetGlobalVarCount(module); }
int         asEngine_GetGlobalVarIDByIndex(asIScriptEngine *e, const char *module, int index)                                                                                  { return e->GetGlobalVarIDByIndex(module, index); }
int         asEngine_GetGlobalVarIDByName(asIScriptEngine *e, const char *module, const char *name)                                                                            { return e->GetGlobalVarIDByName(module, name); }
int         asEngine_GetGlobalVarIDByDecl(asIScriptEngine *e, const char *module, const char *decl)                                                                            { return e->GetGlobalVarIDByDecl(module, decl); }
const char *asEngine_GetGlobalVarDeclaration(asIScriptEngine *e, int gvarID, int *length)                                                                                      { return e->GetGlobalVarDeclaration(gvarID, length); }
const char *asEngine_GetGlobalVarName(asIScriptEngine *e, int gvarID, int *length)                                                                                             { return e->GetGlobalVarName(gvarID, length); }
int         asEngine_GetGlobalVarPointer(asIScriptEngine *e, int gvarID, void **pointer)                                                                                       { return e->GetGlobalVarPointer(gvarID, pointer); }
int         asEngine_GetImportedFunctionCount(asIScriptEngine *e, const char *module)                                                                                          { return e->GetImportedFunctionCount(module); }
int         asEngine_GetImportedFunctionIndexByDecl(asIScriptEngine *e, const char *module, const char *decl)                                                                  { return e->GetImportedFunctionIndexByDecl(module, decl); }
const char *asEngine_GetImportedFunctionDeclaration(asIScriptEngine *e, const char *module, int importIndex, int *length)                                                      { return e->GetImportedFunctionDeclaration(module, importIndex, length); }
const char *asEngine_GetImportedFunctionSourceModule(asIScriptEngine *e, const char *module, int importIndex, int *length)                                                     { return e->GetImportedFunctionSourceModule(module, importIndex, length); }
int         asEngine_BindImportedFunction(asIScriptEngine *e, const char *module, int importIndex, int funcID)                                                                 { return e->BindImportedFunction(module, importIndex, funcID); }
int         asEngine_UnbindImportedFunction(asIScriptEngine *e, const char *module, int importIndex)                                                                           { return e->UnbindImportedFunction(module, importIndex); }
int         asEngine_BindAllImportedFunctions(asIScriptEngine *e, const char *module)                                                                                          { return e->BindAllImportedFunctions(module); }
int         asEngine_UnbindAllImportedFunctions(asIScriptEngine *e, const char *module)                                                                                        { return e->UnbindAllImportedFunctions(module); }
int         asEngine_SetDefaultContextStackSize(asIScriptEngine *e, asUINT initial, asUINT maximum)                                                                            { return e->SetDefaultContextStackSize(initial, maximum); }
int         asEngine_CreateContext(asIScriptEngine *e, asIScriptContext **context)                                                                                             { return e->CreateContext(context); }
int         asEngine_ExecuteString(asIScriptEngine *e, const char *module, const char *script, asOUTPUTFUNC_t outFunc, void *outParam, asIScriptContext **ctx, asDWORD flags)  { asCOutputStreamC out(outFunc, outParam); return e->ExecuteString(module, script, &out, ctx, flags); }
int         asEngine_SaveByteCode(asIScriptEngine *e, const char *module, asBINARYFUNC_t outFunc, void *outParam)                                                              { asCBinaryStreamC out(outFunc, 0, outParam); return e->SaveByteCode(module, &out); }
int         asEngine_LoadByteCode(asIScriptEngine *e, const char *module, asBINARYFUNC_t inFunc, void *inParam)                                                                { asCBinaryStreamC in(0, inFunc, inParam); return e->LoadByteCode(module, &in); }

int              asContext_AddRef(asIScriptContext *c)                                                         { return c->AddRef(); }
int              asContext_Release(asIScriptContext *c)                                                        { return c->Release(); }
asIScriptEngine *asContext_GetEngine(asIScriptContext *c)                                                      { return c->GetEngine(); }
int              asContext_GetState(asIScriptContext *c)                                                       { return c->GetState(); }
int              asContext_Prepare(asIScriptContext *c, int funcID)                                            { return c->Prepare(funcID); }
int              asContext_SetArgDWord(asIScriptContext *c, asUINT arg, asDWORD value)                         { return c->SetArgDWord(arg, value); } 
int              asContext_SetArgQWord(asIScriptContext *c, asUINT arg, asQWORD value)                         { return c->SetArgQWord(arg, value); }
int              asContext_SetArgFloat(asIScriptContext *c, asUINT arg, float value)                           { return c->SetArgFloat(arg, value); }
int              asContext_SetArgDouble(asIScriptContext *c, asUINT arg, double value)                         { return c->SetArgDouble(arg, value); }
int              asContext_SetArgObject(asIScriptContext *c, asUINT arg, void *obj)                            { return c->SetArgObject(arg, obj); }
asDWORD          asContext_GetReturnDWord(asIScriptContext *c)                                                 { return c->GetReturnDWord(); }
asQWORD          asContext_GetReturnQWord(asIScriptContext *c)                                                 { return c->GetReturnQWord(); }
float            asContext_GetReturnFloat(asIScriptContext *c)                                                 { return c->GetReturnFloat(); }
double           asContext_GetReturnDouble(asIScriptContext *c)                                                { return c->GetReturnDouble(); }
void *           asContext_GetReturnObject(asIScriptContext *c)                                                { return c->GetReturnObject(); }
int              asContext_Execute(asIScriptContext *c)                                                        { return c->Execute(); }
#ifdef AS_DEPRECATED
int              asContext_ExecuteStep(asIScriptContext *c, asDWORD flag)                                      { return c->ExecuteStep(flag); }
#endif
int              asContext_Abort(asIScriptContext *c)                                                          { return c->Abort(); }
int              asContext_Suspend(asIScriptContext *c)                                                        { return c->Suspend(); }
int              asContext_GetCurrentLineNumber(asIScriptContext *c, int *column)                              { return c->GetCurrentLineNumber(column); }
int              asContext_GetCurrentFunction(asIScriptContext *c)                                             { return c->GetCurrentFunction(); }
int              asContext_SetException(asIScriptContext *c, const char *string)                               { return c->SetException(string); }
int              asContext_GetExceptionLineNumber(asIScriptContext *c, int *column)                            { return c->GetExceptionLineNumber(column); }
int              asContext_GetExceptionFunction(asIScriptContext *c)                                           { return c->GetExceptionFunction(); }
const char *     asContext_GetExceptionString(asIScriptContext *c, int *length)                                { return c->GetExceptionString(length); }
int              asContext_SetLineCallback(asIScriptContext *c, asUPtr callback, void *obj, int callConv)      { return c->SetLineCallback(callback, obj, callConv); }
void             asContext_ClearLineCallback(asIScriptContext *c)                                              { c->ClearLineCallback(); }
int              asContext_SetExceptionCallback(asIScriptContext *c, asUPtr callback, void *obj, int callConv) { return c->SetExceptionCallback(callback, obj, callConv); }
void             asContext_ClearExceptionCallback(asIScriptContext *c)                                         { c->ClearExceptionCallback(); }
int              asContext_GetCallstackSize(asIScriptContext *c)                                               { return c->GetCallstackSize(); }
int              asContext_GetCallstackFunction(asIScriptContext *c, int index)                                { return c->GetCallstackFunction(index); }
int              asContext_GetCallstackLineNumber(asIScriptContext *c, int index, int *column)                 { return c->GetCallstackLineNumber(index, column); }

asIScriptEngine *asGeneric_GetEngine(asIScriptGeneric *g)                                                      { return g->GetEngine(); }
void *           asGeneric_GetObject(asIScriptGeneric *g)                                                      { return g->GetObject(); }
asDWORD          asGeneric_GetArgDWord(asIScriptGeneric *g, asUINT arg)                                        { return g->GetArgDWord(arg); }
asQWORD          asGeneric_GetArgQWord(asIScriptGeneric *g, asUINT arg)                                        { return g->GetArgQWord(arg); }
float            asGeneric_GetArgFloat(asIScriptGeneric *g, asUINT arg)                                        { return g->GetArgFloat(arg); }
double           asGeneric_GetArgDouble(asIScriptGeneric *g, asUINT arg)                                       { return g->GetArgDouble(arg); }
void *           asGeneric_GetArgObject(asIScriptGeneric *g, asUINT arg)                                       { return g->GetArgObject(arg); }
int              asGeneric_SetReturnDWord(asIScriptGeneric *g, asDWORD val)                                    { return g->SetReturnDWord(val); }
int              asGeneric_SetReturnQWord(asIScriptGeneric *g, asQWORD val)                                    { return g->SetReturnQWord(val); }
int              asGeneric_SetReturnFloat(asIScriptGeneric *g, float val)                                      { return g->SetReturnFloat(val); }
int              asGeneric_SetReturnDouble(asIScriptGeneric *g, double val)                                    { return g->SetReturnDouble(val); }
int              asGeneric_SetReturnObject(asIScriptGeneric *g, void *obj)                                     { return g->SetReturnObject(obj); }

#endif

