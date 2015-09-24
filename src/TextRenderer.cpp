
#include "TextRenderer.h"

#include <Overlay/OgreOverlay.h>

#include "GraphicsSystem.h"

TextRenderer::TextRenderer()
{
	pOverlaySystem = GSYS->GetOverlaySystem();
	GSYS->GetSceneMgr()->addRenderQueueListener(pOverlaySystem);

	_overlayMgr = GSYS->GetOverlayMgr();

	_overlay = _overlayMgr->create("overlay1");
	_panel = static_cast<Ogre::OverlayContainer*>(_overlayMgr->createOverlayElement("Panel", "container1"));
	_panel->setDimensions(1, 1);
	_panel->setPosition(0, 0);

	_overlay->add2D(_panel);

	_overlay->show();

	txts.clear();
}

TextRenderer::~TextRenderer()
{
	//_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
	//_overlayMgr->destroyAll();
	_overlayMgr = NULL;
}

void TextRenderer::addTextBox(
	               const std::string& ID,
	             const std::string& text,
	          Ogre::Real x, Ogre::Real y,
	 Ogre::Real width, Ogre::Real height,
	      const Ogre::ColourValue& color)
{
	Ogre::OverlayElement* textBox = _overlayMgr->createOverlayElement("TextArea", ID);
	textBox->setDimensions(width, height);
	textBox->setMetricsMode(Ogre::GMM_PIXELS);
	textBox->setPosition(x, y);
	textBox->setWidth(width);
	textBox->setHeight(height);
	textBox->setParameter("font_name", "test");
	textBox->setParameter("char_height", "20");
	textBox->setColour(color);

	textBox->setCaption(text);

	_panel->addChild(textBox);

	txts.push_back(ID);
}

void TextRenderer::removeTextBox(const std::string& ID)
{
	_panel->removeChild(ID);
	_overlayMgr->destroyOverlayElement(ID);
	//remove from list
	std::vector<std::string>::iterator it = txts.begin();
	while (it != txts.end())
	{
		if (*it == ID)
		{
			txts.erase(it);
			break;
		}
		it++;
	}
}

void TextRenderer::reset()
{
	for (int i=0;i<(int)txts.size();i++)
	{
		removeTextBox(txts[i]);
	}
}

void TextRenderer::setText(const std::string& ID, const std::string& Text)
{
	Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
	textBox->setCaption(Text);
}

void TextRenderer::setPosition(const std::string& ID,float x,float y)
{
	Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
	textBox->setPosition(x, y);
}