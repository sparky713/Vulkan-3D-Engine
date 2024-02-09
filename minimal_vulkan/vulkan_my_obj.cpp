#include <iomanip>

#define VK_USE_PLATFORM_WIN32_KHR

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/hash.hpp>
 
#include "VulkanCanvas.h"
#include <chrono>
//#include <vulkan/vulkan.h>
#include "bb_my_obj.h"
#include "my_obj.h"
#include "mtl_loader.h"
#include "vulkan_obj_loader.h"
#include "obj_loader.h"

#include "non_vk_common.h"
#include "common.h"
#include "hq_render.h"

#include "vulkan_my_obj.h"

extern glm::mat4 cameraView;
extern glm::mat4 cameraPerspective;

extern VulkanCanvas *g_canvas;

/**
* Default Constructor.
* Constructs a new VulkanMyObj object.
*/
VulkanMyObj::VulkanMyObj() : MyObj() {
    init(NULL, "", -1, -1, "", NULL);
}

/**
* Constructs a new VulkanMyObj object linked to its ObjLoader.
* @param objLoader the linked ObjLoader
*/
VulkanMyObj::VulkanMyObj(ObjLoader *objLoader) : MyObj(objLoader) {
    init(objLoader, "", -1, -1, "", NULL);
}

VulkanMyObj::VulkanMyObj(ObjLoader* objLoader, bool drawBB) : MyObj(objLoader) {
    this->setDrawBB(drawBB);
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
void VulkanMyObj::init(ObjLoader* objLoader, std::string objName, int indexBufStartIndex, int indexBufEndIndex, std::string mtlName, MyMaterial* myMat) {
    MyObj::init(objLoader, objName, indexBufStartIndex, indexBufEndIndex, mtlName, myMat);
    setMouseSelectOn(false);
}

/**
* Updates the uniform buffer of this object.
* updateUniformBuffer() can alter the matrix to apply transformations to this object.
* @param currentImage the index of the image to draw
* @param m_swapchainExtent the resolution of the swap chain images (almost always equal to the resolution of the window that we're drawing to)
*/
void VulkanMyObj::updateUniformBuffer(double deltaTime, uint32_t currentImage, int width, int height) {
    //UniformBufferObject ubo{};
//    ubo.model = glm::rotate(glm::mat4(1.0f), (float)(deltaTime * glm::radians(rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.model = glm::rotate(glm::mat4(1.0f), (float)(glm::radians(rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f));
    uboRotate = ubo.model;
    ubo.model = glm::translate(glm::mat4(ubo.model), position);

    ubo.view = cameraView;

    ubo.proj = cameraPerspective;
    ubo.proj[1][1] *= -1;
    
    localToWorldMat = ubo.model;
    void* curImageUniformBuffersMapped = (*(std::vector<void*>*)(getUniformBuffersMapped()))[currentImage];
    memcpy(curImageUniformBuffersMapped, &ubo, sizeof(ubo));
}

void VulkanMyObj::update(float fDeltaTime) {
    if (getObjName() != "Axis" && getObjName() != "y" && getObjName() != "-y" && getObjName() != "x"
        && getObjName() != "-x" && getObjName() != "z" && getObjName() != "-z") { // turn off update for axis so that we don't highlight it, axis is not a selectable object
        getBBMyObj()->update(fDeltaTime);
    }
    
}

void VulkanMyObj::render(float fDeltaTime, RenderInfo* renderInfo) {
    if (DRAW_OBJ_ON) {

        //----------------------------------------------------------------------
        // outline
        //----------------------------------------------------------------------
        if (getMouseSelectOn()) {
            //vkCmdBindPipeline((*renderInfo->commandBuffers)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, outlinePSO);
            vkCmdBindPipeline((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, g_canvas->outlinePSO);

            if (draw) {
                updateUniformBuffer(fDeltaTime, g_canvas->currentFrame, g_canvas->m_swapchainExtent.width, g_canvas->m_swapchainExtent.height);

                VkBuffer vertexBuffers[] = { (VkBuffer)(getVertexBuffer()) };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], (VkBuffer)(getIndexBuffer()), 0, VK_INDEX_TYPE_UINT32);

                const VkDescriptorSet* curDescSet = &(*(std::vector<VkDescriptorSet>*)(getDescriptorSets()))[g_canvas->currentFrame];

                vkCmdBindDescriptorSets((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, g_canvas->m_pipelineLayout, 0, 1, curDescSet, 0, nullptr);

                vkCmdDrawIndexed((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], static_cast<uint32_t>(getNumIndices()), 1, getIndexBufStartIndex(), 0, 0);
            }
        }

        //----------------------------------------------------------------------
        // regular
        //----------------------------------------------------------------------
        vkCmdBindPipeline((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, g_canvas->m_graphicsPipeline);

        if (draw) {
            updateUniformBuffer(fDeltaTime, g_canvas->currentFrame, g_canvas->m_swapchainExtent.width, g_canvas->m_swapchainExtent.height);

            VkBuffer vertexBuffers[] = { (VkBuffer)(getVertexBuffer()) };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], (VkBuffer)(getIndexBuffer()), 0, VK_INDEX_TYPE_UINT32);

            const VkDescriptorSet* curDescSet = &(*(std::vector<VkDescriptorSet>*)(getDescriptorSets()))[g_canvas->currentFrame];

            vkCmdBindDescriptorSets((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, g_canvas->m_pipelineLayout, 0, 1, curDescSet, 0, nullptr);

            vkCmdDrawIndexed((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], static_cast<uint32_t>(getNumIndices()), 1, getIndexBufStartIndex(), 0, 0);
        }
    }
    //if (isDrawBBOn()) {
    //    getBBMyObj()->render(fDeltaTime, renderInfo);
    //}
}

void VulkanMyObj::renderBB(float fDeltaTime, RenderInfo* renderInfo) {
    if (isDrawBBOn()) {
        getBBMyObj()->render(fDeltaTime, renderInfo);
    }
}

void VulkanMyObj::initBBMyObj() {
    // code removed for copyright
}

void VulkanMyObj::calcMeshCenter(std::vector<Vertex> vertices) {
    double centerX = 0.0, centerY = 0.0, centerZ = 0.0;
    for (int i = (getIndexBufStartIndex() / 3); i < (getIndexBufEndIndex() / 3) + 1; i++) {

        for (int j = 0; j < 3; j++) {
            Vertex vertex = vertices[getObjLoader()->myFaces[i].v[j]];
            centerX += vertex.pos.x;
            centerY += vertex.pos.y;
            centerZ += vertex.pos.z;
        }
    }

    int numVertices = getNumIndices() / 3;
    centerX /= numVertices;
    centerY /= numVertices;
    centerZ /= numVertices;

    center = glm::vec3(centerX, centerY, centerZ);
}

void VulkanMyObj::calcMeshMinAndMax(std::vector<Vertex> vertices) {
    double minX = INFINITY, minY = INFINITY, minZ = INFINITY;
    double maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

    //for (Vertex vertex : vertices) {
    //for (int i = getIndexBufStartIndex(); i < getIndexBufEndIndex(); i++) {
    for (int i = (getIndexBufStartIndex() / 3); i < (getIndexBufEndIndex() / 3) + 1; i++) {
        for (int j = 0; j < 3; j++) {
            Vertex vertex = vertices[getObjLoader()->myFaces[i].v[j]];

            if (vertex.pos.x < minX) minX = vertex.pos.x;
            if (vertex.pos.y < minY) minY = vertex.pos.y;
            if (vertex.pos.z < minZ) minZ = vertex.pos.z;
            if (vertex.pos.x > maxX) maxX = vertex.pos.x;
            if (vertex.pos.y > maxY) maxY = vertex.pos.y;
            if (vertex.pos.z > maxZ) maxZ = vertex.pos.z;
        }
    }
    
    min = glm::vec3(minX, minY, minZ);
    max = glm::vec3(maxX, maxY, maxZ);
}
