#pragma once

#include <vector>
#include <map>

#include "common.h"
#include "mtl_loader.h"

class MyObj;

/**
 * ObjLoader is a reader and parser of the object files used to draw the 3D objects in this application.
 */
class ObjLoader {
public:
    const std::string MTL_PATHNAME = "models/";
    std::string objFilename;
    std::string mtlFilename;

    /**
    * Vector of all geometric vertices used in drawing the object.
    * A vertex consists of three coordinates, x, y, and z.
    */
    std::vector<MyVertex> myVertices;

    /**
    * Vector of all the vertex normals.
    * Vertex normals are vec3 (x, y, z).
    */
    std::vector<MyVn> myVns;

    /**
    * Texture vertices.
    * Specifies the UV (optionally W) mapping. They are floating point values
    * between 0 and 1 and are grouped with a vertex in the face command.
    */
    std::vector<MyVt> myVts;

    /**
    * Index buffer.
    * Stores the indices of vertices.
    */
    std::vector<MyFace> myFaces;

    MtlLoader *mtlLoader;

    std::vector<MyObj*> myObjsArr; // in order of loading
    std::map<std::string, MyObj*> myObjsMap; // obj name, obj

	int numMesh; // number of objects in one object file

	ObjLoader();
	ObjLoader(std::string objFilename);
	ObjLoader(std::string objFilename, bool isBBLoader);

	void load();
	void load(std::string objFilename, glm::vec3 objPosition, glm::vec3 objRotation);

	void print();

    virtual void *getVertexBuffer() = 0;
    virtual void *getIndexBuffer() = 0;

    virtual void *getTextureImages(std::string materialName) = 0;
    virtual void *getTextureImageMemories(std::string materialName) = 0;
    virtual void *getTextureImageViews(std::string materialName) = 0;
    virtual void *getTextureSamplers(std::string materialName) = 0;

    virtual void* getUniformBuffers(std::string materialName) = 0;
    virtual void* getUniformBuffersMemories(std::string materialName) = 0;
    virtual void* getUniformBuffersMapped(std::string materialName) = 0;

    virtual void* getDescriptorPools(std::string materialName) = 0;
    virtual void* getDescriptorSets(std::string materialName) = 0;

    virtual void loadBBObj() = 0;
    bool getIsBBLoader();
    virtual void updateIndexBuffer(std::vector<MyFace> curBBFaces) = 0; // called by BBMyObj::update() to sort transparent faces

private:
    bool isBBLoader;

    //--------------------------------------------------------------------------
    // helper functions of load()
    //--------------------------------------------------------------------------
    virtual void createVertexBufferAndIndexBuffer() = 0; // process api/platform specific code
    virtual int getIndex3DAPIVertex(float posX, float posY, float posZ, float textureU, float textureV) = 0;
    virtual int getIndex3DAPIVertex(float posX, float posY, float posZ, float textureU, float textureV, float normalX, float normalY, float normalZ) = 0;

    virtual void init3DAPIMaterials() = 0; // texture, uniform buffers, descriptors, command buffers
    virtual void initMyObjs() = 0;
    virtual MyObj *createMyObj(ObjLoader *objLoader) = 0;
};
