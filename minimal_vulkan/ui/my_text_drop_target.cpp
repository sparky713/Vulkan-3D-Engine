#include "my_text_drop_target.h"

MyTextDropTarget::MyTextDropTarget() {}

MyTextDropTarget::MyTextDropTarget(wxListCtrl* ownerList) {
    this->ownerList = ownerList;
}

bool MyTextDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {
    //ownerList->InsertItem(0, data);
    //ownerList->DeleteItem(0);
    return true;
}
