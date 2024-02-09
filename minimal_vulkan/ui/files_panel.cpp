#include "VulkanCanvas.h"
#include <wx/filename.h>
#include <wx/dir.h>

#include "dir_panel.h"
#include "files_panel.h"

extern VulkanCanvas *g_canvas;
int filesPanelMouseX = 0;
int filesPanelMouseY = 0;

enum {
    BTN_List = 0,
    BTN_Grid = 1,
};

BEGIN_EVENT_TABLE(FilesPanel, wxScrolledWindow)
EVT_BUTTON(BTN_List, FilesPanel::onListBtnPressed)
EVT_BUTTON(BTN_Grid, FilesPanel::onGridBtnPressed)
END_EVENT_TABLE()

#define LIGHTER_GRAY wxColour(212, 212, 212)
#define MEDIUM_GRAY wxColour(126, 126, 126)
#define DARKER_GRAY wxColour(46, 46, 46)

FilesPanel::FilesPanel(wxWindow* pParent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
    : wxScrolledWindow(pParent, id) {
    SetPosition(pos);
    SetSize(size);
    //SetBackgroundColour(wxColour(*wxWHITE));
    SetBackgroundColour(DARKER_GRAY);

    dirPanel = nullptr;
    curViewType = DIRECTORY_VIEW;
    fileSelected = false;
    selectedFile = "";
    
    //--------------------------------------------------------------------------
    // wxListView
    //--------------------------------------------------------------------------
    wxInitAllImageHandlers();
    imageListListView = new wxImageList(16, 16, true);
    imageListListView->Add(wxIcon(DIR_LIST_VIEW_ICON_FILENAME, wxBITMAP_TYPE_PNG, 16, 16));
    imageListListView->Add(wxIcon(MESH_FILE_LIST_VIEW_ICON_FILENAME, wxBITMAP_TYPE_PNG, 16, 16));
    imageListListView->Add(wxIcon(PNG_LIST_VIEW_ICON_FILENAME, wxBITMAP_TYPE_PNG, 16, 16));
    imageListListView->Add(wxIcon(JPG_LIST_VIEW_ICON_FILENAME, wxBITMAP_TYPE_PNG, 16, 16));
    imageListListView->Add(wxIcon(ETC_LIST_VIEW_ICON_FILENAME, wxBITMAP_TYPE_PNG, 16, 16));

    listView = new wxListView(this, wxID_ANY, wxPoint(LBL_LIST_VIEW_START_X, LBL_LIST_VIEW_START_Y), wxSize(LIST_W, LIST_H), wxLC_LIST | wxLC_SINGLE_SEL | wxNO_BORDER);
    listView->AssignImageList(imageListListView, wxIMAGE_LIST_SMALL);
    listView->SetColumnWidth(0, LIST_W);
    listView->Bind(wxEVT_LIST_ITEM_SELECTED, &FilesPanel::onListItemPressed, this);
    listView->Bind(wxEVT_LIST_BEGIN_DRAG, &FilesPanel::onListItemDragBegin, this);
    listView->SetBackgroundColour(MEDIUM_GRAY);

    //--------------------------------------------------------------------------
    // other UI
    //--------------------------------------------------------------------------
    listViewIconBitMap.LoadFile(LIST_VIEW_ICON_FILENAME, wxBITMAP_TYPE_PNG);
    gridViewIconBitMap.LoadFile(GRID_VIEW_ICON_FILENAME, wxBITMAP_TYPE_PNG);

    listViewBtn = new wxBitmapButton(this, wxID_ANY, listViewIconBitMap, wxPoint(FILES_PANEL_W - 60, LIST_GRID_BTN_Y),
        wxSize(LIST_GRID_BTN_SIZE, LIST_GRID_BTN_SIZE));
    listViewBtn->Bind(wxEVT_BUTTON, &FilesPanel::onListBtnPressed, this);

    gridViewBtn = new wxBitmapButton(this, wxID_ANY, gridViewIconBitMap,
        wxPoint(listViewBtn->GetPosition().x + 23, listViewBtn->GetPosition().y),
        wxSize(LIST_GRID_BTN_SIZE, LIST_GRID_BTN_SIZE));
    //gridViewBtn->Bind(wxEVT_BUTTON, &FilesPanel::onGridBtnPressed, this); //TODO: drag & drop not supported for grid view yet
    
    //--------------------------------------------------------------------------
    // drag & drop
    //--------------------------------------------------------------------------
    textDropTarget = new MyGameObjDropTarget(g_canvas);
    g_canvas->SetDropTarget(textDropTarget);

    //--------------------------------------------------------------------------
    // mouse events
    //--------------------------------------------------------------------------
    Bind(wxEVT_LEFT_UP, &FilesPanel::onMouseReleased, this);

    SetScrollbars(20, 20, FILES_PANEL_W / 20, FILES_PANEL_H / 20);
}

/**
 * Deconstructor.
 */
FilesPanel::~FilesPanel() noexcept {
    delete listView;
    imageListListView->Destroy();
    delete imageListListView;
    delete textDropTarget;
    delete listViewBtn;
    delete gridViewBtn;
}

/**
 * Updates the list on the current directory's sub directories and files. Also
 * used to change the view from list to grid and grid to list.
 * @param curDirPath the current directory to switch to
 * @param updateView true if this update was called to change the view
 */
void FilesPanel::update(std::string curDirPath, bool updateView) { // called by DirPanel::onFolderPressed()
    int totalHeight = 0;
    if ((this->curDirPath != curDirPath && !updateView) || updateView) { // update display
        this->curDirPath = curDirPath;
        //======================================================================
        // directories
        //======================================================================
        for (int s = 0; s < subDirectories.size(); s++) {
            if (curViewType == LIST_VIEW) {
                //std::cout << "subDirectories.size(): " << subDirectories.size() << std::endl;
                listView->InsertItem(listView->GetItemCount(), subDirectories[s], DIR_LIST_VIEW_ICON_INDEX);
            }
            else if (curViewType == GRID_VIEW) { // grid view
                // TODO
                //gridView->InsertItem(gridView->GetItemCount(), subDirectories[s], DIR_LIST_VIEW_ICON_INDEX);
            }
        } // end of for (int s = 0; s < subDirectories.size(); s++)

        //======================================================================
        // files
        //======================================================================
        for (int f = 0; f < files.size(); f++) {
            if (curViewType == LIST_VIEW) {
                wxString* fileExt = new wxString();
                wxFileName::SplitPath((wxString)(curDirPath + "/" + files[f]), NULL, NULL, fileExt);
                if (fileExt->ToStdString() == "png") {
                    listView->InsertItem(listView->GetItemCount(), files[f], PNG_LIST_VIEW_ICON_INDEX);
                }
                else if (fileExt->ToStdString() == "jpg") {
                    listView->InsertItem(listView->GetItemCount(), files[f], JPG_LIST_VIEW_ICON_INDEX);
                }
                else if (fileExt->ToStdString() == "obj" || fileExt->ToStdString() == "md5mesh") {
                    listView->InsertItem(listView->GetItemCount(), files[f], MESH_FILE_LIST_VIEW_ICON_INDEX);
                }
                else {
                    listView->InsertItem(listView->GetItemCount(), files[f], ETC_LIST_VIEW_ICON_INDEX);
                }
            }
            else if (curViewType == GRID_VIEW) {
                // TODO
            }
        } // end of for (int f = 0; f < files.size(); f++)
    }
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// event handlers
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * Checks that the selected item is a directory and sets the current path of the wxGenericDirCtrl
 * (DirPanel::dirTreeCtrl) to the selected item.
 * @param wxListEvent triggered when a list item is selected
 */
void FilesPanel::onListItemPressed(wxListEvent& event) {
    //std::cout << "FilesPanel::onListItemPressed(): start" << std::endl;
    std::string newDirPath = curDirPath + "\\" + listView->GetItemText(event.GetIndex()); // full path of selected folder

    if (std::filesystem::is_directory(newDirPath)) {
        dirPanel->dirTreeCtrl->SetPath(newDirPath); // calls DirPanel::onFolderPressed();
    }
}

/**
 * Checks that the selected item is not a directory (therefore, a file) and starts the drag & drop.
 * @param wxListEvent triggered when a list item is being dragged.
 */
void FilesPanel::onListItemDragBegin(wxListEvent& event) {
    selectedFile = curDirPath + "\\" + listView->GetItemText(event.GetIndex()); // full path of selected folder
    
    if (!std::filesystem::is_directory(selectedFile)) {
        if (fileSelected) {
            return;
        }
        std::cout << "FilesPanel::onListItemDragBegin(): newPath: " << selectedFile << std::endl;
        fileSelected = true;

        wxTextDataObject tdo(selectedFile);
        wxDropSource tds(tdo, listView);
        tds.DoDragDrop(true);
    }
}

/**
 * Changes the current view to list. 
 */
void FilesPanel::onListBtnPressed(wxCommandEvent& event) {
    curViewType = LIST_VIEW;
    update(curDirPath, true);
}

/**
 * Changes the current view to grid.
 */
void FilesPanel::onGridBtnPressed(wxCommandEvent& event) {
    curViewType = GRID_VIEW;
    update(curDirPath, true);
}

//--------------------------------------------------------------------------
// mouse events
//--------------------------------------------------------------------------
/**
 * Handles wxWidgets mouse released events. Triggered when the user releases the left mouse button.
 * releases selected file and sets FilesPanel::fileSelected to false
 */
void FilesPanel::onMouseReleased(wxMouseEvent& event) {
    fileSelected = false;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// getters & setters
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int count = 0;
/**
 * Clears current files, subDirectories and sets it to the given vectors. Also
 * clears the items in the list.
 * @param files the new list of files
 * @param subDirectories the new list of subDirectories
 */
void FilesPanel::setFilesAndSubDirectories(std::vector<std::string> files, std::vector<std::string> subDirectories) {

    this->files.clear();
    this->subDirectories.clear();

    this->files = files;
    this->subDirectories = subDirectories;

    listView->DeleteAllItems();
    //gridView->DeleteAllItems();
}

/**
 * Sets dirPanel to the given dirPanel.
 * @return the given dirPanel
 */
void FilesPanel::setDirPanel(DirPanel* dirPanel) {
    this->dirPanel = dirPanel;
}

/**
 * Returns the dirPanel.
 * @return dirPanel
 */
DirPanel* FilesPanel::getDirPanel() {
    return dirPanel;
}
