#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "Ogre.h"
using namespace Ogre;

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

// Generic Character class
 class Character {
 // Attributes ------------------------------------------------------------------------------
     protected:
         SceneNode *mMainNode; // Main node
		SceneNode *mPlayerNode; // Player Node
         SceneNode *mSightNode; // "Sight" node - The character is supposed to be looking here
		 SceneNode *mTopCamNode; // Node for moving cam
		 SceneNode *mYawNode; // Node for yaw cam
		 SceneNode *mPitchNode; // Node for pitch cam
		 SceneNode *mRollNode; // Node for roll cam
         SceneNode *mCameraNode; // Node for the chase camera
         Entity *mEntity; // Character entity
         SceneManager *mSceneMgr;
		 Ogre::Quaternion Direction;
		 Vector3 Velocity;
		 float mMouseSpeed;
		 	Real mTightness;
		
		AnimationState *mAnimationState;
     public:
		 Vector3 mTranslateVector;
		 bool m_is_attached;
		Radian mRotYaw, mRotPitch;
		Quaternion qr;
		SceneNode * cam_sn;
		SceneNode * center_sn;
		SceneNode * t_cam;
		Vector3 tv3, thePoint, tx, cam_pos_w;
		 static const int MINSPD = 5;
 // Methods ---------------------------------------------------------------------------------
     protected:
     public:
         // Updates the character (movement...)
		 virtual void update (Real elapsedTime, OIS::Keyboard *input, OIS::Mouse * mInput) = 0;
		 
         SceneNode *getSightNode () {
			return mSightNode;
         }
         SceneNode *getCameraNode () {
             return mCameraNode;
         }
		 SceneNode *getTopCamNode () {
             return mTopCamNode;
         }
		 AnimationState *getAnimState(){
			 return mAnimationState;
		 }
		 void setAnimationState(String string) {
			mAnimationState = mEntity->getAnimationState(string.c_str());
			mAnimationState->setLoop(true);
			mAnimationState->setEnabled(true);
		}

		 SceneNode *getYawNode () {
             return mYawNode;
         }
		SceneNode *getPitchNode () {
             return mPitchNode;
         }
		SceneNode *getRollNode () {
             return mRollNode;
         }
		 
		 SceneNode *getMainNode () {
             return mMainNode;
         }
		 SceneNode *getPlayerNode () {
             return mPlayerNode;
         }
		
		void setTightness (Real tightness) {
		 mTightness = tightness;
		 }

		 Real getTightness () {
			 return mTightness;
		 }

		 Vector3 getPlayerDirection()
		{
			Matrix3 matrix;
			mMainNode->getOrientation().ToRotationMatrix(matrix);
			Vector3 mDirection = matrix.GetColumn(2);
			return mDirection;
		}

		  Vector3 getCameraDirection()
		{
			Matrix3 matrix;
			mCameraNode->getOrientation().ToRotationMatrix(matrix);
			Vector3 mDirection = matrix.GetColumn(2);
			return mDirection;
		}

		 void instantUpdate (Vector3 cameraPosition, Vector3 targetPosition) {
			 mCameraNode->setPosition (cameraPosition);
			 mSightNode->setPosition (targetPosition);
		 }
 
		 void update (Real elapsedTime, Vector3 cameraPosition, Vector3 targetPosition) {
			 // Handle movement
			 Vector3 displacement;
 
			 displacement = (cameraPosition - mCameraNode->_getDerivedPosition ()) * mTightness;
			 mCameraNode->translate (displacement);
 
			 displacement = (targetPosition - mSightNode->_getDerivedPosition ()) * mTightness;
			 mSightNode->translate (displacement);

		 	
		 }

		 void SetDirection(Ogre::Quaternion q) { Direction = q; }
		 Ogre::Vector3 GetVelocity() { return Velocity; }
 };

// Specialization of the Character class - Our dear Ogre :D
 class OgreCharacter : public Character {
 // Attributes ------------------------------------------------------------------------------
     protected:
         String mName;
     public:
 // Methods ---------------------------------------------------------------------------------
     protected:
     public:
         OgreCharacter (String name, SceneManager *sceneMgr) {
             // Setup basic member references
             mName = name;
             mSceneMgr = sceneMgr;
			mMouseSpeed = 50;
             // Setup basic node structure to handle 3rd person cameras
			mTranslateVector = Ogre::Vector3::ZERO;

             mMainNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode (mName, Vector3(0,0,0));
			 mMainNode->showBoundingBox(true);
			 
			 
			 
			 mPlayerNode = mMainNode->createChildSceneNode (mName + "_player", Vector3 (0, 0, 0));
             mSightNode = mPlayerNode->createChildSceneNode (mName + "_sight", Vector3 (0, 0, -300));
			 mSightNode->showBoundingBox(true);
			 mTopCamNode = mMainNode->createChildSceneNode (mName + "_camTop", Vector3 (0, 175, 0));
			 mYawNode = mTopCamNode->createChildSceneNode (mName + "_yaw", Vector3 (0, 0, 0));
			 mPitchNode = mYawNode->createChildSceneNode (mName + "_pitch", Vector3 (0, 0, 0));
			 mRollNode = mPitchNode->createChildSceneNode (mName + "_roll", Vector3 (0, 0, 0));
			 mCameraNode = mRollNode->createChildSceneNode (mName + "_camera", Vector3 (0, 0, 0));
			 mCameraNode->showBoundingBox(true);
			mMainNode->setScale(0.1,0.1,0.1);
             // Give this character a shape :)
             mEntity = mSceneMgr->createEntity (mName, "Ninja.mesh");
			 

             mPlayerNode->attachObject (mEntity);


			 // Set idle animation
			setAnimationState("Idle1");
         }
         ~OgreCharacter () {
             mMainNode->detachAllObjects ();
             delete mEntity;
             mMainNode->removeAndDestroyAllChildren ();
             mSceneMgr->destroySceneNode (mName);
         }
 
		 void update (Real elapsedTime, OIS::Keyboard *input, OIS::Mouse * mInput) {
             // Handle movement
			 if(mInput->getMouseState().buttonDown(OIS::MB_Left)) {
				setAnimationState("Attack3");
				getAnimState()->setLoop(false);
				if(getAnimState()->hasEnded())
					getAnimState()->setTimePosition(0);
				
			}
             if (input->isKeyDown (OIS::KC_W)) {
				 mMainNode->translate (mPlayerNode->getOrientation() * Vector3 (0, 0, -100 * elapsedTime),Node::TS_LOCAL);
				 setAnimationState("Walk");
				 getAnimState()->setLoop(false);
				 if(getAnimState()->hasEnded())
					getAnimState()->setTimePosition(0);
             }
			 if (input->isKeyDown (OIS::KC_S)) {
                 mMainNode->translate (mPlayerNode->getOrientation() * Vector3 (0, 0, 50 * elapsedTime),Node::TS_LOCAL);
             }
			 if (input->isKeyDown (OIS::KC_A) && !input->isModifierDown(OIS::Keyboard::Alt)) {
					 mMainNode->translate (mPlayerNode->getOrientation() * Vector3 (-50 * elapsedTime, 0, 0),Node::TS_LOCAL);
             }
             if (input->isKeyDown (OIS::KC_D) && !input->isModifierDown(OIS::Keyboard::Alt)) {
					 mMainNode->translate (mPlayerNode->getOrientation() * Vector3 (50 * elapsedTime, 0, 0),Node::TS_LOCAL);
             }
			if (input->isKeyDown (OIS::KC_A) && input->isModifierDown(OIS::Keyboard::Alt)) {
				mMainNode->rotate (Vector3::NEGATIVE_UNIT_Y,(Radian)-5 * elapsedTime);
             }
			if (input->isKeyDown (OIS::KC_D) && input->isModifierDown(OIS::Keyboard::Alt)) {
				 mMainNode->rotate (Vector3::NEGATIVE_UNIT_Y,(Radian)5* elapsedTime);
             }
         }

         // Change visibility - Useful for 1st person view ;)
         void setVisible (bool visible) {
             mMainNode->setVisible (visible);
         }
 };
#endif