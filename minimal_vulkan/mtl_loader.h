#pragma once

#include <string>
#include <map>

struct MyMaterial { /**< Material information and properties. */
	double ns;
	double ka[3];
	double ks[3];
	double ke[3];
	double ni;
	double d;
	double illum;
    std::string imageFilename;
};

/**
 * MtlLoader is a reader and parser of the material files used to draw the 3D objects in this application.
 */
class MtlLoader {
public:
    const std::string IMAGE_PATHNAME = "models/";
    std::string mtlFilename;
    /**
    * Hold the information of each material.
    */
    std::map<std::string, MyMaterial*> myMaterials; // material name, material

	MtlLoader();
	MtlLoader(std::string mtlFilename);

	void load();
	void load(std::string mtlFilename);

	void print();
};
