#pragma once
#include "wx/wxprec.h"
#include "VulkanCanvas.h"
#include "hierarchy_panel.h"
#include "objects_canvas.h"
#include "dir_panel.h"
#include "files_panel.h"

#define WINDOW_X 0
#define WINDOW_Y 0
//#define WINDOW_W 1050
//#define WINDOW_H 600

/**
 * VulkanWindow is the main frame of this application.
 */
class VulkanWindow : public wxFrame {
public:
    VulkanWindow(wxWindow* parent, wxWindowID id, const wxString &title);
    virtual ~VulkanWindow();

private:
    void OnResize(wxSizeEvent& event);
    void onClose(wxCloseEvent& event);
    VulkanCanvas *m_canvas;
    HierarchyPanel *hierarchyPanel;
    ObjectsCanvas *objCanvas;
    DirPanel *dirPanel;
    FilesPanel *filesPanel;
};

