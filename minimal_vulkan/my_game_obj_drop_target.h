#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include "my_text_drop_target.h";

class VulkanCanvas;

class MyGameObjDropTarget : public MyTextDropTarget {
public:
    VulkanCanvas *vulkanCanvas;

    MyGameObjDropTarget();
    MyGameObjDropTarget(VulkanCanvas *vulkanCanvas);

    bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
    //bool OnDropText(wxCoord x, wxCoord y, std::string modelPath, std::string animPath, bool hasAnim);
};

