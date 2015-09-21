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


#include "GuiSystem.h"

#include "GraphicsSystem.h"
#include "TextRenderer.h"
#include "World.h"
#include "GlobalVars.h"

#include "OgreManualObject.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"



using namespace Ogre;

template<> GuiSystem* Ogre::Singleton<GuiSystem>::msSingleton = 0;

GuiSystem* GuiSystem::getSingletonPtr(void)
{
	return msSingleton;
}

GuiSystem& GuiSystem::getSingleton(void)
{  
	assert( msSingleton );  return ( *msSingleton );
}

GuiSystem::GuiSystem() : /*mGUI(NULL), mPlatform(NULL),*/ mTxt(NULL)
{
}

GuiSystem::~GuiSystem()
{
	Finalize();
}

void GuiSystem::Initialize()
{
	/*
	mPlatform = new MyGUI::OgrePlatform();
	mPlatform->initialise(GSYS->GetWindow(), GSYS->GetSceneMgr());
	mGUI = new MyGUI::Gui();
	mGUI->initialise("core.xml");

	//crossHair
	//init material
	MaterialPtr material = MaterialManager::getSingleton().create("crossHairMaterial","General"); 
	material->setReceiveShadows(false);
	material->getTechnique(0)->setLightingEnabled(false); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0,1,0);
	*/
	//init crossHair obj
	crossHair = GSYS->GetSceneMgr()->createManualObject("crossHair");
	crossHair->setUseIdentityProjection(true);
	crossHair->setUseIdentityView(true);
	crossHair->clear();
	crossHair->begin("crossHairMaterial", RenderOperation::OT_LINE_LIST);

	crossHair->position(0.0, -0.050, 0.0);
	crossHair->position(0.0, -0.015, 0.0);

	crossHair->position(0.0, 0.050, 0.0);
	crossHair->position(0.0, 0.015, 0.0);

	crossHair->position(-0.040, 0.0, 0.0);
	crossHair->position(-0.015, 0.0, 0.0);

	crossHair->position(0.040, 0.0, 0.0);
	crossHair->position(0.015, 0.0, 0.0);
	
	crossHair->end();
	GSYS->GetSceneMgr()->getRootSceneNode()->attachObject(crossHair);
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	crossHair->setBoundingBox(aabInf);
	crossHair->setVisible(false);
}

void GuiSystem::Finalize()
{
	/*
	mGUI->shutdown();
	delete mGUI;
	mGUI = NULL;   
	mPlatform->shutdown();
	
	delete mPlatform;
	mPlatform = NULL;
*/
	DeinitDebuggers();

	crossHair->clear();
	GSYS->GetSceneMgr()->destroyManualObject(crossHair);
	crossHair = NULL;
}

void GuiSystem::Update()
{
	UpdateDebuggers();
}

void GuiSystem::LoadLayout(std::string name)
{
	//mLayout = MyGUI::LayoutManager::getInstance().load(name + ".layout");
}

void GuiSystem::UnloadLayout()
{
	//MyGUI::LayoutManager::getInstance().unloadLayout(mLayout);
}

void GuiSystem::InitDebuggers()
{
	mTxt = OGRE_NEW TextRenderer();
	//
	std::string name = "fps_txt";
	mTxt->addTextBox(name, name, 10, 10, 96, 32, ColourValue(0.9f,0.9f,0.9f,1.0f));
	name = "cam_txt";
	mTxt->addTextBox(name, name, 10, 30, 96, 32, ColourValue(0.9f,0.9f,0.9f,1.0f));
	name = "bhv_txt";
	mTxt->addTextBox(name, name, 10, 50, 96, 32, ColourValue(0.9f,0.9f,0.9f,1.0f));
	name = "pos_txt";
	mTxt->addTextBox(name, name, 10, 70, 96, 32, ColourValue(0.9f,0.9f,0.9f,1.0f));
	name = "vel_txt";
	mTxt->addTextBox(name, name, 10, 90, 96, 32, ColourValue(0.9f,0.9f,0.9f,1.0f));
	name = "acc_txt";
	mTxt->addTextBox(name, name, 10, 110, 96, 32, ColourValue(0.9f,0.9f,0.9f,1.0f));
	name = "sel_txt";
	mTxt->addTextBox(name, name, 10, 130, 96, 32, ColourValue(0.9f,0.9f,0.9f,1.0f));
	//
	name = "tri_txt";
	mTxt->addTextBox(name, name, 10, 0.9000f * GSYS->GetWindow()->getHeight(), 96, 32, Ogre::ColourValue(0.9f,0.9f,0.9f,1.0f));
	name = "t1_txt";
	mTxt->addTextBox(name, name, 10, 0.9225f * GSYS->GetWindow()->getHeight(), 96, 32, Ogre::ColourValue(0.9f,0.9f,0.9f,1.0f));
	name = "t2_txt";
	mTxt->addTextBox(name, name, 10, 0.9450f * GSYS->GetWindow()->getHeight(), 96, 32, Ogre::ColourValue(0.9f,0.9f,0.9f,1.0f));
	name = "t3_txt";
	mTxt->addTextBox(name, name, 10, 0.9675f * GSYS->GetWindow()->getHeight(), 96, 32, Ogre::ColourValue(0.9f,0.9f,0.9f,1.0f));
}

void GuiSystem::DeinitDebuggers()
{
	if (mTxt != NULL)
	{
		mTxt->reset();
		delete mTxt;
		mTxt = NULL;
	}
}

void GuiSystem::UpdateDebuggers()
{
	if (mTxt)
	{
		const RenderTarget::FrameStats& stats = GSYS->GetWindow()->getStatistics();
		std::string name = "fps_txt";
		std::string text = "Fps:" + StringConverter::toString(stats.lastFPS) + "/" + StringConverter::toString(stats.worstFPS)+ "/" + StringConverter::toString(stats.avgFPS);
		mTxt->setText(name,text);
	
		name = "cam_txt";
		text = "CamPosition:" + StringConverter::toString(GSYS->GetCamera()->getPosition().x,2) + "/" +
								StringConverter::toString(GSYS->GetCamera()->getPosition().y,2) + "/" +
								StringConverter::toString(GSYS->GetCamera()->getPosition().z,2);
		mTxt->setText(name,text);

		name = "bhv_txt";
		text = "Behavior:";
		if (WORLD->getPlayerAgent()->isActive(Moving::brake))text += "Brake,";
		if (WORLD->getPlayerAgent()->isActive(Moving::seek))text += "Seek,";
		if (WORLD->getPlayerAgent()->isActive(Moving::arrive))text += "Arrive,";
		if (WORLD->getPlayerAgent()->isActive(Moving::avoid1))text += "Avoid1,";
		if (WORLD->getPlayerAgent()->isActive(Moving::avoid2))text += "Avoid2,";
		if (WORLD->getPlayerAgent()->isActive(Moving::path))text += "Path,";
		if (WORLD->getPlayerAgent()->isActive(Moving::seperate))text += "Seperate,";
		text.substr(0, text.length()-1);
		mTxt->setText(name,text);

		//
		name = "tri_txt";
		text = "Tri:" + StringConverter::toString(stats.triangleCount);
		mTxt->setText(name,text);
		name = "t1_txt";
		text = "T1:" + StringConverter::toString(GlobalVars::test1);
		mTxt->setText(name,text);
		name = "t2_txt";
		text = "T2:" + StringConverter::toString(GlobalVars::test2);
		mTxt->setText(name,text);
		name = "t3_txt";
		text = "T3:" + StringConverter::toString(GlobalVars::test3);
		mTxt->setText(name,text);
	}
}

//sets

inline void GuiSystem::SetCursor(bool show) { 
	//MyGUI::PointerManager::getInstance().setVisible(show); 
}

inline void GuiSystem::SetCrossHair(bool show) { crossHair->setVisible(show); }
