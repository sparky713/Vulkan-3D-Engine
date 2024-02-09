 #pragma once

#include <vulkan/vulkan.h>
#include "common.h"

class RenderInfo;

class MyVulkanTextObj : public MyTextObj {
public:
    MyVulkanTextObj();
    MyVulkanTextObj(std::string text);
    ~MyVulkanTextObj();

    void update(float fDeltaTime);
    void render(float fDeltaTime, RenderInfo* renderInfo);

    void LoadFont(const std::string& filename, int windowWidth, int windowHeight); // loads and sets the font

    void initText(); //called by load font

    void createTexture(std::string textureFilename, VkImage* curTextureImage, VkDeviceMemory* curTextureImageMemory, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler);
    void createTextDescriptorPool(VkDescriptorPool* curDescriptorPool);
    void createTextDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler);
    void createTextVertexBuffer(VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory); // don't need to give text vertices as we are just creating space
    void fillTextVertexBuffer(std::vector<TextVertex> textVertices, VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory);

private:
    VkImage textTextureImage;
    VkDeviceMemory textTextureImageMemory;
    VkImageView textTextureImageView;
    VkSampler textTextureImageSampler;

    VkDescriptorPool textDescriptorPool;
    VkDescriptorSet textDescriptorSet;
    std::vector<VkDescriptorSet> textDescriptorSets;

    //VkPipeline textPSO; // pso containing a pipeline state (m_graphicsPipeline = general PSO)
    //VkPipelineLayout textPipelineLayout;

    VkBuffer textVertexBuffer;
    VkDeviceMemory textVertexBufferMemory;
};

