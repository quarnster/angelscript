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
// as_datatype.cpp
//
// This class describes the datatype for expressions during compilation
//

#include "as_config.h"
#include "as_datatype.h"
#include "as_tokendef.h"
#include "as_objecttype.h"
#include "as_scriptengine.h"
#include "as_arrayobject.h"
#include "as_tokenizer.h"

asCString asCDataType::Format() const
{
	asCString str;

	if( isReadOnly )
		str = "const ";

	if( tokenType != ttIdentifier )
		str += asGetTokenDefinition(tokenType);
	else
	{
		if( extendedType == 0 )
			str += "<unknown>";
		else
			str += extendedType->name;
	}

	int n = pointerLevel;
	while( n-- )
		str += "*";

	n = arrayDimensions;
	while( n-- )
		str += "[]";

    if( isReference )
		str += "&";

	return str;
}


asCDataType &asCDataType::operator =(const asCDataType &dt)
{
	tokenType = dt.tokenType;
	extendedType = dt.extendedType;
	isReference = dt.isReference;
	pointerLevel = dt.pointerLevel;
	arrayDimensions = dt.arrayDimensions;
	objectType = dt.objectType;
	isReadOnly = dt.isReadOnly;

	return (asCDataType &)*this;
}

void asCDataType::SetAsDefaultArray(asCScriptEngine *engine)
{
	// void[] represents the default array
	arrayDimensions = 1;
	objectType      = engine->defaultArrayObjectType;
	extendedType    = 0;
	isReadOnly      = false;
	isReference     = false;
	pointerLevel    = 0;
	tokenType       = ttVoid;
}

bool asCDataType::IsDefaultArrayType(asCScriptEngine *engine) const
{
	if( arrayDimensions > 0 && objectType == engine->defaultArrayObjectType )
		return true;

	return false;
}

asCDataType asCDataType::GetSubType(asCScriptEngine *engine)
{
	asCDataType dt(*this);
	if( dt.arrayDimensions ) 
		dt.arrayDimensions--;
	else if( dt.pointerLevel ) 
		dt.pointerLevel--;

	if( dt.arrayDimensions > 0 )
		dt.objectType = engine->GetArrayType(dt);
	else if( dt.pointerLevel > 0 )
		dt.objectType = 0;
	else
		dt.objectType = dt.extendedType;

	return dt;
}


bool asCDataType::operator !=(const asCDataType &dt) const
{
	return !(*this == dt);
}

bool asCDataType::operator ==(const asCDataType &dt) const
{
	if( tokenType != dt.tokenType ) return false;
	if( extendedType != dt.extendedType ) return false;
	if( isReference != dt.isReference ) return false;
	if( pointerLevel != dt.pointerLevel ) return false;
	if( arrayDimensions != dt.arrayDimensions ) return false;
	if( isReadOnly != dt.isReadOnly ) return false;

	return true;
}

bool asCDataType::IsEqualExceptRef(const asCDataType &dt) const
{
	if( tokenType != dt.tokenType ) return false;
	if( extendedType != dt.extendedType ) return false;
	if( pointerLevel != dt.pointerLevel ) return false;
	if( arrayDimensions != dt.arrayDimensions ) return false;
	if( isReadOnly != dt.isReadOnly ) return false;

	return true;
}

bool asCDataType::IsEqualExceptRefAndConst(const asCDataType &dt) const
{
	if( tokenType != dt.tokenType ) return false;
	if( extendedType != dt.extendedType ) return false;
	if( pointerLevel != dt.pointerLevel ) return false;
	if( arrayDimensions != dt.arrayDimensions ) return false;

	return true;
}

bool asCDataType::IsEqualExceptConst(const asCDataType &dt) const
{
	if( tokenType != dt.tokenType ) return false;
	if( extendedType != dt.extendedType ) return false;
	if( pointerLevel != dt.pointerLevel ) return false;
	if( isReference != dt.isReference ) return false;
	if( arrayDimensions != dt.arrayDimensions ) return false;

	return true;
}

bool asCDataType::IsPrimitive() const
{
	// A registered object is never a primitive neither is a pointer, nor an array
	if( tokenType == ttIdentifier ||
		pointerLevel > 0 ||
		arrayDimensions > 0 )
	{
		return false;
	}

	return true;
}

bool asCDataType::IsSameBaseType(const asCDataType &dt) const
{
	if( pointerLevel != dt.pointerLevel ) return false;
	if( arrayDimensions != dt.arrayDimensions ) return false;

	if( IsIntegerType() && dt.IsIntegerType() ) return true;
	if( IsUnsignedType() && dt.IsUnsignedType() ) return true;
	if( IsFloatType() && dt.IsFloatType() ) return true;
	if( IsBitVectorType() && dt.IsBitVectorType() ) return true;
	if( IsDoubleType() && dt.IsDoubleType() ) return true;
	if( tokenType == dt.tokenType && extendedType == dt.extendedType ) return true;

	return false;
}

bool asCDataType::IsIntegerType() const
{
	if( pointerLevel > 0 ||
		arrayDimensions > 0 )
		return false;

	if( tokenType == ttInt ||
		tokenType == ttInt8 ||
		tokenType == ttInt16 )
		return true;

	return false;
}

bool asCDataType::IsUnsignedType() const
{
	if( pointerLevel > 0 ||
		arrayDimensions > 0 )
		return false;

	if( tokenType == ttUInt ||
		tokenType == ttUInt8 ||
		tokenType == ttUInt16 )
		return true;

	return false;
}

bool asCDataType::IsFloatType() const
{
	if( pointerLevel > 0 ||
		arrayDimensions > 0 )
		return false;

	if( tokenType == ttFloat )
		return true;

	return false;
}

bool asCDataType::IsDoubleType() const
{
	if( pointerLevel > 0 ||
		arrayDimensions > 0 )
		return false;

	if( tokenType == ttDouble )
		return true;

	return false;
}

bool asCDataType::IsBitVectorType() const
{
	if( pointerLevel > 0 ||
		arrayDimensions > 0 )
		return false;

	if( tokenType == ttBits ||
		tokenType == ttBits8 ||
		tokenType == ttBits16 )
		return true;

	return false;
}

bool asCDataType::IsBooleanType() const
{
	if( pointerLevel > 0 ||
		arrayDimensions > 0 )
		return false;

	if( tokenType == ttBool )
		return true;

	return false;
}

bool asCDataType::IsComplex(asCScriptEngine *engine) const
{
	if( IsPrimitive() ) return false;

	if( pointerLevel > 0 && arrayDimensions == 0 ) return false;

	// Only the construct, destruct, and copy behaviour make the type complex
	asSTypeBehaviour *beh = engine->GetBehaviour(this);
	if( beh && (beh->construct || beh->destruct || beh->copy) ) return true;

	return true;
}

int asCDataType::GetSizeInMemoryBytes() const
{
	if( objectType != 0 )
		return objectType->size;

	if( pointerLevel > 0 )
		return 4;

	if( tokenType == ttVoid )
		return 0;

	if( tokenType == ttInt8 ||
		tokenType == ttUInt8 ||
		tokenType == ttBits8 ||
		tokenType == ttBool )
		return 1;

	if( tokenType == ttInt16 ||
		tokenType == ttUInt16 ||
		tokenType == ttBits16 )
		return 2;

	if( tokenType == ttDouble )
		return 8;

	return 4;
}

int asCDataType::GetSizeInMemoryDWords() const
{
	int s = GetSizeInMemoryBytes();
	if( s == 0 ) return 0;
	if( s <= 4 ) return 1;
	
	return s/4;
}

int asCDataType::GetSizeOnStackDWords() const
{
	if( isReference ) return 1;

	return GetSizeInMemoryDWords();
}

