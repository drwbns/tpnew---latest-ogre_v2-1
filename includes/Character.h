#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "OgrePrerequisites.h"

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
		 Ogre::Quaternion * Direction;
		 Vector3 * Velocity;
		 float mMouseSpeed;
		 	Real mTightness;
		
		AnimationState *mAnimationState;
     public:
		 Vector3 * mTranslateVector;
		 bool m_is_attached;
		Radian *mRotYaw, *mRotPitch;
		Quaternion * qr;
		SceneNode * cam_sn;
		SceneNode * center_sn;
		SceneNode * t_cam;
		Vector3 *tv3, *thePoint,* tx, *cam_pos_w;
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
		 void setAnimationState(String string);

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

		 Vector3 getPlayerDirection();

		 Vector3 getCameraDirection();

		 void instantUpdate(Vector3 cameraPosition, Vector3 targetPosition);
 
		 void update(Real elapsedTime, Vector3 cameraPosition, Vector3 targetPosition);

		 void SetDirection(Ogre::Quaternion * q) { Direction = q; }
		 Ogre::Vector3 * GetVelocity() { return Velocity; }
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
		 OgreCharacter(String name, SceneManager *sceneMgr);
		 ~OgreCharacter();
 
		 void update(Real elapsedTime, OIS::Keyboard *input, OIS::Mouse * mInput);

         // Change visibility - Useful for 1st person view ;)
		 void setVisible(bool visible);
 };
#endif