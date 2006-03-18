/*
   AngelCode Scripting Library
   Copyright (c) 2003-2004 Andreas J�nsson

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
// as_callfunc.h
//
// These functions handle the actual calling of system functions
//


#ifndef AS_CALLFUNC_H
#define AS_CALLFUNC_H

#include "as_context.h"

class asCScriptFunction;
struct asSSystemFunctionInterface;

int DetectCallingConvention(const char *, asUPtr &ptr, int callConv, asSSystemFunctionInterface *internal);

int PrepareSystemFunction(asCScriptFunction *func, asSSystemFunctionInterface *internal, asCScriptEngine *engine);

int CallSystemFunction(int id, asCContext *context);

enum internalCallConv
{
	ICC_CDECL,
	ICC_CDECL_RETURNINMEM,
	ICC_STDCALL,
	ICC_STDCALL_RETURNINMEM,
	ICC_THISCALL,
	ICC_THISCALL_RETURNINMEM,
	ICC_VIRTUAL_THISCALL,
	ICC_VIRTUAL_THISCALL_RETURNINMEM,
	ICC_CDECL_OBJLAST,
	ICC_CDECL_OBJLAST_RETURNINMEM,
	ICC_CDECL_OBJFIRST,
	ICC_CDECL_OBJFIRST_RETURNINMEM,
};

struct asSSystemFunctionInterface
{
	asDWORD              func;
	int                  baseOffset;
	internalCallConv     callConv;
	bool                 scriptReturnInMemory;
	int                  scriptReturnSize;
	bool                 hostReturnInMemory;
	bool                 hostReturnFloat;
	int                  hostReturnSize;
	int                  paramSize;
};


#endif

