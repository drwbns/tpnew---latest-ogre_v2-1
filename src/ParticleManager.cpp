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


#include "ParticleManager.h"

#include "GraphicsSystem.h"
#include "tinyxml.h"

#include "OgreSceneNode.h"

using namespace Ogre;

template<> ParticleManager* Ogre::Singleton<ParticleManager>::msSingleton = 0;

ParticleManager* ParticleManager::getSingletonPtr(void)
{
	return msSingleton;
}

ParticleManager& ParticleManager::getSingleton(void)
{  
	assert( msSingleton );  return ( *msSingleton );
}

ParticleManager::ParticleManager()
{
	//pManager = NULL;
	pnode = NULL;
}

ParticleManager::~ParticleManager()
{
	Finalize();
}

void ParticleManager::Initialize()
{
	//get mgr
	//pManager = ParticleUniverse::ParticleSystemManager::getSingletonPtr();

	//create node
	pnode = GSYS->GetSceneMgr()->getRootSceneNode()->createChildSceneNode("ParticleManagerNode");

	//read xml & load sets
	DataStreamPtr data = ResourceGroupManager::getSingleton().openResource("particles.xml");
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
				String system = "";

				if (strcmp(node->Value(), "particle") == 0)
				{
					TiXmlAttribute* att = node->ToElement()->FirstAttribute();
					while (att)
					{
						if (strcmp(att->Name(), "name") == 0)
						{
							name = att->Value();
						}
						else if (strcmp(att->Name(), "system") == 0)
						{
							system = att->Value();
						}
						att = att->Next();
					}

					if (name.length() > 0)
					{
						/* particle universe removal.. 
						//create
						
						ParticleUniverse::ParticleSystem* psys = pManager->createParticleSystem(name, system, GSYS->GetSceneMgr());
						pnode->attachObject(psys);
						//finally add
						syss[name] = psys;
						*/
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

void ParticleManager::Finalize()
{
	//remove pnode
	pnode->detachAllObjects();
	GSYS->GetSceneMgr()->getRootSceneNode()->removeAndDestroyChild(pnode->getName());
	pnode = NULL;

	//traverse all systems and delete all
	/* particle universe removal 
	std::map<Ogre::String, ParticleUniverse::ParticleSystem*>::iterator it;
	it = syss.begin();
	while (it != syss.end())
	{
		//remove
		ParticleUniverse::ParticleSystem* psys = (*it).second;
		psys->stop();
		pManager->destroyParticleSystem(psys->getName(), GSYS->GetSceneMgr());
		it++;
	}
	syss.clear();
	
	//clear mgr.
	pManager->destroyAllParticleSystems(GSYS->GetSceneMgr());
	pManager = NULL;
	*/
}

void ParticleManager::Update()
{
}

void ParticleManager::ShowParticle(String name, Vector3 pos, Vector3 dir)
{
	/* particle universe removal 
	ParticleUniverse::ParticleSystem* psys = syss[name];
	psys->getTechnique(0)->getEmitter(0)->position = pos;
	psys->getTechnique(0)->getEmitter(0)->setParticleDirection(dir);
	psys->start();
	*/
}

void ParticleManager::HideParticle(Ogre::String name)
{
	/* particle universe removal 
	ParticleUniverse::ParticleSystem* psys = syss[name];
	psys->stopFade();
	*/
}
