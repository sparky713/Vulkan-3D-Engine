#include "VulkanCanvas.h"

#include "my_game_obj_drop_target.h"

MyGameObjDropTarget::MyGameObjDropTarget() {
    vulkanCanvas = nullptr;
}

MyGameObjDropTarget::MyGameObjDropTarget(VulkanCanvas *vulkanCanvas) {
    this->vulkanCanvas = vulkanCanvas;
}

bool MyGameObjDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {
    std::cout << "MyGameObjDropTarget::OnDropText(): data: " << data << std::endl;
    vulkanCanvas->loadModel(data.ToStdString(), "", false);
    return true;
}
