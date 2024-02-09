#pragma once

#include "wx/wx.h"
#include "wx/progdlg.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <wx/treectrl.h>
#include <wx/dnd.h>
#include <vector>
#include <map>

#include "my_scene_node_drop_target.h"
#include "scene_node.h"
#include "non_vk_common.h"

#define TREE_START_X 10
#define TREE_START_Y 10
#define TREE_START_W HIERARCHY_PANEL_W - 20
#define TREE_START_H HIERARCHY_PANEL_H - 50
//#define TREE_START_H 150
#define LEFT_MARGIN 15 // for child

class HierarchyPanel : public wxScrolledWindow {
public:
    wxTreeCtrl *sceneHierarchy;
    //--------------------------------------------------------------------------
    // select & node related
    //--------------------------------------------------------------------------
    SceneNode* sceneRoot;
    SceneNode* nodeToRename;
    bool isDragOn;
    std::string dragSrcNodeName;

    MySceneNodeDropTarget *nodeDropTarget;
    //--------------------------------------------------------------------------

    HierarchyPanel(wxWindow* pParent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = "VulkanCanvasName");

    virtual ~HierarchyPanel() noexcept;

    void update();        // update current files
    void reset();         // reset the node labels
    void resetDragging(); // reset the node dragging

    //--------------------------------------------------------------------------
    // tree
    //--------------------------------------------------------------------------
    void createSceneNodesTree(SceneNode* root, int numIndentLeft, wxTreeItemId parentId);

    void printTree(SceneNode *root, int numParents);
    SceneNode *findNode(SceneNode* root, std::string name); //search function

    //--------------------------------------------------------------------------
    // event handlers
    //--------------------------------------------------------------------------
    void onTreeDrop(wxTreeItemId droppedNode);
    void onTreeSelect(wxTreeEvent& event);
    void onTreeDrag(wxTreeEvent& event);
    void onTreeItemRename(wxTreeEvent& event);

    void onMouseReleasedOnPanel(wxMouseEvent& event);

    void test();

private:
    int mouseX;
    int mouseY;

    DECLARE_EVENT_TABLE()

};
