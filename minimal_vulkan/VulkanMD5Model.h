#pragma once;

#include <vector>
#include <map>

#include <vulkan/vulkan.h>
#include "common.h"

class MD5Model;
class RenderInfo;

class VulkanMD5Model : public MD5Model {
public:
    VulkanMD5Model();
    virtual ~VulkanMD5Model();

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // from MyObj
    //==========================================================================
    //==========================================================================
    //==========================================================================
    int getIndexBufStartIndex(int meshIndex);
    int getIndexBufEndIndex(int meshIndex);
    int getNumIndices(int meshIndex);

    void* getVertexBuffer();
    void* getIndexBuffer();

    void* getTextureImages(std::string textureFilename);
    void* getTextureImageMemories(std::string textureFilename);
    void* getTextureImageViews(std::string textureFilename);
    void* getTextureSamplers(std::string textureFilename);

    void* getUniformBuffers(std::string textureFilename);
    void* getUniformBuffersMemories(std::string textureFilename);
    void* getUniformBuffersMapped(std::string textureFilename);

    void* getDescriptorPools(std::string textureFilename);
    void* getDescriptorSets(std::string textureFilename);

    void setIndexBufStartIndex(int meshIndex, int indexBufStartIndex);
    void setIndexBufEndIndex(int meshIndex, int indexBufEndIndex);
    void setNumIndices(int meshIndex, int numIndices);

    //void update(float fDeltaTime); // use parents
    void render(float fDeltaTime, RenderInfo* renderInfo);
    void updateUniformBuffer(MD5Model::Mesh curMesh, double deltaTime, uint32_t currentImage);

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // from VulkanObjLoader
    //==========================================================================
    //==========================================================================
    //==========================================================================
    void combineVertexAndIndexBuffer(); // new sally
    void createVertexBufferAndIndexBuffer(); // helper fn of MD5Model::LoadModel(...)
    void fillVertexBufferAndIndexBuffer(); // helper fn of MD5Model::Update()

    void init3DAPIMaterials();
    //void initMyMD5Models(); //void initMyObjs();
    
    //--------------------------------------------------------------------------
    // init3DAPIMaterials() helper functions
    //--------------------------------------------------------------------------
    void createTexture(std::string textureFilename, VkImage* curTextureImage, VkDeviceMemory* curTextureImageMemory, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler);
    void createUniformBuffers(std::vector<VkBuffer>* curUniformBuffers, std::vector<VkDeviceMemory>* curUniformBuffersMemory, std::vector<void*>* curUniformBuffersMapped);
    void createDescriptorPool(VkDescriptorPool* curDescriptorPool);
    void createDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, std::vector<VkBuffer>* curUniformBuffers, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler);

    void print();

protected:
    //==========================================================================
    //==========================================================================
    //==========================================================================
    // from MyObj
    //==========================================================================
    //==========================================================================
    //==========================================================================
    // index info
    //int indexBufStartIndex;
    //int indexBufEndIndex;
    //int numIndices;
    std::map<int, int> indexBufStartIndex; // mesh index, start index
    std::map<int, int> indexBufEndIndex; // mesh index, end index
    std::map<int, int> numIndices; // mesh index, num indices

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // from VulkanMyObj
    //==========================================================================
    //==========================================================================
    //==========================================================================

    //--------------------------------------------------------------------------
    // camera
    //--------------------------------------------------------------------------
    glm::mat4 worldMat; /**< world matrix (transformation matrix) */
    glm::mat4 rotMat; /**< keeps track of rotation */
    glm::vec4 position; /**< position in space */

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // from VulkanObjLoader
    //==========================================================================
    //==========================================================================
    //==========================================================================
    //--------------------------------------------------------------------------
    // shared
    //--------------------------------------------------------------------------
    std::vector<Vertex> vertices; /**< Stores the vertices used to draw the object without any duplicates. */

    VkBuffer vertexBuffer; /**< a default buffer in GPU memory that we will load vertex data for our triangle into. */
    VkBuffer indexBuffer; /**< a default buffer in GPU memory that we will load index data for our triangle into. */
    VkDeviceMemory vertexBufferMemory;
    VkDeviceMemory indexBufferMemory;

    //--------------------------------------------------------------------------
    // per mesh
    //--------------------------------------------------------------------------
    //material name -> texture image filename (.png)
    std::map<std::string, VkImage> textureImages; // texture name, VkImage
    std::map<std::string, VkDeviceMemory> textureImageMemories; // texture name, VkDeviceMemory
    std::map<std::string, VkImageView> textureImageViews; // texture name, VkImage
    std::map<std::string, VkSampler> textureSamplers; // texture name, VkSampler

    // uniform buffers
    std::map<std::string, std::vector<VkBuffer>> uniformBuffers;
    std::map<std::string, std::vector<VkDeviceMemory>> uniformBuffersMemories;
    std::map<std::string, std::vector<void*>> uniformBuffersMapped;

    // descriptors
    std::map<std::string, VkDescriptorPool> descriptorPools;
    std::map<std::string, std::vector<VkDescriptorSet>> descriptorSets;

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // from VulkanObjLoader
    //==========================================================================
    //==========================================================================
    //==========================================================================
public:
    //struct MyTriangle { /**< Index information of positions and textures of a triangle. */
    //    int v[3];
    //};
    /**
    * Index buffer.
    * Stores the indices of vertices.
    */
    std::vector<MyFace> myTriangles;
};
