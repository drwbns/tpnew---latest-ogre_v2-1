#include "Character.h"

#include "OgreEntity.h"
#include "OgreAnimationState.h"
#include "OgreSceneNode.h"
#include "OgreVector3.h"
#include "OgreSceneManager.h"

inline SceneNode * Character::getSightNode() {
	return mSightNode;
}

inline SceneNode * Character::getCameraNode() {
	return mCameraNode;
}

inline SceneNode * Character::getTopCamNode() {
	return mTopCamNode;
}

inline AnimationState * Character::getAnimState() {
	return mAnimationState;
}

inline void Character::setAnimationState(String string) {
	mAnimationState = mEntity->getAnimationState(string.c_str());
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
}

inline SceneNode * Character::getYawNode() {
	return mYawNode;
}

inline SceneNode * Character::getPitchNode() {
	return mPitchNode;
}

inline SceneNode * Character::getRollNode() {
	return mRollNode;
}

inline SceneNode * Character::getMainNode() {
	return mMainNode;
}

inline SceneNode * Character::getPlayerNode() {
	return mPlayerNode;
}

inline void Character::setTightness(Real tightness) {
	mTightness = tightness;
}

inline Real Character::getTightness() {
	return mTightness;
}

inline Vector3 Character::getPlayerDirection()
{
	Matrix3 matrix;
	mMainNode->getOrientation().ToRotationMatrix(matrix);
	Vector3 mDirection = matrix.GetColumn(2);
	return mDirection;
}

inline Vector3 Character::getCameraDirection()
{
	Matrix3 matrix;
	mCameraNode->getOrientation().ToRotationMatrix(matrix);
	Vector3 mDirection = matrix.GetColumn(2);
	return mDirection;
}

inline void Character::instantUpdate(Vector3 cameraPosition, Vector3 targetPosition) {
	mCameraNode->setPosition(cameraPosition);
	mSightNode->setPosition(targetPosition);
}

inline void Character::update(Real elapsedTime, Vector3 cameraPosition, Vector3 targetPosition) {
	// Handle movement
	Vector3 displacement;

	displacement = (cameraPosition - mCameraNode->_getDerivedPosition()) * mTightness;
	mCameraNode->translate(displacement);

	displacement = (targetPosition - mSightNode->_getDerivedPosition()) * mTightness;
	mSightNode->translate(displacement);


}

inline void Character::SetDirection(Ogre::Quaternion * q) { Direction = q; }

inline OgreCharacter::OgreCharacter(String name, SceneManager * sceneMgr) {
	// Setup basic member references
	mName = name;
	mSceneMgr = sceneMgr;
	mMouseSpeed = 50;
	// Setup basic node structure to handle 3rd person cameras
	*mTranslateVector = Ogre::Vector3::ZERO;

	mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mName, Vector3(0, 0, 0));
	mMainNode->showBoundingBox(true);



	mPlayerNode = mMainNode->createChildSceneNode(mName + "_player", Vector3(0, 0, 0));
	mSightNode = mPlayerNode->createChildSceneNode(mName + "_sight", Vector3(0, 0, -300));
	mSightNode->showBoundingBox(true);
	mTopCamNode = mMainNode->createChildSceneNode(mName + "_camTop", Vector3(0, 175, 0));
	mYawNode = mTopCamNode->createChildSceneNode(mName + "_yaw", Vector3(0, 0, 0));
	mPitchNode = mYawNode->createChildSceneNode(mName + "_pitch", Vector3(0, 0, 0));
	mRollNode = mPitchNode->createChildSceneNode(mName + "_roll", Vector3(0, 0, 0));
	mCameraNode = mRollNode->createChildSceneNode(mName + "_camera", Vector3(0, 0, 0));
	mCameraNode->showBoundingBox(true);
	mMainNode->setScale(0.1, 0.1, 0.1);
	// Give this character a shape :)
	mEntity = mSceneMgr->createEntity(mName, "Ninja.mesh");


	mPlayerNode->attachObject(mEntity);


	// Set idle animation
	setAnimationState("Idle1");
}

inline OgreCharacter::~OgreCharacter() {
	mMainNode->detachAllObjects();
	delete mEntity;
	mMainNode->removeAndDestroyAllChildren();
	mSceneMgr->destroySceneNode(mName);
}

inline void OgreCharacter::update(Real elapsedTime, OIS::Keyboard * input, OIS::Mouse * mInput) {
	// Handle movement
	if (mInput->getMouseState().buttonDown(OIS::MB_Left)) {
		setAnimationState("Attack3");
		getAnimState()->setLoop(false);
		if (getAnimState()->hasEnded())
			getAnimState()->setTimePosition(0);

	}
	if (input->isKeyDown(OIS::KC_W)) {
		mMainNode->translate(mPlayerNode->getOrientation() * Vector3(0, 0, -100 * elapsedTime), Node::TS_LOCAL);
		setAnimationState("Walk");
		getAnimState()->setLoop(false);
		if (getAnimState()->hasEnded())
			getAnimState()->setTimePosition(0);
	}
	if (input->isKeyDown(OIS::KC_S)) {
		mMainNode->translate(mPlayerNode->getOrientation() * Vector3(0, 0, 50 * elapsedTime), Node::TS_LOCAL);
	}
	if (input->isKeyDown(OIS::KC_A) && !input->isModifierDown(OIS::Keyboard::Alt)) {
		mMainNode->translate(mPlayerNode->getOrientation() * Vector3(-50 * elapsedTime, 0, 0), Node::TS_LOCAL);
	}
	if (input->isKeyDown(OIS::KC_D) && !input->isModifierDown(OIS::Keyboard::Alt)) {
		mMainNode->translate(mPlayerNode->getOrientation() * Vector3(50 * elapsedTime, 0, 0), Node::TS_LOCAL);
	}
	if (input->isKeyDown(OIS::KC_A) && input->isModifierDown(OIS::Keyboard::Alt)) {
		mMainNode->rotate(Vector3::NEGATIVE_UNIT_Y, (Radian)-5 * elapsedTime);
	}
	if (input->isKeyDown(OIS::KC_D) && input->isModifierDown(OIS::Keyboard::Alt)) {
		mMainNode->rotate(Vector3::NEGATIVE_UNIT_Y, (Radian)5 * elapsedTime);
	}
}

// Change visibility - Useful for 1st person view ;)

inline void OgreCharacter::setVisible(bool visible) {
	mMainNode->setVisible(visible);
}
