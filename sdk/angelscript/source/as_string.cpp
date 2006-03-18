/*
   ac_string.cpp - version 1.2, Aug 22nd, 2004

   acCString is a character string class, for easier
   manipulation of strings.

   Copyright (c) 2003 Andreas J�nsson

   This software is provided 'as-is', without any form of
   warranty. In no case will the author be held responsible
   for any damage caused by its use.

   Permission is granted to anyone to use the software for
   for any purpose, including commercial. It is also allowed
   to modify the software and redistribute it free of charge.
   The permission is granted with the following restrictions:

   1. The origin of the software may not be misrepresented.
      It must be plainly understandable who is the author of
      the original software.
   2. Altered versions must not be misrepresented as the
      original software, i.e. must be plainly marked as
      altered.
   3. This notice may not be removed or altered from any
      distribution of the software, altered or not.

   Andreas J�nsson
   andreas@angelcode.com
*/

#include <memory.h>
#include <stdarg.h>		// va_list, va_start(), etc
#include <stdlib.h>     // strtod(), strtol()
#include <assert.h>     // assert()

#include "as_config.h"
#include "as_string.h"

asCString::asCString()
{
	length = 0;
	bufferSize = 0;
	buffer = 0;

//	Copy("", 0);
}

// Copy constructor
asCString::asCString(const asCString &str)
{
	length = 0;
	bufferSize = 0;
	buffer = 0;

	Copy(str.buffer, str.length);
}

asCString::asCString(const char *str)
{
	length = 0;
	bufferSize = 0;
	buffer = 0;

	int len = strlen(str);
	Copy(str, len);
}

asCString::asCString(char ch)
{
	length = 0;
	bufferSize = 0;
	buffer = 0;

	Copy(&ch, 1);
}

asCString::~asCString()
{
	if( buffer )
		delete[] buffer;
}

asCString::operator const char *() const
{
	if( buffer == 0 ) return "";
	return buffer;
}

char *asCString::AddressOf()
{
	if( buffer == 0 ) Copy("", 0);
	return buffer;
}

void asCString::SetLength(int len)
{
	if( len >= bufferSize )
		Allocate(len, true);

	buffer[len] = 0;
}

void asCString::Allocate(int len, bool keepData)
{
	char *buf = new char[len+1];
	bufferSize = len+1;

	if( buffer )
	{
		if( keepData )
		{
			if( len < length )
				length = len;
			memcpy(buf, buffer, length);
		}

		delete[] buffer;
	}

	buffer = buf;

	// If no earlier data, set to empty string
	if( !keepData )
		length = 0;

	// Make sure the buffer is null terminated
	buffer[length] = 0;
}

void asCString::Copy(const char *str, int len)
{
	// Allocate memory for the string
	if( bufferSize < len + 1 )
		Allocate(len, false);

	// Copy the string
	length = len;
	memcpy(buffer, str, length);
	buffer[length] = 0;
}

asCString &asCString::operator =(const char *str)
{
	int len = strlen(str);
	Copy(str, len);

	return *this;
}

asCString &asCString::operator =(const asCString &str)
{
	Copy(str.buffer, str.length);

	return *this;
}

asCString &asCString::operator =(char ch)
{
	Copy(&ch, 1);

	return *this;
}

void asCString::Concatenate(const char *str, int len)
{
	// Allocate memory for the string
	if( bufferSize < length + len + 1 )
		Allocate(length + len, true);

	memcpy(&buffer[length], str, len);
	length = length + len;
	buffer[length] = 0;
}

asCString &asCString::operator +=(const char *str)
{
	int len = strlen(str);
	Concatenate(str, len);

	return *this;
}

asCString &asCString::operator +=(const asCString &str)
{
	Concatenate(str.buffer, str.length);

	return *this;
}

asCString &asCString::operator +=(char ch)
{
	Concatenate(&ch, 1);

	return *this;
}

int asCString::GetLength() const
{
	return length;
}

// Returns the length
int asCString::Format(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	char tmp[256];
	int r = vsnprintf(tmp, 255, format, args);

	if( r > 0 )
	{
		Copy(tmp, r);
	}
	else
	{
		int n = 512;
		asCString str; // Use temporary string in case the current buffer is a parameter
		str.Allocate(n, false);

		while( (r = vsnprintf(str.buffer, n, format, args)) < 0 )
		{
			n *= 2;
			str.Allocate(n, false);
		}

		Copy(str.buffer, r);
	}

	va_end(args);

	return length;
}

char &asCString::operator [](int index) const
{
	assert(index >= 0);
	assert(index < length);

	return buffer[index];
}

asCString asCString::SubString(int start, int length) const
{
	if( start >= GetLength() || length == 0 )
		return asCString("");

	if( start < 0 ) start = 0;
	if( length == -1 ) length = GetLength() - start;

	asCString tmp;
	tmp.Copy(buffer + start, length);

	return tmp;
}

int asCString::Compare(const char *str) const
{
	if( buffer == 0 ) return strcmp("",str);
	return strcmp(buffer, str);
}

int asCString::RecalculateLength()
{
	if( buffer == 0 ) Copy("", 0);
	length = strlen(buffer);

	assert(length < bufferSize);

	return length;
}

//-----------------------------------------------------------------------------
// Helper functions

bool operator ==(const asCString &a, const char *b)
{
	return a.Compare(b) == 0;
}

bool operator !=(const asCString &a, const char *b)
{
	return a.Compare(b) != 0;
}

asCString operator +(const asCString &a, const asCString &b)
{
	asCString res = a;
	res += b;

	return res;
}

asCString operator +(const char *a, const asCString &b)
{
	asCString res = a;
	res += b;

	return res;
}

asCString operator +(const asCString &a, const char *b)
{
	asCString res = a;
	res += b;

	return res;
}

