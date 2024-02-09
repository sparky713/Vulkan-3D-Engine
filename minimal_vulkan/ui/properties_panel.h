#pragma once

#include "wx/wx.h"
#include "wx/progdlg.h"

#define BUTTON_START_X 20
#define BUTTON_START_Y 20
#define BUTTON_FACES_W 30
#define BUTTON_W 80
#define BUTTON_H 30
#define BUTTON_MARGIN_LEFT 5

#define BUTTON_MARGIN_TOP 10

// under construction
class PropertiesPanel : public wxPanel {
//class PropertiesPanel : public wxWindow {
public:
    PropertiesPanel(wxWindow* pParent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = "VulkanCanvasName");

    virtual ~PropertiesPanel() noexcept;

    wxButton* bob;
private:
};
