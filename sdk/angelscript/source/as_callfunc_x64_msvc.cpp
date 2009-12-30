/*
   AngelCode Scripting Library
   Copyright (c) 2003-2009 Andreas Jonsson

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


#include <stdio.h>

#include "as_config.h"

#ifndef AS_MAX_PORTABILITY
#ifdef AS_X64_MSVC

#include "as_callfunc.h"
#include "as_scriptengine.h"
#include "as_texts.h"

BEGIN_AS_NAMESPACE

// These functions are implemented in as_callfunc_x64_msvc.asm
extern "C" asQWORD CallX64(const asQWORD *args, const asQWORD *floatArgs, int paramSize, asQWORD func);
extern "C" asDWORD GetReturnedFloat();
extern "C" asQWORD GetReturnedDouble();

int CallSystemFunction(int id, asCContext *context, void *objectPointer)
{
	asCScriptEngine *engine = context->engine;
	asCScriptFunction *descr = engine->scriptFunctions[id];
	asSSystemFunctionInterface *sysFunc = descr->sysFuncIntf;
	int callConv = sysFunc->callConv;
	if( callConv == ICC_GENERIC_FUNC || callConv == ICC_GENERIC_METHOD )
		return context->CallGeneric(id, objectPointer);

	asQWORD  retQW             = 0;
	void    *func              = (void*)sysFunc->func;
	int      paramSize         = sysFunc->paramSize;
	asDWORD *args              = context->regs.stackPointer;
	void    *retPointer        = 0;
	void    *obj               = 0;
	asDWORD *vftable;
	int      popSize           = paramSize;

	asQWORD  allArgBuffer[64];
	asQWORD  floatArgBuffer[4];

	context->regs.objectType = descr->returnType.GetObjectType();
	if( descr->returnType.IsObject() && !descr->returnType.IsReference() && !descr->returnType.IsObjectHandle() )
	{
		// Allocate the memory for the object
		retPointer = engine->CallAlloc(descr->returnType.GetObjectType());

		if( sysFunc->hostReturnInMemory )
		{
			// The return is made in memory
			callConv++;
		}
	}

	if( callConv >= ICC_THISCALL )
	{
		if( objectPointer )
		{
			obj = objectPointer;
		}
		else
		{
			// The object pointer should be popped from the context stack
			popSize++;

			// Check for null pointer
			obj = (void*)*(size_t*)(args);
			if( obj == 0 )
			{
				context->SetInternalException(TXT_NULL_POINTER_ACCESS);
				if( retPointer )
					engine->CallFree(retPointer);
				return 0;
			}

			// Add the base offset for multiple inheritance
			obj = (void*)(size_t(obj) + sysFunc->baseOffset);

			// Skip the object pointer
			args++;
		}
	}

	asDWORD paramBuffer[64];
	if( sysFunc->takesObjByVal )
	{
		paramSize = 0;
		int spos = 0;
		int dpos = 1;
		for( asUINT n = 0; n < descr->parameterTypes.GetLength(); n++ )
		{
			if( descr->parameterTypes[n].IsObject() && !descr->parameterTypes[n].IsObjectHandle() && !descr->parameterTypes[n].IsReference() )
			{
#ifdef COMPLEX_OBJS_PASSED_BY_REF
				if( descr->parameterTypes[n].GetObjectType()->flags & COMPLEX_MASK )
				{
					paramBuffer[dpos++] = args[spos++];
					paramSize++;
				}
				else
#endif
				{
					// Copy the object's memory to the buffer
					memcpy(&paramBuffer[dpos], *(void**)(args+spos), descr->parameterTypes[n].GetSizeInMemoryBytes());

					// Delete the original memory
					engine->CallFree(*(char**)(args+spos));
					spos++;
					dpos += descr->parameterTypes[n].GetSizeInMemoryDWords();
					paramSize += descr->parameterTypes[n].GetSizeInMemoryDWords();
				}
			}
			else
			{
				// Copy the value directly
				paramBuffer[dpos++] = args[spos++];
				if( descr->parameterTypes[n].GetSizeOnStackDWords() > 1 )
					paramBuffer[dpos++] = args[spos++];
				paramSize += descr->parameterTypes[n].GetSizeOnStackDWords();
			}
		}
		// Keep a free location at the beginning
		args = &paramBuffer[1];
	}

	context->isCallingSystemFunction = true;
	switch( callConv )
	{
	case ICC_CDECL:
		retQW = CallX64(allArgBuffer, floatArgBuffer, paramSize<<2, (size_t)func);
		break;

	case ICC_CDECL_RETURNINMEM:
		retQW = CallX64(allArgBuffer, floatArgBuffer, paramSize<<2, (size_t)func);
		break;

/*	case ICC_STDCALL:
		retQW = CallX64(args, paramSize<<2, (size_t)func);
		break;

	case ICC_STDCALL_RETURNINMEM:
		// Push the return pointer on the stack
		paramSize++;
		args--;
		*(size_t*)args = (size_t)retPointer;

		retQW = CallX64(args, paramSize<<2, (size_t)func);
		break;

	case ICC_THISCALL:
		retQW = CallX64(obj, args, paramSize<<2, (size_t)func);
		break;

	case ICC_THISCALL_RETURNINMEM:
		retQW = CallX64(obj, args, paramSize<<2, (size_t)func, retPointer);
		break;

	case ICC_VIRTUAL_THISCALL:
		// Get virtual function table from the object pointer
		vftable = *(asDWORD**)obj;

		retQW = CallX64(obj, args, paramSize<<2, vftable[size_t(func)>>2]);
		break;

	case ICC_VIRTUAL_THISCALL_RETURNINMEM:
		// Get virtual function table from the object pointer
		vftable = *(asDWORD**)obj;

		retQW = CallX64(obj, args, paramSize<<2, vftable[size_t(func)>>2], retPointer);
		break;

	case ICC_CDECL_OBJLAST:
		retQW = CallX64(obj, args, paramSize<<2, (size_t)func);
		break;

	case ICC_CDECL_OBJLAST_RETURNINMEM:
		// Call the system object method as a cdecl with the obj ref as the last parameter
		retQW = CallX64(obj, args, paramSize<<2, (size_t)func, retPointer);
		break;

	case ICC_CDECL_OBJFIRST:
		// Call the system object method as a cdecl with the obj ref as the first parameter
		retQW = CallX64(obj, args, paramSize<<2, (size_t)func);
		break;

	case ICC_CDECL_OBJFIRST_RETURNINMEM:
		// Call the system object method as a cdecl with the obj ref as the first parameter
		retQW = CallX64(obj, args, paramSize<<2, (size_t)func, retPointer);
		break;
*/
	default:
		context->SetInternalException(TXT_INVALID_CALLING_CONVENTION);
	}
	context->isCallingSystemFunction = false;

#ifdef COMPLEX_OBJS_PASSED_BY_REF
	if( sysFunc->takesObjByVal )
	{
		// Need to free the complex objects passed by value
		args = context->regs.stackPointer;
		if( callConv >= ICC_THISCALL && !objectPointer )
		    args++;

		int spos = 0;
		for( asUINT n = 0; n < descr->parameterTypes.GetLength(); n++ )
		{
			if( descr->parameterTypes[n].IsObject() &&
				!descr->parameterTypes[n].IsReference() &&
				(descr->parameterTypes[n].GetObjectType()->flags & COMPLEX_MASK) )
			{
				void *obj = (void*)args[spos++];
				asSTypeBehaviour *beh = &descr->parameterTypes[n].GetObjectType()->beh;
				if( beh->destruct )
					engine->CallObjectMethod(obj, beh->destruct);

				engine->CallFree(obj);
			}
			else
				spos += descr->parameterTypes[n].GetSizeInMemoryDWords();
		}
	}
#endif

	// Store the returned value in our stack
	if( descr->returnType.IsObject() && !descr->returnType.IsReference() )
	{
		if( descr->returnType.IsObjectHandle() )
		{
			context->regs.objectRegister = (void*)(size_t)retQW;

			if( sysFunc->returnAutoHandle && context->regs.objectRegister )
				engine->CallObjectMethod(context->regs.objectRegister, descr->returnType.GetObjectType()->beh.addref);
		}
		else
		{
			if( !sysFunc->hostReturnInMemory )
			{
				// Copy the returned value to the pointer sent by the script engine
				if( sysFunc->hostReturnSize == 1 )
					*(asDWORD*)retPointer = (asDWORD)retQW;
				else
					*(asQWORD*)retPointer = retQW;
			}

			// Store the object in the register
			context->regs.objectRegister = retPointer;
		}
	}
	else
	{
		// Store value in value register
		if( sysFunc->hostReturnFloat )
		{
			if( sysFunc->hostReturnSize == 1 )
				*(asDWORD*)&context->regs.valueRegister = GetReturnedFloat();
			else
				context->regs.valueRegister = GetReturnedDouble();
		}
		else if( sysFunc->hostReturnSize == 1 )
			*(asDWORD*)&context->regs.valueRegister = (asDWORD)retQW;
		else
			context->regs.valueRegister = retQW;
	}

	if( sysFunc->hasAutoHandles )
	{
		args = context->regs.stackPointer;
		if( callConv >= ICC_THISCALL && !objectPointer )
			args++;

		int spos = 0;
		for( asUINT n = 0; n < descr->parameterTypes.GetLength(); n++ )
		{
			if( sysFunc->paramAutoHandles[n] && args[spos] )
			{
				// Call the release method on the type
				engine->CallObjectMethod((void*)*(size_t*)&args[spos], descr->parameterTypes[n].GetObjectType()->beh.release);
				args[spos] = 0;
			}

			if( descr->parameterTypes[n].IsObject() && !descr->parameterTypes[n].IsObjectHandle() && !descr->parameterTypes[n].IsReference() )
				spos++;
			else
				spos += descr->parameterTypes[n].GetSizeOnStackDWords();
		}
	}

	return popSize;
}


END_AS_NAMESPACE

#endif // AS_X64_MSVC
#endif // AS_MAX_PORTABILITY


