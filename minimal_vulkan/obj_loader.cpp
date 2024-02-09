#include <iostream>
#include <iomanip>
#include <fstream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/hash.hpp>

#include <chrono>

#define NUM_INDICES_PER_FACE 3

#include "my_obj.h"
#include "mtl_loader.h"
#include "obj_loader.h"

/**
* Creates a new ObjLoader no associated object file.
*/
ObjLoader::ObjLoader() {
    objFilename = "";
    mtlFilename = "";
    numMesh = 0;
    isBBLoader = false;
}

/**
* Creates a new ObjLoader with the given object filename.
* @param objFilename the object filename
*/
ObjLoader::ObjLoader(std::string objFilename) {
    this->objFilename = objFilename;
    mtlFilename = "";
    numMesh = 0;
    isBBLoader = false;
}

/**
* Creates a new ObjLoader with the given object filename.
* @param objFilename the object filename
* @param isBBLoader true if this obj file contains a bounding box object, false otherwise
*/
ObjLoader::ObjLoader(std::string objFilename, bool isBBLoader) {
    this->objFilename = objFilename;
    mtlFilename = "";
    numMesh = 0;
    this->isBBLoader = isBBLoader;
}


/**
* Calls load(string objFilename) with objFilename.
*/
void ObjLoader::load() {
    if (getIsBBLoader()) {
        numMesh = 1;
        loadBBObj();
        //std::cout << "ObjLoader::load(): here" << std::endl;
    }
    else {
        load(objFilename, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

/**
* Reads in and parses the object file to prepare for rendering.
* @param objFilename the full path and filename of the object file to load
*/
void ObjLoader::load(std::string objFilename, glm::vec3 objPosition, glm::vec3 objRotation) {
    this->objFilename = objFilename;

    std::string curObjName = "";
    std::ifstream myFile(objFilename); // open a file
    std::string fileString;
    MyObj* newObj;
    std::string curMtlName = "";

    if (myFile.is_open()) {
        //std::cout << "ObjLoader::load(): " << objFilename << " FILE OPENED" << std::endl;
        while (getline(myFile, fileString)) { // read line by line
            //cout << fileString << endl;
            char* type = strtok(strdup(fileString.c_str()), " "); // tokenize
            if (type != NULL) {
                if (!strcmp(type, "mtllib")) { // mtlFilename
                    this->mtlFilename = MTL_PATHNAME + strtok(NULL, " ");
                }
                else if (!strcmp(type, "usemtl")) { // material;
                    curMtlName = strtok(NULL, " ");
                    newObj->setMtlName(curMtlName);
                }
                else if (!strcmp(type, "o")) { // new object
                    numMesh++;
                    if (curObjName != "") { // newObj previously created
                        newObj->setIndexBufEndIndex(myFaces.size() * NUM_INDICES_PER_FACE - 1);
                        newObj->setNumIndices((newObj->getIndexBufEndIndex() - newObj->getIndexBufStartIndex() + 1));
                        newObj->setObjName(curObjName);
                        newObj->setMtlName(curMtlName);
                        myObjsArr.push_back(newObj);
                        myObjsMap[curObjName] = newObj;
                    }
                    ObjLoader *curObjLoader = this;
                    //newObj = new MyObj(curObjLoader);
                    newObj = createMyObj(curObjLoader);
                    newObj->position = objPosition;
                    newObj->rotation = objRotation;
                    newObj->setIndexBufStartIndex(myFaces.size() * NUM_INDICES_PER_FACE);
                    curObjName = strtok(NULL, " ");
                }
                else if (!strcmp(type, "v")) { // vertices
                    struct MyVertex myV;
                    myV.x = std::stod(strtok(NULL, " "));
                    myV.y = std::stod(strtok(NULL, " "));
                    myV.z = std::stod(strtok(NULL, " "));

                    myVertices.push_back(myV);
                }
                else if (!strcmp(type, "vn")) { // vertex normals NEW
                    struct MyVn myVn;
                    myVn.x = std::stod(strtok(NULL, " "));
                    myVn.y = std::stod(strtok(NULL, " "));
                    myVn.z = std::stod(strtok(NULL, " "));

                    myVns.push_back(myVn);
                }
                else if (!strcmp(type, "vt")) { // textCoords
                    struct MyVt myVt;
                    myVt.u = std::stod(strtok(NULL, " "));
                    myVt.v = 1.0 - std::stod(strtok(NULL, " "));

                    myVts.push_back(myVt);
                }
                else if (!strcmp(type, "f")) { // faces v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3] ...
                    struct MyFace newMyFace;

                    std::string chunks[3];
                    for (int i = 0; i < 3; i++) {
                        chunks[i] = strtok(NULL, " ");
                    }

                    for (int i = 0; i < 3; i++) {
                        char* curVInStr = strtok((char*)chunks[i].c_str(), "/");
                        char* curVtInStr = strtok(NULL, "/");
                        char* curVnInStr = strtok(NULL, "/");

                        int curV = atoi(curVInStr) - 1;
                        int curVt = atoi(curVtInStr) - 1;
                        int curVn = atoi(curVnInStr) - 1;

                        //newMyFace.v[i] = getIndex3DAPIVertex(myVertices[curV].x, myVertices[curV].y, myVertices[curV].z, myVts[curVt].u, myVts[curVt].v); // API specific helper fn
                        
                        //std::cout << "ObjLoader::load(): myVns[curVn].x: " << myVns[curVn].x << " | myVns[curVn].y: " << myVns[curVn].y <<
                        //    " | myVns[curVn].z: " << myVns[curVn].z << std::endl;
                        newMyFace.v[i] = getIndex3DAPIVertex(myVertices[curV].x, myVertices[curV].y, myVertices[curV].z, myVts[curVt].u, myVts[curVt].v,
                                                             myVns[curVn].x, myVns[curVn].y, myVns[curVn].z); // API specific helper fn
                    }

                    myFaces.push_back(newMyFace);
                }
            }
        } // end of while loop
        if (curObjName != "") { // last obj
            newObj->setIndexBufEndIndex(myFaces.size() * NUM_INDICES_PER_FACE - 1);
            newObj->setNumIndices((newObj->getIndexBufEndIndex() - newObj->getIndexBufStartIndex() + 1));
            newObj->setObjName(curObjName);
            newObj->setMtlName(curMtlName);
            myObjsArr.push_back(newObj);
            myObjsMap[curObjName] = newObj;
        }
    } // end of main if

    myFile.close(); // close the file

    //==========================================================================
    // for testing bb
    //==========================================================================
    //myFaces.erase(myFaces.begin());
    //myFaces.erase(myFaces.begin() + 1);
    //myFaces.erase(std::next(myFaces.begin()));
    //==========================================================================

    createVertexBufferAndIndexBuffer(); // calling api specific code (in child class)

    //--------------------------------------------------------------------------
    // material
    //--------------------------------------------------------------------------
    mtlLoader = new MtlLoader(mtlFilename);
    mtlLoader->load(); // load material

    //std::cout << "ObjLoader::load(): myObjsArr[0]->getObjName(): " << myObjsArr[0]->getObjName() << std::endl;
    init3DAPIMaterials(); // calling api specific code (in child class)
    initMyObjs(); // calling api specific code (in child class)
    //print();
    for (int i = 0; i < myObjsArr.size(); i++) {
        myObjsArr[i]->initBBMyObj();
    }
    
} // end of load();

bool ObjLoader::getIsBBLoader() {
    return isBBLoader;
}
/**
* Prints a string representation of the object.
*/
void ObjLoader::print() {
    std::cout << std::setw(14) << std::setprecision(6) << std::right << std::fixed;
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    std::cout << " ObjLoader::print(): start\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

    std::cout << "objFilename: " << objFilename << std::endl;
    std::cout << "mtlFilename: " << mtlFilename << std::endl;

    std::cout << "myVertices:" << std::endl;
    std::cout << "myVertices.size() = " << myVertices.size() << std::endl;
    for (int i = 0; i < myVertices.size(); i++) {
        std::cout << i << " (" << myVertices[i].x << ", " << myVertices[i].y << ", " << myVertices[i].z << ")" << std::endl;
    }

    std::cout << "myVts:" << std::endl;
    for (int i = 0; i < myVts.size(); i++) {
        std::cout << i << " (" << myVts[i].u << ", " << myVts[i].v << ")" << std::endl;
    }

    std::cout << "myFaces:" << std::endl;
    std::cout << "myFaces.size() = " << myFaces.size() << std::endl;
    for (int i = 0; i < myFaces.size(); i++) {
        std::cout << i << " (" << myFaces[i].v[0] << ", " << myFaces[i].v[1] << ", " << myFaces[i].v[2] << ")" << std::endl;
    }

    //for (int i = 0; i < myObjsArr.size(); i++) {
    //    std::cout << "myObjsArr[" << "i" << "]: " << myObjsArr[i] << std::endl;
    //}

    //std::cout << "myObjsMap:" << std::endl;
    //std::map<std::string, MyObj*>::iterator myObjsMapIt;
    //for (myObjsMapIt = myObjsMap.begin(); myObjsMapIt != myObjsMap.end(); myObjsMapIt++) {
    //    std::cout << myObjsMapIt->first << ": " << myObjsMapIt->second->getMtlName() << std::endl;
    //}

    std::cout << "total number of objects: " << numMesh << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    std::cout << " ObjLoader::print(): end\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
}
