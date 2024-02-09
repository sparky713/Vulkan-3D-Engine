#include "VulkanCanvas.h"
#include "scene_node.h"

#include "hierarchy_panel.h"

extern VulkanCanvas *g_canvas;

BEGIN_EVENT_TABLE(HierarchyPanel, wxScrolledWindow)
END_EVENT_TABLE()

#define LIGHTER_GRAY wxColour(212, 212, 212)
#define MEDIUM_GRAY wxColour(96, 96, 96)
#define DARKER_GRAY wxColour(46, 46, 46)

HierarchyPanel::HierarchyPanel(wxWindow* pParent,
    wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxScrolledWindow(pParent, id, pos, size, style, name) {
    SetPosition(pos);
    SetSize(size);
    SetBackgroundColour(DARKER_GRAY);

    isDragOn = false;
    sceneRoot = g_canvas->sceneNodesRoot;

    //wxTR_SINGLE | wxLC_EDIT_LABELS
    sceneHierarchy = new wxTreeCtrl(this, wxID_ANY, wxPoint(TREE_START_X, TREE_START_Y), wxSize(TREE_START_W, TREE_START_H), wxTR_SINGLE | wxTR_HAS_VARIABLE_ROW_HEIGHT | wxTR_EDIT_LABELS | wxNO_BORDER);
    sceneHierarchy->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxNORMAL, wxNORMAL));
    wxTreeItemId rootId = sceneHierarchy->AddRoot(sceneRoot->getName());
    sceneHierarchy->Bind(wxEVT_TREE_SEL_CHANGED, &HierarchyPanel::onTreeSelect, this);
    sceneHierarchy->Bind(wxEVT_TREE_BEGIN_DRAG, &HierarchyPanel::onTreeDrag, this);
    sceneHierarchy->Bind(wxEVT_TREE_END_LABEL_EDIT, &HierarchyPanel::onTreeItemRename, this);

    nodeDropTarget = new MySceneNodeDropTarget(this, sceneHierarchy);
    sceneHierarchy->SetDropTarget(nodeDropTarget);

    createSceneNodesTree(sceneRoot, 0, rootId);
    sceneHierarchy->ExpandAll();

    sceneHierarchy->SetBackgroundColour(MEDIUM_GRAY);
    sceneHierarchy->SetForegroundColour(wxColour(255, 255, 255));
    
    //--------------------------------------------------------------------------
    // mouse events
    //--------------------------------------------------------------------------
    Bind(wxEVT_LEFT_UP, &HierarchyPanel::onMouseReleasedOnPanel, this);

    //--------------------------------------------------------------------------
    // test
    //--------------------------------------------------------------------------
    test();

    SetScrollbars(20, 20, HIERARCHY_PANEL_W / 20, HIERARCHY_PANEL_H / 20);
}

HierarchyPanel::~HierarchyPanel() noexcept {
    delete sceneHierarchy;
    delete nodeDropTarget;
}

/**
 * Called by onMouseReleased() when the tree has been modified. Updates the hierarchy display.
 */
void HierarchyPanel::update() {
    reset();
    createSceneNodesTree(sceneRoot, 0, sceneHierarchy->GetRootItem());
    sceneHierarchy->ExpandAll();
}

/**
 * Deletes the wxStaticText objects in curHierarchyLbls.
 */
void HierarchyPanel::reset() {
    sceneHierarchy->DeleteAllItems();
    sceneHierarchy->AddRoot("Scene");
}

/**
 * Resets fields related to node dragging.
 */
void HierarchyPanel::resetDragging() {
    isDragOn = false;
    dragSrcNodeName = "";
}

//==========================================================================
//==========================================================================
//==========================================================================
// tree
//==========================================================================
//==========================================================================
//==========================================================================

void HierarchyPanel::createSceneNodesTree(SceneNode* root, int numIndentLeft, wxTreeItemId parentId) {
    if (!root) return;

    if (root->getGameObject() != NULL) {
        parentId = sceneHierarchy->AppendItem(parentId, root->getName());
    }

    if (root->getChildren().size() != 0) {
        for (int c = 0; c < root->getChildren().size(); c++) {
            numIndentLeft++;
            createSceneNodesTree(root->getChildren()[c], numIndentLeft, parentId);
            numIndentLeft--;
        }
    }
}

/**
 * Prints the name of each scene node in the given tree.
 * @param root the root of the tree to traverse
 */
void HierarchyPanel::printTree(SceneNode* root, int numParents) {
    if (!root) return;
    
    std::cout << "HierarchyPanel::printTree(...): root->getName() = " << std::string(numParents, '*') << root->getName() << std::endl;

    if (root->getChildren().size() != 0) {
        for (int c = 0; c < root->getChildren().size(); c++) {
            numParents++;
            printTree(root->getChildren()[c], numParents);
            numParents--;
        }
    }
}

/**
 * Searches and returns the node that matches the given key (name). Returns NULL if node is not found.
 * @param root the root of the tree to search
 * @param name the name to search for
 * @return the node that matches the given key (name). Returns NULL if node is not found.
 */
SceneNode *HierarchyPanel::findNode(SceneNode *root, std::string name) {
    if (root == NULL) {
        return nullptr;
    }

    std::cout << "HierarchyPanel::findNode(...): root->getName() = " << root->getName() << " | name = " << name << std::endl;

    if (root->getName() == name) { // main
        std::cout << "HierarchyPanel::findNode(...): root->getName() == name\n";
        return root;
    }

    if (root->getChildren().size() > 0) {
        for (int n = 0; n < root->getChildren().size(); n++) {
            SceneNode *nodeFound = findNode(root->getChildren()[n], name);
            if (nodeFound != nullptr) { // returns first one found (TODO: make sure objs can't have overlapping names)
                return nodeFound;
            }
        }
    }
    return nullptr;
}

//==========================================================================
//==========================================================================
//==========================================================================
// event handlers
//==========================================================================
//==========================================================================
//==========================================================================
void HierarchyPanel::onTreeSelect(wxTreeEvent& event) {
    // highlight game object in VulkanCanvas [optional TODO]
    std::cout << "HierarchyPanel::onTreeSelect(): start" << std::endl;

}

void HierarchyPanel::onTreeDrag(wxTreeEvent& event) {
    std::cout << "HierarchyPanel::onTreeDrag(): start" << std::endl;

    isDragOn = true;

    sceneHierarchy->SetItemDropHighlight(event.GetItem());
    dragSrcNodeName = sceneHierarchy->GetItemText(event.GetItem());

    wxTextDataObject tdo(dragSrcNodeName);
    wxDropSource tds(tdo, sceneHierarchy);
    tds.DoDragDrop(wxDrag_AllowMove);
}

/**
 * Mouse release event on a wxStaticText on this panel.
 * If a scene node is currently selected and dragged, insert it as a child of the node this function is binded to.
 * @param event mouse event
 */
void HierarchyPanel::onTreeDrop(wxTreeItemId droppedNode) {
    std::string curObjName = sceneHierarchy->GetItemText(droppedNode);

    if (isDragOn && curObjName != dragSrcNodeName) { //dropped on a different node
        SceneNode *newParentNode = findNode(sceneRoot, curObjName);
        SceneNode *selectedNode = findNode(sceneRoot, dragSrcNodeName);

        SceneNode *parentOfSelected = selectedNode->getParent();
        for (int i = 0; i < parentOfSelected->getChildren().size(); i++) {
            if (parentOfSelected->getChildren()[i]->getName() == dragSrcNodeName) {
                parentOfSelected->removeChildAt(i);
                break;
            }
        }

        newParentNode->addChild(selectedNode);
        selectedNode->setParent(newParentNode);

        printTree(sceneRoot, 0);

        resetDragging();
        update();
    }
    else if (isDragOn) { // dropped on itself
        resetDragging();
    }
}

void HierarchyPanel::onTreeItemRename(wxTreeEvent& event) {
    std::cout << "HierarchyPanel::onTreeItemRename(): start" << std::endl;
    std::string prevObjName = sceneHierarchy->GetItemText(event.GetItem());
    std::string newName = event.GetLabel();
    if (newName == "") {
        event.Veto();
    }
    else {
        SceneNode *node = findNode(sceneRoot, prevObjName);
        if (findNode(sceneRoot, newName) == nullptr) {
            node->setName(newName);
        }
        else {
            node->setName(newName + " (1)");
            // or change back to prev name?????????????????????????
        }
    }
    
    // TODO
    // check that there is no object with the same name already
    //std::cout << "HierarchyPanel::onTreeItemRename(): prevObjName: " << prevObjName << std::endl; //past
    //std::cout << "HierarchyPanel::onTreeItemRename(): event.GetLabel(): " << event.GetLabel() << std::endl; // new
}

void HierarchyPanel::onMouseReleasedOnPanel(wxMouseEvent& event) {
    if (isDragOn) {
        resetDragging();
    }
}

void HierarchyPanel::test() {
    //==========================================================================
    // printTree()
    //==========================================================================
    //printTree(sceneRoot, 0);

    //==========================================================================
    // createSceneNodesLbls()
    //==========================================================================
    //createSceneNodesLbls(SceneNode * root);

    //==========================================================================
    // findNode()
    //==========================================================================
    //SceneNode* t = findNode(sceneRoot, "Cube");
    //if (t != nullptr) {
    //    std::cout << "===========================================================================================" << std::endl;
    //    std::cout << "HierarchyPanel::test(): findNode(sceneRoot, \"Cube\"): t->getName() = " << t->getName() << std::endl;
    //    std::cout << "===========================================================================================" << std::endl;
    //}
    //else {
    //    std::cout << "===========================================================================================" << std::endl;
    //    std::cout << "HierarchyPanel::test(): findNode(sceneRoot, \"Cube\"): ERROR" << std::endl;
    //    std::cout << "===========================================================================================" << std::endl;
    //}

    //t = findNode(sceneRoot->getChildren()[4], "-x");
    //if (t != nullptr) { // find itself
    //    std::cout << "===========================================================================================" << std::endl;
    //    std::cout << "HierarchyPanel::test(): findNode(sceneRoot->getChildren()[0], \"-x\"): t->getName() = " << t->getName() << std::endl;
    //    std::cout << "===========================================================================================" << std::endl;
    //}
    //else {
    //    std::cout << "===========================================================================================" << std::endl;
    //    std::cout << "HierarchyPanel::test(): findNode(sceneRoot->getChildren()[4], \"-x\"): ERROR" << std::endl;
    //    std::cout << "===========================================================================================" << std::endl;
    //}

    //t = findNode(sceneRoot->getChildren()[0], "y");
    //if (t != nullptr) { //failed test
    //    std::cout << "===========================================================================================" << std::endl;
    //    std::cout << "HierarchyPanel::test(): findNode(sceneRoot->getChildren()[0], \"y\"): ERROR" << std::endl;
    //    std::cout << "===========================================================================================" << std::endl;
    //}
    //else {
    //    std::cout << "===========================================================================================" << std::endl;
    //    std::cout << "HierarchyPanel::test(): findNode(sceneRoot->getChildren()[0], \"y\"): no such obj found" << std::endl;
    //    std::cout << "===========================================================================================" << std::endl;
    //}
}
