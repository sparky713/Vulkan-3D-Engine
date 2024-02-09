//#include <wx/treectrl.h>
#include "hierarchy_panel.h"

#include "my_scene_node_drop_target.h"

MySceneNodeDropTarget::MySceneNodeDropTarget() {
    hierarchyPanel = nullptr;
    ownerTree = nullptr;
}

MySceneNodeDropTarget::MySceneNodeDropTarget(HierarchyPanel *hierarchyPanel, wxTreeCtrl* ownerTree) {
    this->hierarchyPanel = hierarchyPanel;
    this->ownerTree = ownerTree;
}

bool MySceneNodeDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {
    std::cout << "MyGameObjDropTarget::OnDropText(): dragged: " << data << std::endl;
    wxTreeItemId hit = ownerTree->HitTest(wxPoint(x, y));
    std::cout << "MyGameObjDropTarget::OnDropText(): ownerTree->GetItemText(hit): " << ownerTree->GetItemText(hit) << std::endl;
    hierarchyPanel->onTreeDrop(hit);
    hierarchyPanel->resetDragging();
    return true;
}
