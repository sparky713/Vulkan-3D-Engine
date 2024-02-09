#pragma once

#include <iostream>

class RenderInfo {
public:
    RenderInfo();
};

struct MyVertex { /**< Position vertex. */
    double x;
    double y;
    double z;
    MyVertex();
    MyVertex(double x, double y, double z);
};

struct MyVn { /**< Vertex normal. */
    double x;
    double y;
    double z;
};

struct MyVt { /**< Texture vertex */
    double u;
    double v;
};

struct MyFace { /**< Index information of positions and textures of a triangle. */
    int v[3];
};

struct File {
    std::string fullPath;
    std::string filename;
    std::string fileType; // int?

    File();
    void print();
};

#define VULKAN_CANVAS_W 800
#define VULKAN_CANVAS_H 600

#define FILES_PANEL_W VULKAN_CANVAS_W
#define FILES_PANEL_H 300

#define DIR_PANEL_W 300
#define DIR_PANEL_H (VULKAN_CANVAS_H + FILES_PANEL_H)

#define HIERARCHY_PANEL_W 250
#define HIERARCHY_PANEL_H VULKAN_CANVAS_H

#define OBJECTS_CANVAS_W HIERARCHY_PANEL_W
#define OBJECTS_CANVAS_H FILES_PANEL_H

//#define DRAW_BB_ON true
#define DRAW_BB_ON false


// total width = 1350
// total height = 900
