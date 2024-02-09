#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>

class MyObj;
class MyMaterial;
class ObjLoader;
class RenderInfo;

/**
 * VulkanMyObj represents a single mesh / object in a Vulkan application.
 * The objects are created by an ObjLoader when loading a object file.
 */
class VulkanMyObj : public MyObj {
public:
    VulkanMyObj();
    VulkanMyObj(ObjLoader *objLoader);
    VulkanMyObj(ObjLoader *objLoader, bool drawBB);
    void init(ObjLoader* objLoader, std::string objName, int indexBufStartIndex, int indexBufEndIndex, std::string mtlName, MyMaterial* myMat);

    void updateUniformBuffer(double deltaTime, uint32_t currentImage, int width, int height);

    void update(float fDeltaTime);
    void render(float fDeltaTime, RenderInfo* renderInfo);
    void renderBB(float fDeltaTime, RenderInfo* renderInfo);

    void initBBMyObj();

    void calcMeshCenter(std::vector<Vertex> vertices);
    void calcMeshMinAndMax(std::vector<Vertex> vertices);

    UniformBufferObject ubo{};
    glm::mat4 uboRotate;
private:
    
    //--------------------------------------------------------------------------
    // camera
    //--------------------------------------------------------------------------
    glm::mat4 localToWorldMat; /**< model matrix */
    glm::mat4 rotMat; /**< keeps track of rotation */
    glm::vec4 camPosition; /**< position in space */
};
