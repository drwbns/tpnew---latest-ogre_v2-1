/**
 * File: MovableText.h
 *
 * description: This create create a billboarding object that display a text.
 * 
 * @author  2003 by cTh see gavocanov@rambler.ru
 * @update  2006 by barraq see nospam@barraquand.com
 * @update  2007 by independentCreations see independentCreations@gmail.com
 */

#ifndef __MovableText_H__
#define __MovableText_H__

#include "OGRE\OgrePrerequisites.h"

#include "OGRE\OgreRenderable.h"
#include <OGRE/OgreUTFString.h>
#include "OGRE\OgreMovableObject.h"
#include "OGRE\OgreRenderOperation.h"

//#include "OGRE\Overlay\OgreFont.h"

namespace Ogre{
	//class RenderOperation;
}

class MovableText : public Ogre::MovableObject, public Ogre::Renderable
{
public:
	enum HorizontalAlignment    {H_LEFT, H_CENTER};
	enum VerticalAlignment      {V_BELOW, V_ABOVE};

protected:
	Ogre::String			mFontName;
	Ogre::String			mType;
	Ogre::String			mName;
	Ogre::UTFString			mCaption;
	HorizontalAlignment		mHorizontalAlignment;
	VerticalAlignment		mVerticalAlignment;

	Ogre::ColourValue		mColor;
	Ogre::v1::RenderOperation	mRenderOp;
	Ogre::AxisAlignedBox	mAABB;
	Ogre::LightList			mLList;

	unsigned int			mCharHeight;
	unsigned int			mSpaceWidth;

	bool					mNeedUpdate;
	bool					mUpdateColors;
	bool					mOnTop;

	float					mTimeUntilNextToggle;
	float					mRadius;
	float					mAdditionalHeight;
	float					mAdditionalLeft;
	float					mAdditionalFront;

	Ogre::Camera		  * mpCam;
	Ogre::RenderWindow	  * mpWin;
	//Ogre::Font			  * mpFont;
	Ogre::MaterialPtr		mpMaterial;
	Ogre::MaterialPtr		mpBackgroundMaterial;

	/******************************** public methods ******************************/
public:
	MovableText(const Ogre::String & name, const Ogre::UTFString & caption,
		const Ogre::String & fontName = "BlueHighway", int charHeight = 32,
		const Ogre::ColourValue & color = Ogre::ColourValue::White);
	virtual ~MovableText();

	// Set settings
	void    setFontName(const Ogre::String & fontName);
	void    setCaption(const Ogre::UTFString & caption);
	void    setColor(const Ogre::ColourValue & color);
	void    setCharacterHeight(unsigned int height);
	void    setSpaceWidth(unsigned int width);
	void    setTextAlignment(const HorizontalAlignment & horizontalAlignment, const VerticalAlignment & verticalAlignment);
	void    setAdditionalHeight( float height );
	void    setAdditionalLeft( float left );
	void	setAdditionalFront( float front );
	void    showOnTop(bool show = true);

	// Get settings
	const   Ogre::String		& getFontName() const {return mFontName;}
	const   Ogre::UTFString     & getCaption() const {return mCaption;}
	const   Ogre::ColourValue	& getColor() const {return mColor;}
	
	unsigned int				getCharacterHeight() const {return mCharHeight;}
	unsigned int				getSpaceWidth() const {return mSpaceWidth;}
	float						getAdditionalHeight() const {return mAdditionalHeight;}
	float                       getRadius() const { return mRadius; }
	bool						getShowOnTop() const {return mOnTop;}
	Ogre::AxisAlignedBox		&GetAABB(void) { return mAABB; }

	virtual void visitRenderables()
	{}

	/******************************** protected methods and overload **************/
protected:

	float	mViewportAspectCoef;

	// from MovableText, create the object
	void	_setupGeometry();
	void	_updateColors();

	// from MovableObject
	void    getWorldTransforms(Ogre::Matrix4 *xform) const override;
	float   getBoundingRadius(void) const
	{return mRadius;};
	float   getSquaredViewDepth(const Ogre::Camera *cam) const
	{return 0;};
	const   Ogre::Quaternion        & getWorldOrientation(void) const;
	const   Ogre::Vector3           & getWorldPosition(void) const;
	const   Ogre::AxisAlignedBox    & getBoundingBox(void) const
	{return mAABB;};
	const   Ogre::String            & getName(void) const
	{return mName;};
	const   Ogre::String            & getMovableType(void) const override
	{static Ogre::String movType = "MovableText"; return movType;};

	void    _notifyCurrentCamera(Ogre::Camera *cam);
	void    _updateRenderQueue(Ogre::RenderQueue* queue);

	// from renderable
	void    getRenderOperation(Ogre::v1::RenderOperation &op,bool b) override;
	const   Ogre::MaterialPtr       &getMaterial(void) const
	{assert(!mpMaterial.isNull());return mpMaterial;};
	const   Ogre::LightList         &getLights(void) const override
	{return mLList;};
};

#endif /* __MovableText_H__ */