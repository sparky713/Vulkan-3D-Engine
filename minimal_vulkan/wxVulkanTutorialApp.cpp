#include <wx/wxprec.h>
#include <sstream>
#include "wxVulkanTutorialApp.h"
#include "VulkanWindow.h"
#include "VulkanException.h"

#pragma warning(disable: 28251)

#ifdef _UNICODE
#ifdef _DEBUG
#pragma comment(lib, "wxbase32ud.lib")
#else
#pragma comment(lib, "wxbase32u.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "wxbase32d.lib")
#else
#pragma comment(lib, "wxbase32.lib")
#endif
#endif

/**
 * Constructs a wxVulkanTutorialApp object.
 */
wxVulkanTutorialApp::wxVulkanTutorialApp() {}

/**
 * Deconstructor.
 */
wxVulkanTutorialApp::~wxVulkanTutorialApp() {}


/**
 * Creates and initializes the main frame (VulkanWindow) and shows it.
 * Catches a VulkanException if VulkanWindow throws an exception.
 */
bool wxVulkanTutorialApp::OnInit() {
    //--------------------------------------------------------------------------
    // create a debug console
    //--------------------------------------------------------------------------
    //Alloc Console
    //print some stuff to the console
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
    printf("Debugging Window:\n");
    //--------------------------------------------------------------------------

    VulkanWindow* mainFrame;
    try {
        mainFrame = new VulkanWindow(nullptr, wxID_ANY, L"VulkanApp");
    } 
    catch(VulkanException& ve) {
        std::string status = ve.GetStatus();
        std::stringstream ss;
        ss << ve.what() << "\n" << status;
        wxMessageBox(ss.str(), "Vulkan Error");
        return false;
    }
    catch (std::runtime_error& err) {
        std::stringstream ss;
        ss << "Error encountered trying to create the Vulkan canvas:\n";
        ss << err.what();
        wxMessageBox(ss.str(), "Vulkan Error");
        return false;
    }
    mainFrame->Show(true);
    return true;
}

wxIMPLEMENT_APP(wxVulkanTutorialApp);
