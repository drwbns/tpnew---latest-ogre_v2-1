/**
 * File: MovableText.cpp
 *
 * description: This create create a billboarding object that display a text.
 *
 * @author  2003 by cTh see gavocanov@rambler.ru
 * @update  2006 by barraq see nospam@barraquand.com
 * @update  2007 by independentCreations see independentCreations@gmail.com
 */

#include "MovableText.h"

#include "OgreMaterialManager.h"
////#include "Overlay\OgreFontManager.h"
#include "OgreHardwareBufferManager.h"
#include "OgreRoot.h"
#include "OgreAxisAlignedBox.h"
#include "OgreCamera.h"
#include "OgreSceneNode.h"
//#include "Overlay/OgreOverlayPrerequisites.h"
//#include "Overlay\OgreFont.h"

#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1

MovableText::MovableText(const Ogre::String & name, const Ogre::UTFString & caption,
	const Ogre::String & fontName, int charHeight, const Ogre::ColourValue & color)
	: mFontName(fontName)
	, mType("MovableText")
	, mName(name)
	, mCaption(caption)
	, mHorizontalAlignment(H_LEFT)
	, mVerticalAlignment(V_BELOW)
	, mColor(color)
	, mCharHeight(charHeight)
	, mSpaceWidth(0)
	, mUpdateColors(true)
	, mOnTop(false)
	, mTimeUntilNextToggle(0)
	, mAdditionalHeight(0.0)
	, mAdditionalLeft(0.0)
	, mAdditionalFront(0.0)
	, mpCam(nullptr)
	, mpWin(nullptr)
	//, mpFont(nullptr)
	, mViewportAspectCoef(0.75) //set the attribute value before the first _setupGeometry call
{
	if (name == "")
		Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Trying to create MovableText without name", "MovableText::MovableText");

	if (caption == "")
		Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Trying to create MovableText without caption", "MovableText::MovableText");

	mRenderOp.vertexData = nullptr;
	this->setFontName(mFontName);
	this->_setupGeometry();

	//mViewportAspectCoef = 0.75;
}

MovableText::~MovableText()
{
	if (mRenderOp.vertexData)
		delete mRenderOp.vertexData;
}
/*
void MovableText::setFontName(const Ogre::String & fontName)
{
	if ((Ogre::MaterialManager::getSingletonPtr()->resourceExists(mName + "Material")))
	{
		Ogre::MaterialManager::getSingleton().remove(mName + "Material");
	}

	if (mFontName != fontName || mpMaterial.isNull() || !mpFont)
	{
		mFontName = fontName;
		mpFont = static_cast<Ogre::Font *>(Ogre::FontManager::getSingleton().getByName(mFontName).getPointer());
		if (!mpFont)
			Ogre::Exception(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Could not find font " + fontName, "MovableText::setFontName");

		mpFont->load();
		if (!mpMaterial.isNull())
		{
			Ogre::MaterialManager::getSingletonPtr()->remove(mpMaterial->getName());
			mpMaterial.setNull();
		}

		mpMaterial = mpFont->getMaterial()->clone(mName + "Material");
		if (!mpMaterial->isLoaded())
			mpMaterial->load();

		mpMaterial->setDepthCheckEnabled(!mOnTop);
		mpMaterial->setDepthBias(!mOnTop, 0);
		mpMaterial->setDepthWriteEnabled(mOnTop);
		mpMaterial->setLightingEnabled(false);
		mNeedUpdate = true;
	}
}
*/
void MovableText::setCaption(const Ogre::UTFString & caption)
{
	if (caption != mCaption)
	{
		mCaption = caption;
		mNeedUpdate = true;
	}
}

void MovableText::setColor(const Ogre::ColourValue & color)
{
	if (color != mColor)
	{
		mColor = color;
		mUpdateColors = true;
	}
}

void MovableText::setCharacterHeight(unsigned int height)
{
	if (height != mCharHeight)
	{
		mCharHeight = height;
		mNeedUpdate = true;
	}
}

void MovableText::setSpaceWidth(unsigned int width)
{
	if (width != mSpaceWidth)
	{
		mSpaceWidth = width;
		mNeedUpdate = true;
	}
}

void MovableText::setTextAlignment(const HorizontalAlignment & horizontalAlignment, const VerticalAlignment & verticalAlignment)
{
	if (mHorizontalAlignment != horizontalAlignment)
	{
		mHorizontalAlignment = horizontalAlignment;
		mNeedUpdate = true;
	}
	if (mVerticalAlignment != verticalAlignment)
	{
		mVerticalAlignment = verticalAlignment;
		mNeedUpdate = true;
	}
}

void MovableText::setAdditionalHeight(float height)
{
	if (mAdditionalHeight != height)
	{
		mAdditionalHeight = height;
		mNeedUpdate = true;
	}
}

void MovableText::setAdditionalLeft(float left)
{
	if (mAdditionalLeft != left)
	{
		mAdditionalLeft = left;
		mNeedUpdate = true;
	}
}

void MovableText::setAdditionalFront(float front)
{
	if (mAdditionalFront != front)
	{
		mAdditionalFront = front;
		mNeedUpdate = true;
	}
}

void MovableText::showOnTop(bool show)
{
	if (mOnTop != show && !mpMaterial.isNull())
	{
		mOnTop = show;
		//mpMaterial->setDepthBias(!mOnTop, 0);
		//mpMaterial->setDepthCheckEnabled(!mOnTop);
		//mpMaterial->setDepthWriteEnabled(mOnTop);
	}
}

/*
void MovableText::_setupGeometry()
{
	//assert(mpFont);
	assert(!mpMaterial.isNull());

	unsigned int vertexCount = static_cast<unsigned int>(mCaption.size() * 6);

	if (mRenderOp.vertexData)
	{
		// Removed this test as it causes problems when replacing a caption
		// of the same size: replacing "Hello" with "hello"
		// as well as when changing the text alignment
		//if (mRenderOp.vertexData->vertexCount != vertexCount)
		{
			delete mRenderOp.vertexData;
			mRenderOp.vertexData = nullptr;
			mUpdateColors = true;
		}
	}

	if (!mRenderOp.vertexData)
		mRenderOp.vertexData = new Ogre::v1::VertexData();

	mRenderOp.indexData = nullptr;
	mRenderOp.vertexData->vertexStart = 0;
	mRenderOp.vertexData->vertexCount = vertexCount;
	mRenderOp.operationType = Ogre::v1::RenderOperation::OT_TRIANGLE_LIST;
	mRenderOp.useIndexes = false;

	Ogre::v1::VertexDeclaration * decl = mRenderOp.vertexData->vertexDeclaration;
	Ogre::v1::VertexBufferBinding * bind = mRenderOp.vertexData->vertexBufferBinding;
	size_t offset = 0;

	// create/bind positions/tex.ccord. buffer
	if (!decl->findElementBySemantic(Ogre::VES_POSITION))
		decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

	offset += Ogre::v1::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	if (!decl->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES))
		decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

	Ogre::v1::HardwareVertexBufferSharedPtr ptbuf = Ogre::v1::HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(POS_TEX_BINDING),
		mRenderOp.vertexData->vertexCount,
		Ogre::v1::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	bind->setBinding(POS_TEX_BINDING, ptbuf);

	// Colours - store these in a separate buffer because they change less often
	if (!decl->findElementBySemantic(Ogre::VES_DIFFUSE))
		decl->addElement(COLOUR_BINDING, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);

	Ogre::v1::HardwareVertexBufferSharedPtr cbuf = Ogre::v1::HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(COLOUR_BINDING),
		mRenderOp.vertexData->vertexCount,
		Ogre::v1::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	bind->setBinding(COLOUR_BINDING, cbuf);

	size_t charlen = mCaption.size();
	float *pVert = static_cast<float*>(ptbuf->lock(Ogre::v1::HardwareBuffer::HBL_DISCARD));

	float largestWidth = 0;
	float left = 0 * 2.0 - 1.0;
	float top = -((0 * 2.0) - 1.0);

	// Derive space width from a capital A
	//if (mSpaceWidth == 0)
		//mSpaceWidth = mpFont->getGlyphAspectRatio('A') * mCharHeight * 2.0;

	// for calculation of AABB
	Ogre::Vector3 min = Ogre::Vector3(0, 0, 0);
	Ogre::Vector3 max = Ogre::Vector3(1, 1, 1);
	Ogre::Vector3 currPos;
	float maxSquaredRadius = 0.0f;
	bool first = true;

	// Use iterator
	Ogre::UTFString::iterator i, iend;
	iend = mCaption.end();
	bool newLine = true;
	float len = 0.0f;

	if (mVerticalAlignment == V_ABOVE)
	{
		// Raise the first line of the caption
		top += mCharHeight;
		for (i = mCaption.begin(); i != iend; ++i)
		{
			if (*i == '\n')
				top += mCharHeight * 2.0;
		}
	}

	iend = mCaption.end();
	for (i = mCaption.begin(); i != iend; ++i)
	{
		if (newLine)
		{
			float len1 = 0.0f;
			for (Ogre::UTFString::iterator j = i; j != iend; ++j)
			{
				Ogre::Font::CodePoint character = j.getCharacter();
				if (character == 0x000D // CR
					|| character == 0x0085) // NEL
				{
					break;
				}
				if (character == 0x0020) // space
				{
					len1 += mSpaceWidth;
				}
				else
				{
					len1 += mpFont->getGlyphAspectRatio(character) * mCharHeight * 2.0 * mViewportAspectCoef;
				}
			}

			/*if( mAlignment == Right )
				left -= len;
			else if( mAlignment == Center )
				left -= len * 0.5;

			newLine = false;
		}

		Ogre::Font::CodePoint character = i.getCharacter();
		if (character == 0x000D // CR
			|| character == 0x0085) // NEL
		{
			left = /*_getDerivedLeft() * 2.0 -1.0;
			top -= mCharHeight * 2.0;
			newLine = true;
			// Also reduce tri count
			mRenderOp.vertexData->vertexCount -= 6;
			continue;
		}
		else if (character == 0x0020) // space
		{
			// Just leave a gap, no tris
			left += mSpaceWidth;
			// Also reduce tri count
			mRenderOp.vertexData->vertexCount -= 6;
			continue;
		}

		float horiz_height = mpFont->getGlyphAspectRatio(character) * mViewportAspectCoef;
		const Ogre::Font::UVRect& uvRect = mpFont->getGlyphTexCoords(character);

		// each vert is (x, y, z, u, v)
		//-------------------------------------------------------------------------------------
		// First tri
		//
		// Upper left
		*pVert++ = left;
		*pVert++ = top;
		*pVert++ = -1.0;
		*pVert++ = uvRect.left;
		*pVert++ = uvRect.top;

		top -= mCharHeight * 2.0;

		// Bottom left
		*pVert++ = left;
		*pVert++ = top;
		*pVert++ = -1.0;
		*pVert++ = uvRect.left;
		*pVert++ = uvRect.bottom;

		top += mCharHeight * 2.0;
		left += horiz_height * mCharHeight * 2.0;

		// Top right
		*pVert++ = left;
		*pVert++ = top;
		*pVert++ = -1.0;
		*pVert++ = uvRect.right;
		*pVert++ = uvRect.top;
		//-------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// Second tri
		//
		// Top right (again)
		*pVert++ = left;
		*pVert++ = top;
		*pVert++ = -1.0;
		*pVert++ = uvRect.right;
		*pVert++ = uvRect.top;

		top -= mCharHeight * 2.0;
		left -= horiz_height  * mCharHeight * 2.0;

		// Bottom left (again)
		*pVert++ = left;
		*pVert++ = top;
		*pVert++ = -1.0;
		*pVert++ = uvRect.left;
		*pVert++ = uvRect.bottom;

		left += horiz_height  * mCharHeight * 2.0;

		// Bottom right
		*pVert++ = left;
		*pVert++ = top;
		*pVert++ = -1.0;
		*pVert++ = uvRect.right;
		*pVert++ = uvRect.bottom;
		//-------------------------------------------------------------------------------------

		// Go back up with top
		top += mCharHeight * 2.0;

		float currentWidth = (left + 1) / 2 /*- _getDerivedLeft();
		if (currentWidth > largestWidth)
		{
			largestWidth = currentWidth;
		}
	}

	// Unlock vertex buffer
	ptbuf->unlock();

	// update AABB/Sphere radius
	mAABB = Ogre::AxisAlignedBox(min, max);
	mRadius = Ogre::Math::Sqrt(maxSquaredRadius);

	if (mUpdateColors)
		this->_updateColors();

	mNeedUpdate = false;
}
*/

void MovableText::_updateColors(void)
{
	//assert(mpFont);
	assert(!mpMaterial.isNull());

	// Convert to system-specific
	Ogre::RGBA color;
	Ogre::Root::getSingleton().convertColourValue(mColor, &color);
	Ogre::v1::HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);
	Ogre::RGBA *pDest = static_cast<Ogre::RGBA*>(vbuf->lock(Ogre::v1::HardwareBuffer::HBL_DISCARD));
	for (unsigned int i = 0; i < mRenderOp.vertexData->vertexCount; ++i)
		*pDest++ = color;
	vbuf->unlock();
	mUpdateColors = false;
}

const Ogre::Quaternion & MovableText::getWorldOrientation(void) const
{
	assert(mpCam);
	return const_cast<Ogre::Quaternion&>(mpCam->getDerivedOrientation());
}

const Ogre::Vector3 & MovableText::getWorldPosition(void) const
{
	assert(mParentNode);
	return mParentNode->_getDerivedPosition();
}

void MovableText::getWorldTransforms(Ogre::Matrix4 * xform) const
{
	if (this->isVisible() && mpCam)
	{
		Ogre::Matrix3 rot3x3, scale3x3 = Ogre::Matrix3::IDENTITY;

		// store rotation in a matrix
		mpCam->getDerivedOrientation().ToRotationMatrix(rot3x3);

		// parent node position
		Ogre::Vector3 ppos = mParentNode->_getDerivedPosition() + Ogre::Vector3::UNIT_X * mAdditionalLeft + Ogre::Vector3::UNIT_Y * mAdditionalHeight + Ogre::Vector3::UNIT_Z * mAdditionalFront;

		// apply scale
		scale3x3[0][0] = mParentNode->_getDerivedScale().x / 2;
		scale3x3[1][1] = mParentNode->_getDerivedScale().y / 2;
		scale3x3[2][2] = mParentNode->_getDerivedScale().z / 2;

		// apply all transforms to xform
		*xform = (rot3x3 * scale3x3);
		xform->setTrans(ppos);
	}
}

void MovableText::getRenderOperation(Ogre::v1::RenderOperation & op, bool b)
{
	if (this->isVisible())
	{
		if (mNeedUpdate)
			this->_setupGeometry();
		if (mUpdateColors)
			this->_updateColors();
		op = mRenderOp;
	}
}

void MovableText::_notifyCurrentCamera(Ogre::Camera *cam)
{
	mpCam = cam;
}

void MovableText::_updateRenderQueue(Ogre::RenderQueue* queue)
{
	if (this->isVisible())
	{
		if (mNeedUpdate)
			this->_setupGeometry();
		if (mUpdateColors)
			this->_updateColors();

		//queue->addRenderable(this, mRenderQueueID, OGRE_RENDERABLE_DEFAULT_PRIORITY);
		//      queue->addRenderable(this, mRenderQueueID, RENDER_QUEUE_SKIES_LATE);
	}
}