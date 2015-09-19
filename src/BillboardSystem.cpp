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


#include "BillboardSystem.h"
#include "GraphicsSystem.h"
#include "tinyxml.h"
using namespace Ogre;

template<> BillboardSystem* Ogre::Singleton<BillboardSystem>::msSingleton = 0;

BillboardSystem* BillboardSystem::getSingletonPtr(void)
{
	return msSingleton;
}

BillboardSystem& BillboardSystem::getSingleton(void)
{  
	assert( msSingleton );  return ( *msSingleton );
}

BillboardSystem::BillboardSystem()
{
	bnode = NULL;
}

BillboardSystem::~BillboardSystem()
{
	Finalize();
}

void BillboardSystem::Initialize()
{
	//create node
	bnode = GSYS->GetSceneMgr()->getRootSceneNode()->createChildSceneNode("BillboardSystemNode");

	//read xml & load sets
	DataStreamPtr data = ResourceGroupManager::getSingleton().openResource("billboards.xml");
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
				String material = "";
				int type = 0;
				int poolsize = 0;
				float width = 0;
				float height = 0;

				if (strcmp(node->Value(), "billboard") == 0)
				{
					TiXmlAttribute* att = node->ToElement()->FirstAttribute();
					while (att)
					{
						if (strcmp(att->Name(), "name") == 0)
						{
							name = att->Value();
						}
						else if (strcmp(att->Name(), "type") == 0)
						{
							type = StringConverter::parseInt(att->Value());
						}
						else if (strcmp(att->Name(), "poolsize") == 0)
						{
							poolsize = StringConverter::parseInt(att->Value());
						}
						else if (strcmp(att->Name(), "material") == 0)
						{
							material = att->Value();
						}
						else if (strcmp(att->Name(), "width") == 0)
						{
							width = StringConverter::parseReal(att->Value());
						}
						else if (strcmp(att->Name(), "height") == 0)
						{
							height = StringConverter::parseReal(att->Value());
						}
						att = att->Next();
					}

					//add set
					if (name.length() > 0)
					{
						BillboardNodeSet* bset = new BillboardNodeSet(poolsize);
						bset->set = GSYS->GetSceneMgr()->createBillboardSet(name, poolsize);
						bset->set->setMaterialName(material);
						bset->set->setDefaultDimensions(width, height);
						bset->set->setBillboardType((BillboardType)type);
						bset->set->setRenderQueueGroup(RENDER_QUEUE_SKIES_LATE);
						AxisAlignedBox aabInf;aabInf.setInfinite();;
						bset->set->setBounds(aabInf, 1024);
						bnode->attachObject(bset->set);

						//create billboard buffer
						for (int i=0;i<poolsize;i++)
						{
							BillboardNode* bbnode = new BillboardNode();
							bset->buffer.push_back(bbnode);
						}

						//finally add
						sets[name] = bset;
					}
				}
			}
		}
	}
	else
	{
		throw Exception(Exception::ERR_FILE_NOT_FOUND, std::string(doc.ErrorDesc()) + " : billboards.xml", __FUNCTION__);
	}
}

void BillboardSystem::Finalize()
{
	//traverse all sets and delete all
	std::map<Ogre::String, BillboardNodeSet*>::iterator it;
	it = sets.begin();
	while (it != sets.end())
	{
		//remove
		BillboardNodeSet* bset = (*it).second;
		bset->Finalize(bnode);
		delete bset;
		bset = NULL;

		it++;
	}
	sets.clear();

	//remove bnode
	GSYS->GetSceneMgr()->getRootSceneNode()->removeAndDestroyChild(bnode->getName());
	bnode = NULL;
}

void BillboardSystem::Update()
{
	//traverse all sets and update as necessary
	std::map<Ogre::String, BillboardNodeSet*>::iterator it;
	it = sets.begin();
	while (it != sets.end())
	{
		BillboardNodeSet* bset = (*it).second;
		bset->Update();
		it++;
	}
}

int BillboardSystem::ShowBillboard(Ogre::String name, Ogre::Vector3 pos, float time)
{
	//remove if exists
	BillboardNodeSet* bset = sets[name];
	int ret = bset->counter;

	if (bset->GetCurrent()->bill != NULL)
	{
		bset->set->removeBillboard(bset->GetCurrent()->bill);
	}

	bset->GetCurrent()->bill = bset->set->createBillboard(pos, ColourValue::White);
	bset->GetCurrent()->liveTime = time;
	bset->IncCounter();

	return ret;
}

void BillboardSystem::UpdateBillboard(Ogre::String name, int id, Ogre::Vector3 pos)
{
	BillboardNodeSet* bset = sets[name];
	if (bset->buffer[id]->bill != NULL)
	{
		bset->buffer[id]->bill->setPosition(pos);
	}
}

void BillboardSystem::HideBillboard(Ogre::String name, int id)
{
	BillboardNodeSet* bset = sets[name];
	if (bset->buffer[id]->bill != NULL)
	{
		bset->buffer[id]->liveTime = 0;
	}
}