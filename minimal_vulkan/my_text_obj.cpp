//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

//#include "VulkanCanvas.h"
#include <chrono>

#include "my_text_obj.h"

//extern VulkanCanvas *g_canvas;

/**
* Default Constructor.
* Constructs a new MyTextObj object.
*/
MyTextObj::MyTextObj() {
    text = "";
    numChar = 0;
    //int maxNumTextCharacters = 1024; // the maximum number of characters you can render during a frame. This is just used to make sure
    //std::string fontImageFilename = "Arial.png";
}

/**
* Constructs a new MyTextObj with the given text.
* @param text the text this object represents
*/
MyTextObj::MyTextObj(std::string text) {
    this->text = text;
    this->numChar = text.length();

    for (int i = 0; i < numChar; i++) {
        textVertices[numChar] = TextVertex(
            //color.x,
            //color.y,
            //color.z,
            //color.w,
            //fc->u,
            //fc->v,
            //fc->twidth,
            //fc->theight,
            //x + ((fc->xoffset + kerning) * scale.x),
            //y - (fc->yoffset * scale.y),
            //fc->width * scale.x,
            //fc->height * scale.y
        );
    }
}

void MyTextObj::update(float fDeltaTime) {
    //updateUniformBuffer
}

void MyTextObj::render(float fDeltaTime, VkCommandBuffer commandBuffer) { // call child's render
}

//==============================================================================
// getters & setters
//==============================================================================
int MyTextObj::getNumChar() {
    return numChar;
}

int MyTextObj::getMaxNumTextCharacters() {
    return maxNumTextCharacters;
}

std::string MyTextObj::getText() {
    return text;
}

Font MyTextObj::getFont() {
    return font;
}

std::string MyTextObj::getFontImageFilename() {
    return fontImageFilename;
}

void MyTextObj::setMaxNumTextCharacters(int maxNumTextCharacters) {
    this->maxNumTextCharacters = maxNumTextCharacters;
}

void MyTextObj::setText(std::string text) {
    this->text = text;
}

void MyTextObj::setFont(Font font) {
    this->font = font;
}
