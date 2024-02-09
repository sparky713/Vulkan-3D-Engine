#pragma once

#include "obj_loader.h"

/**
 * VulkanObjLoader is a reader and parser of the object files used to draw the 3D objects in this application
 * that is specific to Vulkan.
 */
class VulkanObjLoader : public ObjLoader {
public:

    std::vector<Vertex> vertices; /**< Stores the vertices used to draw the object without any duplicates. */

    VulkanObjLoader();
    VulkanObjLoader(std::string objFilename, bool isBBLoader);

    void loadBBObj();

    //--------------------------------------------------------------------------
    // getters
    //--------------------------------------------------------------------------
    void* getVertexBuffer();
    void* getIndexBuffer();

    void* getTextureImages(std::string materialName);
    void* getTextureImageMemories(std::string materialName);
    void* getTextureImageViews(std::string materialName);
    void* getTextureSamplers(std::string materialName);

    void* getUniformBuffers(std::string materialName);
    void* getUniformBuffersMemories(std::string materialName);
    void* getUniformBuffersMapped(std::string materialName);

    void* getDescriptorPools(std::string materialName);
    void* getDescriptorSets(std::string materialName);

    void print();

    std::string bbMtlFilename = "bb/bb_cube.mtl";

private:
    //--------------------------------------------------------------------------
    // shared
    //--------------------------------------------------------------------------
    VkBuffer vertexBuffer; /**< a default buffer in GPU memory that we will load vertex data for our triangle into. */
    VkBuffer indexBuffer; /**< a default buffer in GPU memory that we will load index data for our triangle into. */
    VkDeviceMemory vertexBufferMemory;
    VkDeviceMemory indexBufferMemory;

    //--------------------------------------------------------------------------
    // per obj
    //--------------------------------------------------------------------------
    // texture image
    std::map<std::string, VkImage> textureImages; // material name, VkImage
    std::map<std::string, VkDeviceMemory> textureImageMemories; // material name, VkDeviceMemory
    std::map<std::string, VkImageView> textureImageViews; // material name, VkImage
    std::map<std::string, VkSampler> textureSamplers; // material name, VkSampler

    // uniform buffers
    std::map<std::string, std::vector<VkBuffer>> uniformBuffers;
    std::map<std::string, std::vector<VkDeviceMemory>> uniformBuffersMemories;
    std::map<std::string, std::vector<void*>> uniformBuffersMapped;

    // descriptors
    std::map<std::string, VkDescriptorPool> descriptorPools;
    std::map<std::string, std::vector<VkDescriptorSet>> descriptorSets;
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // methods
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int getIndex3DAPIVertex(float posX, float posY, float posZ, float textureU, float textureV); // called by load()
    int getIndex3DAPIVertex(float posX, float posY, float posZ, float textureU, float textureV, float normalX, float normalY, float normalZ); // called by load()

    void createVertexBufferAndIndexBuffer(); // helper fn of load()
    void updateIndexBuffer(std::vector<MyFace> curBBFaces); //called by BBMyObj::update()

    void init3DAPIMaterials();
    void initMyObjs();

    //--------------------------------------------------------------------------
    // init3DAPIMaterials() helper functions
    //--------------------------------------------------------------------------
    void createTexture(std::string textureFilename, VkImage *curTextureImage, VkDeviceMemory *curTextureImageMemory, VkImageView *curTextureImageView, VkSampler *curTextureImageSampler);
    void createUniformBuffers(std::vector<VkBuffer>* curUniformBuffers, std::vector<VkDeviceMemory>* curUniformBuffersMemory, std::vector<void*>* curUniformBuffersMapped);
    void createDescriptorPool(VkDescriptorPool *curDescriptorPool);
    void createDescriptorSets(VkDescriptorPool *curDescriptorPool, std::vector<VkDescriptorSet> *curDescriptorSets, std::vector<VkBuffer> *curUniformBuffers, VkImageView *curTextureImageView, VkSampler *curTextureImageSampler);

    //--------------------------------------------------------------------------
    // etc
    //--------------------------------------------------------------------------
    MyObj *createMyObj(ObjLoader* objLoader);
};
