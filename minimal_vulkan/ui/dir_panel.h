#pragma once

#include "wx/wx.h"
#include "wx/progdlg.h"
//#include "common.h"
#include <wx/dirctrl.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <vector>
#include <map>

#include "files_panel.h"
#include "non_vk_common.h"

#define FILE_DIR_CTRL_W (DIR_PANEL_W - 10)
#define FILE_DIR_CTRL_H (DIR_PANEL_H - 10)

#define BUTTON_START_X 20
#define BUTTON_START_Y 20
#define BUTTON_FACES_W 30
#define BUTTON_W 80
#define BUTTON_H 30
#define BUTTON_MARGIN_LEFT 5
#define BUTTON_MARGIN_TOP 10

//class DirPanel : public wxPanel {
class DirPanel : public wxWindow {
public:
    wxGenericDirCtrl *dirTreeCtrl;
    FilesPanel* filesPanel;
    //==========================================================================

    DirPanel(wxWindow* pParent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = "VulkanCanvasName");

    virtual ~DirPanel() noexcept;

    void update(); // update current files

    void onFolderPressed(wxTreeEvent& event);

    void setFilesPanel(FilesPanel *filesPanel);
    FilesPanel *getFilesPanel();

    DECLARE_EVENT_TABLE()


};
