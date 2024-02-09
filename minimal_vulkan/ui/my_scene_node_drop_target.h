#pragma once

#include <wx/treectrl.h>

#include "my_text_drop_target.h"

class HierarchyPanel;

class MySceneNodeDropTarget : public MyTextDropTarget {
public:
    HierarchyPanel *hierarchyPanel;
    wxTreeCtrl *ownerTree;

    MySceneNodeDropTarget();
    MySceneNodeDropTarget(HierarchyPanel *hierarchyPanel, wxTreeCtrl *ownerTree);

    bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
};
