/*
-----------------------------------------------------------------------------
Filename:    mainApplication.h
-----------------------------------------------------------------------------
 
This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#ifndef __mainApplication_h_
#define __mainApplication_h_



#include "BaseApplication.h"
#include "NavMesher.h"
#include "Sample_TileMesh.h"
#include "Character.h"



class SampleApplication : public BaseApplication
 {
 public:
     SampleApplication()
     {
     }
     ~SampleApplication()
     {
     }
 protected:
     // Just override the mandatory create scene method
     void createScene(void)
     {
         // Set ambient light
         mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));
 
         // LIGHTS!!
         // Create a point light
         Light* l = mSceneMgr->createLight("MainLight");
         // Accept default settings: point light, white diffuse, just set position
         // NB I could attach the light to a SceneNode if I wanted it to move automatically with
         //  other objects, but I don't
         l->setType(Light::LT_DIRECTIONAL);
         l->setDirection(-0.5, -0.5, 0);
 
         // CAMERA!!
         mCamera->setPosition (0, 0, 0);    // Required or else the camera will have an offset

         // ACTION!!!
         // Fill the scene with some razors
			 SceneNode *razorNode;
			 Entity *razorEntity;
			 for (unsigned int i = 0; i < 30; ++i) {
				 razorNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode (StringConverter::toString (i), Vector3 (Math::RangeRandom (-1000, 1000), 0, Math::RangeRandom (-1000, 1000)));
				 razorEntity = mSceneMgr->createEntity (StringConverter::toString (i), "razor.mesh");
				 razorNode->attachObject (razorEntity);
			 }

		 // creating plane
			Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
			Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane, 12000, 12000, 1, 1, true, 1, 50, 50, Ogre::Vector3::UNIT_Z);
			// placing plane
			Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
			
			mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0,0,0))->attachObject(entGround);

			// texturing plane
			entGround->setMaterialName("Examples/Rockwall");
			entGround->setCastShadows(false);
 
         // Main character
			 mChar = new OgreCharacter ("Ogre 1", mSceneMgr);
         // Frame listener to manage both character and camera updating and different camera modes
         // Need to create it here as we want to change some parameters here, thus avoiding defining 
         // ogre and exCamera as member variables
			mChar->getCameraNode()->attachObject(mCamera);
			
			// Initialize shared data, can use BASE instead ???
			new SharedData();
			SharedData::getSingleton().iGlobalActivate = true;
			SharedData::getSingleton().iCamera = mCamera;
			SharedData::getSingleton().iSceneMgr = mSceneMgr;
			SharedData::getSingleton().iRoot = mRoot;
			SharedData::getSingleton().iWindow = mWindow;

			//NavMesher * nv = new NavMesher();
			Sample_TileMesh * tileMesh = new Sample_TileMesh();
			if(!tileMesh->handleBuild())  Ogre::Exception(Exception::ERR_ITEM_NOT_FOUND,"No tile nav Mesh!","TileMesh::Build");
			//if(!nv->Build()) Ogre::Exception(Exception::ERR_ITEM_NOT_FOUND,"No nav Mesh!","NavMesher::Build");
     }
     void destroyScene(void)
     {
     }
     void createFrameListener(void)
     {
         // This is where we instantiate our own frame listener
         //mFrameListener= new SampleListener(mWindow, mCamera);
         mRoot->addFrameListener(this);
     }
 };
#endif // #ifndef __mainApplication_h_