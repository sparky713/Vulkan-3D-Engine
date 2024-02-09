#include <iostream>
#include <iomanip>
#include <string>

#include "VulkanCanvas.h"
#include "VulkanException.h"
#include <wx/wxprec.h>
#include "wxVulkanTutorialApp.h"
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/hash.hpp>

#include "common.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Obj
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Vertex::Vertex() {}
Vertex::Vertex(float x, float y, float z, float u, float v) : pos(x, y, z), texCoord(u, v) {}
Vertex::Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) : pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 4> Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    // ver1124 - for boundary highlighting
    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, normal);

    return attributeDescriptions;
}

bool Vertex::operator==(const Vertex& other) const {
    return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
}

void printVec3(glm::vec3 vec) {
    std::cout << "[ ";
    //for (int i = 0; i < vec.length(); i++) {
    //std::cout << std::setprecision(7);
    for (int i = 0; i < 3; i++) {
         std::cout << std::setw(14) << std::setprecision(6) << std::right << std::fixed << vec[i];
    }
    std::cout << " ]" << std::endl;
    std::cout << "------------------------------------------------------------------------" << std::endl;
}

void printVec4(glm::vec4 vec) {
    std::cout << "[ ";
    //for (int i = 0; i < vec.length(); i++) {
    for (int i = 0; i < 4; i++) {
        std::cout << std::setw(14) << std::setprecision(6) << std::right << std::fixed << vec[i];
    }
    std::cout << " ]" << std::endl;
    std::cout << "------------------------------------------------------------------------" << std::endl;
}

void printMat3(glm::mat3 mat) {
    //for (int i = 0; i < mat.length(); i++) {
    for (int i = 0; i < 3; i++) {
        std::cout << "| ";
        //for (int j = 0; j < mat[0].length(); j++) {
        for (int j = 0; j < 3; j++) {
             std::cout << std::setw(14) << std::setprecision(5) << std::right << std::fixed << mat[i][j];
        }
        std::cout << " |" << std::endl;
        std::cout << "------------------------------------------------------------------------" << std::endl;
    }
}

void printMat4(glm::mat4 mat) {
    //for (int i = 0; i < mat.length(); i++) {
    for (int i = 0; i < 4; i++) {
        std::cout << "| ";
        for (int j = 0; j < 4; j++) {
            std::cout << std::setw(14) << std::setprecision(5) << std::right << std::fixed << mat[i][j];
        }
        std::cout << " |" << std::endl;
        std::cout << "------------------------------------------------------------------------" << std::endl;
    }
}

//------------------------------------------------------------------------------
// Transformation
//------------------------------------------------------------------------------
void Transform::setPos(glm::vec3 pos) {
    this->pos = pos;
}

void Transform::setRotation(glm::quat rot) {
    this->rot = rot;
}

void Transform::setScale(glm::vec3 scale) {
    this->scale = scale;
}

glm::vec3 Transform::getPos() {
    return pos;
}

glm::quat Transform::getRotation() {
    return rot;
}

glm::vec3 Transform::getScale() {
    return scale;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Text
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

TextVertex::TextVertex() {}

TextVertex::TextVertex(float x, float y, float w, float h, float u, float v, float tw, float th, float r, float g, float b, float a)
                        : pos(x, y, w, h), texCoord(u, v, tw, th), color(r, g, b, a) {}

VkVertexInputBindingDescription TextVertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(TextVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
    //std::cout << "VkVertexInputBindingDescription getBindingDescription(): bindingDescription.binding = " << bindingDescription.binding << std::endl;
    //std::cout << "VkVertexInputBindingDescription getBindingDescription(): bindingDescription.stride = " << bindingDescription.stride << std::endl;
    //std::cout << "VkVertexInputBindingDescription getBindingDescription(): bindingDescription.inputRate = " << bindingDescription.inputRate << std::endl;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> TextVertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(TextVertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(TextVertex, texCoord);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(TextVertex, color);

    return attributeDescriptions;
}

bool TextVertex::operator==(const TextVertex& other) const {
    return pos == other.pos && color == other.color && texCoord == other.texCoord;
}

Timer::Timer() {
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);

    // seconds
    //timerFrequency = double(li.QuadPart);

    // milliseconds
    //timerFrequency = double(li.QuadPart) / 1000.0;

    // microseconds
    timerFrequency = double(li.QuadPart) / 1000000.0;

    QueryPerformanceCounter(&li);
    lastFrameTime = li.QuadPart;
}

double Timer::GetFrameDelta() {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    frameDelta = double(li.QuadPart - lastFrameTime) / timerFrequency;
    if (frameDelta > 0) {
        //fps = 1000 / frameDelta; // <-- milliseconds
        fps = 1000000.0 / frameDelta; // <-- microseconds
    }

    lastFrameTime = li.QuadPart;
    //return frameDelta / 1000.0;    // <-- milliseconds
    return frameDelta / 1000000.0; // <-- microseconds
}

Font::Font() {

}

float Font::GetKerning(wchar_t first, wchar_t second) {
    for (int i = 0; i < numKernings; ++i)
    {
        if ((wchar_t)KerningsList[i].firstid == first && (wchar_t)KerningsList[i].secondid == second)
            return KerningsList[i].amount;
    }
    return 0.0f;
}

FontChar *Font::GetChar(wchar_t c) {
    for (int i = 0; i < numCharacters; ++i)
    {
        if (c == (wchar_t)CharList[i].id)
            return &CharList[i];
    }
    return nullptr;
}

bool replace(std::string& str, const std::string& from, const std::string& to) { // replace part of string
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

////------------------------------------------------------------------------------
////------------------------------------------------------------------------------
////------------------------------------------------------------------------------
//// MD5
////------------------------------------------------------------------------------
////------------------------------------------------------------------------------
////------------------------------------------------------------------------------
//MD5Vertex::MD5Vertex() {}
//
//MD5Vertex::MD5Vertex(float x, float y, float z, float nx, float ny, float nz, float tx, float ty, int startWeight, int weightCount)
//    : m_Pos(x, y, z), m_Normal(nx, ny, nz), m_Tex0(tx, ty), m_StartWeight(startWeight), m_WeightCount(weightCount) {}
//
//VkVertexInputBindingDescription MD5Vertex::getBindingDescription() {
//    VkVertexInputBindingDescription bindingDescription{};
//    bindingDescription.binding = 0;
//    bindingDescription.stride = sizeof(MD5Vertex);
//    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
//    std::cout << "VkVertexInputBindingDescription getBindingDescription(): bindingDescription.binding = " << bindingDescription.binding << std::endl;
//    std::cout << "VkVertexInputBindingDescription getBindingDescription(): bindingDescription.stride = " << bindingDescription.stride << std::endl;
//    std::cout << "VkVertexInputBindingDescription getBindingDescription(): bindingDescription.inputRate = " << bindingDescription.inputRate << std::endl;
//    return bindingDescription;
//}
//
//std::array<VkVertexInputAttributeDescription, 3> MD5Vertex::getAttributeDescriptions() {
//    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
//
//    attributeDescriptions[0].binding = 0;
//    attributeDescriptions[0].location = 0;
//    attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
//    attributeDescriptions[0].offset = offsetof(MD5Vertex, m_Pos);
//
//    // need normal?????????????????????????????????????????????????????
//    attributeDescriptions[1].binding = 0;
//    attributeDescriptions[1].location = 1;
//    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
//    attributeDescriptions[1].offset = offsetof(MD5Vertex, m_Normal);
//
//    attributeDescriptions[2].binding = 0;
//    attributeDescriptions[2].location = 2;
//    attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
//    attributeDescriptions[2].offset = offsetof(MD5Vertex, m_Tex0);
//
//    return attributeDescriptions;
//}
//
//bool MD5Vertex::operator==(const MD5Vertex& other) const {
//    return m_Pos == other.m_Pos && m_Normal == other.m_Normal && m_Tex0 == other.m_Tex0;
//}

VulkanRenderInfo::VulkanRenderInfo(std::vector<VkCommandBuffer>* commandBuffers) {
    this->commandBuffers = commandBuffers;
}
