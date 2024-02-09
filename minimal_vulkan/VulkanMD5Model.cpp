#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/hash.hpp>

#include "VulkanCanvas.h"

#include "md5/MD5Model.h"

#include "non_vk_common.h"
#include "common.h"
#include "hq_render.h"

#include "VulkanMD5Model.h"

extern glm::mat4 cameraView;
extern glm::mat4 cameraPerspective;

extern VulkanCanvas* g_canvas;

VulkanMD5Model::VulkanMD5Model() : MD5Model() { }
VulkanMD5Model::~VulkanMD5Model() { }

//==========================================================================
//==========================================================================
//==========================================================================
// from VulkanMD5Model
//==========================================================================
//==========================================================================
//==========================================================================
/**
 * Returns the starting index of the index buffer.
 * @return the starting index of the index buffer
 */
int VulkanMD5Model::getIndexBufStartIndex(int meshIndex) {
    return indexBufStartIndex[meshIndex];
}

/**
 * Returns the end index of the index buffer.
 * @return the end index of the index buffer
 */
int VulkanMD5Model::getIndexBufEndIndex(int meshIndex) {
    return indexBufEndIndex[meshIndex];
}

/**
 * Returns the number of indices of the index buffer.
 * @return the number of indices of the index buffer
 */
int VulkanMD5Model::getNumIndices(int meshIndex) {
    return numIndices[meshIndex];
}

//std::string getMtlName(){ return;}
//MyMaterial* getMyMaterial() { return; }

/**
 * Returns the vertex buffer.
 * @return the vertex buffer
 */
void* VulkanMD5Model::getVertexBuffer() {
    return (void*)vertexBuffer;
}

/**
 * Returns the index buffer.
 * @return the index buffer
 */
void* VulkanMD5Model::getIndexBuffer() {
    return (void*)indexBuffer;
}

/**
 * Returns the texture image of the given material.
 * @param textureFilename the name of the owner material
 * @return the texture image of the given material
 */
void* VulkanMD5Model::getTextureImages(std::string textureFilename) {
    return (void*)textureImages[textureFilename];
}

/**
 * Returns the texture memory of the given material.
 * @param textureFilename the name of the owner material
 * @return the texture memory of the given material
 */
void* VulkanMD5Model::getTextureImageMemories(std::string textureFilename) {
    return (void*)textureImageMemories[textureFilename];
}

/**
 * Returns the texture image view of the given material
 * @return the texture image view of the given material
 */
void* VulkanMD5Model::getTextureImageViews(std::string textureFilename) {
    return (void*)textureImageViews[textureFilename];
}

/**
 * Returns the texture samplers of the given material.
 * @param textureFilename the name of the owner material
 * @return the texture samplers of the given material
 */
void* VulkanMD5Model::getTextureSamplers(std::string textureFilename) {
    return (void*)textureSamplers[textureFilename];
}

/**
 * Returns the uniform buffers of the given material.
 * @param textureFilename the name of the owner material
 * @return the uniform buffers of the given material
 */
void* VulkanMD5Model::getUniformBuffers(std::string textureFilename) {
    return &uniformBuffers[textureFilename];
}

/**
 * Returns the uniform buffers memory of the given material.
 * @param textureFilename the name of the owner material
 * @return the uniform buffers memory of the given material
 */
void* VulkanMD5Model::getUniformBuffersMemories(std::string textureFilename) {
    return &uniformBuffersMemories[textureFilename];
}

/**
 * Returns the uniform buffers mapped of the given material.
 * @param textureFilename the name of the owner material
 * @return the uniform buffers mapped of the given material
 */
void* VulkanMD5Model::getUniformBuffersMapped(std::string textureFilename) {
    return &uniformBuffersMapped[textureFilename];
}

/**
 * Returns the descriptor pool of the given material.
 * @param textureFilename the name of the owner material
 * @return the descriptor pool of the given material
 */
void* VulkanMD5Model::getDescriptorPools(std::string textureFilename) {
    return (void*)descriptorPools[textureFilename];
}

/**
 * Returns a pointer to the descriptor sets of the given material.
 * @param textureFilename the name of the owner material
 * @return a pointer to the descriptor sets of the given material
 */
void* VulkanMD5Model::getDescriptorSets(std::string textureFilename) {
    return &descriptorSets[textureFilename];
}

/**
 * sets the starting index of the index buffer.
 * @param indexBufStartIndex the starting index of the index buffer
 */
void VulkanMD5Model::setIndexBufStartIndex(int meshIndex, int indexBufStartIndex) {
    this->indexBufStartIndex[meshIndex] = indexBufStartIndex;
}

/**
 * sets the end index of the index buffer.
 * @param indexBufEndIndex the end index of the index buffer
 */
void VulkanMD5Model::setIndexBufEndIndex(int meshIndex, int indexBufEndIndex) {
    this->indexBufEndIndex[meshIndex] = indexBufEndIndex;
}

/**
 * sets the number of indices of the index buffer.
 * @param numIndices the number of indices of the index buffer
 */
void VulkanMD5Model::setNumIndices(int meshIndex, int numIndices) {
    this->numIndices[meshIndex] = numIndices;
}

//void VulkanMD5Model::update() {
    //updateUniformBuffer
    //anim
//}
/**
* Updates the uniform buffer of this mesh.
* updateUniformBuffer() can alter the matrix to apply transformations to this mesh.
* @param curMesh the mesh to apply the transformation to
* @param deltaTime the mesh to apply the transformation to
* @param currentImage the index of the image to draw
*/
void VulkanMD5Model::updateUniformBuffer(MD5Model::Mesh curMesh, double deltaTime, uint32_t currentImage) {
        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), (float)(deltaTime * glm::radians(0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = cameraView;
        ubo.proj = cameraPerspective;
        ubo.proj[1][1] *= -1;
        std::string curTextureFilename = curMesh.m_Shader;
        void* curImageUniformBuffersMapped = (*(std::vector<void*>*)(getUniformBuffersMapped(curTextureFilename)))[currentImage];
        memcpy(curImageUniformBuffersMapped, &ubo, sizeof(ubo));
}

//==========================================================================
//==========================================================================
//==========================================================================
// from VulkanObjLoader
//==========================================================================
//==========================================================================
//==========================================================================

//Combines all the vertices and indices into one vertex and index buffer.
void VulkanMD5Model::combineVertexAndIndexBuffer() {
    vertices.clear();
    //myTriangles.clear();

    int prevNumVertsTotal = 0; // for indices calculation

    if (!getHasAnim()) { // bind pose
        for (int m = 0; m < getMeshList().size(); m++) {
            // code removed for copyright
        }
    }
    else { // animation
        // code removed for copyright
    }
    
    //print();
}

/**
 * Helper function of MD5Model::LoadModel(...) that creates the vertex and index buffer for the objects.
 * Initializes vertexBuffer and indexBuffer.
 * Calls createMD5VertexBufferAndIndexBuffer() in VulkanCanvas.
 */
void VulkanMD5Model::createVertexBufferAndIndexBuffer() { // called by MD5Model::LoadModel(...)
    g_canvas->createMD5VertexBufferAndIndexBuffer(vertices, myTriangles, &vertexBuffer, &indexBuffer, &vertexBufferMemory, &indexBufferMemory);
}

void VulkanMD5Model::fillVertexBufferAndIndexBuffer() {
    g_canvas->fillMD5VertexBufferAndIndexBuffer(vertices, myTriangles, &vertexBuffer, &indexBuffer, &vertexBufferMemory, &indexBufferMemory);
}

/**
* Helper function of LoadModel() that initializes the API specific resources for the 3D rendering.
* Inits texture images, uniform buffers, descriptor pool and descriptor sets.
*/
void VulkanMD5Model::init3DAPIMaterials() {
    //std::map<std::string, MyMaterial*>::iterator it;

    //for (it = mtlLoader->myMaterials.begin(); it != mtlLoader->myMaterials.end(); it++) { // for every mesh
    for (int i = 0; i < getNumMeshes(); i++) { // for every mesh
        //std::string curImageFilename = it->second->imageFilename;
        std::string curImageFilename = getMeshList()[i].m_Shader;
        //std::string curImageFilename = "models/checker_01_.png";
        //std::string curMaterialName = it->first;

        //--------------------------------------------------------------------------
        // texture image
        //--------------------------------------------------------------------------
        VkImage curTextureImage;
        VkDeviceMemory curTextureImageMemory;
        VkImageView curTextureImageView;
        VkSampler curTextureImageSampler;

        //std::cout << "VulkanMD5Model::init3DAPIMaterials(): curImageFilename: " << curImageFilename << std::endl;
        createTexture(curImageFilename, &curTextureImage, &curTextureImageMemory, &curTextureImageView, &curTextureImageSampler); // calling api specific code (in child class)

        textureImages[curImageFilename] = curTextureImage;
        textureImageMemories[curImageFilename] = curTextureImageMemory;
        textureImageViews[curImageFilename] = curTextureImageView;
        textureSamplers[curImageFilename] = curTextureImageSampler;

        //--------------------------------------------------------------------------
        // uniform buffers
        //--------------------------------------------------------------------------
        std::vector<VkBuffer> curUniformBuffers;
        std::vector<VkDeviceMemory> curUniformBuffersMemory;
        std::vector<void*> curUniformBuffersMapped;

        createUniformBuffers(&curUniformBuffers, &curUniformBuffersMemory, &curUniformBuffersMapped);

        uniformBuffers[curImageFilename] = curUniformBuffers;
        uniformBuffersMemories[curImageFilename] = curUniformBuffersMemory;
        uniformBuffersMapped[curImageFilename] = curUniformBuffersMapped;

        //--------------------------------------------------------------------------
        // descriptor pool
        //--------------------------------------------------------------------------
        VkDescriptorPool curDescriptorPool;

        createDescriptorPool(&curDescriptorPool);

        descriptorPools[curImageFilename] = curDescriptorPool;

        //--------------------------------------------------------------------------
        // descriptor sets
        //--------------------------------------------------------------------------
        std::vector<VkDescriptorSet> curDescriptorSets;

        createDescriptorSets(&curDescriptorPool, &curDescriptorSets, &curUniformBuffers, &curTextureImageView, &curTextureImageSampler);

        descriptorSets[curImageFilename] = curDescriptorSets;
    }
}

//--------------------------------------------------------------------------
// init3DAPIMaterials() helper functions
//--------------------------------------------------------------------------
/**
 * Helper function of init3DAPIMaterials() that initializes the texture image of a material with the given information.
 * Initializes textureImages, textureImageMemories, textureImageViews, textureSamplers.
 * @param textureFilename texture filename of the current material
 * @param curTextureImage textureImage of the current material
 * @param curTextureImageMemory textureImageMemory of the current material
 * @param curTextureImageView textureImageView of the current material
 * @param curTextureImageSampler textureSampler of the current material
 */
void VulkanMD5Model::createTexture(std::string textureFilename, VkImage* curTextureImage, VkDeviceMemory* curTextureImageMemory, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) { // called by load()
    g_canvas->createTexture(textureFilename, curTextureImage, curTextureImageMemory, curTextureImageView, curTextureImageSampler); // createTextureImage() + createTextureImageView() + createTextureSampler()
}

/**
 * @param curUniformBuffer texture filename of the current material
 * @param curUniformBuffersMemories textureImage of the current material
 * @param curUniformBuffersMapped textureImageMemory of the current material
 */
void VulkanMD5Model::createUniformBuffers(std::vector<VkBuffer>* curUniformBuffer, std::vector<VkDeviceMemory>* curUniformBuffersMemories, std::vector<void*>* curUniformBuffersMapped) {
    g_canvas->createUniformBuffers(curUniformBuffer, curUniformBuffersMemories, curUniformBuffersMapped);
}

void VulkanMD5Model::createDescriptorPool(VkDescriptorPool* curDescriptorPool) {
    g_canvas->createDescriptorPool(curDescriptorPool);
}

void VulkanMD5Model::createDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, std::vector<VkBuffer>* curUniformBuffers, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) {
    g_canvas->createDescriptorSets(curDescriptorPool, curDescriptorSets, curUniformBuffers, curTextureImageView, curTextureImageSampler);
}

/**
* Prints a string representation of the object.
*/
void VulkanMD5Model::print() {
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    std::cout << " VulkanMD5Model::print(): start\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

    //std::cout << "numMeshes: " << getNumMeshes() << std::endl;
    //std::cout << "numJoints: " << getNumJoints() << std::endl;
    
    double minX = vertices[0].pos.x;
    double maxX = vertices[0].pos.x;
    double minY = vertices[0].pos.y;
    double maxY = vertices[0].pos.y;
    double minZ = vertices[0].pos.z;
    double maxZ = vertices[0].pos.z;

    std::cout << "vertices:" << std::endl;
    for (int i = 0; i < vertices.size(); i++) {
        //std::cout << i << "pos: (" << vertices[i].pos.x << ", " << vertices[i].pos.y << ", " << vertices[i].pos.z << ") ";
        //std::cout << i << "texture: (" << vertices[i].texCoord.x << ", " << vertices[i].texCoord.y << ")" << std::endl;

        if (vertices[i].pos.x < minX) {
            minX = vertices[i].pos.x;
        }
        if (vertices[i].pos.x > maxX) {
            maxX = vertices[i].pos.x;
        }
        if (vertices[i].pos.y < minY) {
            minY = vertices[i].pos.y;
        }
        if (vertices[i].pos.y > maxY) {
            maxY = vertices[i].pos.y;
        }
        if (vertices[i].pos.z < minZ) {
            minZ = vertices[i].pos.z;
        }
        if (vertices[i].pos.z > maxZ) {
            maxZ = vertices[i].pos.z;
        }
    }

    std::cout << "minX: " << minX << " | maxX: " << maxX << " | minY: " << minY << " | maxY: " << maxY << " | minZ: " << minZ << " | maxZ: " << maxZ << std::endl;
    std::cout << "myTriangles.size(): " << myTriangles.size() << std::endl;

    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    std::cout << " VulkanMD5Model::print(): end\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
}

//void update(float fDeltaTime);

void VulkanMD5Model::render(float fDeltaTime, RenderInfo* renderInfo) {
    if (DRAW_MD5_ON) {
        for (unsigned int i = 0; i < getMeshList().size(); ++i) {
            MD5Model::Mesh curMesh = getMeshList()[i];

            updateUniformBuffer(curMesh, fDeltaTime, g_canvas->currentFrame);

            VkBuffer vertexBuffers[] = { (VkBuffer)(getVertexBuffer()) };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], (VkBuffer)(getIndexBuffer()), 0, VK_INDEX_TYPE_UINT32);

            std::string curTextureFilename = curMesh.m_Shader;
            const VkDescriptorSet* curDescSet = &(*(std::vector<VkDescriptorSet>*)(getDescriptorSets(curTextureFilename)))[g_canvas->currentFrame];
            vkCmdBindDescriptorSets((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, g_canvas->m_pipelineLayout, 0, 1, curDescSet, 0, nullptr);

            vkCmdDrawIndexed((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], static_cast<uint32_t>(getNumIndices(i)), 1, getIndexBufStartIndex(i), 0, 0);
        }
    }
}
