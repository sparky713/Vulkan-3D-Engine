#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/hash.hpp>

#include "VulkanCanvas.h"
#include <chrono>
//#include <vulkan/vulkan.h>

//#include "my_obj.h"
#include "mtl_loader.h"
#include "obj_loader.h"
#include "vulkan_obj_loader.h"

#include "hq_render.h"

#include "bb_my_obj.h"

extern glm::mat4 cameraView;
extern glm::mat4 cameraPerspective;

extern VulkanCanvas* g_canvas;

/**
* Default Constructor.
* Constructs a new VulkanMyObj object.
*/
BBMyObj::BBMyObj() : MyObj() {
    init(NULL, "", -1, -1, "", NULL);
}

/**
* Constructs a new VulkanMyObj object liked to its ObjLoader.
* @param objLoader the linked ObjLoader
*/
BBMyObj::BBMyObj(ObjLoader* objLoader) : MyObj(objLoader) {
    init(objLoader, "", -1, -1, "", NULL);
}

/**
* Initializes the given member fields of this VulkanMyObj.
* @param objLoader the linked ObjLoader
* @param objName the name of this 3D object
* @param indexBufStartIndex the starting index of objLoader's myFaces for this object
* @param indexBufEndIndex the ending index of objLoader's myFaces for this object
* @param mtlName the name of this object's material
* @param myMat this object's material
*/
void BBMyObj::init(ObjLoader* objLoader, std::string objName, int indexBufStartIndex, int indexBufEndIndex, std::string mtlName, MyMaterial* myMat) {
    MyObj::init(objLoader, objName, indexBufStartIndex, indexBufEndIndex, mtlName, myMat);
}

/**
 * Updates the uniform and index buffer.
 * Calls updateUniformBuffer() and updateIndices().
 */
void BBMyObj::update(float fDeltaTime) {
    setCurCode(g_canvas->camPosition);
    sortFaces();
    updateIndexBuffer();
}

void BBMyObj::render(float fDeltaTime, RenderInfo* renderInfo) {
    updateUniformBuffer(fDeltaTime, g_canvas->currentFrame, g_canvas->m_swapchainExtent.width, g_canvas->m_swapchainExtent.height);

    VkBuffer vertexBuffers[] = { (VkBuffer)(getVertexBuffer()) };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], (VkBuffer)(getIndexBuffer()), 0, VK_INDEX_TYPE_UINT32);

    const VkDescriptorSet* curDescSet = &(*(std::vector<VkDescriptorSet>*)(getDescriptorSets()))[g_canvas->currentFrame];

    vkCmdBindDescriptorSets((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, g_canvas->m_pipelineLayout, 0, 1, curDescSet, 0, nullptr);

    vkCmdDrawIndexed((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], static_cast<uint32_t>(getNumIndices()), 1, getIndexBufStartIndex(), 0, 0);
}

/**
* Sorts the index buffer of this object.
* Algorithm:
*   - normal of faces
*   - check if colliding with camera forward
*       - keep colliding faces in a list
*   - draw non-colliding faces first
*/
void BBMyObj::sortFaces() {
    std::vector<MyFace>& myFaces = getObjLoader()->myFaces;
    curBBFaces.clear();
    int curCaseIndex = bbFaceCases[curCode];
    //std::cout << "BBMyObj::sortFaces(): myFaces.size() = " << myFaces.size() << std::endl;

    for (int i = 0; i < FACES_NUM_INDICES; i++) {
        //std::cout << "BBMyObj::sortFaces(): i = " << i << std::endl;
        curBBFaces.push_back(getObjLoader()->myFaces[myFaceIndices[curCaseIndex][i]]); //ERROR HERE
    }

    //std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    //std::cout << " BBMyObj::sortFaces()2: myFaces\n";
    //std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    //for (int i = 0; i < myFaces.size(); i++) {
    //    //std::cout << i << " (" << myFaces[i].v[0] << ", " << myFaces[i].v[1] << ", " << myFaces[i].v[2] << ")" << std::endl;
    //    //std::cout << i << " (" << getObjLoader()->myFaces[i].v[0] << ", " << getObjLoader()->myFaces[i].v[1] << ", " << getObjLoader()->myFaces[i].v[2] << ")" << std::endl;
    //    std::cout << i << " (" << curBBFaces[i].v[0] << ", " << curBBFaces[i].v[1] << ", " << curBBFaces[i].v[2] << ")" << std::endl;
    //}
    //std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
}

/**
* Updates the index buffer of this object.
*/
void BBMyObj::updateIndexBuffer() {
    getObjLoader()->updateIndexBuffer(curBBFaces);
}

void BBMyObj::setCurCode(glm::vec3 camPos) {
    //--------------------------------------------------------------------------
    // convert camera position from world to local
    //--------------------------------------------------------------------------
    glm::vec4 camPosVec4 = glm::vec4(camPos.x, camPos.y, camPos.z, 0.0f);
    glm::vec3 camLocalPos = glm::vec3((inverse(localToWorldMat) * camPosVec4).x, (inverse(localToWorldMat) * camPosVec4).y,
                                      (inverse(localToWorldMat) * camPosVec4).z);
    //camLocalPos = 
    //glm::vec3 camLocalPos = glm::vec3(camPos.x - center.x, camPos.y - center.y, camPos.z - center.z);
    //camLocalPos = glm::normalize(camLocalPos); // normalise the vector
    //std::cout << "BBMyObj::checkCamera(): printVec3(camLocalPos)---------" << std::endl;
    //printVec3(camLocalPos);
    //--------------------------------------------------------------------------
    // check camera and bb's relative pos
    //--------------------------------------------------------------------------
    //...
    findBBFaceCode(camLocalPos);
}

void BBMyObj::findBBFaceCode(glm::vec3 camLocalPos) {
    curCode = "";
    ObjLoader *objLoader = getObjLoader();
    //--------------------------------------------------------------------------
    // update bb's pos
    //--------------------------------------------------------------------------
    if (objLoader != NULL) {
        calcBoxCenter(objLoader->myVertices);
        calcBoxLengths(objLoader->myVertices);
        calcBoxMinAndMax(objLoader->myVertices);
    }

    //--------------------------------------------------------------------------
    // find code
    //--------------------------------------------------------------------------
    //top view (F, R, L, Bk, T)
    if (camLocalPos.x >= center.x) { //1) F, R, L, T
        //curCode.append("F");
        //if (camLocalPos.x >= verts[objLoader->myFaces[0].v[0]].pos.x) {
        if (camLocalPos.x >= max.x) { //1a) F
            curCode.append("F");
            if (camLocalPos.z >= max.z) { //1ai) FL(T/Bt/none)
                curCode.append("L");
                if (camLocalPos.y >= max.y) { //FLT
                    curCode.append("T");
                }
                else if (camLocalPos.y <= min.y) { //FLBt
                    curCode.append("Bt");
                }
                else { //FL
                    // done
                }
            }
            else if (camLocalPos.z <= min.z) { //1aii) FR //CAN COMBINE
                curCode.append("R");
                if (camLocalPos.y >= max.y) { //FRT
                    curCode.append("T");
                }
                else if (camLocalPos.y <= min.y) { //FRBt
                    curCode.append("Bt");
                }
                else { //FR
                    // done
                }
            }
            else { //1aiii) F(T/Bt/none)
                if (camLocalPos.y >= max.y) { //FT
                    curCode.append("T");
                }
                else if (camLocalPos.y <= min.y) { //FBt
                    curCode.append("Bt");
                }
                else { //F
                    // done
                }
            }
        } //F
        else { //1b) L, R, T
            if (camLocalPos.z >= max.z) { //1ai) L(T/Bt/none)
                curCode.append("L");
                if (camLocalPos.y >= max.y) { //LT
                    curCode.append("T");
                }
                else if (camLocalPos.y <= min.y) { //LBt
                    curCode.append("Bt");
                }
                else { //L
                    // done
                }
            }
            else if (camLocalPos.z <= min.z) { //1aii) R
                curCode.append("R");
                if (camLocalPos.y >= max.y) { //RT
                    curCode.append("T");
                }
                else if (camLocalPos.y <= min.y) { //RBt
                    curCode.append("Bt");
                }
                else { //R
                    // done
                }
            }
            else { //1aiii) T/Bt/Center
                if (camLocalPos.y >= max.y) { //T
                    curCode.append("T");
                }
                else if (camLocalPos.y <= min.y) { //Bt
                    curCode.append("Bt");
                }
                else { //Center
                    curCode.append("Center");
                    // done
                }
            }
        } //Middle
    }
    else { //3) R, L, T, Bk
        if (camLocalPos.x <= min.x) { //1a) Bk
            //curCode.append("Bk");
            if (camLocalPos.z >= max.z) { //1ai) LBk(T/Bt/none)
                curCode.append("L");
                if (camLocalPos.y >= max.y) { //LTBk
                    curCode.append("TBk");
                }
                else if (camLocalPos.y <= min.y) { //LBtBk
                    curCode.append("BtBk");
                }
                else { //LBk
                    curCode.append("Bk");
                    // done
                }
            }
            else if (camLocalPos.z <= min.z) { //1aii) RBk
                curCode.append("R");
                if (camLocalPos.y >= max.y) { //RTBk
                    curCode.append("TBk");
                }
                else if (camLocalPos.y <= min.y) { //RBtBk
                    curCode.append("BtBk");
                }
                else { //RBk
                    curCode.append("Bk");
                    // done
                }
            }
            else { //1aiii) Bk(T/Bt/none)
                if (camLocalPos.y >= max.y) { //TBk
                    curCode.append("TBk");
                }
                else if (camLocalPos.y <= min.y) { //BtBk
                    curCode.append("BtBk");
                }
                else { //Bk
                    curCode.append("Bk");
                    // done
                }
            }
        } //F
        else { //1b) L, R, T
            if (camLocalPos.z >= max.z) { //1ai) L(T/Bt/none)
                curCode.append("L");
                if (camLocalPos.y >= max.y) { //LT
                    curCode.append("T");
                }
                else if (camLocalPos.y <= min.y) { //LBt
                    curCode.append("Bt");
                }
                else { //L
                    // done
                }
            }
            else if (camLocalPos.z <= min.z) { //1aii) R
                curCode.append("R");
                if (camLocalPos.y >= max.y) { //RT
                    curCode.append("T");
                }
                else if (camLocalPos.y <= min.y) { //RBt
                    curCode.append("Bt");
                }
                else { //R
                    // done
                }
            }
            else { //1aiii) T/Bt/Center
                if (camLocalPos.y >= max.y) { //T
                    curCode.append("T");
                }
                else if (camLocalPos.y <= min.y) { //Bt
                    curCode.append("Bt");
                }
                else { //Center
                    curCode.append("Center");
                    // done
                }
            }
        } //Middle
    }
    //std::cout << "BBMyObj::findBBFaceCode(...) = " << curCode << std::endl;
}

///**
//* Updates the uniform buffer of this object.
//* updateUniformBuffer() can alter the matrix to apply transformations to this object.
//* @param currentImage the index of the image to draw
//* @param m_swapchainExtent the resolution of the swap chain images (almost always equal to the resolution of the window that we're drawing to)
//*/
void BBMyObj::updateUniformBuffer(double deltaTime, uint32_t currentImage, int width, int height) {
    //std::cout << "BBMyObj::updateUniformBuffer(...): start" << std::endl;
    //std::cout << "BBMyObj::updateUniformBuffer(): myOwnerObj->getObjName(): " << myOwnerObj->getObjName() << std::endl;
    //std::cout << "BBMyObj::updateUniformBuffer(): printVec3(myOwnerObj->position): \n";
    //printVec3(myOwnerObj->position);
    
    //UniformBufferObject ubo{};
    //ubo.model = glm::rotate(glm::mat4(1.0f), (float)(deltaTime * glm::radians(0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = glm::rotate(glm::mat4(1.0f), (float)(glm::radians(myOwnerObj->rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.model = glm::translate(ubo.model, myOwnerObj->position);
    ubo.view = cameraView;
    ubo.proj = cameraPerspective;
    ubo.proj[1][1] *= -1;
    localToWorldMat = ubo.model;

    void* curImageUniformBuffersMapped = (*(std::vector<void*>*)(getUniformBuffersMapped()))[currentImage]; //??????????????????????????????? check
    memcpy(curImageUniformBuffersMapped, &ubo, sizeof(ubo));
}

/**
 * Calculate the center point of a box given all its vertices.
 */
void BBMyObj::calcBoxCenter(std::vector<MyVertex> vertices) {
    double centerX = 0.0, centerY = 0.0, centerZ = 0.0;

    for (MyVertex vertex : vertices) {
        centerX += vertex.x;
        centerY += vertex.y;
        centerZ += vertex.z;
    }

    int numVertices = vertices.size();
    centerX /= numVertices;
    centerY /= numVertices;
    centerZ /= numVertices;

    center = glm::vec3(centerX, centerY, centerZ);
}

/**
 * Calculate the lengths of a box given all its vertices.
 */
void BBMyObj::calcBoxLengths(std::vector<MyVertex> vertices) {
    double minX = INFINITY, minY = INFINITY, minZ = INFINITY;
    double maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

    for (MyVertex vertex : vertices) {
        if (vertex.x < minX) minX = vertex.x;
        if (vertex.y < minY) minY = vertex.y;
        if (vertex.z < minZ) minZ = vertex.z;
        if (vertex.x > maxX) maxX = vertex.x;
        if (vertex.y > maxY) maxY = vertex.y;
        if (vertex.z > maxZ) maxZ = vertex.z;
    }

    width = maxX - minX;
    height = maxY - minY;
    depth = maxZ - minZ;
};

void BBMyObj::calcBoxMinAndMax(std::vector<MyVertex> vertices) {
    double minX = INFINITY, minY = INFINITY, minZ = INFINITY;
    double maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

    for (MyVertex vertex : vertices) {
        if (vertex.x < minX) minX = vertex.x;
        if (vertex.y < minY) minY = vertex.y;
        if (vertex.z < minZ) minZ = vertex.z;
        if (vertex.x > maxX) maxX = vertex.x;
        if (vertex.y > maxY) maxY = vertex.y;
        if (vertex.z > maxZ) maxZ = vertex.z;
    }

    min = glm::vec3(minX, minY, minZ);
    max = glm::vec3(maxX, maxY, maxZ);
}
