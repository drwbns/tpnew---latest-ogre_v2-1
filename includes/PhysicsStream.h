// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//							         USER STREAM
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

#ifndef STREAM_H
#define STREAM_H

#include "foundation\PxIO.h"
#include "foundation/Px.h"
#include "foundation\PxAssert.h"

using namespace physx;

#include <stdio.h>

class UserInputStream : public PxInputStream
{
public:
								UserInputStream(const char* filename, bool load);
	virtual						~UserInputStream();

	virtual		PxU8			readByte()								const;
	virtual		PxU16			readWord()								const;
	virtual		PxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, PxU32 size)	const;

	virtual		PxInputStream&		storeByte(PxU8 b);
	virtual		PxInputStream&		storeWord(PxU16 w);
	virtual		PxInputStream&		storeDword(PxU32 d);
	virtual		PxInputStream&		storeFloat(PxReal f);
	virtual		PxInputStream&		storeDouble(PxF64 f);
	virtual		PxInputStream&		storeBuffer(const void* buffer, PxU32 size);

				FILE*			fp;
};

class MemoryWriteBuffer : public PxInputStream
{
	public:
								MemoryWriteBuffer();
	virtual						~MemoryWriteBuffer();

	virtual		PxU8			readByte()								const	{ PX_ASSERT(0);	return 0;	}
	virtual		PxU16			readWord()								const	{ PX_ASSERT(0);	return 0;	}
	virtual		PxU32			readDword()								const	{ PX_ASSERT(0);	return 0;	}
	virtual		float			readFloat()								const	{ PX_ASSERT(0);	return 0.0f;}
	virtual		double			readDouble()							const	{ PX_ASSERT(0);	return 0.0;	}
	virtual		void			readBuffer(void* /*buffer*/, PxU32 /*size*/)	const	{ PX_ASSERT(0);				}

	virtual		PxInputStream&		storeByte(PxU8 b);
	virtual		PxInputStream&		storeWord(PxU16 w);
	virtual		PxInputStream&		storeDword(PxU32 d);
	virtual		PxInputStream&		storeFloat(PxReal f);
	virtual		PxInputStream&		storeDouble(PxF64 f);
	virtual		PxInputStream&		storeBuffer(const void* buffer, PxU32 size);

				PxU32			currentSize;
				PxU32			maxSize;
				PxU8*			data;
};

class MemoryReadBuffer : public PxInputStream
{
	public:
								MemoryReadBuffer(const PxU8* data);
	virtual						~MemoryReadBuffer();

	virtual		PxU8			readByte()								const;
	virtual		PxU16			readWord()								const;
	virtual		PxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, PxU32 size)	const;

	virtual		PxInputStream&		storeByte(PxU8 /*b*/)							{ PX_ASSERT(0);	return *this;	}
	virtual		PxInputStream&		storeWord(PxU16 /*w*/)							{ PX_ASSERT(0);	return *this;	}
	virtual		PxInputStream&		storeDword(PxU32 /*d*/)							{ PX_ASSERT(0);	return *this;	}
	virtual		PxInputStream&		storeFloat(PxReal /*f*/)						{ PX_ASSERT(0);	return *this;	}
	virtual		PxInputStream&		storeDouble(PxF64 /*f*/)						{ PX_ASSERT(0);	return *this;	}
	virtual		PxInputStream&		storeBuffer(const void* /*buffer*/, PxU32 /*size*/)	{ PX_ASSERT(0);	return *this;	}

	mutable		const PxU8*		buffer;
};

#endif  // STREAM_H
