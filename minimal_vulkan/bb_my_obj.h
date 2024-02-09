#pragma once

#include <vulkan/vulkan.h>

#define FACES_NUM_INDICES 12

#include "my_obj.h"

class ObjLoader;
//class MyObj;
class RenderInfo;

/**
 * BBMyObj represents a single bounding box object use for collision detection.
 * The ObjLoader of this object only contains a single bounding box object.
 */
class BBMyObj : public MyObj {
public:
    MyObj *myOwnerObj;

    BBMyObj();
    BBMyObj(ObjLoader* objLoader);
    void init(ObjLoader* objLoader, std::string objName, int indexBufStartIndex, int indexBufEndIndex, std::string mtlName, MyMaterial* myMat);

    void update(float fDeltaTime);
    void render(float fDeltaTime, RenderInfo* renderInfo);
    void setCurCode(glm::vec3 camPos);
    void findBBFaceCode(glm::vec3 camLocalPos); // helper of checkCamera()
    void sortFaces(); // helper of update()
    void updateIndexBuffer(); // helper of update()
    void updateUniformBuffer(double deltaTime, uint32_t currentImage, int width, int height);
    
    void calcBoxCenter(std::vector<MyVertex> vertices); // called by init
    void calcBoxLengths(std::vector<MyVertex> vertices); // called by init
    void calcBoxMinAndMax(std::vector<MyVertex> vertices); // called by init

    //--------------------------------------------------------------------------
    // obj picking
    //--------------------------------------------------------------------------
    UniformBufferObject ubo{};

public:
    glm::vec3 center;
    double width, height, depth; // x, y ,z
    glm::vec3 min;
    glm::vec3 max;

    //--------------------------------------------------------------------------
    // camera
    //--------------------------------------------------------------------------
    glm::mat4 localToWorldMat; /**< model matrix */

    //==========================================================================
    // for sorting faces
    //==========================================================================
    std::string curCode = "";
    std::vector<MyFace> curBBFaces;
    std::map<std::string, int> bbFaceCases = { // code, myFaceIndices[?] index
      { "F", 0 },
      { "FR", 1 },
      { "FL", 2 },
      { "FT", 3 },
      { "FBt", 4 },
      { "FRT", 5 },
      { "FLT", 6 },
      { "FRBt", 7 },
      { "FLBt", 8 },

      { "R", 9 },
      { "RT", 10 },
      { "RBt", 11 },
      { "RBk", 12 },
      { "RTBk", 13 },
      { "RBtBk", 14 },

      { "L", 15 },
      { "LT", 16 },
      { "LBt", 17 },
      { "LBk", 18 },
      { "LTBk", 19 },
      { "LBtBk", 20 },

      { "T", 21 },
      { "TBk", 22 },

      { "Bk", 23 },

      { "Bt", 24 },
      { "BtBk", 25 },

      { "Center", 26 },
    };
    int myFaceIndices[27][12] = {
        { 1,2,3,4,5,7,8,9,10,11,0,6 }, //f
        { 1,2,4,5,7,8,10,11,0,6,3,9 }, //fr;
        { 2,3,4,5,8,9,10,11,0,6,1,7 }, //fl;
        { 1,2,3,4,7,8,9,10,0,6,5,11 }, //ft;
        { 1,2,3,5,7,8,9,11,0,6,4,10 }, //fbt;
        { 1,2,4,7,8,10,0,6,3,9,5,11 }, //frt;
        { 2,3,4,8,9,10,0,6,1,7,5,11 }, //flt;
        { 1,2,5,7,8,11,0,6,3,9,4,10 }, //frbt;
        { 2,3,5,8,9,11,0,6,1,7,4,10 }, //flbt;

        { 0,1,2,4,5,6,7,8,10,11,3,9 }, //r
        { 0,1,2,4,6,7,8,10,3,9,5,11 }, //rt;
        { 0,1,2,5,6,7,8,11,3,9,4,10 }, //rbt;
        { 0,1,4,5,6,7,10,11,3,9,2,8 }, //rbk;
        { 0,1,4,6,7,10,3,9,5,11,2,8 }, //rtbk;
        { 0,1,5,6,7,11,3,9,4,10,2,8 }, //rbtbk;

        { 0,2,3,4,5,6,8,9,10,11,1,7 }, //l
        { 0,2,3,4,6,8,9,10,1,7,5,11 }, //lt;
        { 0,2,3,5,6,8,9,11,1,7,4,10 }, //lbt;
        { 0,3,4,5,6,9,10,11,1,7,2,8 }, //lbk;
        { 0,3,4,6,9,10,1,7,5,11,2,8 }, //ltbk;
        { 0,3,5,6,9,11,1,7,4,10,2,8 }, //lbtbk;

        { 0,1,2,3,4,6,7,8,9,10,5,11 }, //t
        { 0,1,3,4,6,7,9,10,5,11,2,8 }, //tbk;

        { 0,1,3,4,5,6,7,9,10,11,2,8 }, //bk

        { 0,1,2,3,5,6,7,8,9,11,4,10 }, // bt
        { 0,1,3,5,6,7,9,11,2,8,4,10 }, //btbk

        { 0,1,2,3,4,5,6,7,8,9,10,11 }, // center
    };
    //==========================================================================
};
