#include <iostream>
#include <fstream>
#include <sstream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VulkanCanvas.h"

#include "my_text_obj.h"

#include "non_vk_common.h"
#include "common.h"

#include "my_vulkan_text_obj.h"

extern VulkanCanvas* g_canvas;

MyVulkanTextObj::MyVulkanTextObj() {

}

MyVulkanTextObj::~MyVulkanTextObj() {

}

void MyVulkanTextObj::update(float fDeltaTime) { //setText(...) called in VulkanCanvas
}

void MyVulkanTextObj::render(float fDeltaTime, RenderInfo* renderInfo) {
    g_canvas->RenderText((*((VulkanRenderInfo*)renderInfo)->commandBuffers)[g_canvas->currentFrame], getFont(), getText(), glm::vec2(0.02f, 0.01f), glm::vec2(2.0f, 2.0f));
}

void MyVulkanTextObj::LoadFont(const std::string& filename, int windowWidth, int windowHeight) {
    std::wifstream fs;
    try {
        std::cout << "VulkanCanvas::LoadFont(): filename: " << filename << '\n';
        fs.open(filename);
    }
    catch (std::exception& e) {
        std::cout << "An exception occurred. Exception Nr. " << e.what() << '\n';
    }

    Font font;
    std::wstring tmp;
    int startpos;

    // extract font name
    fs >> tmp >> tmp; // info face="Arial"
    startpos = tmp.find(L"\"") + 1;
    font.name = tmp.substr(startpos, tmp.size() - startpos - 1);

    // get font size
    fs >> tmp; // size=73
    startpos = tmp.find(L"=") + 1;
    font.size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
    fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 

    // get padding
    fs >> tmp; // padding=5,5,5,5 
    startpos = tmp.find(L"=") + 1;
    tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

    // get up padding
    startpos = tmp.find(L",") + 1;
    font.toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get right padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    startpos = tmp.find(L",") + 1;
    font.rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get down padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    startpos = tmp.find(L",") + 1;
    font.bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get left padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    font.leftpadding = std::stoi(tmp) / (float)windowWidth;

    fs >> tmp; // spacing=0,0

    // get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
    fs >> tmp >> tmp; // common lineHeight=95
    startpos = tmp.find(L"=") + 1;
    font.lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

    // get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
    fs >> tmp; // base=68
    startpos = tmp.find(L"=") + 1;
    font.baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

    // get texture width
    fs >> tmp; // scaleW=512
    startpos = tmp.find(L"=") + 1;
    font.textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // get texture height
    fs >> tmp; // scaleH=512
    startpos = tmp.find(L"=") + 1;
    font.textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // get pages, packed, page id
    fs >> tmp >> tmp; // pages=1 packed=0
    fs >> tmp >> tmp; // page id=0

    // get texture filename
    std::wstring wtmp;
    fs >> wtmp; // file="Arial.png"
    startpos = wtmp.find(L"\"") + 1;
    font.fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

    // get number of characters
    fs >> tmp >> tmp; // chars count=97
    startpos = tmp.find(L"=") + 1;
    font.numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // initialize the character list
    font.CharList = new FontChar[font.numCharacters];

    for (int c = 0; c < font.numCharacters; ++c) {
        // get unicode id
        fs >> tmp >> tmp; // char id=0
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get x
        fs >> tmp; // x=392
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureWidth;

        // get y
        fs >> tmp; // y=340
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureHeight;

        // get width
        fs >> tmp; // width=47
        startpos = tmp.find(L"=") + 1;
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        font.CharList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
        font.CharList[c].twidth = (float)std::stoi(tmp) / (float)font.textureWidth;

        // get height
        fs >> tmp; // height=57
        startpos = tmp.find(L"=") + 1;
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        font.CharList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
        font.CharList[c].theight = (float)std::stoi(tmp) / (float)font.textureHeight;

        // get xoffset
        fs >> tmp; // xoffset=-6
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

        // get yoffset
        fs >> tmp; // yoffset=16
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

        // get xadvance
        fs >> tmp; // xadvance=65
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

        // get page
        // get channel
        fs >> tmp >> tmp; // page=0    chnl=0
    }

    // get number of kernings
    fs >> tmp >> tmp; // kernings count=96
    startpos = tmp.find(L"=") + 1;
    font.numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // initialize the kernings list
    font.KerningsList = new FontKerning[font.numKernings];

    for (int k = 0; k < font.numKernings; ++k) {
        // get first character
        fs >> tmp >> tmp; // kerning first=87
        startpos = tmp.find(L"=") + 1;
        font.KerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get second character
        fs >> tmp; // second=45
        startpos = tmp.find(L"=") + 1;
        font.KerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get amount
        fs >> tmp; // amount=-1
        startpos = tmp.find(L"=") + 1;
        int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
        font.KerningsList[k].amount = (float)t / (float)windowWidth;
    }

    setFont(font);
    initText();

    createTextVertexBuffer(&textVertexBuffer, &textVertexBufferMemory); //sally
}

void MyVulkanTextObj::initText() {

    createTexture(getFontImageFilename(), &textTextureImage, &textTextureImageMemory, &textTextureImageView, &textTextureImageSampler); // createTextureImage() + createTextureImageView() + createTextureSampler()
    createTextDescriptorPool(&textDescriptorPool);
    createTextDescriptorSets(&textDescriptorPool, &textDescriptorSets, &textTextureImageView, &textTextureImageSampler);
    getFont().srvHandle = textDescriptorSets;
}

void MyVulkanTextObj::createTexture(std::string textureFilename, VkImage* curTextureImage, VkDeviceMemory* curTextureImageMemory, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) {
    g_canvas->createTexture(textureFilename, curTextureImage, curTextureImageMemory, curTextureImageView, curTextureImageSampler); // createTextureImage() + createTextureImageView() + createTextureSampler()

}

void MyVulkanTextObj::createTextDescriptorPool(VkDescriptorPool* curDescriptorPool) {
    g_canvas->createTextDescriptorPool(curDescriptorPool);
}

void MyVulkanTextObj::createTextDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) {
    g_canvas->createTextDescriptorSets(curDescriptorPool, curDescriptorSets, curTextureImageView, curTextureImageSampler);
}

void MyVulkanTextObj::createTextVertexBuffer(VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory) {
    g_canvas->createTextVertexBuffer(pTextVertexBuffer, pTextVertexBufferMemory);
}

void MyVulkanTextObj::fillTextVertexBuffer(std::vector<TextVertex> textVertices, VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory) {
    g_canvas->fillTextVertexBuffer(textVertices, pTextVertexBuffer,  pTextVertexBufferMemory);
}
