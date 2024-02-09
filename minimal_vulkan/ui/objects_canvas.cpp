#include "VulkanCanvas.h"
#include "obj_loader.h"
#include "my_obj.h"
//#include "VulkanMD5Model.h"

#include "objects_canvas.h"

extern VulkanCanvas *g_canvas;
ObjectsCanvas *o_canvas;

enum {
    BUTTON_Any = 0,
    BUTTON_Bob = 1,
    TEXT_AREA_Camera = 2,
};

BEGIN_EVENT_TABLE(ObjectsCanvas, wxWindow)
EVT_BUTTON(BUTTON_Any, ObjectsCanvas::onButtonPressed)
EVT_BUTTON(BUTTON_Bob, ObjectsCanvas::onBobButtonPressed)
EVT_BUTTON(BUTTON_Bob, ObjectsCanvas::onCubeFaceButtonPressed)
EVT_TEXT_ENTER(TEXT_AREA_Camera, ObjectsCanvas::cameraPosTextCtrlHandler)
END_EVENT_TABLE()

#define LIGHTER_GRAY wxColour(212, 212, 212)
#define MEDIUM_GRAY wxColour(126, 126, 126)
#define DARKER_GRAY wxColour(46, 46, 46)

ObjectsCanvas::ObjectsCanvas(wxWindow* pParent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
    : wxWindow(pParent, id, pos, size, style, name) {
    SetPosition(pos);
    SetSize(size);
    //SetBackgroundColour(wxColour(*wxBLUE));
    SetBackgroundColour(DARKER_GRAY);

    int btnIndexCounter = 0;

    ////==========================================================================
    //// obj
    ////==========================================================================
    //for (int i = 0; i < g_canvas->objLoaders.size(); i++) { // for every obj file
    //    ObjLoader* curObjFile = g_canvas->objLoaders[i];
    //    for (int j = 0; j < curObjFile->myObjsArr.size(); j++) { // for every object/mesh
    //        std::string curObjName = curObjFile->myObjsArr[j]->getObjName();
    //        //std::cout << "ObjectsCanvas::ObjectsCanvas(): curObjName: " << curObjName << std::endl;
    //        if (curObjName != "x" && curObjName != "y" && curObjName != "z"
    //            && curObjName != "-x" && curObjName != "-y" && curObjName != "-z"
    //            && curObjName != "") {
    //            // create button
    //            wxButton* curObjButton = new wxButton(this, wxID_ANY, curObjFile->myObjsArr[j]->getObjName(),
    //                wxPoint(BUTTON_START_X, BUTTON_START_Y + ((BUTTON_H + BUTTON_MARGIN_TOP) * btnIndexCounter)),
    //                wxSize(BUTTON_W, BUTTON_H));
    //            buttons.push_back(curObjButton);
    //            objButtonsMap[curObjButton] = curObjFile->myObjsArr[j];
    //            curObjButton->Bind(wxEVT_BUTTON, &ObjectsCanvas::onButtonPressed, this);
    //            btnIndexCounter++;
    //        }
    //    }
    //}

    ////==========================================================================
    //// md5
    ////==========================================================================
    ////for (int i = 0; i < g_canvas->vulkanMD5Models.size(); i++) { // for every obj file
    ////    VulkanMD5Model *curMD5File = g_canvas->vulkanMD5Models[i];
    ////    for (int j = 0; j < curMD5File->getNumMeshes(); j++) { // for every mesh
    ////        std::string curObjName = curObjFile->myObjsArr[j]->getObjName();
    ////        //std::cout << "ObjectsCanvas::ObjectsCanvas(): curObjName: " << curObjName << std::endl;
    ////        if (curObjName != "x" && curObjName != "y" && curObjName != "z"
    ////            && curObjName != "-x" && curObjName != "-y" && curObjName != "-z"
    ////            && curObjName != "") {
    ////            // create button
    ////            wxButton* curObjButton = new wxButton(this, wxID_ANY, curObjFile->myObjsArr[j]->getObjName(),
    ////                wxPoint(BUTTON_START_X, BUTTON_START_Y + ((BUTTON_H + BUTTON_MARGIN_TOP) * btnIndexCounter)),
    ////                wxSize(BUTTON_W, BUTTON_H));
    ////            buttons.push_back(curObjButton);
    ////            objButtonsMap[curObjButton] = curObjFile->myObjsArr[j];
    ////            curObjButton->Bind(wxEVT_BUTTON, &ObjectsCanvas::onButtonPressed, this);
    ////            btnIndexCounter++;
    ////        }
    ////    }
    ////}

    //bob = new wxButton(this, wxID_ANY, "bob <o>",
    //    wxPoint(BUTTON_START_X, BUTTON_START_Y + ((BUTTON_H + BUTTON_MARGIN_TOP) * btnIndexCounter)),
    //    wxSize(BUTTON_W, BUTTON_H));
    //buttons.push_back(bob);
    //bob->Bind(wxEVT_BUTTON, &ObjectsCanvas::onBobButtonPressed, this);
    //btnIndexCounter++;
    ////==========================================================================
    //// for testing cube
    ////==========================================================================
    //initCubeFaceButtons(btnIndexCounter);

    //==========================================================================
    // camera pos-+
    // 
    //==========================================================================
    btnIndexCounter++;
    int labelW = 15;
    int labelH = 20;
    int posW = 53;
    int marginLeftPos = 2;
    int marginLeft = labelW + posW + marginLeftPos + 5;
    int marginTop = labelH + 5;
    cameraTitleLbl = new wxStaticText(this, wxID_ANY, "Camera:",
        wxPoint(BUTTON_START_X, BUTTON_START_Y), wxSize(BUTTON_W, 20));
        //wxPoint(BUTTON_START_X, BUTTON_START_Y + ((BUTTON_H + BUTTON_MARGIN_TOP) * btnIndexCounter)), wxSize(BUTTON_W, 20));
        
    cameraTitleLbl->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    //btnIndexCounter++;
    cameraXPosLbl = new wxStaticText(this, wxID_ANY, "x:",
        wxPoint(BUTTON_START_X, cameraTitleLbl->GetPosition().y + 25), wxSize(labelW, labelH));
    cameraYPosLbl = new wxStaticText(this, wxID_ANY, "y:", wxPoint(cameraXPosLbl->GetPosition().x + marginLeft, cameraXPosLbl->GetPosition().y), wxSize(labelW, labelH));
    cameraZPosLbl = new wxStaticText(this, wxID_ANY, "z:", wxPoint(cameraYPosLbl->GetPosition().x + marginLeft, cameraXPosLbl->GetPosition().y), wxSize(labelW, labelH));

    cameraXPos = new wxStaticText(this, wxID_ANY, "x:", wxPoint(cameraXPosLbl->GetPosition().x + labelW + marginLeftPos, cameraXPosLbl->GetPosition().y), wxSize(posW, labelH));
    cameraYPos = new wxStaticText(this, wxID_ANY, "y:", wxPoint(cameraYPosLbl->GetPosition().x + labelW + marginLeftPos, cameraXPosLbl->GetPosition().y), wxSize(posW, labelH));
    cameraZPos = new wxStaticText(this, wxID_ANY, "z:", wxPoint(cameraZPosLbl->GetPosition().x + labelW + marginLeftPos, cameraXPosLbl->GetPosition().y), wxSize(posW, labelH));

    cameraXPosTA = new wxTextCtrl(this, wxID_ANY, "x", wxPoint(cameraXPosLbl->GetPosition().x, cameraXPosLbl->GetPosition().y + marginTop),
                                  wxSize(posW, labelH), wxTE_PROCESS_ENTER);
    cameraXPosTA->Bind(wxEVT_TEXT_ENTER, &ObjectsCanvas::cameraPosTextCtrlHandler, this);

    cameraYPosTA = new wxTextCtrl(this, wxID_ANY, "y", wxPoint(cameraYPosLbl->GetPosition().x, cameraXPosLbl->GetPosition().y + marginTop),
                                  wxSize(posW, labelH), wxTE_PROCESS_ENTER);
    cameraYPosTA->Bind(wxEVT_TEXT_ENTER, &ObjectsCanvas::cameraPosTextCtrlHandler, this);

    cameraZPosTA = new wxTextCtrl(this, wxID_ANY, "z", wxPoint(cameraZPosLbl->GetPosition().x, cameraXPosLbl->GetPosition().y + marginTop),
                                  wxSize(posW, labelH), wxTE_PROCESS_ENTER);
    cameraZPosTA->Bind(wxEVT_TEXT_ENTER, &ObjectsCanvas::cameraPosTextCtrlHandler, this);

    //EVT_TEXT_ENTER(TEXT_AREA_Camera, ObjectsCanvas::cameraPosTextCtrlHandler)

    o_canvas = this;
}

ObjectsCanvas::~ObjectsCanvas() noexcept {}

//extern glm::vec3 position;

void ObjectsCanvas::update() {
    glm::vec3& cameraPos = g_canvas->camPosition;
    cameraXPos->SetLabelText(wxString::Format(wxT("%0.5f"), cameraPos.x));
    cameraYPos->SetLabelText(wxString::Format(wxT("%0.5f"), cameraPos.y));
    cameraZPos->SetLabelText(wxString::Format(wxT("%0.5f"), cameraPos.z));
}

void ObjectsCanvas::initCubeFaceButtons(int& btnIndexCounter) {
    f = new wxButton(this, wxID_ANY, "f", wxPoint(0.0f, 0.0f), wxSize(BUTTON_FACES_W, BUTTON_H));
    //fr = new wxButton(this, wxID_ANY, "fr", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //fl = new wxButton(this, wxID_ANY, "fl", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //ft = new wxButton(this, wxID_ANY, "ft", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //fbt = new wxButton(this, wxID_ANY, "fbt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //frt = new wxButton(this, wxID_ANY, "frt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //flt = new wxButton(this, wxID_ANY, "flt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //frbt = new wxButton(this, wxID_ANY, "frbt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //flbt = new wxButton(this, wxID_ANY, "flbt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));

    r = new wxButton(this, wxID_ANY, "r", wxPoint(0.0f, 0.0f), wxSize(BUTTON_FACES_W, BUTTON_H));
    //rt = new wxButton(this, wxID_ANY, "rt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //rbt = new wxButton(this, wxID_ANY, "rbt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //rbk = new wxButton(this, wxID_ANY, "rbk", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //rtbk = new wxButton(this, wxID_ANY, "rtbk", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //rbtbk = new wxButton(this, wxID_ANY, "rbtbk", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));

    l = new wxButton(this, wxID_ANY, "l", wxPoint(0.0f, 0.0f), wxSize(BUTTON_FACES_W, BUTTON_H));
    //lt = new wxButton(this, wxID_ANY, "lt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //lbt = new wxButton(this, wxID_ANY, "lbt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //lbk = new wxButton(this, wxID_ANY, "lbk", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //ltbk = new wxButton(this, wxID_ANY, "ltbk", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));
    //lbtbk = new wxButton(this, wxID_ANY, "lbtbk", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));

    t = new wxButton(this, wxID_ANY, "t", wxPoint(0.0f, 0.0f), wxSize(BUTTON_FACES_W, BUTTON_H));
    //tbk = new wxButton(this, wxID_ANY, "tbk", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));

    bk = new wxButton(this, wxID_ANY, "bk", wxPoint(0.0f, 0.0f), wxSize(BUTTON_FACES_W, BUTTON_H));
    //bkbt = new wxButton(this, wxID_ANY, "bkbt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));

    bt = new wxButton(this, wxID_ANY, "bt", wxPoint(0.0f, 0.0f), wxSize(BUTTON_FACES_W, BUTTON_H));

    center = new wxButton(this, wxID_ANY, "center", wxPoint(0.0f, 0.0f), wxSize(BUTTON_W, BUTTON_H));

    cubeFaceButtons.push_back(f);
    cubeFaceButtons.push_back(r);
    cubeFaceButtons.push_back(l);
    cubeFaceButtons.push_back(t);
    cubeFaceButtons.push_back(bk);
    cubeFaceButtons.push_back(bt);
    cubeFaceButtons.push_back(center);

    //int btnIndex = 0;
    for (int b = 0; b < cubeFaceButtons.size(); b++) {
        cubeFaceButtonsMap[cubeFaceButtons[b]] = b;
        //btnIndex++;
    }
    
    int btnXPosIndex = 0;
    int btnYPosIndex = btnIndexCounter;
    for (int i = 0; i < cubeFaceButtons.size(); i++) {
        cubeFaceButtons[i]->SetPosition(wxPoint(BUTTON_START_X + ((BUTTON_FACES_W + BUTTON_MARGIN_LEFT) * btnXPosIndex),
                                                BUTTON_START_Y + ((BUTTON_H + BUTTON_MARGIN_TOP) * btnYPosIndex)));
        cubeFaceButtons[i]->Bind(wxEVT_BUTTON, &ObjectsCanvas::onCubeFaceButtonPressed, this);
        //btnIndexCounter++;

        btnXPosIndex++;
        if (btnXPosIndex >= 3) {
            btnXPosIndex = 0;
            btnYPosIndex++;
            btnIndexCounter++;
        }
    }
}

void ObjectsCanvas::onCubeFaceButtonPressed(wxCommandEvent& event) {
    //std::cout << "ObjectsCanvas::onCubeFaceButtonPressed(): started" << std::endl;

    int btnIndex = cubeFaceButtonsMap[(wxButton*)(event.GetEventObject())];

    switch (btnIndex) { //g_canvas->position = camera position
    case 0: //f
        g_canvas->camPosition = glm::vec3(7.0f, 0.0f, 0.0f);
        break;
    case 1: //r
        g_canvas->camPosition = glm::vec3(0.0f, 0.0f, -7.0f);
        break;
    case 2: //l
        g_canvas->camPosition = glm::vec3(0.0f, 0.0f, 7.0f);
        break;
    case 3: //t
        g_canvas->camPosition = glm::vec3(0.00001f, 7.0f, 0.0f);
        break;
    case 4: //bk
        g_canvas->camPosition = glm::vec3(-7.0f, 0.0f, 0.0f);
        break;
    case 5: //bt
        g_canvas->camPosition = glm::vec3(0.00001f, -7.0f, 0.0f);
        break;
    case 6: //center 
        //g_canvas->position = glm::vec3(0.0f, 0.0f, 0.0f);
        g_canvas->camPosition = glm::vec3(0.0f, 0.0f, 0.00001f);
        g_canvas->camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    default:
        std::cout << "ObjectsCanvas::onCubeFaceButtonPressed(...): no matching button index found." << std::endl;
    }
    g_canvas->camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
}

void ObjectsCanvas::onButtonPressed(wxCommandEvent& event) {
    //std::cout << "ObjectsCanvas::onButtonPressed(): started" << std::endl;
    objButtonsMap[(wxButton *)(event.GetEventObject())]->draw = !objButtonsMap[(wxButton*)(event.GetEventObject())]->draw;
    //std::cout << "ObjectsCanvas::onButtonPressed(): objButtonsMap[(wxButton *)(event.GetEventObject())]->name: " << objButtonsMap[(wxButton*)(event.GetEventObject())]->getObjName() << std::endl;
}

void ObjectsCanvas::onBobButtonPressed(wxCommandEvent& event) {
    std::cout << "ObjectsCanvas::onBobButtonPressed(): started" << std::endl;
    // move camera position
    g_canvas->camPosition = glm::vec3(0.0f, 30.0f, 100.0f);
    g_canvas->camTarget = glm::vec3(0.0f, 32.0f, 0.0f);
}

void ObjectsCanvas::cameraPosTextCtrlHandler(wxCommandEvent& event) {
    std::cout << "ObjectsCanvas::cameraPosTextCtrlHandler(): started" << std::endl;

    // get value
    float x = wxAtof(cameraXPosTA->GetValue());
    float y = wxAtof(cameraYPosTA->GetValue());
    float z = wxAtof(cameraZPosTA->GetValue());

    std::cout << "x: " << x << " | y: " << y << " | z: " << z << std::endl;
    //std::cout << "x: " << x << std::endl;
    g_canvas->camPosition = glm::vec3(x, y, z);
    g_canvas->camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
}
