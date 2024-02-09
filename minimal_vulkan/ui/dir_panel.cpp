#include <iostream>
#include <fstream>

#include <wx/filename.h>
#include <wx/dir.h>

#include "dir_panel.h"

enum {
    FILE_Any = 0,
    FOLDER_Any = 1,
};

#define LIGHTER_GRAY wxColour(212, 212, 212)
#define MEDIUM_GRAY wxColour(96, 96, 96)
#define DARKER_GRAY wxColour(46, 46, 46)

BEGIN_EVENT_TABLE(DirPanel, wxWindow)
//EVT_DIRCTRL_FILEACTIVATED(FILE_Any, DirPanel::onFilePressed)
EVT_DIRCTRL_SELECTIONCHANGED(FOLDER_Any, DirPanel::onFolderPressed)
END_EVENT_TABLE()

DirPanel::DirPanel(wxWindow* pParent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
    //: wxPanel(pParent, id, pos, size, style, name) {
    : wxWindow(pParent, id, pos, size, style, name) {
    SetPosition(pos);
    SetSize(size);
    //SetBackgroundColour(wxColour(*wxWHITE));
    SetBackgroundColour(DARKER_GRAY);

    int btnIndexCounter = 0;

    //wxLaunchDefaultBrowser("https://www.netflix.com/browse");
    //wxLaunchDefaultApplication("C:/Users/sally/Downloads/season2_p2/wxWidgets-3.2.1/samples/minimal_vulkan/models/Arial.png");
    dirTreeCtrl = new wxGenericDirCtrl(this, wxID_ANY, "C:/Users/sally/Downloads/season2_p2/wxWidgets-3.2.1/samples/minimal_vulkan",
        wxPoint(5, 10), wxSize(FILE_DIR_CTRL_W, FILE_DIR_CTRL_H), wxDIRCTRL_DIR_ONLY | wxNO_BORDER);
    dirTreeCtrl->Bind(wxEVT_DIRCTRL_SELECTIONCHANGED, &DirPanel::onFolderPressed, this);

    dirTreeCtrl->GetTreeCtrl()->SetForegroundColour(wxColour(255, 255, 255));
    //dirTreeCtrl->GetTreeCtrl()->SetBackgroundColour(MEDIUM_GRAY);
    dirTreeCtrl->GetTreeCtrl()->SetBackgroundColour(DARKER_GRAY);
}

DirPanel::~DirPanel() noexcept {}

void DirPanel::update() { // not used yet
}

void DirPanel::onFolderPressed(wxTreeEvent& event) {
//void DirPanel::onFolderPressed(wxMouseEvent& event) {
    std::cout << "FilesPanel::onFolderPressed: START" << std::endl;

    if (dirTreeCtrl) {
        std::cout << "Folder activated " << dirTreeCtrl->GetPath(event.GetItem()) << std::endl;
        dirTreeCtrl->ExpandPath(dirTreeCtrl->GetPath(event.GetItem()));
        std::string curFolderPath = dirTreeCtrl->GetPath(event.GetItem());
        //dirTreeCtrl->SetPath("C:/Users/sally/Downloads/season2_p2/wxWidgets-3.2.1/samples/minimal_vulkan/models");

        //----------------------------------------------------------------------
        // get subdirectories and files under this folder
        //----------------------------------------------------------------------
        wxDir dir(curFolderPath);
        if (!dir.IsOpened()) {
            std::cout << "FilesPanel::onFolderPressed: ERROR. Cannot open directory." << std::endl;
            return;
        }
        //puts("Files in current directory:");
        
        wxString filename;
        wxString dirname;
        std::vector<std::string> curFiles;
        std::vector<std::string> curSubDirectories;
        //bool cont = dir.GetFirst(&filename); // files & directories
        bool dirCont = dir.GetFirst(&dirname, wxEmptyString, wxDIR_DIRS); // directories only
        while (dirCont) {
            //std::cout << "FilesPanel::onFolderPressed: dirname = " << dirname.ToStdString() << std::endl;
            curSubDirectories.push_back(dirname.ToStdString());
            dirCont = dir.GetNext(&dirname);
        }

        bool filesCont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES); // files only
        while (filesCont) {
            //std::cout << "FilesPanel::onFolderPressed: filename = " << filename.ToStdString() << std::endl;
            curFiles.push_back(filename.ToStdString());
            filesCont = dir.GetNext(&filename);
        }


        //----------------------------------------------------------------------
        // collect and send to filesPanel
        //----------------------------------------------------------------------
        filesPanel->setFilesAndSubDirectories(curFiles, curSubDirectories);
        filesPanel->update(curFolderPath, false);

        std::ofstream myfile;
        myfile.open("last_opened_dir.txt");
        myfile << curFolderPath;
        myfile.close();
    }

    event.Skip();
}

void DirPanel::setFilesPanel(FilesPanel *filesPanel) {
    this->filesPanel = filesPanel;
}

FilesPanel *DirPanel::getFilesPanel() {
    return filesPanel;
}
