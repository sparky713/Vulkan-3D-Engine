#include <vulkan/vulkan.h>

#include "game_object.h"

#include "scene_node.h"
#include "non_vk_common.h"

SceneNode::SceneNode() {
    gameObject = NULL;
    //components.push_back(new Transform());
    children.reserve(1000);
    parent = NULL;
}

SceneNode::SceneNode(std::string name, GameObject* gameObject) {
    this->name = name;
    this->gameObject = gameObject;

    children.reserve(1000);
    parent = NULL;
}

SceneNode::~SceneNode() {
    if (gameObject != NULL) {
        delete gameObject;
    }
}

void SceneNode::setName(std::string name) {
    this->name = name;
    getGameObject()->setObjName(name);
}

void SceneNode::setParent(SceneNode* parent) {
    this->parent = parent;
}

void SceneNode::setGameObject(GameObject* gameObject) {
    this->gameObject = gameObject;
}

std::string SceneNode::getName() {
    return name;
}

GameObject* SceneNode::getGameObject() {
    return gameObject;
}

SceneNode* SceneNode::getParent() {
    return parent;
}

std::vector<SceneNode*>& SceneNode::getChildren() {
    return children;
}

void SceneNode::addChild(SceneNode* newChild) {
    //std::cout << "SceneNode::addChild(): start" << std::endl;
    children.push_back(newChild);
}

void SceneNode::removeChildAt(int index) {
    children.erase(children.begin() + index);
}


/**
 * Calls the gameObject (if gameObject is not null) and its children's
 * update.
 */
void SceneNode::update(float fDeltaTime) {
    if (gameObject != NULL) {
        gameObject->update(fDeltaTime);
    }

    for (int i = 0; i < children.size(); i++) {
        children[i]->update(fDeltaTime);
    }
}

/**
 * Calls the gameObject (if gameObject is not null) and its children's
 * render.
 */
void SceneNode::render(float fDeltaTime, RenderInfo *renderInfo) {
    if (gameObject != NULL) {
        gameObject->render(fDeltaTime, renderInfo);
    }

    for (int i = 0; i < children.size(); i++) {
        children[i]->render(fDeltaTime, renderInfo);
    }
}

void SceneNode::renderBB(float fDeltaTime, RenderInfo* renderInfo) {
    if (gameObject != NULL) {
        gameObject->renderBB(fDeltaTime, renderInfo);
    }

    for (int i = 0; i < children.size(); i++) {
        children[i]->renderBB(fDeltaTime, renderInfo);
    }
}
