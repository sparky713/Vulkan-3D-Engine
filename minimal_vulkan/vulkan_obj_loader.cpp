#include <iostream>
#include <iomanip>
#include <fstream>

#include "VulkanCanvas.h"

#include "my_obj.h"
#include "vulkan_my_obj.h"
#include "bb_my_obj.h"
#include "vulkan_obj_loader.h"

#define NUM_INDICES_PER_FACE 3

extern VulkanCanvas* g_canvas;

/**
 * Constructs a VulkanObjLoader object.
 */
VulkanObjLoader::VulkanObjLoader() : ObjLoader() {
}

/**
* Creates a new VulkanObjLoader with the given object filename.
* @param objFilename the object filename
* @param isBBLoader true if this obj file contains a bounding box object, false otherwise
*/
VulkanObjLoader::VulkanObjLoader(std::string objFilename, bool isBBLoader) : ObjLoader(objFilename, isBBLoader) {
}

/**
 * Returns the vertex buffer.
 * @return the vertex buffer
 */
void* VulkanObjLoader::getVertexBuffer() {
    return (void *)vertexBuffer;
}

/**
 * Returns the index buffer.
 * @return the index buffer
 */
void* VulkanObjLoader::getIndexBuffer() {
    return (void *)indexBuffer;
}

/**
 * Returns the texture image of the given material.
 * @param materialName the name of the owner material
 * @return the texture image of the given material
 */
void* VulkanObjLoader::getTextureImages(std::string materialName) {
    return (void *)textureImages[materialName];
}

/**
 * Returns the texture memory of the given material.
 * @param materialName the name of the owner material
 * @return the texture memory of the given material
 */
void* VulkanObjLoader::getTextureImageMemories(std::string materialName) {
    return (void *)textureImageMemories[materialName];
}

/**
 * Returns the texture image view of the given material
 * @return the texture image view of the given material
 */
void* VulkanObjLoader::getTextureImageViews(std::string materialName) {
    return (void *)textureImageViews[materialName];
}

/**
 * Returns the texture samplers of the given material.
 * @param materialName the name of the owner material
 * @return the texture samplers of the given material
 */
void* VulkanObjLoader::getTextureSamplers(std::string materialName) {
    return (void *)textureSamplers[materialName];
}

/**
 * Returns the uniform buffers of the given material.
 * @param materialName the name of the owner material
 * @return the uniform buffers of the given material
 */
void* VulkanObjLoader::getUniformBuffers(std::string materialName) {
    return &uniformBuffers[materialName];
}

/**
 * Returns the uniform buffers memory of the given material.
 * @param materialName the name of the owner material
 * @return the uniform buffers memory of the given material
 */
void* VulkanObjLoader::getUniformBuffersMemories(std::string materialName) {
    return &uniformBuffersMemories[materialName];
}

/**
 * Returns the uniform buffers mapped of the given material.
 * @param materialName the name of the owner material
 * @return the uniform buffers mapped of the given material
 */
void* VulkanObjLoader::getUniformBuffersMapped(std::string materialName) {
    return &uniformBuffersMapped[materialName];
}

/**
 * Returns the descriptor pool of the given material.
 * @param materialName the name of the owner material
 * @return the descriptor pool of the given material
 */
void* VulkanObjLoader::getDescriptorPools(std::string materialName) {
    return (void *)descriptorPools[materialName];
}

/**
 * Returns a pointer to the descriptor sets of the given material.
 * @param materialName the name of the owner material
 * @return a pointer to the descriptor sets of the given material
 */
void* VulkanObjLoader::getDescriptorSets(std::string materialName) {
    return &descriptorSets[materialName];
}

/**
 * Helper function of load() that returns the index of the given vertex to create the index vector.
 * @param posX x-coordinate of the vertex position
 * @param posY y-coordinate of the vertex position
 * @param posZ z-coordinate of the vertex position
 * @param textureU x-coordinate of the vertex texture
 * @param textureV y-coordinate of the vertex texture
 * @return the index of the given vertex
 */
int VulkanObjLoader::getIndex3DAPIVertex(float posX, float posY, float posZ, float textureU, float textureV) { // called by load()
        struct Vertex newVertex(posX, posY, posZ, textureU, textureV);

    int indexOfVertex = -1;
    for (int i = 0; i < vertices.size() && indexOfVertex == -1; i++) { // create and store the vertices
        struct Vertex curVertex = vertices[i];
        if (newVertex.pos.x == curVertex.pos.x &&
            newVertex.pos.y == curVertex.pos.y &&
            newVertex.pos.z == curVertex.pos.z &&
            newVertex.texCoord.x == curVertex.texCoord.x &&
            newVertex.texCoord.y == curVertex.texCoord.y) {
            indexOfVertex = i;
        }
    }

    if (indexOfVertex == -1) { // if vertex is not already found in vector(vertices)
        indexOfVertex = vertices.size();
        vertices.push_back(newVertex);
    }
    return indexOfVertex;
}

/**
 * Helper function of load() that returns the index of the given vertex to create the index vector.
 * @param posX x-coordinate of the vertex position
 * @param posY y-coordinate of the vertex position
 * @param posZ z-coordinate of the vertex position
 * @param textureU x-coordinate of the vertex texture
 * @param textureV y-coordinate of the vertex texture
 * @return the index of the given vertex
 */
int VulkanObjLoader::getIndex3DAPIVertex(float posX, float posY, float posZ, float textureU, float textureV, float normalX, float normalY, float normalZ) { // called by load()
    struct Vertex newVertex(posX, posY, posZ, textureU, textureV, normalX, normalY, normalZ);

    int indexOfVertex = -1;
    for (int i = 0; i < vertices.size() && indexOfVertex == -1; i++) { // create and store the vertices
        struct Vertex curVertex = vertices[i];
        if (newVertex.pos.x == curVertex.pos.x &&
            newVertex.pos.y == curVertex.pos.y &&
            newVertex.pos.z == curVertex.pos.z &&
            newVertex.texCoord.x == curVertex.texCoord.x &&
            newVertex.texCoord.y == curVertex.texCoord.y &&
            newVertex.normal.x == curVertex.normal.x &&
            newVertex.normal.y == curVertex.normal.y &&
            newVertex.normal.z == curVertex.normal.z) {
            indexOfVertex = i;
        }
    }
    //std::cout << "ObjLoader::getIndex3DAPIVertex(): newVertex.normal.x: " << newVertex.normal.x << " | newVertex.normal.y: " << newVertex.normal.y <<
    //    " | newVertex.normal.z: " << newVertex.normal.z << std::endl;
    if (indexOfVertex == -1) {
        indexOfVertex = vertices.size();
        vertices.push_back(newVertex);
    }

    return indexOfVertex;
}

/**
 * Helper function of load() that creates the vertex and index buffer for the objects.
 * Initializes vertexBuffer and indexBuffer.
 * Calls createVertexBufferAndIndexBuffer() in VulkanCanvas.
 */
void VulkanObjLoader::createVertexBufferAndIndexBuffer() { // called by load()
    g_canvas->createVertexBufferAndIndexBuffer(vertices, myFaces, &vertexBuffer, &indexBuffer, &vertexBufferMemory, &indexBufferMemory);
}

void VulkanObjLoader::updateIndexBuffer(std::vector<MyFace> curBBFaces) {
    g_canvas->updateIndexBuffer(curBBFaces, &indexBuffer, &indexBufferMemory);
}

/**
* Helper function of load() that initializes the API specific resources for the 3D rendering.
* Inits texture images, uniform buffers, descriptor pool and descriptor sets.
*/
void VulkanObjLoader::init3DAPIMaterials() {
    std::map<std::string, MyMaterial*>::iterator it;
    for (it = mtlLoader->myMaterials.begin(); it != mtlLoader->myMaterials.end(); it++) { // for every object
        std::string curImageFilename = it->second->imageFilename;
        std::string curMaterialName = it->first;
        
        //--------------------------------------------------------------------------
        // texture image
        //--------------------------------------------------------------------------
        VkImage curTextureImage;
        VkDeviceMemory curTextureImageMemory;
        VkImageView curTextureImageView;
        VkSampler curTextureImageSampler;

        createTexture(curImageFilename, &curTextureImage, &curTextureImageMemory, &curTextureImageView, &curTextureImageSampler); // calling api specific code (in child class)

        textureImages[it->first] = curTextureImage;
        textureImageMemories[it->first] = curTextureImageMemory;
        textureImageViews[it->first] = curTextureImageView;
        textureSamplers[it->first] = curTextureImageSampler;

        //--------------------------------------------------------------------------
        // uniform buffers
        //--------------------------------------------------------------------------
        std::vector<VkBuffer> curUniformBuffers;
        std::vector<VkDeviceMemory> curUniformBuffersMemory;
        std::vector<void*> curUniformBuffersMapped;

        createUniformBuffers(&curUniformBuffers, &curUniformBuffersMemory, &curUniformBuffersMapped);

        uniformBuffers[it->first] = curUniformBuffers;
        uniformBuffersMemories[it->first] = curUniformBuffersMemory;
        uniformBuffersMapped[it->first] = curUniformBuffersMapped;

        //--------------------------------------------------------------------------
        // descriptor pool
        //--------------------------------------------------------------------------
        VkDescriptorPool curDescriptorPool;

        createDescriptorPool(&curDescriptorPool);

        descriptorPools[it->first] = curDescriptorPool;

        //--------------------------------------------------------------------------
        // descriptor sets
        //--------------------------------------------------------------------------
        std::vector<VkDescriptorSet> curDescriptorSets;

        createDescriptorSets(&curDescriptorPool, &curDescriptorSets, &curUniformBuffers, &curTextureImageView, &curTextureImageSampler);

        descriptorSets[it->first] = curDescriptorSets;
    }
}

/**
 * Helper function of load() that links the objects to their materials (sets the object's myMat).
 */
void VulkanObjLoader::initMyObjs() {
    for (int i = 0; i < myObjsArr.size(); i++) { // for every object
        std::string curObjMaterialName = myObjsArr[i]->getMtlName();
        myObjsArr[i]->setMyMaterial(mtlLoader->myMaterials[curObjMaterialName]); //get MyMaterial
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
void VulkanObjLoader::createTexture(std::string textureFilename, VkImage* curTextureImage, VkDeviceMemory* curTextureImageMemory, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) { // called by load()
    g_canvas->createTexture(textureFilename, curTextureImage, curTextureImageMemory, curTextureImageView, curTextureImageSampler); // createTextureImage() + createTextureImageView() + createTextureSampler()
}

/** ?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????? (linked to material or object?)
 * @param curUniformBuffer texture filename of the current material
 * @param curUniformBuffersMemories textureImage of the current material
 * @param curUniformBuffersMapped textureImageMemory of the current material
 */
void VulkanObjLoader::createUniformBuffers(std::vector<VkBuffer>* curUniformBuffer, std::vector<VkDeviceMemory>* curUniformBuffersMemories, std::vector<void*>* curUniformBuffersMapped) {
    g_canvas->createUniformBuffers(curUniformBuffer, curUniformBuffersMemories, curUniformBuffersMapped);
}

void VulkanObjLoader::createDescriptorPool(VkDescriptorPool* curDescriptorPool) {
    g_canvas->createDescriptorPool(curDescriptorPool);
}

void VulkanObjLoader::createDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, std::vector<VkBuffer>* curUniformBuffers, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) {
    g_canvas->createDescriptorSets(curDescriptorPool, curDescriptorSets, curUniformBuffers, curTextureImageView, curTextureImageSampler);
}

/**
* Prints a string representation of the object.
*/
void VulkanObjLoader::print() {
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    std::cout << " ObjLoader::print(): start\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

    std::cout << "objFilename: " << objFilename << std::endl;
    std::cout << "mtlFilename: " << mtlFilename << std::endl;

    std::cout << "myVertices:" << std::endl;
    for (int i = 0; i < myVertices.size(); i++) {
        std::cout << i << " (" << myVertices[i].x << ", " << myVertices[i].y << ", " << myVertices[i].z << ")" << std::endl;
    }

    std::cout << "myVts:" << std::endl;
    for (int i = 0; i < myVts.size(); i++) {
        std::cout << i << " (" << myVts[i].u << ", " << myVts[i].v << ")" << std::endl;
    }

    std::cout << "myFaces:" << std::endl;
    for (int i = 0; i < myFaces.size(); i++) {
        std::cout << i << " (" << myFaces[i].v[0] << ", " << myFaces[i].v[1] << ", " << myFaces[i].v[2] << ")" << std::endl;
    }

    std::cout << "vertices:" << std::endl;
    for (int i = 0; i < vertices.size(); i++) {
        std::cout << "pos:      (" << vertices[i].pos.x << ", " << vertices[i].pos.y << ", " << vertices[i].pos.z << ")" << std::endl;
        std::cout << "texCoord: (" << vertices[i].texCoord.x << ", " << vertices[i].texCoord.y << ")" << std::endl;
    }

    std::cout << "myObjsArr.size() = " << myObjsArr.size() << std::endl;
    for (int i = 0; i < myObjsArr.size(); i++) {
        std::cout << "myObjsArr[" << "i" << "]: " << myObjsArr[i] << std::endl;
    }

    std::cout << "myObjsMap:" << std::endl;
    std::map<std::string, MyObj*>::iterator myObjsMapIt;
    for (myObjsMapIt = myObjsMap.begin(); myObjsMapIt != myObjsMap.end(); myObjsMapIt++) {
        std::cout << myObjsMapIt->first << ": " << myObjsMapIt->second->getMtlName() << std::endl;
    }

    std::cout << "total number of objects: " << numMesh << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    std::cout << " ObjLoader::print(): end\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
}

MyObj* VulkanObjLoader::createMyObj(ObjLoader* objLoader) {
    if (getIsBBLoader()) { // bb obj
        return new BBMyObj(objLoader);
    }
    else { // regular obj
        return new VulkanMyObj(objLoader);
    }
}

void VulkanObjLoader::loadBBObj() { // only called if this is a bb loader
    //init vts
    std::string bbVtsFilename = "bb/bb_cube_vts_and_faces.txt";
    std::ifstream myFile(bbVtsFilename); // open a file
    std::string fileString;

    if (myFile.is_open()) {
        // code removed for copyright
    } // end of main if

    myFile.close();

    createVertexBufferAndIndexBuffer();

    //--------------------------------------------------------------------------
    // material
    //--------------------------------------------------------------------------
    mtlLoader = new MtlLoader(bbMtlFilename);
    mtlLoader->load();

    //std::cout << "VulkanObjLoader::loadBBObj(): myObjsArr[0]->getObjName(): " << myObjsArr[0]->getObjName() << std::endl;
    init3DAPIMaterials();
    initMyObjs();

    //print();
} // end of load();
