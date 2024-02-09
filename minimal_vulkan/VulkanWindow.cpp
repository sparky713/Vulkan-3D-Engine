#include <iostream>
#include <fstream>

#include "VulkanWindow.h"
#include "VulkanException.h"

/**
 * Flag used to determine if vulkan initialization has been completed.
 * Used when initializng g_canvas in VulkanCanvas::VulkanCanvas().
 */
bool vulkanInitDone = false;

/**
 * Constructs a VulkanWindow object and initializes VulkanCanvas.
 * @param parent the parent wxWindow
 * @param id window ID
 * @param title the title of this window
 */
VulkanWindow::VulkanWindow(wxWindow* parent, wxWindowID id, const wxString &title)
    : wxFrame(parent, id, title), m_canvas(nullptr) {
    Bind(wxEVT_SIZE, &VulkanWindow::OnResize, this);
    Bind(wxEVT_CLOSE_WINDOW, &VulkanWindow::onClose, this);

    //==========================================================================
    // add panels
    //==========================================================================
    //filesPanel = new FilesPanel(this, wxID_ANY, wxPoint(WINDOW_X, WINDOW_Y), { FilesPanel::W, FilesPanel::H });
    dirPanel = new DirPanel(this, wxID_ANY, wxPoint(WINDOW_X, WINDOW_Y), { DIR_PANEL_W, DIR_PANEL_H });

    m_canvas = new VulkanCanvas(this, wxID_ANY, wxPoint(dirPanel->GetPosition().x + DIR_PANEL_W, WINDOW_Y),
                                { VULKAN_CANVAS_W, VULKAN_CANVAS_H });
    vulkanInitDone = true;

    hierarchyPanel = new HierarchyPanel(this, wxID_ANY, wxPoint(m_canvas->GetPosition().x + VULKAN_CANVAS_W, WINDOW_Y),
                                  { HIERARCHY_PANEL_W, HIERARCHY_PANEL_H });
    m_canvas->setHierarchyPanel(hierarchyPanel);

    objCanvas = new ObjectsCanvas(this, wxID_ANY, wxPoint(m_canvas->GetPosition().x + VULKAN_CANVAS_W, WINDOW_Y + HIERARCHY_PANEL_H),
                                  { OBJECTS_CANVAS_W, OBJECTS_CANVAS_H });

    filesPanel = new FilesPanel(this, wxID_ANY, wxPoint(dirPanel->GetPosition().x + DIR_PANEL_W, m_canvas->GetPosition().y + VULKAN_CANVAS_H),
                                                  { FILES_PANEL_W, FILES_PANEL_H });
    m_canvas->setFilesPanel(filesPanel);

    filesPanel->setDirPanel(dirPanel);
    dirPanel->setFilesPanel(filesPanel);
    //==========================================================================

    //--------------------------------------------------------------------------
    // open last opened path
    //--------------------------------------------------------------------------
    std::ifstream myfile("last_opened_dir.txt");
    std::string path;
    getline(myfile, path);
    std::cout << "VulkanWindow::VulkanWindow(): opening path: " << path << std::endl;
    myfile.close();

    if (path.length() > 0) {
        //dirTreeCtrl->ExpandPath(path);
        dirPanel->dirTreeCtrl->SelectPath(path);
    }
    //--------------------------------------------------------------------------

    Fit();
}

/**
 * Deconstructor.
 */
VulkanWindow::~VulkanWindow() {}

/**
 * Resizes the window.
 * @param event wxSizeEvent
 */
void VulkanWindow::OnResize(wxSizeEvent& event) {
    wxSize clientSize = GetClientSize();
    //m_canvas->SetSize(clientSize);
}

void VulkanWindow::onClose(wxCloseEvent& event) {
    // save last dir path opened to file
    std::string lastOpenedPath = dirPanel->dirTreeCtrl->GetPath();

    std::ofstream myfile;
    myfile.open("last_opened_dir.txt");
    myfile << lastOpenedPath;
    myfile.close();


    // example from https://docs.wxwidgets.org/3.0/classwx_close_event.html
    //if (event.CanVeto() && m_bFileNotSaved)
    //{
    //    if (wxMessageBox("The file has not been saved... continue closing?",
    //        "Please confirm",
    //        wxICON_QUESTION | wxYES_NO) != wxYES)
    //    {
    //        event.Veto();
    //        return;
    //    }
    //}
    Destroy();  // you may also do:  event.Skip();
    // since the default event handler does call Destroy(), too
}
