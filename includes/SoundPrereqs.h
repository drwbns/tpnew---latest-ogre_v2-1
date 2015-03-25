#ifndef _SOUNDPREREQS_H
#define _SOUNDPREREQS_H

//OpenAL Includes
#include <al.h>
#include <alc.h>
#include <xram.h>
#include <alut.h>

#include <map>

//xiph includes
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

struct DeleteSecond
{
	template<typename T1, typename T2>
	void operator()(std::pair<T1, T2*> pair)const
	{
		delete pair.second;
	}
};

#endif