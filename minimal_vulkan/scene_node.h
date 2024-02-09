#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

class GameObject;
class RenderInfo;

class SceneNode { // <- wrapper
private:
    std::string name;
    GameObject *gameObject;

    //std::vector<Component *> components;

    SceneNode *parent;
    //std::vector<SceneNode*> children;
public:
    std::vector<SceneNode*> children;

    SceneNode();
    SceneNode(std::string name, GameObject *gameObject);
    virtual ~SceneNode();

    void setName(std::string name);
    void setParent(SceneNode *parent);
    void setGameObject(GameObject *gameObject);

    std::string getName();
    GameObject *getGameObject();
    SceneNode *getParent();
    std::vector<SceneNode*>& getChildren();

    void addChild(SceneNode* newChild);
    void removeChildAt(int index);
    // SceneNode *removeChildByName(string name);
    // SceneNode *getChildAt(int index);
    // SceneNode *getChildByName(string name);

    virtual void update(float fDeltaTime);
    virtual void render(float fDeltaTime, RenderInfo *renderInfo);
    virtual void renderBB(float fDeltaTime, RenderInfo* renderInfo);
};
