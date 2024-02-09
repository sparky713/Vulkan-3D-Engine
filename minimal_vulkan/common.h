#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "non_vk_common.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Obj
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal; // not used for now

    Vertex();
    Vertex(float x, float y, float z, float u, float v);
    Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz); // with normals

    static VkVertexInputBindingDescription getBindingDescription();
    //static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();

    bool operator==(const Vertex& other) const;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

void printVec3(glm::vec3 vec);
void printVec4(glm::vec4 vec);
void printMat3(glm::mat3 mat);
void printMat4(glm::mat4 mat);

//------------------------------------------------------------------------------
// Transformation
//------------------------------------------------------------------------------
struct Transform { // a component of SceneNode
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 scale;

    void setPos(glm::vec3 pos);
    void setRotation(glm::quat rot);
    void setScale(glm::vec3 scale);

    glm::vec3 getPos();
    glm::quat getRotation();
    glm::vec3 getScale();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Text
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//sally ver1046
struct TextVertex {
    glm::vec4 pos;     // x, y, w, h
    glm::vec4 texCoord; // u, v, tw, th
    glm::vec4 color;    // r, g, b, a
    TextVertex();
    TextVertex(float x, float y, float w, float h, float u, float v, float tw, float th, float r, float g, float b, float a);

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

    bool operator==(const TextVertex& other) const;
};

struct FontChar {
    int id; // the unicode id

    // these need to be converted to texture coordinates 
    // (where 0.0 is 0 and 1.0 is textureWidth of the font)
    float u;        // u texture coordinate
    float v;        // v texture coordinate
    float twidth;   // width of character on texture
    float theight;  // height of character on texture

    float width;    // width of character in screen coords
    float height;   // height of character in screen coords

    // these need to be normalized based on size of font
    float xoffset;  // offset from current cursor pos to left side of character
    float yoffset;  // offset from top of line to top of character
    float xadvance; // how far to move to right for next character
};

struct FontKerning {
    int firstid;  // the first character
    int secondid; // the second character
    float amount; // the amount to add/subtract to second characters x
};

struct Font {
    std::wstring name;                     // name of the font
    std::wstring fontImage;                // image of the font
    int size;                              // size of font, lineheight and baseheight will be based on this as if this is a single unit (1.0)
    float lineHeight;                      // how far to move down to next line, will be normalized
    float baseHeight;                      // height of all characters, will be normalized
    int textureWidth;                      // width of the font texture
    int textureHeight;                     // height of the font texture
    int numCharacters;                     // number of characters in the font
    FontChar* CharList;                    // list of characters
    int numKernings;                       // the number of kernings
    FontKerning* KerningsList;             // list to hold kerning values
    //ID3D12Resource* textureBuffer;         // the font texture resource
    VkImage* textureBuffer;         // the font texture resource
    //VkBuffer* textureBuffer;         // the font texture resource ????????????????????????????????????????????????
    //D3D12_GPU_DESCRIPTOR_HANDLE srvHandle; // the font srv
    std::vector<VkDescriptorSet> srvHandle; // the font srv ??????????????????????????????????????????????
    //VkDescriptorSet srvHandle; // the font srv

    // these are how much the character is padded in the texture.
    float leftpadding;
    float toppadding;
    float rightpadding;
    float bottompadding;

    Font();
    float GetKerning(wchar_t first, wchar_t second); // this will return the amount of kerning we need to use for two characters
    FontChar* GetChar(wchar_t c); // this will return a FontChar given a wide character
};

struct Timer {
    double timerFrequency = 0.0;
    long long lastFrameTime = 0;
    long long lastSecond = 0;
    double frameDelta = 0;
    int fps = 0;

    Timer();
    double GetFrameDelta(); // Call this once per frame
};

bool replace(std::string& str, const std::string& from, const std::string& to);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// MD5
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//struct MD5Vertex {
//    glm::vec3   m_Pos;
//    glm::vec3   m_Normal;
//    glm::vec2   m_Tex0;
//    int         m_StartWeight;
//    int         m_WeightCount;
//
//    MD5Vertex();
//    MD5Vertex(float x, float y, float z, float nx, float ny, float nz, float tx, float ty, int startWeight, int weightCount);
//
//    static VkVertexInputBindingDescription getBindingDescription();
//    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
//
//    bool operator==(const MD5Vertex& other) const;
//};
//
//typedef std::vector<MD5Vertex> VertexList;
//typedef std::vector<unsigned int> IndexBuffer;


class VulkanRenderInfo : public RenderInfo {
public:
    std::vector<VkCommandBuffer> *commandBuffers;

    VulkanRenderInfo(std::vector<VkCommandBuffer>* commandBuffers);
};
