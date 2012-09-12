/**
 * Interface of Angelscript Stream for reading and writing in main memory
 */
#ifndef _AS_TEST_MEMORY_STREAM_H
#define _AS_TEST_MEMORY_STREAM_H

#include <vector>
#include <assert.h>
#include "utils.h"
#include "angelscript.h"

class CBytecodeStream : public asIBinaryStream
{
public:
	CBytecodeStream(const char *name) {wpointer = 0;rpointer = 0;}

	void Write(const void *ptr, asUINT size) 
	{
		if( size == 0 ) return; 
		buffer.resize(buffer.size() + size);
		memcpy(&buffer[wpointer], ptr, size); 
		wpointer += size;
		// Are we writing zeroes?
		for( asUINT n = 0; n < size; n++ )
			if( *(asBYTE*)ptr == 0 )
			{
				n = n; // <== Set break point here
				break;
			}
	}
	void Read(void *ptr, asUINT size) 
	{
		assert( rpointer + size <= buffer.size() );
		memcpy(ptr, &buffer[rpointer], size); 
		rpointer += size;
	}
	void Restart() {rpointer = 0;}

	asUINT CountZeroes() { asUINT z = 0; for( asUINT n = 0; n < buffer.size(); n++ ) if( buffer[n] == 0 ) z++; return z; }
	std::vector<asBYTE> buffer;

protected:
	int rpointer;
	int wpointer;
};
 
#endif
