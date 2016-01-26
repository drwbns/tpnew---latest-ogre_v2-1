//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef RECASTALLOC_H
#define RECASTALLOC_H

enum rcAllocHint
{
	RC_ALLOC_PERM,		///< Memory persist after a function call.
	RC_ALLOC_TEMP		///< Memory used temporarily within a function.
};

typedef void* (rcAllocFunc)(int size, rcAllocHint hint);
typedef void (rcFreeFunc)(void* ptr);

void rcAllocSetCustom(rcAllocFunc *allocFunc, rcFreeFunc *freeFunc);

void* rcAlloc(int size, rcAllocHint hint);
void rcFree(void* ptr);



/// Simple dynamic array ints.
class rcIntArray
{
	int* m_data;
	int m_size, m_cap;
	inline rcIntArray(const rcIntArray&);
	inline rcIntArray& operator=(const rcIntArray&);
public:
	rcIntArray() : m_data(0), m_size(0), m_cap(0) {}
	rcIntArray(int n) : m_data(0), m_size(0), m_cap(0) { resize(n); }
	~rcIntArray() { rcFree(m_data); }
	void resize(int n);
	void push(int item) { resize(m_size+1); m_data[m_size-1] = item; }
	int pop() { if (m_size > 0) m_size--; return m_data[m_size]; }
	const int& operator[](int i) const { return m_data[i]; }
	int& operator[](int i) { return m_data[i]; }
	int size() const { return m_size; }
};

/// Simple internal helper class to delete array in scope
template<class T> class rcScopedDelete
{
	T* ptr;
	inline T* operator=(T* p);
public:
	rcScopedDelete() : ptr(nullptr) {}

	rcScopedDelete(T* p) : ptr(p) {}

	~rcScopedDelete() { rcFree(ptr); }

	operator T*() { return ptr; }
};

template <class T>
T* rcScopedDelete<T>::operator=(T* p)
{
	return nullptr;
}
#endif
