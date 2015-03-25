/*
Copyright (c) 2010 Yunus Kara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef zzzSndSystem_H_
#define zzzSndSystem_H_

#include "SoundSystem.h"
#include "SoundSource.h"
#include "SoundListener.h"

#define SND zzzSndSystem::getSingletonPtr()

class zzzSndSystem : public Ogre::Singleton<zzzSndSystem>, public Ogre::GeneralAllocatedObject
{
public:
	zzzSndSystem();
	~zzzSndSystem();
	static zzzSndSystem& getSingleton();
	static zzzSndSystem* getSingletonPtr();
	
	void Initialize();
	void Finalize();
	void Update();

	//snds
	void LoadSounds();
	void UnloadSounds();
	void LoadSound(std::string name, std::string file, Ogre::SceneNode* owner, bool loop, float gain);
	void PlaySound(std::string name);
	void PlaySound(std::string name, Ogre::Vector3 pos);
	void PlaySound(std::string name, Ogre::Vector3 pos, float vol);
	void StopSound(std::string name);

	//gets & sets
	SoundSystem* GetSoundSys() const { return soundsys; }
	SoundListener* GetListener() { return slistener; }
	SoundSource* GetSound(std::string s) { return snds[s]; }

private:
	SoundSystem* soundsys;
	SoundListener* slistener;

	//snd
	std::map<std::string,SoundSource*> snds;
};

#endif