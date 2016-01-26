#ifndef _SOUNDPREREQS_H
#define _SOUNDPREREQS_H

//OpenAL Includes
#include <alut.h>

//xiph includes

struct DeleteSecond
{
	template<typename T1, typename T2>
	void operator()(std::pair<T1, T2*> pair)const
	{
		delete pair.second;
	}
};

#endif