#pragma once

#include <vector>

#include <vulkan/vulkan.h>
#include "common.h"

#include "game_object.h"

class MyTextObj: public GameObject { //GameObject????????????????????????????????????
public:
    std::vector<TextVertex> textVertices; // size = numChar

    MyTextObj();
    MyTextObj(std::string text);

    virtual void update(float fDeltaTime);
    virtual void render(float fDeltaTime, VkCommandBuffer commandBuffer);
    //void updateUniformBuffer(uint32_t currentImage, int width, int height);

    virtual void LoadFont(const std::string& filename, int windowWidth, int windowHeight) = 0;

    virtual void initText() = 0;

    virtual void createTexture(std::string textureFilename, VkImage* curTextureImage, VkDeviceMemory* curTextureImageMemory, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) = 0;
    virtual void createTextDescriptorPool(VkDescriptorPool* curDescriptorPool) = 0;
    virtual void createTextDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) = 0;
    virtual void createTextVertexBuffer(VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory) = 0; // don't need to give text vertices as we are just creating space
    virtual void fillTextVertexBuffer(std::vector<TextVertex> textVertices, VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory) = 0;

    //void CreateTextGraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile); // text pso

    int getNumChar();
    int getMaxNumTextCharacters();
    std::string getText();
    Font getFont();
    std::string getFontImageFilename();

    void setMaxNumTextCharacters(int maxNumTextCharacters);
    void setText(std::string text);
    void setFont(Font font);
    
private:
    int numChar;
    int maxNumTextCharacters = 1024; // the maximum number of characters you can render during a frame. This is just used to make sure
    std::string text;

    Font font; // this will store our arial font information
    std::string fontImageFilename = "Arial.png";
};
