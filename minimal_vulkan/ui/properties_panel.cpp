#include "properties_panel.h"

#define LIGHTER_GRAY wxColour(212, 212, 212)
#define MEDIUM_GRAY wxColour(126, 126, 126)
#define DARKER_GRAY wxColour(46, 46, 46)

PropertiesPanel::PropertiesPanel(wxWindow* pParent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
    : wxPanel(pParent, id, pos, size, style, name) {
    //: wxWindow(pParent, id, pos, size, style, name) {
    SetPosition(pos);
    SetSize(size);
    //SetBackgroundColour(wxColour(*wxBLUE));
    SetBackgroundColour(MEDIUM_GRAY);
}

PropertiesPanel::~PropertiesPanel() noexcept {}
