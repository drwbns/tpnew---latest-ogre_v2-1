#include "SampleListener.h"

//-------------------------------------------------------------------------------------
bool SampleListener::keyPressed( const OIS::KeyEvent &arg )
{
	return true;
}

bool SampleListener::keyReleased( const OIS::KeyEvent &arg )
{
	if (arg.key == OIS::KC_W){
		mChar->setAnimationState("Idle1");
	}
    return true;
}

bool SampleListener::mouseMoved( const OIS::MouseEvent &arg )
{


    return true;
}

bool SampleListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    return true;
}

bool SampleListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    return true;
}