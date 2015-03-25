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

#include "stdafx.h"
#include "zzzSndSystem.h"
#include "tinyxml.h"
using namespace Ogre;

template<> zzzSndSystem* Ogre::Singleton<zzzSndSystem>::ms_Singleton = 0;

zzzSndSystem* zzzSndSystem::getSingletonPtr(void)
{
	return ms_Singleton;
}

zzzSndSystem& zzzSndSystem::getSingleton(void)
{  
	assert( ms_Singleton );  return ( *ms_Singleton );
}

zzzSndSystem::zzzSndSystem()
{
}

zzzSndSystem::~zzzSndSystem()
{
	Finalize();
}

void zzzSndSystem::Initialize()
{
	soundsys = OGRE_NEW SoundSystem();
	if (!soundsys->initialize())
	{
		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Sound System Init Failed", __FUNCTION__);
	}
	slistener = SoundListener::getSingletonPtr();
	slistener->setGain(1.0);
	LoadSounds();
}

void zzzSndSystem::Finalize()
{
	UnloadSounds();
}

void zzzSndSystem::Update()
{
	slistener->setPosition(0,0,0);
	slistener->setDirection(0,0,1);
}

void zzzSndSystem::PlaySound(std::string name)
{
	snds[name]->setGain(1);
	snds[name]->seek(0);
	snds[name]->play();
}

void zzzSndSystem::PlaySound(std::string name, Ogre::Vector3 pos)
{
	snds[name]->setGain(1);
	snds[name]->setPosition(pos);
	snds[name]->seek(0);
	snds[name]->play();
}

void zzzSndSystem::PlaySound(std::string name, Ogre::Vector3 pos, float vol)
{
	snds[name]->setGain(vol);
	snds[name]->setPosition(pos);
	snds[name]->seek(0);
	snds[name]->play();
}

void zzzSndSystem::StopSound(std::string name)
{
	snds[name]->stop();
}

void zzzSndSystem::LoadSound(std::string name, std::string file, SceneNode* owner = NULL, bool loop = false, float gain = 1)
{
	SoundSource* s = soundsys->createSource(name, file);
	s->setLoop(loop);
	s->setMinGain(0);
	s->setGain(gain);
	s->setMaxGain(1);
	s->setDistanceValues(75.0,0.5,2.5);
	if (owner != NULL)
	{
		owner->attachObject(s);
	}
	snds[name] = s;
}

void zzzSndSystem::LoadSounds()
{
	//read xml & load sets
	DataStreamPtr data = ResourceGroupManager::getSingleton().openResource("sounds.xml");
	String str = data->getAsString();
	TiXmlDocument doc;
	doc.Parse(str.c_str());

	if (!doc.Error())
	{
		TiXmlNode* node;
		node=doc.FirstChild();
		node=node->FirstChild();
		for (;node!=0;node=node->NextSibling()) 
		{
			if (node->Type() == TiXmlNode::ELEMENT)
			{
				//get params
				String name = "";
				String file = "";

				if (strcmp(node->Value(), "sound") == 0)
				{
					TiXmlAttribute* att = node->ToElement()->FirstAttribute();
					while (att)
					{
						if (strcmp(att->Name(), "name") == 0)
						{
							name = att->Value();
						}
						else if (strcmp(att->Name(), "file") == 0)
						{
							file = att->Value();
						}
						att = att->Next();
					}

					if (name.length() > 0)
					{
						SND->LoadSound(name, file);
					}
				}
			}
		}
	}
	else
	{
		throw Exception(Exception::ERR_FILE_NOT_FOUND, std::string(doc.ErrorDesc()) + " : particles.xml", __FUNCTION__);
	}
}

void zzzSndSystem::UnloadSounds()
{
	//snds
	std::map<std::string,SoundSource*>::iterator it = snds.begin();
	while (it != snds.end())
	{
		((*it).second)->stop();
		soundsys->destroySource(((*it).second));
		it++;
	}
}