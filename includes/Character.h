#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "OgrePrerequisites.h"
#include "OgreEntity.h"

using namespace Ogre;



#include <OIS/OIS.h>

// Generic Character class
 class Character {
 public:
	 Character(): mMainNode(nullptr), mPlayerNode(nullptr), mSightNode(nullptr), mTopCamNode(nullptr), mYawNode(nullptr), mPitchNode(nullptr), mRollNode(nullptr), 
		 mCameraNode(nullptr), mEntity(nullptr), mSceneMgr(nullptr), Direction(nullptr), Velocity(nullptr), mMouseSpeed(0), mTightness(0), mAnimationState(nullptr), 
		 mTranslateVector(nullptr), m_is_attached(false), mRotYaw(nullptr), mRotPitch(nullptr), qr(nullptr), cam_sn(nullptr), center_sn(nullptr), t_cam(nullptr), 
		 tv3(nullptr), thePoint(nullptr), tx(nullptr), cam_pos_w(nullptr)
	 {
	 }

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
         v1::Entity *mEntity; // Character entity
         SceneManager *mSceneMgr;
		 Quaternion * Direction;
		 Vector3 * Velocity;
		 float mMouseSpeed;
		 	Real mTightness;
		
		v1::AnimationState *mAnimationState;
     public:
	 virtual ~Character()
	 {
	 }

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
     public:
         // Updates the character (movement...)
		 virtual void update (Real elapsedTime, OIS::Keyboard *input, OIS::Mouse * mInput) = 0;
		 
		 SceneNode *getSightNode() const;
		 SceneNode *getCameraNode() const;
		 SceneNode *getTopCamNode() const;
		 v1::AnimationState *getAnimState() const;
		 void setAnimationState(String string);

		 SceneNode *getYawNode() const;
		 SceneNode *getPitchNode() const;
		 SceneNode *getRollNode() const;
		 
		 SceneNode *getMainNode() const;
		 SceneNode *getPlayerNode() const;
		
		 void setTightness(Real tightness);

		 Real getTightness() const;

		 Vector3 getPlayerDirection() const;

		 Vector3 getCameraDirection() const;

		 void instantUpdate(Vector3 cameraPosition, Vector3 targetPosition) const;
 
		 void update(Vector3 cameraPosition, Vector3 targetPosition) const;

		 void SetDirection(Quaternion * q);
		 Vector3 * GetVelocity() const
		 { return Velocity; }
 };

// Specialization of the Character class - Our dear Ogre :D
 class OgreCharacter : public Character {
 // Attributes ------------------------------------------------------------------------------
     protected:
         String mName;
     public:
		 OgreCharacter(String name, SceneManager *sceneMgr);
		 ~OgreCharacter();
 
		 void update(Real elapsedTime, OIS::Keyboard *input, OIS::Mouse * mInput);

         // Change visibility - Useful for 1st person view ;)
		 void setVisible(bool visible) const;
 };
#endif