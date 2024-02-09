#include "game_object.h"

//#include <vulkan/vulkan.h>

#include "non_vk_common.h"

GameObject::GameObject() {
    sceneNode = NULL;
}

GameObject::~GameObject() {

}

void GameObject::setSceneNode(SceneNode* sceneNode) {
    this->sceneNode = sceneNode;
}

void GameObject::update(float fDeltaTime) {

}

void GameObject::render(float fDeltaTime, RenderInfo *renderInfo) {

}

void GameObject::renderBB(float fDeltaTime, RenderInfo* renderInfo) {

}

bool GameObject::isDrawBBOn() {
    // child
    return false;
}

void GameObject::setDrawBB(bool drawBB) {
    // child
}

void GameObject::setObjName(std::string objName) {
    // child
}

void GameObject::setMouseSelectOn(bool mouseSelectOn) {
    this->mouseSelectOn = mouseSelectOn;
}

bool GameObject::getMouseSelectOn() {
    return mouseSelectOn;
}
