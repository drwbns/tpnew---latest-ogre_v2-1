#include "Character.h"

#include "OgreEntity.h"
#include "OgreAnimationState.h"
#include "OgreSceneNode.h"
#include "OgreVector3.h"
#include "OgreSceneManager.h"

SceneNode * Character::getSightNode() const
{
	return mSightNode;
}

SceneNode * Character::getCameraNode() const
{
	return mCameraNode;
}

SceneNode * Character::getTopCamNode() const
{
	return mTopCamNode;
}

v1::AnimationState * Character::getAnimState() const
{
	return mAnimationState;
}

void Character::setAnimationState(String string) {
	mAnimationState = mEntity->getAnimationState(string.c_str());
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
}

SceneNode * Character::getYawNode() const
{
	return mYawNode;
}

SceneNode * Character::getPitchNode() const
{
	return mPitchNode;
}

SceneNode * Character::getRollNode() const
{
	return mRollNode;
}

SceneNode * Character::getMainNode() const
{
	return mMainNode;
}

SceneNode * Character::getPlayerNode() const
{
	return mPlayerNode;
}

void Character::setTightness(Real tightness) {
	mTightness = tightness;
}

Real Character::getTightness() const
{
	return mTightness;
}

Vector3 Character::getPlayerDirection() const
{
	Matrix3 matrix;
	mMainNode->getOrientation().ToRotationMatrix(matrix);
	Vector3 mDirection = matrix.GetColumn(2);
	return mDirection;
}

Vector3 Character::getCameraDirection() const
{
	Matrix3 matrix;
	mCameraNode->getOrientation().ToRotationMatrix(matrix);
	Vector3 mDirection = matrix.GetColumn(2);
	return mDirection;
}

void Character::instantUpdate(Vector3 cameraPosition, Vector3 targetPosition) const
{
	mCameraNode->setPosition(cameraPosition);
	mSightNode->setPosition(targetPosition);
}

void Character::update(Vector3 cameraPosition, Vector3 targetPosition) const
{
	// Handle movement
	Vector3 displacement;

	displacement = (cameraPosition - mCameraNode->_getDerivedPosition()) * mTightness;
	mCameraNode->translate(displacement);

	displacement = (targetPosition - mSightNode->_getDerivedPosition()) * mTightness;
	mSightNode->translate(displacement);
}

void Character::SetDirection(Ogre::Quaternion * q) { Direction = q; }

OgreCharacter::OgreCharacter(String name, SceneManager * sceneMgr): Character()
{
	// Setup basic member references
	mName = name;
	mSceneMgr = sceneMgr;
	mMouseSpeed = 50;
	// Setup basic node structure to handle 3rd person cameras
	*mTranslateVector = Ogre::Vector3::ZERO;

	mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//mMainNode->showBoundingBox(true);

	mPlayerNode = mMainNode->createChildSceneNode();
	mSightNode = mPlayerNode->createChildSceneNode(Ogre::SCENE_DYNAMIC, Vector3(0, 0, -300));
	//mSightNode->showBoundingBox(true);
	mTopCamNode = mMainNode->createChildSceneNode(Ogre::SCENE_DYNAMIC, Vector3(0, 175, 0));
	mYawNode = mTopCamNode->createChildSceneNode();
	mPitchNode = mYawNode->createChildSceneNode();
	mRollNode = mPitchNode->createChildSceneNode();
	mCameraNode = mRollNode->createChildSceneNode();
	//mCameraNode->showBoundingBox(true);
	mMainNode->setScale(0.1, 0.1, 0.1);
	// Give this character a shape :)
	mEntity = mSceneMgr->createEntity(mName, "Ninja.mesh");

	mPlayerNode->attachObject(mEntity);

	// Set idle animation
	setAnimationState("Idle1");
}

OgreCharacter::~OgreCharacter() {
	mMainNode->detachAllObjects();
	delete mEntity;
	mMainNode->removeAndDestroyAllChildren();
	mSceneMgr->destroySceneNode(mMainNode);
}

void OgreCharacter::update(Real elapsedTime, OIS::Keyboard * input, OIS::Mouse * mInput) {
	// Handle movement
	if (mInput->getMouseState().buttonDown(OIS::MB_Left)) {
		setAnimationState("Attack3");
		getAnimState()->setLoop(false);
		if (getAnimState()->hasEnded())
			getAnimState()->setTimePosition(0);
	}
	// Handle movement
	if (mInput->getMouseState().buttonDown(OIS::MB_Right)) {
		setAnimationState("Walk");
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
		mMainNode->rotate(Vector3::NEGATIVE_UNIT_Y, static_cast<Radian>(-5) * elapsedTime);
	}
	if (input->isKeyDown(OIS::KC_D) && input->isModifierDown(OIS::Keyboard::Alt)) {
		mMainNode->rotate(Vector3::NEGATIVE_UNIT_Y, static_cast<Radian>(5) * elapsedTime);
	}
}

// Change visibility - Useful for 1st person view ;)

void OgreCharacter::setVisible(bool visible) const
{
	mMainNode->setVisible(visible);
}