#ifndef _EXTENDED_CAMERA_H
#define _EXTENDED_CAMERA_H

#include "Ogre.h"
using namespace Ogre;

// Our extended camera class
 class ExtendedCamera {
 // Attributes ------------------------------------------------------------------------------
     protected:
         SceneNode *mTargetNode; // The camera node target
         SceneNode *mCameraNode; // The camera node itself
         Camera *mCamera; // Ogre camera
 
         SceneManager *mSceneMgr;
         String mName;
 
         bool mOwnCamera; // To know if the ogre camera binded has been created outside or inside of this class
 
         Real mTightness; // Determines the movement of the camera - 1 means tight movement, while 0 means no movement
     public:
 // Methods ---------------------------------------------------------------------------------
     protected:
     public:
         ExtendedCamera (String name, SceneManager *sceneMgr, Camera *camera = 0) {
             // Basic member references setup
             mName = name;
             mSceneMgr = sceneMgr;

				// creating plane
			Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
			Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane, 12000, 12000, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
			// placing plane
			Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
			
			mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);

			// texturing plane
			entGround->setMaterialName("Examples/Rockwall");
			entGround->setCastShadows(false);

             // Create the camera's node structure
             //mCameraNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode (mName,Vector3(0.0,0.0,-50.0));
             //mTargetNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode (mName + "_target",Vector3(0.0,0.0,50.0));
             //mCameraNode->setAutoTracking (false, mTargetNode); // The camera will always look at the camera target
             //mCameraNode->setFixedYawAxis (false); // Needed because of auto tracking
			 //mCameraNode->showBoundingBox(true);
			 //mTargetNode->showBoundingBox(true);
             // Create our camera if it wasn't passed as a parameter
             if (camera == 0) {
                 mCamera = mSceneMgr->createCamera (mName);
				 mCamera->setPosition(Vector3(0.0,0.0,-50.0));
				 mOwnCamera = true;
             }
             else {
                 mCamera = camera;
                                 // just to make sure that mCamera is set to 'origin' (same position as the mCameraNode)
                                 mCamera->setPosition(0.0,0.0,-50.0);
                 mOwnCamera = false;
             }
             // ... and attach the Ogre camera to the camera node
             //mCameraNode->attachObject (mCamera);
 
             // Default tightness
             mTightness = 0.01f;
         }
         ~ExtendedCamera () {
             //mCameraNode->detachAllObjects ();
             if (mOwnCamera)
                 delete mCamera;
             mSceneMgr->destroySceneNode (mName);
             //mSceneMgr->destroySceneNode (mName + "_target");
         }
 
         void setTightness (Real tightness) {
             mTightness = tightness;
         }
 
         Real getTightness () {
             return mTightness;
         }
 /*
         Vector3 getCameraPosition () {
             return mCameraNode->getPosition();
         }

		 Vector3 getTargetPosition () {
             return mTargetNode->getPosition();
         }
			
         SceneNode *getTargetNode () {
			return mTargetNode;
         }
         SceneNode *getCameraNode () {
             return mCameraNode;
         }
		 */
		 Camera * getCamera() {
			 return mCamera;
		 }

         
 };

#endif