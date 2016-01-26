//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef DOT_SCENELOADER_HPP
#define DOT_SCENELOADER_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

// Includes
#include "OgrePrerequisites.h"


#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainGroup.h"

#include "rapidxml.hpp"

class nodeProperty
{
public:
    Ogre::String nodeName;
    Ogre::String propertyNm;
    Ogre::String valueName;
    Ogre::String typeName;

    nodeProperty(const Ogre::String &node, const Ogre::String &propertyName, const Ogre::String &value, const Ogre::String &type)
        : nodeName(node), propertyNm(propertyName), valueName(value), typeName(type) {}
};

//|||||||||||||||||||||||||||||||||||||||||||||||

class DotSceneLoader
{
public:
    Ogre::TerrainGlobalOptions *mTerrainGlobalOptions;

    DotSceneLoader();
    virtual ~DotSceneLoader();

    void parseDotScene(const Ogre::String &SceneName, const Ogre::String &groupName, Ogre::SceneManager *yourSceneMgr, Ogre::SceneNode *pAttachNode = NULL, const Ogre::String &sPrependNode = "");
    Ogre::String getProperty(const Ogre::String &ndNm, const Ogre::String &prop);

    Ogre::TerrainGroup* getTerrainGroup() const 
	{ return mTerrainGroup; }

    std::vector<nodeProperty> nodeProperties;
    std::vector<Ogre::String> staticObjects;
    std::vector<Ogre::String> dynamicObjects;

protected:
    void processScene(rapidxml::xml_node<>* XMLRoot);

    void processNodes(rapidxml::xml_node<>* XMLNode);
	static void processExternals();
    void processEnvironment(rapidxml::xml_node<>* XMLNode) const;
    void processTerrain(rapidxml::xml_node<>* XMLNode);
    void processTerrainPage(rapidxml::xml_node<>* XMLNode);
    void processBlendmaps(rapidxml::xml_node<>* XMLNode) const;
	static void processUserDataReference();
	static void processUserDataReference(rapidxml::xml_node<>* XMLNode, Ogre::v1::Entity *pEntity);
	static void processOctree();
    void processLight(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent = 0);
    void processCamera(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent = 0) const;

    void processNode(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent = 0);
    void processLookTarget(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent) const;
    void processTrackTarget(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent) const;
    void processEntity(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);
    void processParticleSystem(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent) const;
	static void processBillboardSet();
	static void processPlane();

    void processFog(rapidxml::xml_node<>* XMLNode) const;
    void processSkyBox(rapidxml::xml_node<>* XMLNode) const;
    void processSkyDome(rapidxml::xml_node<>* XMLNode) const;
    void processSkyPlane(rapidxml::xml_node<>* XMLNode) const;
	static void processClipping(rapidxml::xml_node<>* XMLNode);

	static void processLightRange(rapidxml::xml_node<>* XMLNode, Ogre::Light *pLight);
	static void processLightAttenuation(rapidxml::xml_node<>* XMLNode, Ogre::Light *pLight);

	static Ogre::String getAttrib(rapidxml::xml_node<>* XMLNode, const Ogre::String &parameter, const Ogre::String &defaultValue = "");
	static Ogre::Real getAttribReal(rapidxml::xml_node<>* XMLNode, const Ogre::String &parameter, Ogre::Real defaultValue = 0);
	static bool getAttribBool(rapidxml::xml_node<>* XMLNode, const Ogre::String &parameter, bool defaultValue = false);

	static Ogre::Vector3 parseVector3(rapidxml::xml_node<>* XMLNode);
	static Ogre::Quaternion parseQuaternion(rapidxml::xml_node<>* XMLNode);
	static Ogre::ColourValue parseColour(rapidxml::xml_node<>* XMLNode);

    Ogre::SceneManager *mSceneMgr;
    Ogre::SceneNode *mAttachNode;
    Ogre::String m_sGroupName;
    Ogre::String m_sPrependNode;
    Ogre::TerrainGroup* mTerrainGroup;
    Ogre::Vector3 mTerrainPosition;
    Ogre::Vector3 mLightDirection;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||