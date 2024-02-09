#include <iostream>
#include <fstream>

#include "mtl_loader.h"

/**
* Creates a new MtlLoader no associated material file.
*/
MtlLoader::MtlLoader() {
	mtlFilename = "";
}

/**
* Creates a new MtlLoader with the given material filename.
* @param mtlFilename the material filename
*/
MtlLoader::MtlLoader(std::string mtlFilename) {
	this->mtlFilename = mtlFilename;
}

/**
* Calls load(string mtlFilename) with mtlFilename.
*/
void MtlLoader::load() {
	load(mtlFilename);
}

/**
* Reads in and parses the material file to prepare for rendering.
* @param mtlFilename the full path and filename of the material file to load
*/
void MtlLoader::load(std::string mtlFilename) {
	this->mtlFilename = mtlFilename;
    //std::cout << "MtlLoader::load(): mtlFilename: " << mtlFilename << std::endl;

    std::ifstream myMtlFile(mtlFilename); // open a file
    std::string mtlFileString;
    std::string matName;

	bool firstMtlFound = false;

	if (myMtlFile.is_open()) {
		// code removed for copyright
	} // end of main if

	myMtlFile.close(); // close the file

	//print();
} // end of load()

/**
* Prints a string representation of the object.
*/
void MtlLoader::print() {
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    std::cout << " MtlLoader::print(): start\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    std::cout << "mtlFilename: " << mtlFilename << std::endl;

    std::map<std::string, MyMaterial*>::iterator it;
	for (it = myMaterials.begin(); it != myMaterials.end(); it++) {
        std::cout << it->first << ": " << std::endl;
        std::cout << "ns: " << it->second->ns << std::endl;
        std::cout << "ka: (" << it->second->ka[0] << ", " << it->second->ka[1] << ", " << it->second->ka[2] << ")" << std::endl;
        std::cout << "ks: (" << it->second->ks[0] << ", " << it->second->ks[1] << ", " << it->second->ks[2] << ")" << std::endl;
        std::cout << "ke: (" << it->second->ke[0] << ", " << it->second->ke[1] << ", " << it->second->ke[2] << ")" << std::endl;
        std::cout << "ni: " << it->second->ni << std::endl;
        std::cout << "d: " << it->second->d << std::endl;
        std::cout << "illum: " << it->second->illum << std::endl;
        std::cout << "imageFilename: " << it->second->imageFilename << std::endl;
    }

    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    std::cout << " MtlLoader::print(): end\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
}
