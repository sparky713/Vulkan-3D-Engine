#pragma once

#include "wx/wx.h"
#include "wx/progdlg.h"
#include <wx/filename.h>
#include <filesystem>
#include "non_vk_common.h"

#include "my_game_obj_drop_target.h"

#define LIST_VIEW 0
#define GRID_VIEW 1
#define DIRECTORY_VIEW LIST_VIEW
//#define DIRECTORY_VIEW GRID_VIEW

class DirPanel;

class FilesPanel : public wxScrolledWindow {
public:
    //--------------------------------------------------------------------------
    // files & folders
    //--------------------------------------------------------------------------
    DirPanel* dirPanel;
    wxListView* listView;
    wxImageList* imageListListView;
    std::vector<std::string> files;
    std::vector<std::string> subDirectories;
    
    //--------------------------------------------------------------------------
    // drag & drop
    //--------------------------------------------------------------------------
    MyGameObjDropTarget *textDropTarget;

    std::string curDirPath;
    int curViewType; //0 = list, 1 = grid
    bool fileSelected;
    std::string selectedFile;
    //--------------------------------------------------------------------------

    FilesPanel(wxWindow *pParent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "VulkanCanvasName");

    virtual ~FilesPanel() noexcept;

    void update(std::string curDirPath, bool updateView); // not called regularly, only called when selected file has changed. called by FilesPanel::onFilePressed().

    void setDirPanel(DirPanel *dirPanel); //called when this panel is created in VulkanWindow constructor
    void setFilesAndSubDirectories(std::vector<std::string> files, std::vector<std::string> subDirectories);

    DirPanel *getDirPanel();

private:
    wxBitmap listViewIconBitMap;
    wxBitmap gridViewIconBitMap;

    wxBitmapButton* listViewBtn;
    wxBitmapButton* gridViewBtn;

    //--------------------------------------------------------------------------
    // event handlers
    //--------------------------------------------------------------------------
    void onListItemPressed(wxListEvent& event); // calls DirPanel::onFolderPressed(...), previously FilesPanel::onSubDirPressed()
    void onListItemDragBegin(wxListEvent& event); // previously FilesPanel::onFilePressed()

    void onMouseReleased(wxMouseEvent& event);

    void onListBtnPressed(wxCommandEvent& event);
    void onGridBtnPressed(wxCommandEvent& event);

public:
    const int LIST_W = FILES_PANEL_W - 50;
    const int LIST_H = FILES_PANEL_H - 70;

    const int LIST_GRID_BTN_SIZE = 23;
    const int LIST_GRID_BTN_Y = 10;

    const int LBL_LIST_VIEW_START_X = 25;
    const int LBL_LIST_VIEW_START_Y = (LIST_GRID_BTN_Y + LIST_GRID_BTN_SIZE + 3);

    const std::string DIR_ICON_FILENAME = "assets/icons/folder.png";
    const std::string DIR_LIST_VIEW_ICON_FILENAME = "assets/icons/folder_list.png";
    const std::string MESH_FILE_ICON_FILENAME = "assets/icons/object_color.png";
    const std::string MESH_FILE_LIST_VIEW_ICON_FILENAME = "assets/icons/object_color_list.png";
    const std::string PNG_ICON_FILENAME = "assets/icons/png.png";
    const std::string PNG_LIST_VIEW_ICON_FILENAME = "assets/icons/png_list.png";
    const std::string JPG_ICON_FILENAME = "assets/icons/jpg.png";
    const std::string JPG_LIST_VIEW_ICON_FILENAME = "assets/icons/jpg_list.png";
    const std::string ETC_ICON_FILENAME = "assets/icons/attach.png";
    const std::string ETC_LIST_VIEW_ICON_FILENAME = "assets/icons/attach_list.png";

    const int DIR_LIST_VIEW_ICON_INDEX = 0;
    const int MESH_FILE_LIST_VIEW_ICON_INDEX = 1;
    const int PNG_LIST_VIEW_ICON_INDEX = 2;
    const int JPG_LIST_VIEW_ICON_INDEX = 3;
    const int ETC_LIST_VIEW_ICON_INDEX = 4;

    //const std::string LIST_VIEW_ICON_FILENAME = "assets/icons/list.png";
    const std::string LIST_VIEW_ICON_FILENAME = "assets/icons/list_color.png";
    //const std::string GRID_VIEW_ICON_FILENAME = "assets/icons/grid.png";
    const std::string GRID_VIEW_ICON_FILENAME = "assets/icons/grid_color.png";
    DECLARE_EVENT_TABLE()
};
