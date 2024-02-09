#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

#include "common.h"

class SceneNode;
class RenderInfo;

class GameObject { // <- wrapper
private:
    SceneNode* sceneNode;
    // obj highlight
    bool mouseSelectOn;

public:
    GameObject();
    virtual ~GameObject();

    void setSceneNode(SceneNode *sceneNode);

    virtual void update(float fDeltaTime);
    virtual void render(float fDeltaTime, RenderInfo* renderInfo);
    virtual void renderBB(float fDeltaTime, RenderInfo* renderInfo);

    virtual bool isDrawBBOn();
    virtual void setDrawBB(bool drawBB);

    virtual void setObjName(std::string objName);
    //virtual void* getBBMyObj();

    //--------------------------------------------------------------------------
    // Obj Highlight
    //--------------------------------------------------------------------------
    void setMouseSelectOn(bool mouseSelectOn);
    bool getMouseSelectOn();

    //--------------------------------------------------------------------------
    // transform
    //--------------------------------------------------------------------------
    glm::vec3 position; //obj's initial position when loaded
    glm::vec3 rotation; //obj's initial position when loaded
};
