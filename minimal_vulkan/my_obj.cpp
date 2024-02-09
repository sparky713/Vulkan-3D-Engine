//#define VK_USE_PLATFORM_WIN32_KHR
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "non_vk_common.h"
#include "my_obj.h"
#include "bb_my_obj.h"
#include "obj_loader.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// MyObj Class Implementation
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructors
//------------------------------------------------------------------------------
/**
* Default Constructor.
* Constructs a new MyObj object.
*/
MyObj::MyObj() {
    init(NULL, "", -1, -1, "", NULL);
    draw = true;
}

/**
* Constructs a new MyObj object liked to its ObjLoader.
* @param objLoader the linked ObjLoader
*/
MyObj::MyObj(ObjLoader* objLoader) {
    init(objLoader, "", -1, -1, "", NULL);
    draw = true;
}

MyObj::~MyObj() {
    delete bbMyObj;
}

/**
* Initializes the given member fields of this MyObj.
* @param objLoader the linked ObjLoader
* @param objName the name of this 3D object
* @param indexBufStartIndex the starting index of objLoader's myFaces for this object
* @param indexBufEndIndex the ending index of objLoader's myFaces for this object
* @param mtlName the name of this object's material
* @param myMat this object's material
*/
void MyObj::init(ObjLoader* objLoader, std::string objName, int indexBufStartIndex, int indexBufEndIndex, std::string mtlName, MyMaterial* myMat) {
    this->objLoader = objLoader;
    this->objName = objName;
    this->indexBufStartIndex = indexBufStartIndex;
    this->indexBufEndIndex = indexBufEndIndex;
    this->mtlName = mtlName;
    this->myMat = myMat;

    this->numIndices = (indexBufEndIndex - indexBufStartIndex + 1) * 3;
}

//------------------------------------------------------------------------------
// getters
//------------------------------------------------------------------------------
/**
 * Returns objLoader.
 * @return objLoader
 */
ObjLoader* MyObj::getObjLoader() {
    return objLoader;
}

/**
 * Returns the name of the object.
 * @return the name of the object
 */
std::string MyObj::getObjName() {
    return objName;
}

/**
 * Returns the starting index of the index buffer.
 * @return the starting index of the index buffer
 */
int MyObj::getIndexBufStartIndex() {
    return indexBufStartIndex;
}

/**
 * Returns the end index of the index buffer.
 * @return the end index of the index buffer
 */
int MyObj::getIndexBufEndIndex() {
    return indexBufEndIndex;
}

/**
 * Returns the number of indices of the index buffer.
 * @return the number of indices of the index buffer
 */
int MyObj::getNumIndices() {
    return numIndices;
}

/**
 * Returns the name of the object's material.
 * @return the name of the object's material
 */
std::string MyObj::getMtlName() {
    return mtlName;
}

/**
 * Returns the object's material.
 * @return the object's material
 */
MyMaterial* MyObj::getMyMaterial() {
    return myMat;
}

/**
 * Returns the vertex buffer.
 * @return the vertex buffer
 */
void* MyObj::getVertexBuffer() {
    return objLoader->getVertexBuffer();
}

/**
 * Returns the index buffer.
 * @return the index buffer
 */
void* MyObj::getIndexBuffer() {
    return objLoader->getIndexBuffer();
}

/**
 * Returns the texture image.
 * @return the texture image
 */
void* MyObj::getTextureImages() {
    return objLoader->getTextureImages(mtlName);
}

/**
 * Returns the texture image memory.
 * @return the texture image memory
 */
void* MyObj::getTextureImageMemories() {
    return objLoader->getTextureImageMemories(mtlName);
}

/**
 * Returns the texture image view.
 * @return the texture image view
 */
void* MyObj::getTextureImageViews() {
    return objLoader->getTextureImageViews(mtlName);
}

/**
 * Returns the texture samplers.
 * @return the texture samplers
 */
void* MyObj::getTextureSamplers() {
    return objLoader->getTextureSamplers(mtlName);
}

/**
 * Returns the uniform buffer.
 * @return the uniform buffer
 */
void* MyObj::getUniformBuffers() {
    return objLoader->getUniformBuffers(mtlName);
}

/**
 * Returns the uniform buffer memory.
 * @return the uniform buffer memory
 */
void* MyObj::getUniformBuffersMemories() {
    return objLoader->getUniformBuffersMemories(mtlName);
}

/**
 * Returns the uniform buffer mapped.
 * @return the uniform buffer mapped
 */
void* MyObj::getUniformBuffersMapped() {
    return objLoader->getUniformBuffersMapped(mtlName);
}

/**
 * Returns the desscriptor pool.
 * @return the desscriptor pool
 */
void* MyObj::getDescriptorPools() {
    return objLoader->getDescriptorPools(mtlName);
}

/**
 * Returns the desscriptor set.
 * @return the desscriptor set
 */
void* MyObj::getDescriptorSets() {
    return objLoader->getDescriptorSets(mtlName);
}

//bool MyObj::getMouseSelectOn() {
//    return mouseSelectOn;
//}

//------------------------------------------------------------------------------
// setters
//------------------------------------------------------------------------------
/**
 * sets the objLoader.
 * @param objLoader objLoader
 */
void MyObj::setObjLoader(ObjLoader* objLoader) {
    this->objLoader = objLoader;
}

/**
 * sets the name of the object.
 * @param objName the name of the object
 */
void MyObj::setObjName(std::string objName) {
    this->objName = objName;
}

/**
 * sets the starting index of the index buffer.
 * @param indexBufStartIndex the starting index of the index buffer
 */
void MyObj::setIndexBufStartIndex(int indexBufStartIndex) {
    this->indexBufStartIndex = indexBufStartIndex;
}

/**
 * sets the end index of the index buffer.
 * @param indexBufEndIndex the end index of the index buffer
 */
void MyObj::setIndexBufEndIndex(int indexBufEndIndex) {
    this->indexBufEndIndex = indexBufEndIndex;
}

/**
 * sets the starting and end index of the index buffer.
 * @param indexBufStartIndex the starting index of the index buffer
 * @param indexBufEndIndex the end index of the index buffer
 */
void MyObj::setIndexBufIndices(int indexBufStartIndex, int indexBufEndIndex) {
    this->indexBufStartIndex = indexBufStartIndex;
    this->indexBufEndIndex = indexBufEndIndex;
    setNumIndices((indexBufEndIndex - indexBufStartIndex + 1) * 3);
}

/**
 * sets the number of indices of the index buffer.
 * @param numIndices the number of indices of the index buffer
 */
void MyObj::setNumIndices(int numIndices) {
    this->numIndices = numIndices;
}

/**
 * sets the name of the material.
 * @param mtlName the name of the material
 */
void MyObj::setMtlName(std::string mtlName) {
    this->mtlName = mtlName;
}

/**
 * sets the material.
 * @param myMat the material
 */
void MyObj::setMyMaterial(MyMaterial* myMat) {
    this->myMat = myMat;
}

//void MyObj::setMouseSelectOn(bool mouseSelectOn) {
//    this->mouseSelectOn = mouseSelectOn;
//}
//------------------------------------------------------------------------------

void MyObj::update(float fDeltaTime) {

}

void MyObj::render(float fDeltaTime, RenderInfo* renderInfo) {
    // to child
}

void MyObj::renderBB(float fDeltaTime, RenderInfo* renderInfo) {
    // to child
}

void MyObj::initBBMyObj() {
    std::cout << "MyObj::initBBMyObj()" << std::endl;
    // goes to child
}

/**
 * Returns true if drawBB is true, false otherwise.
 * @return true if drawBB is true, false otherwise
 */
bool MyObj::isDrawBBOn() {
    return drawBB;
}

void MyObj::setDrawBB(bool drawBB) {
    this->drawBB = drawBB;
}

void MyObj::setBBMyObj(MyObj* bbMyObj) {
    this->bbMyObj = bbMyObj;
}

/**
 * Returns this object's bounding box object.
 */
MyObj *MyObj::getBBMyObj() {
    if (bbMyObj != NULL) return bbMyObj;
}
