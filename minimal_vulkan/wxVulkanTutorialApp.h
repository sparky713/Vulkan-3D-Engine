#pragma once
#include <wx/wx.h>

/**
 * wxVulkanTutorialApp is the starting point of this application.
 */
class wxVulkanTutorialApp : public wxApp {
public:
    wxVulkanTutorialApp();
    virtual ~wxVulkanTutorialApp();
    virtual bool OnInit() override;
};
