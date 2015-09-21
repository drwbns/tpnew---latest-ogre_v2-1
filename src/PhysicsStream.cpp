#include "PhysicsStream.h"

#include "PxPhysics.h"
#include "foundation\PxSimpleTypes.h"



UserInputStream::UserInputStream(const char* filename, bool load) : fp(NULL)
{
	fopen_s(&fp, filename, load ? "rb" : "wb");
}

UserInputStream::~UserInputStream()
{
	if(fp)	fclose(fp);
}

// Loading API
PxU8 UserInputStream::readByte() const
{
	PxU8 b;
	size_t r = fread(&b, sizeof(PxU8), 1, fp);
	PX_ASSERT(r);
	return b;
}

PxU16 UserInputStream::readWord() const
{
	PxU16 w;
	size_t r = fread(&w, sizeof(PxU16), 1, fp);
	PX_ASSERT(r);
	return w;
}

PxU32 UserInputStream::readDword() const
{
	PxU32 d;
	size_t r = fread(&d, sizeof(PxU32), 1, fp);
	PX_ASSERT(r);
	return d;
}

float UserInputStream::readFloat() const
{
	PxReal f;
	size_t r = fread(&f, sizeof(PxReal), 1, fp);
	PX_ASSERT(r);
	return f;
}

double UserInputStream::readDouble() const
{
	PxF64 f;
	size_t r = fread(&f, sizeof(PxF64), 1, fp);
	PX_ASSERT(r);
	return f;
}

void UserInputStream::readBuffer(void* buffer, PxU32 size)	const
{
	size_t w = fread(buffer, size, 1, fp);
	PX_ASSERT(w);
}

// Saving API
PxInputStream& UserInputStream::storeByte(PxU8 b)
{
	size_t w = fwrite(&b, sizeof(PxU8), 1, fp);
	PX_ASSERT(w);
	return *this;
}

PxInputStream& UserInputStream::storeWord(PxU16 w)
{
	size_t ww = fwrite(&w, sizeof(PxU16), 1, fp);
	PX_ASSERT(ww);
	return *this;
}

PxInputStream& UserInputStream::storeDword(PxU32 d)
{
	size_t w = fwrite(&d, sizeof(PxU32), 1, fp);
	PX_ASSERT(w);
	return *this;
}

PxInputStream& UserInputStream::storeFloat(PxReal f)
{
	size_t w = fwrite(&f, sizeof(PxReal), 1, fp);
	PX_ASSERT(w);
	return *this;
}

PxInputStream& UserInputStream::storeDouble(PxF64 f)
{
	size_t w = fwrite(&f, sizeof(PxF64), 1, fp);
	PX_ASSERT(w);
	return *this;
}

PxInputStream& UserInputStream::storeBuffer(const void* buffer, PxU32 size)
{
	size_t w = fwrite(buffer, size, 1, fp);
	PX_ASSERT(w);
	return *this;
}



MemoryWriteBuffer::MemoryWriteBuffer() : currentSize(0), maxSize(0), data(NULL)
{
}

MemoryWriteBuffer::~MemoryWriteBuffer()
{
	//NxGetPhysicsSDKAllocator()->free(data);
}

PxInputStream& MemoryWriteBuffer::storeByte(PxU8 b)
{
	storeBuffer(&b, sizeof(PxU8));
	return *this;
}

PxInputStream& MemoryWriteBuffer::storeWord(PxU16 w)
{
	storeBuffer(&w, sizeof(PxU16));
	return *this;
}

PxInputStream& MemoryWriteBuffer::storeDword(PxU32 d)
{
	storeBuffer(&d, sizeof(PxU32));
	return *this;
}

PxInputStream& MemoryWriteBuffer::storeFloat(PxReal f)
{
	storeBuffer(&f, sizeof(PxReal));
	return *this;
}

PxInputStream& MemoryWriteBuffer::storeDouble(PxF64 f)
{
	storeBuffer(&f, sizeof(PxF64));
	return *this;
}

PxInputStream& MemoryWriteBuffer::storeBuffer(const void* buffer, PxU32 size)
{
	PxU32 expectedSize = currentSize + size;
	if(expectedSize > maxSize)
	{
		maxSize = expectedSize + 4096;
		/*
		PxU8* newData = (PxU8*)NxGetPhysicsSDKAllocator()->malloc(maxSize, NX_MEMORY_PERSISTENT);
		if(data)
		{
			memcpy(newData, data, currentSize);
			NxGetPhysicsSDKAllocator()->free(data);
		}
		data = newData;
		*/
	}
	memcpy(data+currentSize, buffer, size);
	currentSize += size;
	return *this;
}

MemoryReadBuffer::MemoryReadBuffer(const PxU8* data) : buffer(data)
{
}

MemoryReadBuffer::~MemoryReadBuffer()
{
	// We don't own the data => no delete
}

PxU8 MemoryReadBuffer::readByte() const
{
	PxU8 b;
	memcpy(&b, buffer, sizeof(PxU8));
	buffer += sizeof(PxU8);
	return b;
}

PxU16 MemoryReadBuffer::readWord() const
{
	PxU16 w;
	memcpy(&w, buffer, sizeof(PxU16));
	buffer += sizeof(PxU16);
	return w;
}

PxU32 MemoryReadBuffer::readDword() const
{
	PxU32 d;
	memcpy(&d, buffer, sizeof(PxU32));
	buffer += sizeof(PxU32);
	return d;
}

float MemoryReadBuffer::readFloat() const
{
	float f;
	memcpy(&f, buffer, sizeof(float));
	buffer += sizeof(float);
	return f;
}

double MemoryReadBuffer::readDouble() const
{
	double f;
	memcpy(&f, buffer, sizeof(double));
	buffer += sizeof(double);
	return f;
}

void MemoryReadBuffer::readBuffer(void* dest, PxU32 size) const
{
	memcpy(dest, buffer, size);
	buffer += size;
}
