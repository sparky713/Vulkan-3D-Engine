#pragma once

#include "wx/wx.h"
#include "wx/progdlg.h"
#include <wx/listctrl.h>
#include <wx/dnd.h>

class MyTextDropTarget : public wxTextDropTarget {
public:
    wxListCtrl* ownerList;

    MyTextDropTarget();
    MyTextDropTarget(wxListCtrl *ownerList);

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
};
