#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "game_object.h"
#include "obj_loader.h"

class MyMaterial;
class RenderInfo;

/**
 * MyObj represents a single mesh / object.
 * The objects are created by an ObjLoader when loading a object file.
 */
class MyObj: public GameObject {
public:
    bool draw;

    MyObj();
    MyObj(ObjLoader *objLoader);
    ~MyObj();
    virtual void init(ObjLoader* objLoader, std::string objName, int indexBufStartIndex, int indexBufEndIndex, std::string mtlName, MyMaterial* myMat);
    
    ObjLoader* getObjLoader();
    std::string getObjName();
    
    int getIndexBufStartIndex();
    int getIndexBufEndIndex();
    int getNumIndices();
    
    std::string getMtlName();
    MyMaterial *getMyMaterial();

    void* getVertexBuffer();
    void* getIndexBuffer();

    void* getTextureImages();
    void* getTextureImageMemories();
    void* getTextureImageViews();
    void* getTextureSamplers();

    void* getUniformBuffers();
    void* getUniformBuffersMemories();
    void* getUniformBuffersMapped();

    void* getDescriptorPools();
    void* getDescriptorSets();
    
    void setObjLoader(ObjLoader* objLoader);
    void setObjName(std::string objName);
    
    void setIndexBufStartIndex(int indexBufStartIndex);
    void setIndexBufEndIndex(int indexBufEndIndex);
    void setIndexBufIndices(int indexBufStartIndex, int indexBufEndIndex);
    void setNumIndices(int numIndices);
    
    void setMtlName(std::string mtlName);
    void setMyMaterial(MyMaterial *myMat);

    virtual void update(float fDeltaTime);
    virtual void render(float fDeltaTime, RenderInfo *renderInfo);
    virtual void renderBB(float fDeltaTime, RenderInfo *renderInfo);

    virtual void updateUniformBuffer(double deltaTime, uint32_t currentImage, int width, int height) = 0;

    //--------------------------------------------------------------------------
    // Bounding Box
    //--------------------------------------------------------------------------
    virtual void initBBMyObj();

    bool isDrawBBOn();
    void setDrawBB(bool drawBB);
    void setBBMyObj(MyObj* bbMyObj);
    MyObj *getBBMyObj();

    // bounding box
    MyObj* bbMyObj;
    bool drawBB;
    int bbOffsetX;
    int bbOffsetY;
    int bbRatio;

    //dimensions of this obj
    glm::vec3 center; //mesh's center (calculated)
    //double width, height, depth; // x, y ,z
    glm::vec3 min;
    glm::vec3 max;

    //glm::vec3 position; //obj's initial position when loaded
    //glm::vec3 rotation; //obj's initial position when loaded

    ////--------------------------------------------------------------------------
    //// Obj Highlight
    ////--------------------------------------------------------------------------
    //void setMouseSelectOn(bool mouseSelectOn);
    //bool getMouseSelectOn();

private:
    ObjLoader* objLoader;
    std::string objName;
    
    // material info
    std::string mtlName;
    MyMaterial* myMat; /**< This object's material. */

    // index info
    int indexBufStartIndex;
    int indexBufEndIndex;
    int numIndices;

    //// obj highlight
    //bool mouseSelectOn;
};

//MyObjInstance:
//    MyObj
//    uniform buffer
//    texture - if same texture use texture in MyObj
