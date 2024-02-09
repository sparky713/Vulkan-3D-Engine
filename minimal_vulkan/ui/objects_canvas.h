#pragma once

#include "wx/wx.h"
#include "wx/progdlg.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <vector>
#include <map>

#define BUTTON_START_X 20
#define BUTTON_START_Y 20
#define BUTTON_FACES_W 30
#define BUTTON_W 80
#define BUTTON_H 30
#define BUTTON_MARGIN_LEFT 5
#define BUTTON_MARGIN_TOP 10

class VulkanCanvas;
class MyObj;

class ObjectsCanvas : public wxWindow {
public:

    std::vector<wxButton*> buttons;
    std::map<wxButton*, MyObj*> objButtonsMap;

    wxButton* bob;

    //==========================================================================
    // for testing bb
    //==========================================================================
    std::vector<wxButton*> cubeFaceButtons;
    std::map<wxButton*, int> cubeFaceButtonsMap;
    wxButton* f; //0
    wxButton* fr;
    wxButton* fl;
    wxButton* ft;
    wxButton* fbt;
    wxButton* frt;
    wxButton* flt;
    wxButton* frbt;
    wxButton* flbt;

    wxButton* r; //1
    wxButton* rt;
    wxButton* rbt;
    wxButton* rbk;
    wxButton* rtbk;
    wxButton* rbtbk;

    wxButton* l; //2
    wxButton* lt;
    wxButton* lbt;
    wxButton* lbk;
    wxButton* ltbk;
    wxButton* lbtbk;

    wxButton* t; //3
    wxButton* tbk;

    wxButton* bk; //4
    //wxButton* bkbt;

    wxButton* bt; //5
    wxButton* btbk; //5

    wxButton* center; //6

    void initCubeFaceButtons(int& btnIndexCounter);
    void onCubeFaceButtonPressed(wxCommandEvent& event);

    //==========================================================================
    // camera pos
    //==========================================================================
    wxStaticText *cameraTitleLbl;
    wxStaticText *cameraXPosLbl;
    wxStaticText *cameraYPosLbl;
    wxStaticText *cameraZPosLbl;
    wxStaticText *cameraXPos;
    wxStaticText *cameraYPos;
    wxStaticText *cameraZPos;
    wxTextCtrl *cameraXPosTA;
    wxTextCtrl *cameraYPosTA;
    wxTextCtrl *cameraZPosTA;
    //==========================================================================

    ObjectsCanvas(wxWindow* pParent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = "VulkanCanvasName");

    virtual ~ObjectsCanvas() noexcept;

    void update();

    void onButtonPressed(wxCommandEvent& event);
    void onBobButtonPressed(wxCommandEvent& event);

    void cameraPosTextCtrlHandler(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};
