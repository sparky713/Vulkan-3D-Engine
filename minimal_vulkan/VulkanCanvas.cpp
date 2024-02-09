#define VK_USE_PLATFORM_WIN32_KHR

#include "VulkanCanvas.h"
#include "VulkanException.h"
#include <wx/wxprec.h>
#include <wx/filename.h>
#include "wxVulkanTutorialApp.h"
#include <vulkan/vulkan.h>
//sally 1127:
#include "vulkanexamplebase.h"
#include "VulkanglTFModel.h"

//chai30: start
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <fstream>
#include <sstream>
#include <array>
#include <chrono>
#include <unordered_map>
#pragma comment(lib, "vulkan-1.lib")
#include <exception>
#include <algorithm>

//------------------------------------------------------------------------------
// sally
//------------------------------------------------------------------------------
#include "non_vk_common.h"
#include "common.h"
#include "my_obj.h"
#include "bb_my_obj.h"
#include "vulkan_obj_loader.h"

#include "md5/MD5Model.h"
#include "VulkanMD5Model.h"
#include "md5/MD5Animation.h"

#include "scene_node.h"
#include "game_object.h"

#include "objects_canvas.h"

#define OBJ_TYPE_BOUNDING_BOX 0
#define OBJ_TYPE_NORMAL 1

#include "hq_render.h"

#define MOUSELOOK_ON true
//#define MOUSELOOK_ON false
//#define TEST_CAMERA_ON true
#define TEST_CAMERA_ON false

#define CAMERA_POS glm::vec3(5.0f,5.0f,-3.0f)
//#define CAMERA_POS glm::vec3(10.0f,100.0f,10.0f)

#define TEST_OUTLINE_ON true
//#define TEST_OUTLINE_ON false

#define AXIS_AND_GRID_FILENAME "models/axis_and_grid_for_opengl_with_axis_labels.obj"

const std::vector<std::string> objectFilenames = {
    "models/axis_and_grid_for_opengl_with_axis_labels.obj",
    //"models/cube_and_sphere.obj",
    "models/icosphere.obj",
    "models/icosphere.obj",
    //"models/cube_origin.obj",
    //"models/viking_room.obj",
    //"models/cube_and_sphere.obj",
    //"models/cube_and_sphere_bounding_box.obj", // is actually only just a cube, no sphere
    "models/flat_square.obj",
};

const std::vector<glm::vec3> objectPosition = {
    glm::vec3(0.0f,0.0f,0.0f),
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(-5.0f, 0.0f, -5.0f),
    //glm::vec3(5.0f, 3.0f, 5.0f),
    //glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 1.0f, 5.0f),
    //glm::vec3(0.0f, 0.0f, 1.0f),
    //glm::vec3(0.0f, 0.0f, 0.0f),
    //glm::vec3(0.0f, 0.0f, 0.0f),
    //glm::vec3(0.0f, 0.0f, 0.0f),
    //glm::vec3(0.0f, 0.0f, 0.0f),
};

const std::vector<glm::vec3> objectRotation = {
    glm::vec3(0.0f,0.0f,0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(23.0f, 45.0f, 270.0f),
    //glm::vec3(0.0f, 180.0f, 0.0f),
    //glm::vec3(0.0f, 1.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    //glm::vec3(0.0f, 0.0f, 0.0f),
    //glm::vec3(0.0f, 0.0f, 0.0f),
    //glm::vec3(0.0f, 0.0f, 0.0f),
    //glm::vec3(0.0f, 0.0f, 0.0f),
};

BBMyObj *bbMyObj;

struct ObjFileInfo {
    std::string filename;
    int objType; // 0 normal game object / 1:bounding box
    BBMyObj *bbMyObj;
};

const std::vector<const char*> validationLayers = {
//    "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef _DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

VulkanCanvas* g_canvas;
extern ObjectsCanvas *o_canvas;
extern bool vulkanInitDone;
 
//sally17
VkResult VulkanCanvas::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanCanvas::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void VulkanCanvas::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VulkanCanvas::setupDebugMessenger() { // not used
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
//            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
            long posHashVal = vertex.pos.x * vertex.pos.y * vertex.pos.z;
            long colorHashVal = vertex.color.x * vertex.color.y * vertex.color.z;
            colorHashVal = colorHashVal << 1;
            colorHashVal = colorHashVal >> 1;
            long texCoordHashVal = vertex.texCoord.x * vertex.texCoord.y;
            texCoordHashVal = texCoordHashVal << 1;
            return (size_t)(posHashVal ^ colorHashVal ^ texCoordHashVal);
        }
    };
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VulkanCanvas::VulkanCanvas(wxWindow* pParent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
    : wxWindow(pParent, id, pos, size, style, name),
    m_vulkanInitialized(false), m_instance(VK_NULL_HANDLE),
    m_surface(VK_NULL_HANDLE), m_physicalDevice(VK_NULL_HANDLE),
    m_logicalDevice(VK_NULL_HANDLE), m_swapchain(VK_NULL_HANDLE),
    m_renderPass(VK_NULL_HANDLE), m_pipelineLayout(VK_NULL_HANDLE),
    m_graphicsPipeline(VK_NULL_HANDLE), m_commandPool(VK_NULL_HANDLE)
    //    m_imageAvailableSemaphore(VK_NULL_HANDLE), m_renderFinishedSemaphore(VK_NULL_HANDLE)
{
    g_canvas = this; // used in VulkanObjLoader to call methods in this class (for VulkanCanvas member fields)

    std::cout << "VulkanCanvas::VulkanCanvas(): start" << std::endl;
    //--------------------------------------------------------------------------
    // mouse & key events
    //--------------------------------------------------------------------------
    Bind(wxEVT_KEY_DOWN, &VulkanCanvas::onKeyDown, this);
    Bind(wxEVT_KEY_UP, &VulkanCanvas::onKeyUp, this);

    Bind(wxEVT_MOTION, &VulkanCanvas::onMouseMoved, this);
    Bind(wxEVT_LEFT_DOWN, &VulkanCanvas::onMouseDown, this);
    Bind(wxEVT_MIDDLE_DOWN, &VulkanCanvas::onMouseDown, this);
    Bind(wxEVT_RIGHT_DOWN, &VulkanCanvas::onMouseDown, this);
    Bind(wxEVT_LEFT_UP, &VulkanCanvas::onMouseReleased, this);
    Bind(wxEVT_MIDDLE_UP, &VulkanCanvas::onMMBReleased, this);
    Bind(wxEVT_MOUSEWHEEL, &VulkanCanvas::onMouseWheel, this);
    rotationOn = false;
    panningOn = false;
    zoomOn = false;
    //--------------------------------------------------------------------------

    Bind(wxEVT_PAINT, &VulkanCanvas::OnPaint, this);
    Bind(wxEVT_SIZE, &VulkanCanvas::OnResize, this);
    std::vector<const char*> requiredExtensions = { "VK_KHR_surface", "VK_KHR_win32_surface" }; //sally19
    InitializeVulkan(requiredExtensions);
    VkApplicationInfo appInfo = CreateApplicationInfo("VulkanApp1");
    std::vector<const char*> layerNames;
    if (enableValidationLayers) {
        layerNames = validationLayers;
    }
    VkInstanceCreateInfo createInfo = CreateInstanceCreateInfo(appInfo, requiredExtensions, layerNames);

    CreateInstance(createInfo);
    //setupDebugMessenger(); //sally17 <-- turned off for now
    CreateWindowSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    createSwapChain();
    initCamera(); // must call after createSwapChain() - m_swapchainextent must be initialized

    CreateImageViews();
    CreateRenderPass();
    createDescriptorSetLayout();
    createTextDescriptorSetLayout();

    //CreateGraphicsPipeline("shaders/vert.spv", "shaders/frag.spv"); // general obj file pso
    CreateOutlineGraphicsPipeline("shaders/vert.spv", "shaders/frag.spv", "shaders/outline_vert.spv", "shaders/outline_frag.spv"); // outline pso
    CreateTextGraphicsPipeline("shaders/text_vert.spv", "shaders/text_frag.spv"); // text pso
    
    CreateCommandPool();
    createColorResources(); //chai31
    createDepthResources(); //chai31
    CreateFrameBuffers(); //chai31

    sceneNodesRoot = new SceneNode("Scene", nullptr);
    curSelectedFile = new wxStaticText(this, wxID_ANY, "");
    curSelectedObj = NULL;

    loadModel();
    //initUI(); //sally1046
    initText();

    createCommandBuffers(); //chai31

    renderInfo = new VulkanRenderInfo(&m_commandBuffers);

    createSyncObjects();

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // OnIdle (to call drawFrame() periodically)
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    render_loop_on = false;
    activateRenderLoop(true);
    std::cout << "VulkanCanvas::VulkanCanvas(): end" << std::endl;
}

void VulkanCanvas::activateRenderLoop(bool on) {
    if (on && !render_loop_on) {
        Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(VulkanCanvas::onIdle));
        render_loop_on = true;
    }
    else if (!on && render_loop_on) {
        Disconnect(wxEVT_IDLE, wxIdleEventHandler(VulkanCanvas::onIdle));
        render_loop_on = false;
    }
}

void VulkanCanvas::onIdle(wxIdleEvent& evt) {
    //std::cout << "VulkanCanvas::onIdle(): start" << std::endl;

    if (render_loop_on) {
        //Sleep(200);
        double deltaTime = timer.GetFrameDelta();
        //std::cout << "VulkanCanvas::onIdle(): deltaTime: " << deltaTime << std::endl;
        drawFrame(deltaTime);
        //drawPane->paintNow();
        evt.RequestMore(); // render continuously, not only once on idle
    }
    //std::cout << "VulkanCanvas::onIdle(): end" << std::endl;
}

VulkanCanvas::~VulkanCanvas() noexcept {
    if (m_instance != VK_NULL_HANDLE) {
        if (m_logicalDevice != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(m_logicalDevice);

            cleanupSwapChain();

            if (m_graphicsPipeline != VK_NULL_HANDLE) {
                vkDestroyPipeline(m_logicalDevice, m_graphicsPipeline, nullptr);
            }
            if (m_pipelineLayout != VK_NULL_HANDLE) {
                vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr);
            }
            if (m_renderPass != VK_NULL_HANDLE) {
                vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr);
            }
            if (m_swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(m_logicalDevice, m_swapchain, nullptr);
            }
            for (auto& imageView : m_swapchainImageViews) {
                vkDestroyImageView(m_logicalDevice, imageView, nullptr);
            }
            for (auto& framebuffer : m_swapchainFramebuffers) {
                vkDestroyFramebuffer(m_logicalDevice, framebuffer, nullptr);
            }
            if (m_commandPool != VK_NULL_HANDLE) {
                vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);
            }

            //vkDestroyBuffer(m_logicalDevice, vertexBuffer, nullptr); //sally19
            //vkFreeMemory(m_logicalDevice, vertexBufferMemory, nullptr); //sally19

            //sally16
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroySemaphore(m_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
                vkDestroySemaphore(m_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
                vkDestroyFence(m_logicalDevice, inFlightFences[i], nullptr);
            }
            //if (m_imageAvailableSemaphore != VK_NULL_HANDLE) {
            //    vkDestroySemaphore(m_logicalDevice, m_imageAvailableSemaphore, nullptr);
            //}
            //if (m_renderFinishedSemaphore != VK_NULL_HANDLE) {
            //    vkDestroySemaphore(m_logicalDevice, m_renderFinishedSemaphore, nullptr);
            //}
            vkDestroyDevice(m_logicalDevice, nullptr);
        }

        //sally17
        //if (enableValidationLayers) {
        //    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        //}

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }
}

void VulkanCanvas::InitializeVulkan(std::vector<const char*> requiredExtensions) {
    // make sure that the Vulkan library is available on this system
#ifdef _WIN32
    HMODULE vulkanModule = ::LoadLibraryA("vulkan-1.dll");
    if (vulkanModule == NULL) {
        throw std::runtime_error("Vulkan library is not available on this system, so program cannot run.\n"
            "You must install the appropriate Vulkan library and also have a graphics card that supports Vulkan.");
    }
#else
#error Only Win32 is currently supported. To see how to support other windowing systems, \
 see the definition of _glfw_dlopen in XXX_platform.h and its use in vulkan.c in the glfw\
 source code. XXX specifies the windowing system (e.g. x11 for X11, and wl for Wayland).
#endif
    // make sure that the correct extensions are available
    uint32_t count;
    VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
    if (err != VK_SUCCESS) {
        throw VulkanException(err, "Failed to retrieve the instance extension properties:");
    }
    std::vector<VkExtensionProperties> extensions(count);
    err = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
    if (err != VK_SUCCESS) {
        throw VulkanException(err, "Failed to retrieve the instance extension properties:");
    }
    for (int extNum = 0; extNum < extensions.size(); ++extNum) {
        for (auto& iter = requiredExtensions.begin(); iter < requiredExtensions.end(); ++iter) {
            if (std::string(*iter) == extensions[extNum].extensionName) {
                requiredExtensions.erase(iter);
                break;
            }
        }
    };
    if (!requiredExtensions.empty()) {
        std::stringstream ss;
        ss << "The following required Vulkan extensions could not be found:\n";
        for (int extNum = 0; extNum < requiredExtensions.size(); ++extNum) {
            ss << requiredExtensions[extNum] << "\n";
        }
        ss << "Program cannot continue.";
        throw std::runtime_error(ss.str());
    }

    m_vulkanInitialized = true;
}

VkApplicationInfo VulkanCanvas::CreateApplicationInfo(const std::string& appName,
    const int32_t appVersion,
    const std::string& engineName,
    const int32_t engineVersion,
    const int32_t apiVersion) const noexcept {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName.c_str();
    appInfo.applicationVersion = appVersion;
    appInfo.pEngineName = engineName.c_str();
    appInfo.engineVersion = engineVersion;
    appInfo.apiVersion = apiVersion;
    return appInfo;
}

VkInstanceCreateInfo VulkanCanvas::CreateInstanceCreateInfo(const VkApplicationInfo& appInfo,
    const std::vector<const char*>& extensionNames,
    const std::vector<const char*>& layerNames) const noexcept {
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensionNames.size();
    createInfo.ppEnabledExtensionNames = extensionNames.data();
    createInfo.enabledLayerCount = layerNames.size();
    createInfo.ppEnabledLayerNames = layerNames.data();
    return createInfo;
}

void VulkanCanvas::CreateInstance(const VkInstanceCreateInfo& createInfo) {
    if (!m_vulkanInitialized) {
        throw std::runtime_error("Programming Error:\nAttempted to create a Vulkan instance before Vulkan was initialized.");
    }
    VkResult err = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (err != VK_SUCCESS) {
        throw VulkanException(err, "Unable to create a Vulkan instance:");
    }
}

#ifdef _WIN32
VkWin32SurfaceCreateInfoKHR VulkanCanvas::CreateWin32SurfaceCreateInfo() const noexcept
{
    VkWin32SurfaceCreateInfoKHR sci = {};
    sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    sci.hwnd = GetHWND();
    sci.hinstance = GetModuleHandle(NULL);
    return sci;
}
#endif

void VulkanCanvas::CreateWindowSurface()
{
    if (!m_instance) {
        throw std::runtime_error("Programming Error:\n"
            "Attempted to create a window surface before the Vulkan instance was created.");
    }
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR sci = CreateWin32SurfaceCreateInfo();
    VkResult err = vkCreateWin32SurfaceKHR(m_instance, &sci, nullptr, &m_surface);
    if (err != VK_SUCCESS) {
        throw VulkanException(err, "Cannot create a Win32 Vulkan surface:");
    }
#else
#error The code in VulkanCanvas::CreateWindowSurface only supports Win32. Changes are \
required to support other windowing systems.
#endif
}

void VulkanCanvas::PickPhysicalDevice() {
    if (!m_instance) {
        throw std::runtime_error("Programming Error:\n"
            "Attempted to get a Vulkan physical device before the Vulkan instance was created.");
    }
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find a GPU with Vulkan support.");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
    for (const auto& device : devices) {
        if (IsDeviceSuitable(device)) {
            m_physicalDevice = device;
            msaaSamples = getMaxUsableSampleCount(); //chai30
            break;
        }
    }
    if (m_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("No physical GPU could be found with the required extensions and swap chain support.");
    }
}

bool VulkanCanvas::IsDeviceSuitable(const VkPhysicalDevice& device) const {
    QueueFamilyIndices indices = FindQueueFamilies(device);
    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return indices.IsComplete() & extensionsSupported && swapChainAdequate;
}

QueueFamilyIndices VulkanCanvas::FindQueueFamilies(const VkPhysicalDevice& device) const {
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
        if (result != VK_SUCCESS) {
            throw VulkanException(result, "Error while attempting to check if a surface supports presentation:");
        }
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
        }
        if (indices.IsComplete()) {
            break;
        }
        ++i;
    }
    return indices;
}

bool VulkanCanvas::CheckDeviceExtensionSupport(const VkPhysicalDevice& device) const {
    uint32_t extensionCount;
    VkResult result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "Cannot retrieve count of properties for a physical device:");
    }
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "Cannot retrieve properties for a physical device:");
    }
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanCanvas::QuerySwapChainSupport(const VkPhysicalDevice& device) const {
    SwapChainSupportDetails details;

    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "Unable to retrieve physical device surface capabilities:");
    }
    uint32_t formatCount = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "Unable to retrieve the number of formats for a surface on a physical device:");
    }
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
        if (result != VK_SUCCESS) {
            throw VulkanException(result, "Unable to retrieve the formats for a surface on a physical device:");
        }
    }

    uint32_t presentModeCount = 0;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "Unable to retrieve the count of present modes for a surface on a physical device:");
    }
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
        if (result != VK_SUCCESS) {
            throw VulkanException(result, "Unable to retrieve the present modes for a surface on a physical device:");
        }
    }
    return details;
}

VkDeviceQueueCreateInfo VulkanCanvas::CreateDeviceQueueCreateInfo(int queueFamily) const noexcept {
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    return queueCreateInfo;
}

std::vector<VkDeviceQueueCreateInfo> VulkanCanvas::CreateQueueCreateInfos(
    const std::set<int>& uniqueQueueFamilies) const noexcept {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for (int queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = CreateDeviceQueueCreateInfo(queueFamily);
        queueCreateInfos.push_back(queueCreateInfo);
    }
    return queueCreateInfos;
}

VkDeviceCreateInfo VulkanCanvas::CreateDeviceCreateInfo(
    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
    const VkPhysicalDeviceFeatures& deviceFeatures) const noexcept {
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    //std::cout << "VulkanCanvas::CreateDeviceCreateInfo(): enableValidationLayers: true" << std::endl;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
    return createInfo;
}

void VulkanCanvas::CreateLogicalDevice() {
    QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
    std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = CreateQueueCreateInfos(uniqueQueueFamilies);
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE; //chai30
    VkDeviceCreateInfo createInfo = CreateDeviceCreateInfo(queueCreateInfos, deviceFeatures);

    VkResult result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_logicalDevice);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "Unable to create a logical device");
    }
    vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice, indices.presentFamily, 0, &m_presentQueue);
}

void VulkanCanvas::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices_ = findQueueFamilies(m_physicalDevice);
    uint32_t queueFamilyIndices[] = { indices_.graphicsFamily, indices_.presentFamily };

    if (indices_.graphicsFamily != indices_.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(m_logicalDevice, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_logicalDevice, m_swapchain, &imageCount, nullptr);
    m_swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_logicalDevice, m_swapchain, &imageCount, m_swapchainImages.data());

    m_swapchainImageFormat = surfaceFormat.format;
    m_swapchainExtent = extent;
}

SwapChainSupportDetails VulkanCanvas::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR VulkanCanvas::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanCanvas::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanCanvas::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        //int width, height;
        //glfwGetFramebufferSize(window, &width, &height);
        wxSize size = GetSize();

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(size.GetWidth()),
            static_cast<uint32_t>(size.GetHeight())
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

QueueFamilyIndices VulkanCanvas::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.IsComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

VkSwapchainCreateInfoKHR VulkanCanvas::CreateSwapchainCreateInfo(
    const SwapChainSupportDetails& swapChainSupport,
    const VkSurfaceFormatKHR& surfaceFormat,
    uint32_t imageCount, const VkExtent2D& extent) {
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
    uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(indices.graphicsFamily),
        static_cast<uint32_t>(indices.presentFamily) };
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    return createInfo;
}

VkSurfaceFormatKHR VulkanCanvas::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) const noexcept {
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
        return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VulkanCanvas::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes) const noexcept {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanCanvas::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
    const wxSize& size) const noexcept {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D actualExtent = { static_cast<uint32_t>(size.GetWidth()),
            static_cast<uint32_t>(size.GetHeight()) };
        actualExtent.width = std::max(capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }
}

VkImageViewCreateInfo VulkanCanvas::CreateImageViewCreateInfo(uint32_t swapchainImage) const noexcept {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = m_swapchainImages[swapchainImage];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = m_swapchainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    return createInfo;
}

void VulkanCanvas::CreateImageViews() {
    m_swapchainImageViews.resize(m_swapchainImages.size());
    for (uint32_t i = 0; i < m_swapchainImages.size(); i++) {
        VkImageViewCreateInfo createInfo = CreateImageViewCreateInfo(i);

        VkResult result = vkCreateImageView(m_logicalDevice, &createInfo, nullptr, &m_swapchainImageViews[i]);
        if (result != VK_SUCCESS) {
            throw VulkanException(result, "Unable to create an image view for a swap chain image");
        }
    }
}

VkAttachmentDescription VulkanCanvas::CreateAttachmentDescription() const noexcept {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_swapchainImageFormat;
    //colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.samples = msaaSamples; //chai30
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //chai30
    return colorAttachment;
}

VkAttachmentReference VulkanCanvas::CreateAttachmentReference() const noexcept {
    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    //colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //chai30
    return colorAttachmentRef;
}

VkSubpassDescription VulkanCanvas::CreateSubpassDescription(
    const VkAttachmentReference& attachmentRef) const noexcept {
    VkSubpassDescription subPass = {};
    subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subPass.colorAttachmentCount = 1;
    subPass.pColorAttachments = &attachmentRef;
    return subPass;
}

VkSubpassDependency VulkanCanvas::CreateSubpassDependency() const noexcept {
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; //chai30
    dependency.srcAccessMask = 0; //chai30
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; //chai30
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //chai30

    return dependency;
}

VkRenderPassCreateInfo VulkanCanvas::CreateRenderPassCreateInfo(
    const VkAttachmentDescription& colorAttachment,
    const VkSubpassDescription& subPass,
    const VkSubpassDependency& dependency) const noexcept {
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subPass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    return renderPassInfo;
}

void VulkanCanvas::CreateRenderPass() {
    VkAttachmentDescription colorAttachment = CreateAttachmentDescription();

    //chai30: start
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = m_swapchainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    //chai30: end

    VkAttachmentReference colorAttachmentRef = CreateAttachmentReference();

    //chai30: start
    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //chai30: end

    VkSubpassDescription subPass = CreateSubpassDescription(colorAttachmentRef);
    subPass.pDepthStencilAttachment = &depthAttachmentRef; //chai30
    subPass.pResolveAttachments = &colorAttachmentResolveRef; //chai30

    //VkSubpassDependency dependency = CreateSubpassDependency();
    // Subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    dependencies[0].dependencyFlags = 0;

    dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].dstSubpass = 0;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].srcAccessMask = 0;
    dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    dependencies[1].dependencyFlags = 0;

    //VkRenderPassCreateInfo renderPassInfo = CreateRenderPassCreateInfo(colorAttachment,
    //    subPass, dependency);
    //chai30: start
    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subPass;
    //renderPassInfo.dependencyCount = 1;
    //renderPassInfo.pDependencies = &dependency;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    //chai30: end

    VkResult result = vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &m_renderPass);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "Failed to create a render pass:");
    }
}

//chai30: start
void VulkanCanvas::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}
//chai30: end

void VulkanCanvas::createTextDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = { samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &textDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

VkPipelineShaderStageCreateInfo VulkanCanvas::CreatePipelineShaderStageCreateInfo(
    VkShaderStageFlagBits stage, VkShaderModule& module, const char* entryName) const noexcept {
    VkPipelineShaderStageCreateInfo shaderStageInfo = {};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = stage;
    shaderStageInfo.module = module;
    shaderStageInfo.pName = entryName;
    return shaderStageInfo;
}

// NOT CALLED
VkPipelineVertexInputStateCreateInfo VulkanCanvas::CreatePipelineVertexInputStateCreateInfo() const noexcept {
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertexInputInfo.vertexBindingDescriptionCount = 0;
    //vertexInputInfo.vertexAttributeDescriptionCount = 0;

    //auto bindingDescription = Vertex::getBindingDescription();
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    //auto attributeDescriptions = Vertex::getAttributeDescriptions();
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo VulkanCanvas::CreatePipelineInputAssemblyStateCreateInfo(
    const VkPrimitiveTopology& topology, uint32_t restartEnable) const noexcept {
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = topology;
    inputAssembly.primitiveRestartEnable = restartEnable;
    return inputAssembly;
}

VkViewport VulkanCanvas::CreateViewport() const noexcept {
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapchainExtent.width;
    viewport.height = (float)m_swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}

VkRect2D VulkanCanvas::CreateScissor() const noexcept {
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapchainExtent;
    return scissor;
}

VkPipelineViewportStateCreateInfo VulkanCanvas::CreatePipelineViewportStateCreateInfo(
    const VkViewport& viewport, const VkRect2D& scissor) const noexcept {
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    return viewportState;
}

VkPipelineRasterizationStateCreateInfo VulkanCanvas::CreatePipelineRasterizationStateCreateInfo() const noexcept {
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    //rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.cullMode = VK_CULL_MODE_NONE; //ver1044
    //rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; //ver1044
    //rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    //ver1044
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    return rasterizer;
}

VkPipelineMultisampleStateCreateInfo VulkanCanvas::CreatePipelineMultisampleStateCreateInfo() const noexcept {
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    //multisampling.sampleShadingEnable = VK_FALSE;
    //multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.rasterizationSamples = msaaSamples; //chai30
    multisampling.flags = 0;
    return multisampling;
}

VkPipelineColorBlendAttachmentState VulkanCanvas::CreatePipelineColorBlendAttachmentState() const noexcept {
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    //colorBlendAttachment.blendEnable = VK_FALSE; //sally ver1132 to render transparent meshes
     //ver1044 start
    colorBlendAttachment.blendEnable = VK_TRUE; //sally ver1132 to render transparent meshes
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; //sally ver1132 to render transparent meshes
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; //sally ver1132 to render transparent meshes
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    //ver1044 end
    return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo VulkanCanvas::CreatePipelineColorBlendStateCreateInfo(
    const VkPipelineColorBlendAttachmentState& colorBlendAttachment) const noexcept {
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    //colorBlending.logicOpEnable = VK_FALSE;
    //colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    //colorBlending.blendConstants[0] = 0.0f;
    //colorBlending.blendConstants[1] = 0.0f;
    //colorBlending.blendConstants[2] = 0.0f;
    //colorBlending.blendConstants[3] = 0.0f;

    //ver1044
    //colorBlending.blendConstants[0] = 1.0f;
    //colorBlending.blendConstants[1] = 1.0f;
    //colorBlending.blendConstants[2] = 1.0f;
    //colorBlending.blendConstants[3] = 1.0f;
    return colorBlending;
}

VkPipelineLayoutCreateInfo VulkanCanvas::CreatePipelineLayoutCreateInfo() const noexcept {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //pipelineLayoutInfo.setLayoutCount = 0;
    //pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.setLayoutCount = 1; //chai30
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; //chai30
    return pipelineLayoutInfo;
}

VkPipelineLayoutCreateInfo VulkanCanvas::CreateTextPipelineLayoutCreateInfo() const noexcept {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //pipelineLayoutInfo.setLayoutCount = 0;
    //pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.setLayoutCount = 1; //chai30
    pipelineLayoutInfo.pSetLayouts = &textDescriptorSetLayout; //chai30
    return pipelineLayoutInfo;
}

VkGraphicsPipelineCreateInfo VulkanCanvas::CreateGraphicsPipelineCreateInfo(
    const VkPipelineShaderStageCreateInfo shaderStages[],
    const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
    const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
    const VkPipelineViewportStateCreateInfo& viewportState,
    const VkPipelineRasterizationStateCreateInfo& rasterizer,
    const VkPipelineMultisampleStateCreateInfo& multisampling,
    const VkPipelineDepthStencilStateCreateInfo& depthStencil,
    const VkPipelineColorBlendStateCreateInfo& colorBlending) const noexcept {
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil; //chai30
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    return pipelineInfo;
}

VkGraphicsPipelineCreateInfo VulkanCanvas::CreateBoundaryGraphicsPipelineCreateInfo( // NOT CURRENTLY USED
    const VkPipelineShaderStageCreateInfo shaderStages[],
    const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
    const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
    const VkPipelineViewportStateCreateInfo& viewportState,
    const VkPipelineRasterizationStateCreateInfo& rasterizer,
    const VkPipelineMultisampleStateCreateInfo& multisampling,
    const VkPipelineDepthStencilStateCreateInfo& depthStencil,
    const VkPipelineColorBlendStateCreateInfo& colorBlending) const noexcept {
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
//    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//    pipelineInfo.stageCount = 2;
//    //pipelineInfo.stageCount = 3;
//    pipelineInfo.pStages = shaderStages;
//    pipelineInfo.pVertexInputState = &vertexInputInfo;
//    pipelineInfo.pInputAssemblyState = &inputAssembly;
//    pipelineInfo.pViewportState = &viewportState;
//    pipelineInfo.pRasterizationState = &rasterizer;
//    pipelineInfo.pMultisampleState = &multisampling;
//    pipelineInfo.pDepthStencilState = &depthStencil; //chai30
//    pipelineInfo.pColorBlendState = &colorBlending;
//    pipelineInfo.layout = boundaryPipelineLayout;
//    pipelineInfo.renderPass = m_renderPass;
//    pipelineInfo.subpass = 0;
//    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    return pipelineInfo;
}

VkGraphicsPipelineCreateInfo VulkanCanvas::CreateTextGraphicsPipelineCreateInfo(
    const VkPipelineShaderStageCreateInfo shaderStages[],
    const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
    const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
    const VkPipelineViewportStateCreateInfo& viewportState,
    const VkPipelineRasterizationStateCreateInfo& rasterizer,
    const VkPipelineMultisampleStateCreateInfo& multisampling,
    const VkPipelineDepthStencilStateCreateInfo& depthStencil,
    const VkPipelineColorBlendStateCreateInfo& colorBlending) const noexcept {
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil; //chai30
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = textPipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    return pipelineInfo;
}

void VulkanCanvas::CreateGraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
    auto vertShaderCode = ReadFile(vertexShaderFile);
    auto fragShaderCode = ReadFile(fragmentShaderFile);

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    CreateShaderModule(vertShaderCode, vertShaderModule);
    CreateShaderModule(fragShaderCode, fragShaderModule);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = CreatePipelineShaderStageCreateInfo(
        VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule, "main");
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = CreatePipelineShaderStageCreateInfo(
        VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule, "main");
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    //VkPipelineVertexInputStateCreateInfo vertexInputInfo = CreatePipelineVertexInputStateCreateInfo();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertexInputInfo.vertexBindingDescriptionCount = 0;
    //vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription(); // ver1124

    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = CreatePipelineInputAssemblyStateCreateInfo(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
    VkViewport viewport = CreateViewport();
    VkRect2D scissor = CreateScissor();
    VkPipelineViewportStateCreateInfo viewportState = CreatePipelineViewportStateCreateInfo(
        viewport, scissor);
    VkPipelineRasterizationStateCreateInfo rasterizer = CreatePipelineRasterizationStateCreateInfo();
    VkPipelineMultisampleStateCreateInfo multisampling = CreatePipelineMultisampleStateCreateInfo();

    //chai30: start
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    //chai30: end

    VkPipelineColorBlendAttachmentState colorBlendAttachment = CreatePipelineColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo colorBlending = CreatePipelineColorBlendStateCreateInfo(
        colorBlendAttachment);
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = CreatePipelineLayoutCreateInfo();

    VkResult result = vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "VulkanCanvas::CreateGraphicsPipeline(): Failed to create pipeline layout:");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = CreateGraphicsPipelineCreateInfo(shaderStages,
        vertexInputInfo, inputAssembly, viewportState, rasterizer, multisampling, depthStencil, colorBlending);
    std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    pipelineInfo.pDynamicState = &dynamicState;

    result = vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
    // vkDestroyShaderModule calls below must be placed before possible throw of exception
    vkDestroyShaderModule(m_logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_logicalDevice, vertShaderModule, nullptr);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "VulkanCanvas::CreateGraphicsPipeline(): Failed to create graphics pipeline:");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CreateOutlineGraphicsPipeline()
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VulkanCanvas::CreateOutlineGraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile, const std::string& outlineVertexShaderFile, const std::string& outlineFragmentShaderFile) {
    auto vertShaderCode = ReadFile(vertexShaderFile);
    auto fragShaderCode = ReadFile(fragmentShaderFile);

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    CreateShaderModule(vertShaderCode, vertShaderModule);
    CreateShaderModule(fragShaderCode, fragShaderModule);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = CreatePipelineShaderStageCreateInfo(
        VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule, "main");
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = CreatePipelineShaderStageCreateInfo(
        VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule, "main");
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    //VkPipelineVertexInputStateCreateInfo vertexInputInfo = CreatePipelineVertexInputStateCreateInfo();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertexInputInfo.vertexBindingDescriptionCount = 0;
    //vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription(); // ver1124

    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = CreatePipelineInputAssemblyStateCreateInfo(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
    VkViewport viewport = CreateViewport();
    VkRect2D scissor = CreateScissor();
    VkPipelineViewportStateCreateInfo viewportState = CreatePipelineViewportStateCreateInfo(
        viewport, scissor);
    VkPipelineRasterizationStateCreateInfo rasterizer = CreatePipelineRasterizationStateCreateInfo();
    VkPipelineMultisampleStateCreateInfo multisampling = CreatePipelineMultisampleStateCreateInfo();

    //chai30: start
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    //depthStencil.depthBoundsTestEnable = VK_FALSE;
    //depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.back.compareOp = VK_COMPARE_OP_ALWAYS;

    depthStencil.stencilTestEnable = VK_TRUE;
    depthStencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencil.back.failOp = VK_STENCIL_OP_REPLACE;
    depthStencil.back.depthFailOp = VK_STENCIL_OP_REPLACE;
    depthStencil.back.passOp = VK_STENCIL_OP_REPLACE;
    depthStencil.back.compareMask = 0xff;
    depthStencil.back.writeMask = 0xff;
    depthStencil.back.reference = 1;
    depthStencil.front = depthStencil.back;
    //chai30: end

    VkPipelineColorBlendAttachmentState colorBlendAttachment = CreatePipelineColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo colorBlending = CreatePipelineColorBlendStateCreateInfo(
        colorBlendAttachment);
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = CreatePipelineLayoutCreateInfo();

    VkResult result = vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "VulkanCanvas::CreateGraphicsPipeline(): Failed to create pipeline layout:");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = CreateGraphicsPipelineCreateInfo(shaderStages,
        vertexInputInfo, inputAssembly, viewportState, rasterizer, multisampling, depthStencil, colorBlending);
    std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    pipelineInfo.pDynamicState = &dynamicState;

    result = vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
    // vkDestroyShaderModule calls below must be placed before possible throw of exception
    vkDestroyShaderModule(m_logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_logicalDevice, vertShaderModule, nullptr);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "VulkanCanvas::CreateGraphicsPipeline(): Failed to create graphics pipeline:");
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // for outline
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    depthStencil.back.compareOp = VK_COMPARE_OP_NOT_EQUAL;
    depthStencil.back.failOp = VK_STENCIL_OP_KEEP;
    depthStencil.back.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencil.back.passOp = VK_STENCIL_OP_REPLACE;
    depthStencil.front = depthStencil.back;
    depthStencil.depthTestEnable = VK_FALSE;

    auto outlineVertShaderCode = ReadFile(outlineVertexShaderFile);
    auto outlineFragShaderCode = ReadFile(outlineFragmentShaderFile);

    VkShaderModule outlineVertShaderModule;
    VkShaderModule outlineFragShaderModule;

    CreateShaderModule(outlineVertShaderCode, outlineVertShaderModule);
    CreateShaderModule(outlineFragShaderCode, outlineFragShaderModule);

    VkPipelineShaderStageCreateInfo outlineVertShaderStageInfo = CreatePipelineShaderStageCreateInfo(
        VK_SHADER_STAGE_VERTEX_BIT, outlineVertShaderModule, "main");
    VkPipelineShaderStageCreateInfo outlineFragShaderStageInfo = CreatePipelineShaderStageCreateInfo(
        VK_SHADER_STAGE_FRAGMENT_BIT, outlineFragShaderModule, "main");
    VkPipelineShaderStageCreateInfo outlineShaderStages[] = { outlineVertShaderStageInfo, outlineFragShaderStageInfo };

    VkGraphicsPipelineCreateInfo outlinePipelineInfo = CreateGraphicsPipelineCreateInfo(outlineShaderStages,
        vertexInputInfo, inputAssembly, viewportState, rasterizer, multisampling, depthStencil, colorBlending);

    result = vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &outlinePipelineInfo, nullptr, &outlinePSO);
    // vkDestroyShaderModule calls below must be placed before possible throw of exception
    vkDestroyShaderModule(m_logicalDevice, outlineFragShaderModule, nullptr);
    vkDestroyShaderModule(m_logicalDevice, outlineVertShaderModule, nullptr);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "VulkanCanvas::CreateGraphicsPipeline(): Failed to create graphics pipeline:");
    }

}

/**
* Reads in given file. Used to read shader files in CreateGraphicsPipeline().
* @param filename the name of the shader file to read
*/
std::vector<char> VulkanCanvas::ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::stringstream ss;
        ss << "VulkanCanvas::ReadFile(): Failed to open file: " << filename;
        throw std::runtime_error(ss.str().c_str());
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModuleCreateInfo VulkanCanvas::CreateShaderModuleCreateInfo(
    const std::vector<char>& code) const noexcept {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = (uint32_t*)code.data();
    return createInfo;
}

void VulkanCanvas::CreateShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule) const {
    VkShaderModuleCreateInfo createInfo = CreateShaderModuleCreateInfo(code);

    VkResult result = vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "VulkanCanvas::CreateShaderModule(): Failed to create shader module:");
    }
}

VkFramebufferCreateInfo VulkanCanvas::CreateFramebufferCreateInfo(
    const VkImageView& attachments) const noexcept {
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderPass;
    //framebufferInfo.attachmentCount = 1;
    framebufferInfo.attachmentCount = 3;
    framebufferInfo.pAttachments = &attachments;
    framebufferInfo.width = m_swapchainExtent.width;
    framebufferInfo.height = m_swapchainExtent.height;
    framebufferInfo.layers = 1;
    return framebufferInfo;
}

void VulkanCanvas::CreateFrameBuffers() {
    VkFramebuffer framebuffer;
    m_swapchainFramebuffers.resize(m_swapchainImageViews.size(), framebuffer);

    for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
        VkImageView attachments[] = {
            colorImageView, //chai30
            depthImageView, //chai30
            m_swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo = CreateFramebufferCreateInfo(*attachments);

        VkResult result = vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &m_swapchainFramebuffers[i]);
        if (result != VK_SUCCESS) {
            throw VulkanException(result, "VulkanCanvas::CreateFrameBuffers(): Failed to create framebuffer:");
        }
    }
}

VkCommandPoolCreateInfo VulkanCanvas::CreateCommandPoolCreateInfo(
    QueueFamilyIndices& queueFamilyIndices) const noexcept {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; //chai30
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    return poolInfo;
}

void VulkanCanvas::CreateCommandPool() {
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_physicalDevice);
    VkCommandPoolCreateInfo poolInfo = CreateCommandPoolCreateInfo(queueFamilyIndices);
    VkResult result = vkCreateCommandPool(m_logicalDevice, &poolInfo, nullptr, &m_commandPool);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "VulkanCanvas::CreateCommandPool(): Failed to create command pool:");
    }
}

VkCommandBufferBeginInfo VulkanCanvas::CreateCommandBufferBeginInfo() const noexcept {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    return beginInfo;
}

VkRenderPassBeginInfo VulkanCanvas::CreateRenderPassBeginInfo(size_t swapchainBufferNumber,
    const VkClearValue& clearValue) const noexcept {
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_swapchainFramebuffers[swapchainBufferNumber];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_swapchainExtent;

    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;
    return renderPassInfo;
}

//==============================================================================
//==============================================================================
//==============================================================================
//chai31:start
//==============================================================================
//==============================================================================
//==============================================================================

void VulkanCanvas::createColorResources() {
    VkFormat colorFormat = m_swapchainImageFormat;

    createImage(m_swapchainExtent.width, m_swapchainExtent.height, 1, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
    colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void VulkanCanvas::createDepthResources() {
    VkFormat depthFormat = findDepthFormat();

    createImage(m_swapchainExtent.width, m_swapchainExtent.height, 1, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
    depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

VkFormat VulkanCanvas::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("VulkanCanvas::findSupportedFormat(): failed to find supported format!");
}

VkFormat VulkanCanvas::findDepthFormat() {
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool VulkanCanvas::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanCanvas::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_physicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("VulkanCanvas::generateMipmaps(): texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

VkSampleCountFlagBits VulkanCanvas::getMaxUsableSampleCount() {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

VkImageView VulkanCanvas::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(m_logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createImageView(): failed to create texture image view!");
    }

    return imageView;
}

void VulkanCanvas::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(m_logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createImage(): failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_logicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createImage(): failed to allocate image memory!");
    }

    vkBindImageMemory(m_logicalDevice, image, imageMemory, 0);
}

void VulkanCanvas::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("VulkanCanvas::transitionImageLayout(): unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void VulkanCanvas::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

void VulkanCanvas::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createBuffer(): failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createBuffer(): failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_logicalDevice, buffer, bufferMemory, 0);
}

VkCommandBuffer VulkanCanvas::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanCanvas::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBuffer);
}

void VulkanCanvas::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

uint32_t VulkanCanvas::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("VulkanCanvas::copyBuffer(): failed to find suitable memory type!");
}

void VulkanCanvas::createCommandBuffers() {
    m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

    if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void VulkanCanvas::recordCommandBuffer(double deltaTime, VulkanRenderInfo *renderInfo, uint32_t imageIndex, uint32_t currentFrame) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer((*renderInfo->commandBuffers)[currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::recordCommandBuffer(): failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_swapchainExtent;

    std::array<VkClearValue, 2> clearValues{};
    // CHANGE BACKGROUND
    //clearValues[0].color = { {1.0f, 0.0f, 0.0f, 1.0f} }; // red
    //clearValues[0].color = { {0.0f, 0.0f, 1.0f, 1.0f} }; // blue
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; // black
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass((*renderInfo->commandBuffers)[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapchainExtent.width;
    viewport.height = (float)m_swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport((*renderInfo->commandBuffers)[currentFrame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapchainExtent;
    vkCmdSetScissor((*renderInfo->commandBuffers)[currentFrame], 0, 1, &scissor);


    //==========================================================================
    // Draw game objects
    //==========================================================================

    sceneNodesRoot->update(deltaTime);
    sceneNodesRoot->render(deltaTime, renderInfo);
    sceneNodesRoot->renderBB(deltaTime, renderInfo); // render the BB last since it is transparent and will cover the other objects behind it


    ////==========================================================================
    //// Update/Draw text / UI (under construction)
    ////==========================================================================
    //if (DRAW_TEXT_ON) {
    //    RenderText((*renderInfo->commandBuffers)[currentFrame], arialFont, "FPS: " + timer.fps, glm::vec2(0.02f, 0.01f), glm::vec2(2.0f, 2.0f));
    //}
    //////RenderText(commandBuffer, arialFont, "@ : " + timer.fps, glm::vec2(0.02f, 0.01f), glm::vec2(2.0f, 2.0f)); // test use
    //////==========================================================================


    //// First pass renders object (toon shaded) and fills stencil buffer
    //vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.stencil);
    //model.draw(drawCmdBuffers[i]);

    //// Second pass renders scaled object only where stencil was not set by first pass
    //vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.outline);
    //model.draw(drawCmdBuffers[i]);

    vkCmdEndRenderPass((*renderInfo->commandBuffers)[currentFrame]);

    if (vkEndCommandBuffer((*renderInfo->commandBuffers)[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::recordCommandBuffer(): failed to record command buffer!");
    }
}

void VulkanCanvas::createSyncObjects() {
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("VulkanCanvas::createSyncObjects(): failed to create synchronization objects for a frame!");
        }
    }
}

//==============================================================================
//==============================================================================
//==============================================================================
//chai30: end
//==============================================================================
//==============================================================================
//==============================================================================

void VulkanCanvas::RecreateSwapchain() {
    vkDeviceWaitIdle(m_logicalDevice);

    cleanupSwapChain();

    wxSize size = GetSize();
    //CreateSwapChain(size);
    createSwapChain();
    CreateImageViews();
    //CreateRenderPass();
    //CreateGraphicsPipeline("vert.spv", "frag.spv");
    //CreateGraphicsPipeline("shaders/vert.spv", "shaders/frag.spv");
    createColorResources();
    createDepthResources();
    CreateFrameBuffers();
    //CreateCommandBuffers();

    //sally17
}

void VulkanCanvas::drawFrame(double deltaTime) {
    if (!vulkanInitDone) {
        return;
    }

    //updateUI(); //sally_ver1046

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_logicalDevice, m_swapchain, UINT64_MAX, m_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("VulkanCanvas::drawFrame(): failed to acquire swap chain image!");
    }

    //updateUniformBuffer(currentFrame); // moved to recordCommandBuffer()
    updateCamera(deltaTime);

    vkResetFences(m_logicalDevice, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(m_commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    //recordCommandBuffer(m_commandBuffers[currentFrame], imageIndex);
    recordCommandBuffer(deltaTime, renderInfo, imageIndex, currentFrame);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::drawFrame(): failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        RecreateSwapchain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::drawFrame(): failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
//chai30: end

void VulkanCanvas::cleanupSwapChain() {
    for (auto framebuffer : m_swapchainFramebuffers) {
        vkDestroyFramebuffer(m_logicalDevice, framebuffer, nullptr);
    }

    for (auto imageView : m_swapchainImageViews) {
        vkDestroyImageView(m_logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(m_logicalDevice, m_swapchain, nullptr);
}

VkPresentInfoKHR VulkanCanvas::CreatePresentInfoKHR(uint32_t& imageIndex) const noexcept {
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_renderFinishedSemaphores[imageIndex];

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;

    presentInfo.pImageIndices = &imageIndex;
    return presentInfo;
}

void VulkanCanvas::OnPaint(wxPaintEvent& event) {
    //std::cout << "VulkanCanvas::OnPaint(): start" << std::endl;

    //drawFrame();
    //  try {
    //      //vkWaitForFences(m_logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX); //sally15
    //      //(m_logicalDevice, 1, &inFlightFence); //sally15

    //      std::cout << "VulkanCanvas::OnPaint(): hello1" << std::endl;
     
    //      uint32_t imageIndex;
    //      VkResult result = vkAcquireNextImageKHR(m_logicalDevice, m_swapchain,
    //          std::numeric_limits<uint64_t>::max(), m_imageAvailableSemaphores[imageIndex], VK_NULL_HANDLE, &imageIndex);

    //      if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    //          RecreateSwapchain();
    //          return;
    //      }
    //      else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    //          throw VulkanException(result, "Failed to acquire swap chain image");
    //      }
          //VkPipelineStageFlags waitFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    //      VkSubmitInfo submitInfo = CreateSubmitInfo(imageIndex, waitFlags);
    //      result = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    //      if (result != VK_SUCCESS) {
    //          throw VulkanException(result, "Failed to submit draw command buffer:");
    //      }

    //      VkPresentInfoKHR presentInfo = CreatePresentInfoKHR(imageIndex);
    //      result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
    //      if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    //          RecreateSwapchain();
    //      }
    //      else if (result != VK_SUCCESS) {
    //          throw VulkanException(result, "Failed to present swap chain image:");
    //      }
    //  }
    //  catch (const VulkanException& ve) {
    //      std::string status = ve.GetStatus();
    //      std::stringstream ss;
    //      ss << ve.what() << "\n" << status;
    //      CallAfter(&VulkanCanvas::OnPaintException, ss.str());
    //  }
    //  catch (const std::exception& err) {
    //      std::stringstream ss;
    //      ss << "Error encountered trying to create the Vulkan canvas:\n";
    //      ss << err.what();
    //      CallAfter(&VulkanCanvas::OnPaintException, ss.str());
    //  }

    //std::cout << "VulkanCanvas::OnPaint(): end" << std::endl;
}

void VulkanCanvas::OnResize(wxSizeEvent& event) {
    wxSize size = GetSize();
    if (size.GetWidth() == 0 || size.GetHeight() == 0) {
        return;
    }
    RecreateSwapchain();
    wxRect refreshRect(size);
    RefreshRect(refreshRect, false);
}

void VulkanCanvas::OnPaintException(const std::string& msg) {
    wxMessageBox(msg, "VulkanCanvas::OnResize(): Vulkan Error");
    wxTheApp->ExitMainLoop();
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// sally
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
* Loads and prepares the 3D objects for rendering.
* The object files to load are stored in objectFilenames.
*/
void VulkanCanvas::loadModel() {
    //==========================================================================
    //==========================================================================
    //==========================================================================
    // obj
    //==========================================================================
    //==========================================================================
    //==========================================================================
    
    if (LOAD_OBJ_ON) {
        for (int i = 0; i < objectFilenames.size(); i++) {
            VulkanObjLoader *objFile = new VulkanObjLoader();
            objFile->load(objectFilenames[i], objectPosition[i], objectRotation[i]); // load obj file
            objLoaders.push_back(objFile);

            for (int j = 0; j < objFile->myObjsArr.size(); j++) {
                SceneNode *curSceneNode = new SceneNode(objFile->myObjsArr[j]->getObjName(), objFile->myObjsArr[j]);
                curSceneNode->setParent(sceneNodesRoot);
                sceneNodesRoot->addChild(curSceneNode);

                if (!DRAW_BB_ON) {
                    objFile->myObjsArr[j]->setDrawBB(false);
                }
            }
        }
    }
    //objLoaders[0]->myObjsArr[0]->drawBB = false;

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // md5
    //==========================================================================
    //==========================================================================
    //==========================================================================
    if (LOAD_MD5_ON) {
        for (int i = 0; i < md5MeshFilenames.size(); i++) {
            VulkanMD5Model* md5File = new VulkanMD5Model();
            md5File->LoadModel(md5MeshFilenames[i]);
            md5File->LoadAnim(md5AnimFilenames[i]);
            vulkanMD5Models.push_back(md5File);

            SceneNode* curSceneNode = new SceneNode(objectFilenames[i], md5File);
            //sceneNodes.push_back(curSceneNode);
            curSceneNode->setParent(sceneNodesRoot);
            sceneNodesRoot->addChild(curSceneNode);
        }
    }
}

// all drag & dropped objects' positions will be (0.0f,0.0f,0.0f)
void VulkanCanvas::loadModel(std::string modelPath, std::string animPath, bool hasAnim) {
    wxFileName *fn = new wxFileName(modelPath);

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // obj
    //==========================================================================
    //==========================================================================
    //==========================================================================
    if (LOAD_OBJ_ON && fn->GetExt() == "obj") {
        VulkanObjLoader *objFile = new VulkanObjLoader();
        std::cout << "VulkanCanvas::loadModel(...): modelPath: " << modelPath << std::endl;
        objFile->load(modelPath, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // load obj file
        objLoaders.push_back(objFile);

        for (int j = 0; j < objFile->myObjsArr.size(); j++) {
            //SceneNode *curSceneNode = new SceneNode(modelPath, objFile->myObjsArr[j]);
            SceneNode *curSceneNode = new SceneNode(objFile->myObjsArr[j]->getObjName(), objFile->myObjsArr[j]);
            //sceneNodes.push_back(curSceneNode);
            curSceneNode->setParent(sceneNodesRoot);
            sceneNodesRoot->addChild(curSceneNode);
            if (modelPath == "C:\\Users\\sally\\Downloads\\season2_p2\\wxWidgets-3.2.1\\samples\\minimal_vulkan\\models\\axis_and_grid_for_opengl_with_axis_labels.obj" ||
                modelPath == "C:\\Users\\sally\\Downloads\\season2_p2\\wxWidgets-3.2.1\\samples\\minimal_vulkan\\models\\axis_and_grid_for_vulkan.obj" ||
                modelPath == "C:\\Users\\sally\\Downloads\\season2_p2\\wxWidgets-3.2.1\\samples\\minimal_vulkan\\models\\axis_and_grid.obj") { // grid and axis
                std::cout << "VulkanCanvas::loadModel(...): AXIS" << std::endl;
                objFile->myObjsArr[j]->setDrawBB(false);
            }
        }
    }
    //==========================================================================
    //==========================================================================
    //==========================================================================
    // md5
    //==========================================================================
    //==========================================================================
    //==========================================================================
    if (LOAD_MD5_ON && fn->GetExt() == "md5mesh") {
        VulkanMD5Model* md5File = new VulkanMD5Model();
        md5File->LoadModel(modelPath);
        if (hasAnim && animPath != "") { //??????????????????????????????????????????????????????????
            md5File->LoadAnim(animPath);
        }
        vulkanMD5Models.push_back(md5File);

        SceneNode* curSceneNode = new SceneNode(modelPath, md5File);
        //sceneNodes.push_back(curSceneNode);
        curSceneNode->setParent(sceneNodesRoot);
        sceneNodesRoot->addChild(curSceneNode);
    }
}

void VulkanCanvas::createVertexBufferAndIndexBuffer(std::vector<Vertex> vertices, std::vector<MyFace> myFaces, VkBuffer* pVertexBuffer,
    VkBuffer* pIndexBuffer, VkDeviceMemory* pVertexBufferMemory, VkDeviceMemory* pIndexBufferMemory) { // called by VulkanObjLoader::createVertexBufferAndIndexBuffer()
    //--------------------------------------------------------------------------
    // create vertex buffer
    //--------------------------------------------------------------------------
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer vertexStagingBuffer;
    VkDeviceMemory vertexStagingBufferMemory;
    //cout << "VulkanCanvas::createVertexBufferAndIndexBuffer: m_logicalDevice: " << m_logicalDevice << endl;
    createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexStagingBuffer, vertexStagingBufferMemory);

    void* vertexData;

    vkMapMemory(m_logicalDevice, vertexStagingBufferMemory, 0, vertexBufferSize, 0, &vertexData);
    memcpy(vertexData, vertices.data(), (size_t)vertexBufferSize);
    vkUnmapMemory(m_logicalDevice, vertexStagingBufferMemory);

    createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *pVertexBuffer, *pVertexBufferMemory);

    //VkBuffer vertexBuffer;
    copyBuffer(vertexStagingBuffer, *pVertexBuffer, vertexBufferSize);
    //*pVertexBuffer = vertexBuffer;

    vkDestroyBuffer(m_logicalDevice, vertexStagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, vertexStagingBufferMemory, nullptr);

    //--------------------------------------------------------------------------
    // create index buffer
    //--------------------------------------------------------------------------
    VkDeviceSize indexBufferSize = sizeof(struct MyFace) * myFaces.size();
    //std::cout << "VulkanCanvas::createVertexBufferAndIndexBuffer(): indexBufferSize = " << indexBufferSize << std::endl;

    VkBuffer indexStagingBuffer;
    VkDeviceMemory indexStagingBufferMemory;
    createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexStagingBuffer, indexStagingBufferMemory);

    void* indexData;
    vkMapMemory(m_logicalDevice, indexStagingBufferMemory, 0, indexBufferSize, 0, &indexData);
    memcpy(indexData, myFaces.data(), (size_t)indexBufferSize);
    vkUnmapMemory(m_logicalDevice, indexStagingBufferMemory);

    createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *pIndexBuffer, *pIndexBufferMemory);

    copyBuffer(indexStagingBuffer, *pIndexBuffer, indexBufferSize);

    vkDestroyBuffer(m_logicalDevice, indexStagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, indexStagingBufferMemory, nullptr);
}

// called by BBMyObj::update()
void VulkanCanvas::updateIndexBuffer(std::vector<MyFace> myFaces, VkBuffer* pIndexBuffer, VkDeviceMemory* pIndexBufferMemory) {

    VkDeviceSize indexBufferSize = sizeof(struct MyFace) * myFaces.size();
    //std::cout << "VulkanCanvas::updateIndexBuffer(): indexBufferSize = " << indexBufferSize << std::endl;

    VkBuffer indexStagingBuffer;
    VkDeviceMemory indexStagingBufferMemory;
    createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexStagingBuffer, indexStagingBufferMemory);

    void* indexData;
    vkMapMemory(m_logicalDevice, indexStagingBufferMemory, 0, indexBufferSize, 0, &indexData);
    memcpy(indexData, myFaces.data(), (size_t)indexBufferSize);
    vkUnmapMemory(m_logicalDevice, indexStagingBufferMemory);

    createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *pIndexBuffer, *pIndexBufferMemory);

    copyBuffer(indexStagingBuffer, *pIndexBuffer, indexBufferSize);

    vkDestroyBuffer(m_logicalDevice, indexStagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, indexStagingBufferMemory, nullptr);
}

// original function: createVertexBufferAndIndexBuffer
//    - called in init stage
//    - (task 1)creates vertex buffer and (task 2)fills vertex data
// this new function will be called in the init stage
//    - (task 1)only creates the buffer with no contents
//    - text vertex data will be filled in real time in 
//void VulkanCanvas::createTextVertexBuffer(std::vector<TextVertex> textVertices, VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory) {
void VulkanCanvas::createTextVertexBuffer(VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory) {
    //std::cout << "VulkanCanvas::createTextVertexBuffer(): start" << std::endl;
    VkDeviceSize vertexBufferSize = maxNumTextCharacters * sizeof(TextVertex);
    createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *pTextVertexBuffer, *pTextVertexBufferMemory);
}

// this new function will be called in RenderText
//  	- called in real time
//      - fills new text vertex data
//int countFillTextVertexBufferCall = 0;
void VulkanCanvas::fillTextVertexBuffer(std::vector<TextVertex> textVertices, VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory) {
    VkDeviceSize vertexBufferSize = maxNumTextCharacters * sizeof(TextVertex);

    VkBuffer vertexStagingBuffer;
    VkDeviceMemory vertexStagingBufferMemory;
    createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexStagingBuffer, vertexStagingBufferMemory);

    void* vertexData;

    vkMapMemory(m_logicalDevice, vertexStagingBufferMemory, 0, vertexBufferSize, 0, &vertexData);
    memcpy(vertexData, textVertices.data(), (size_t)(textVertices.size() * sizeof(TextVertex)));
    vkUnmapMemory(m_logicalDevice, vertexStagingBufferMemory);

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // VulkanCanvas::createTextVertexBuffer()
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // VkDeviceSize vertexBufferSize = maxNumTextCharacters * sizeof(TextVertex);
    // createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *pTextVertexBuffer, *pTextVertexBufferMemory);
    //
    // maxNumTextCharacters(1024) size buffer created already(in the above two lines). Only created once and reused here.
    // So, the line below is turned off.
    //
    // the two parameters, pTextVertexBuffer and pTextVertexBufferMemory, keeps the address.
    //--------------------------------------------------------------------------
    //createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *pTextVertexBuffer, *pTextVertexBufferMemory);
    //--------------------------------------------------------------------------

    copyBuffer(vertexStagingBuffer, *pTextVertexBuffer, vertexBufferSize);

    vkDestroyBuffer(m_logicalDevice, vertexStagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, vertexStagingBufferMemory, nullptr);

    //countFillTextVertexBufferCall++;
}

// original function: createVertexBufferAndIndexBuffer
//    - called in init stage
//    - (task 1)creates vertex buffer and (task 2)fills vertex data
// this new function will be called in the init stage and in MD5Model::Update()
void VulkanCanvas::createMD5VertexBufferAndIndexBuffer(std::vector<Vertex> vertices, std::vector<MyFace> indices, VkBuffer* pVertexBuffer, VkBuffer* pIndexBuffer,
                                                       VkDeviceMemory* pVertexBufferMemory, VkDeviceMemory* pIndexBufferMemory) {
    std::cout << "VulkanCanvas::createMD5VertexBufferAndIndexBuffer(): start" << std::endl;

    //--------------------------------------------------------------------------
    // create vertex buffer
    //--------------------------------------------------------------------------
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
    std::cout << "VulkanCanvas::createMD5VertexBufferAndIndexBuffer(): vertexBufferSize = " << sizeof(vertices[0]) * vertices.size() << std::endl;
    VkBuffer vertexStagingBuffer;
    VkDeviceMemory vertexStagingBufferMemory;
    createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexStagingBuffer, vertexStagingBufferMemory);

    void* vertexData;

    vkMapMemory(m_logicalDevice, vertexStagingBufferMemory, 0, vertexBufferSize, 0, &vertexData);
    memcpy(vertexData, vertices.data(), (size_t)vertexBufferSize);
    vkUnmapMemory(m_logicalDevice, vertexStagingBufferMemory);

    createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *pVertexBuffer, *pVertexBufferMemory);

    copyBuffer(vertexStagingBuffer, *pVertexBuffer, vertexBufferSize);

    vkDestroyBuffer(m_logicalDevice, vertexStagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, vertexStagingBufferMemory, nullptr);

    //--------------------------------------------------------------------------
    // create index buffer
    //--------------------------------------------------------------------------
    VkDeviceSize indexBufferSize = sizeof(struct MyFace) * indices.size();
    std::cout << "VulkanCanvas::createMD5VertexBufferAndIndexBuffer(): indexBufferSize = " << indexBufferSize << std::endl;

    VkBuffer indexStagingBuffer;
    VkDeviceMemory indexStagingBufferMemory;
    createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexStagingBuffer, indexStagingBufferMemory);

    void* indexData;
    vkMapMemory(m_logicalDevice, indexStagingBufferMemory, 0, indexBufferSize, 0, &indexData);
    memcpy(indexData, indices.data(), (size_t)indexBufferSize);
    vkUnmapMemory(m_logicalDevice, indexStagingBufferMemory);

    createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *pIndexBuffer, *pIndexBufferMemory);

    copyBuffer(indexStagingBuffer, *pIndexBuffer, indexBufferSize);

    vkDestroyBuffer(m_logicalDevice, indexStagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, indexStagingBufferMemory, nullptr);

    std::cout << "VulkanCanvas::createMD5VertexBufferAndIndexBuffer(): end" << std::endl;
}

// this new function will be called in RenderText
//  	- called in real time
//      - fills new text vertex data
void VulkanCanvas::fillMD5VertexBufferAndIndexBuffer(std::vector<Vertex> vertices, std::vector<MyFace> indices, VkBuffer* pVertexBuffer, VkBuffer* pIndexBuffer,
                                                     VkDeviceMemory* pVertexBufferMemory, VkDeviceMemory* pIndexBufferMemory) {
    //--------------------------------------------------------------------------
    // fill vertex buffer
    //--------------------------------------------------------------------------
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer vertexStagingBuffer;
    VkDeviceMemory vertexStagingBufferMemory;
    createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexStagingBuffer, vertexStagingBufferMemory);

    void* vertexData;

    vkMapMemory(m_logicalDevice, vertexStagingBufferMemory, 0, vertexBufferSize, 0, &vertexData);
    memcpy(vertexData, vertices.data(), (size_t)vertexBufferSize);
    vkUnmapMemory(m_logicalDevice, vertexStagingBufferMemory);

    copyBuffer(vertexStagingBuffer, *pVertexBuffer, vertexBufferSize);

    vkDestroyBuffer(m_logicalDevice, vertexStagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, vertexStagingBufferMemory, nullptr);

    //--------------------------------------------------------------------------
    // fill index buffer
    //--------------------------------------------------------------------------
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
    //std::cout << "VulkanCanvas::fillMD5VertexBufferAndIndexBuffer(): indexBufferSize = " << indexBufferSize << std::endl;

    VkBuffer indexStagingBuffer;
    VkDeviceMemory indexStagingBufferMemory;
    createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexStagingBuffer, indexStagingBufferMemory);

    void* indexData;
    vkMapMemory(m_logicalDevice, indexStagingBufferMemory, 0, indexBufferSize, 0, &indexData);
    memcpy(indexData, indices.data(), (size_t)indexBufferSize);
    vkUnmapMemory(m_logicalDevice, indexStagingBufferMemory);

    //createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *pIndexBuffer, *pIndexBufferMemory);

    copyBuffer(indexStagingBuffer, *pIndexBuffer, indexBufferSize);

    vkDestroyBuffer(m_logicalDevice, indexStagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, indexStagingBufferMemory, nullptr);
}

void VulkanCanvas::createTexture(std::string textureFilename, VkImage* curTextureImage, VkDeviceMemory* curTextureImageMemory, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) {
    //--------------------------------------------------------------------------
    // createTextureImage()
    //--------------------------------------------------------------------------
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(textureFilename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    if (!pixels) {
        throw std::runtime_error("VulkanCanvas::createTexture(): failed to load " + textureFilename);
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    //std::cout << "VulkanCanvas::createTexture():" << std::endl;
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(m_logicalDevice, stagingBufferMemory);

    stbi_image_free(pixels);

    createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *curTextureImage, *curTextureImageMemory);

    transitionImageLayout(*curTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    copyBufferToImage(stagingBuffer, *curTextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

    vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, stagingBufferMemory, nullptr);

    generateMipmaps(*curTextureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);

    //--------------------------------------------------------------------------
    // createTextureImageView()
    //--------------------------------------------------------------------------
    //textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
    *curTextureImageView = createImageView(*curTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);

    //--------------------------------------------------------------------------
    // createTextureSampler()
    //--------------------------------------------------------------------------
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.mipLodBias = 0.0f;

    //if (vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
    if (vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, curTextureImageSampler) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createTexture(): failed to create texture sampler!");
    }
}

void VulkanCanvas::createUniformBuffers(std::vector<VkBuffer> *curUniformBuffers, std::vector<VkDeviceMemory>* curUniformBuffersMemory, std::vector<void*>* curUniformBuffersMapped) {
    //--------------------------------------------------------------------------
    // createUniformBuffers()
    //--------------------------------------------------------------------------
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    (*curUniformBuffers).resize(MAX_FRAMES_IN_FLIGHT);
    (*curUniformBuffersMemory).resize(MAX_FRAMES_IN_FLIGHT);
    (*curUniformBuffersMapped).resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, (*curUniformBuffers)[i], (*curUniformBuffersMemory)[i]);

        vkMapMemory(m_logicalDevice, (*curUniformBuffersMemory)[i], 0, bufferSize, 0, &(*curUniformBuffersMapped)[i]);
    }
}

void VulkanCanvas::createDescriptorPool(VkDescriptorPool *curDescriptorPool) {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, curDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createDescriptorPool(): failed to create descriptor pool!");
    }
}

void VulkanCanvas::createTextDescriptorPool(VkDescriptorPool* curDescriptorPool) {
    std::array<VkDescriptorPoolSize, 1> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, curDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createDescriptorPool(): failed to create descriptor pool!");
    }
}

void VulkanCanvas::createDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, std::vector<VkBuffer>* curUniformBuffers, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = *curDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    (*curDescriptorSets).resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, (*curDescriptorSets).data()) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createDescriptorSets(): failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = (*curUniformBuffers)[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = *curTextureImageView;
        imageInfo.sampler = *curTextureImageSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = (*curDescriptorSets)[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = (*curDescriptorSets)[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void VulkanCanvas::createTextDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler) {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, textDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = *curDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    (*curDescriptorSets).resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, (*curDescriptorSets).data()) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCanvas::createTextDescriptorSets(): failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = *curTextureImageView;
        imageInfo.sampler = *curTextureImageSampler;

        //std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
        
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = (*curDescriptorSets)[i];
        descriptorWrites[0].dstBinding = 1;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

// not called???????????????????????
VkCommandBufferAllocateInfo VulkanCanvas::CreateCommandBufferAllocateInfo(std::vector<VkCommandBuffer> *curCommandBuffers) const noexcept {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)(*curCommandBuffers).size();
    return allocInfo;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// camera
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// camera (global var for extern - used by VulkanMyObj::updateUniformBuffer()) and VulkanMD5Model::updateUniformBuffer())
glm::mat4 cameraPerspective;
glm::mat4 cameraView;
//glm::vec3 position;

/**
 * Initializes the Vulkan camera, setting the perspective and view.
 * Called by VulkanCanvas().
 */
void VulkanCanvas::initCamera() {
    rotationOn = false;
    panningOn = false;
    zoomOn = false;

    pixelRatio = 0.0001f;
    fov = 45.0f;

    camViewPos = glm::vec4();
    camRotation = glm::vec3();
    //position = glm::vec3(0.0f, 8.0f, 8.0f);
    //camPosition = glm::vec3(7.0f, 0.0f, 0.0f);
    //camPosition = glm::vec3(10.0f, 10.0f, 10.0f);
    camPosition = CAMERA_POS;
    //position = glm::vec3(20.0f, 100.0f, 30.0f);
    //position = glm::vec3(0.0f, 50.0f, 50.0f);
    camTarget = glm::vec3(0.0f, 0.0f, 0.0f);

    //std::cout << "VulkanCanvas::initCamera(): cameraPerspective: -------------------------------------------------- \n";
    cameraPerspective = glm::perspective(glm::radians(fov), m_swapchainExtent.width / (float)m_swapchainExtent.height, 0.1f, 1000.0f);
    cameraView = glm::lookAt(camPosition, camTarget, glm::vec3(0.0f, 1.0f, 0.0f));

    if (TEST_CAMERA_ON) {
        std::cout << "VulkanCanvas::initCamera(): cameraPerspective: -------------------------------------------------- \n";
        printMat4(cameraPerspective);
        std::cout << "VulkanCanvas::initCamera(): cameraView: -------------------------------------------------- \n";
        printMat4(cameraView);
        std::cout << "------------------------------------------------------------------------------------------------- \n";
    }

    right = glm::vec3(cameraView[0][0], cameraView[1][0], cameraView[2][0]);
    up = glm::vec3(cameraView[0][1], cameraView[1][1], cameraView[2][1]);
    forward = glm::vec3(cameraView[0][2], cameraView[1][2], cameraView[2][2]);

    if (TEST_CAMERA_ON) {
        std::cout << "VulkanCanvas::initCamera(): forward: -------------------------------------------------- \n";
        printVec3(forward);
        std::cout << "------------------------------------------------------------------------------------------------- \n";
    }

    setMovementSpeed(2.0f);
}

/**
 * Sets movementSpeed to the given speed.
 * @param movementSpeed the speed to set movementSpeed to
 */
void VulkanCanvas::setMovementSpeed(float movementSpeed) {
    this->movementSpeed = movementSpeed;
}

/**
 * Sets rotationSpeed to the given speed.
 * @param rotationSpeed the speed to set rotationSpeed to
 */
void VulkanCanvas::setRotationSpeed(float rotationSpeed) {
    this->rotationSpeed = rotationSpeed;
}

/**
 * Updates the camera's view matrix. 
 */
void VulkanCanvas::updateViewMatrix() {
    glm::mat4 rotM = glm::mat4(1.0f);
    glm::mat4 transM;

    rotM = glm::rotate(rotM, glm::radians(camRotation.x * (flipY ? -1.0f : 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(camRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(camRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 translation = camPosition;
    if (flipY) {
        translation.y *= -1.0f;
    }
    transM = glm::translate(glm::mat4(1.0f), translation);

    if (type == CameraType::firstperson) {
        cameraView = rotM * transM;
    }
    else {
        cameraView = transM * rotM;
    }

    //viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
    //cameraView = glm::lookAt(position, target, up);
    cameraView = glm::lookAt(camPosition, camTarget, glm::vec3(0.0f, 1.0f, 0.0f));

    updated = true;
};

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

/**
 * Updates the camera's speed, osition and view matrix.
 * @param deltaTime seconds per frame
 */
void VulkanCanvas::updateCamera(double deltaTime) {
    //std::cout << "VulkanCanvas::updateCamera(...): position = ";
    //printVec3(position);
    //--------------------------------------------------------------------------
    // from Sascha (camera.hpp)
    //--------------------------------------------------------------------------
    updated = false;
    if (type == CameraType::firstperson) {
        right = glm::vec3(cameraView[0][0], cameraView[1][0], cameraView[2][0]);
        up = glm::vec3(cameraView[0][1], cameraView[1][1], cameraView[2][1]);
        forward = glm::vec3(cameraView[0][2], cameraView[1][2], cameraView[2][2]);
        //target = glm::vec3(0.0f, 0.0f, 0.0f);

        glm::vec3 camFront;
        camFront.x = -cos(glm::radians(camRotation.x)) * sin(glm::radians(camRotation.y));
        camFront.y = sin(glm::radians(camRotation.x));
        camFront.z = cos(glm::radians(camRotation.x)) * cos(glm::radians(camRotation.y));
        camFront = glm::normalize(camFront);

        //ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        float moveSpeed = deltaTime * movementSpeed;
        //std::cout << "moveSpeed: " << std::endl;
        //moveSpeed = deltaTime * movementSpeed;

        //------------------------------------------------------------------
        // key control
        //------------------------------------------------------------------
        if (keys.up) {
            camPosition -= forward * moveSpeed;
            camTarget -= forward * moveSpeed;
        }
        if (keys.down) {
            camPosition += forward * moveSpeed;
            camTarget += forward * moveSpeed;
        }
        if (keys.left) {
            camPosition -= right * moveSpeed;
            camTarget -= right * moveSpeed;
        }
        if (keys.right) {
            camPosition += right * moveSpeed;
            camTarget += right * moveSpeed;
        }

        //------------------------------------------------------------------
        // panning
        //------------------------------------------------------------------
        if (panningOn) {
            float curDistToMoveX = ((float)xDraggingDiff * pixelRatio);
            float curDistToMoveY = ((float)yDraggingDiff * pixelRatio);
            camPosition -= right * curDistToMoveX;
            camPosition += up * curDistToMoveY;

            camTarget -= right * curDistToMoveX;
            camTarget += up * curDistToMoveY;
        }
        //------------------------------------------------------------------
        // rotation
        //------------------------------------------------------------------
        else if (rotationOn) {
            // Extra step to handle the problem when the camera direction is the same as the up vector
            glm::vec3 camViewingDir = -glm::transpose(cameraView)[2];
            float cosAngle = glm::dot(camViewingDir, up) * (float)deltaTime;
            if (cosAngle * sgn(rotDeltaAngleY) > 0.99f) {
                rotYAngle = 0;
            }   

            // step 2: Rotate the camera around the pivot point on the first axis.
            glm::vec4 positionForRotation(camPosition.x, camPosition.y, camPosition.z, 1);
            glm::vec4 targetForRotation(camTarget.x, camTarget.y, camTarget.z, 1);

            glm::mat4x4 rotationMatrixX(1.0f);

            //rotationMatrixX = glm::rotate(rotationMatrixX, rotXAngle, up);
            rotationMatrixX = glm::rotate(rotationMatrixX, rotXAngle, -up);
            positionForRotation = (rotationMatrixX * (positionForRotation - targetForRotation)) + targetForRotation;

            // step 3: Rotate the camera around the pivot point on the second axis.
            glm::mat4x4 rotationMatrixY(1.0f);
            //rotationMatrixY = glm::rotate(rotationMatrixY, rotYAngle, right);
            rotationMatrixY = glm::rotate(rotationMatrixY, rotYAngle, -right);
            glm::vec3 finalPosition = (rotationMatrixY * (positionForRotation - targetForRotation)) + targetForRotation;
            camPosition = finalPosition;

            dragStartX = dragEndX;
            dragStartY = dragEndY;
            xDraggingDiff = 0.0f;
            yDraggingDiff = 0.0f;
        }

        //------------------------------------------------------------------
        // mouselook
        //------------------------------------------------------------------
        if (MOUSELOOK_ON) { // fixed position, only rotate camera TODO
            //glm::vec3 camViewingDir = -glm::transpose(cameraView)[2];
            //float cosAngle = glm::dot(camViewingDir, up) * (float)deltaTime;
            //if (cosAngle * sgn(rotDeltaAngleY) > 0.99f) {
            //    rotYAngle = 0;
            //}

            //// step 2: Rotate the camera around the pivot point on the first axis.
            //glm::vec4 positionForRotation(position.x, position.y, position.z, 1);
            //glm::vec4 targetForRotation(target.x, target.y, target.z, 1);

            //glm::mat4x4 rotationMatrixX(1.0f);

            ////rotationMatrixX = glm::rotate(rotationMatrixX, rotXAngle, up);
            //rotationMatrixX = glm::rotate(rotationMatrixX, rotXAngle, -up);
            //positionForRotation = (rotationMatrixX * (positionForRotation - targetForRotation)) + targetForRotation;

            //// step 3: Rotate the camera around the pivot point on the second axis.
            //glm::mat4x4 rotationMatrixY(1.0f);
            ////rotationMatrixY = glm::rotate(rotationMatrixY, rotYAngle, right);
            //rotationMatrixY = glm::rotate(rotationMatrixY, rotYAngle, -right);
            //glm::vec3 finalPosition = (rotationMatrixY * (positionForRotation - targetForRotation)) + targetForRotation;
            ////position = finalPosition;
            //target = finalPosition;

            //dragStartX = dragEndX;
            //dragStartY = dragEndY;
            //xDraggingDiff = 0.0f;
            //yDraggingDiff = 0.0f;

            //float curDistToMoveX = ((float)xDraggingDiff * pixelRatio);
            //float curDistToMoveY = ((float)yDraggingDiff * pixelRatio);
            ////position -= right * curDistToMoveX;
            ////position += up * curDistToMoveY;

            //target -= right * curDistToMoveX;
            //target += up * curDistToMoveY;
        }
    }
    updateViewMatrix();
    o_canvas->update();
    //if (DRAW_BOUNDING_BOX_ON) {
    //    for (int i = 0; i < objLoaders.size(); i++) {
    //        if (objLoaders[i]->getIsBBLoader()) { // a bb obj loader
    //            //std::cout << "VulkanCanvas::updateCamera(...): if (objLoaders[i]->getIsBBLoader())" << std::endl;
    //            ObjLoader* curBBObjFile = objLoaders[i];
    //            MyObj* curMyBBObj = curBBObjFile->myObjsArr[0]; // only one object per file
    //            ((BBMyObj*)curMyBBObj)->checkCamera(camPosition);
    //        }
    //    }
    //}
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// key events
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * Handles wxWidgets key pressed events. 
 */
void VulkanCanvas::onKeyDown(wxKeyEvent& event) {
    wxChar uc = event.GetUnicodeKey();
    if (uc != WXK_NONE) {
        switch (uc) {
        case 'W':
            keys.up = true;
            break;
        case 'A':
            keys.left = true;
            break;
        case 'S':
            keys.down = true;
            break;
        case 'D':
            keys.right = true;
            break;
        case 'F':
            keys.fKeyPressed = true;
            // 1) get target object
            // 2) zoom in to target object
            //      - adjust distance so that object fits in screen
            // position = 
            break;
        }
    }
    else { // No Unicode equivalent.
        switch (event.GetKeyCode()) {
        case WXK_UP:
            keys.up = true;
            break;
        case WXK_LEFT:
            keys.left = true;
            break;
        case WXK_DOWN:
            keys.down = true;
            break;
        case WXK_RIGHT:
            keys.right = true;
            break;
        case WXK_SHIFT:
            keys.shiftKeyPressed = true;
            break;
        case WXK_F1:
            exit(1);
        }
    }
}

/**
 * Handles wxWidgets key released events.
 */
void VulkanCanvas::onKeyUp(wxKeyEvent& event) {
    wxChar uc = event.GetUnicodeKey();
    if (uc != WXK_NONE) {
        switch (uc) {
        case 'W':
            keys.up = false;
            break;
        case 'A':
            keys.left = false;
            break;
        case 'S':
            keys.down = false;
            break;
        case 'D':
            keys.right = false;
            break;
        case 'F':
            keys.fKeyPressed = false;
            break;
        }
    }
    else { // No Unicode equivalent.
        switch (event.GetKeyCode()) {
        case WXK_UP:
            keys.up = false;
            break;
        case WXK_LEFT:
            keys.left = false;
            break;
        case WXK_DOWN:
            keys.down = false;
            break;
        case WXK_RIGHT:
            keys.right = false;
            break;
        case WXK_SHIFT:
            keys.shiftKeyPressed = false;
            panningOn = false;
            break;
        }
    }
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// mouse events
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * Handles wxWidgets mouse moved events.
 */
void VulkanCanvas::onMouseMoved(wxMouseEvent& event) {
    //std::cout << "VulkanCanvas::onMouseMoved(): started" << std::endl;
    //
    if (filesPanel->fileSelected) {
        curSelectedFile->SetPosition(wxPoint(event.GetPosition().x, event.GetPosition().y + 10));
    }
    // mouse look?
    dragEndX = event.GetPosition().x;
    dragEndY = event.GetPosition().y;

    xDraggingDiff = (dragEndX - dragStartX);
    yDraggingDiff = (dragEndY - dragStartY);
    if (panningOn) { // PANNING
    }
    else if (rotationOn) { // ROTATE
        rotDeltaAngleX = (2 * M_PI / (float)m_swapchainExtent.width); // a movement from left to right = 2*PI = 360 deg
        rotDeltaAngleY = (M_PI / (float)m_swapchainExtent.height);  // a movement from top to bottom = PI = 180 deg

        rotXAngle = xDraggingDiff * rotDeltaAngleX * 0.3f;
        rotYAngle = yDraggingDiff * rotDeltaAngleY * 0.3f;
    }

    if (MOUSELOOK_ON) {
        // move camera view
    }

    // move obj with mouse
    if (objDragOn) { //TODO: activate with G key
        //glm::vec3 rayWorld = castRay(event.GetPosition().x, event.GetPosition().y);
        //curSelectedObj->position = rayWorld; // TODO change to object coordinates
        //std::cout << "VulkanCanvas::onMouseMoved(): rayWorld: ";
        //printVec3(rayWorld);
        //std::cout << "VulkanCanvas::onMouseMoved(): mouseX: " << event.GetPosition().x << " | mouseY: " << event.GetPosition().y << std::endl;
    }
}

void VulkanCanvas::onMouseReleased(wxMouseEvent& event) {
    //std::cout << "VulkanCanvas::onMouseReleased(): start" << std::endl;
    if (filesPanel->fileSelected) {
        wxTextDataObject tdo(filesPanel->selectedFile);
        wxDropSource tds(tdo, filesPanel->listView);
        tds.DoDragDrop(true);

        filesPanel->fileSelected = false;

        curSelectedFile->SetLabelText("");
        curSelectedFile->SetPosition(wxPoint(0, 0));
        curSelectedFile->Hide();
    }

    objDragOn = false;
}

/**
 * Handles wxWidgets mouse down events.
 */
void VulkanCanvas::onMMBReleased(wxMouseEvent& event) {
    if (panningOn) {
        panningOn = false;
    }
    else if (rotationOn) {
        rotationOn = false;
    }
}

/**
 * Handles wxWidgets mouse scroll events.
 *    - zoom in/out
 */
void VulkanCanvas::onMouseWheel(wxMouseEvent& event) {
    if (event.GetWheelRotation() > 0) { // scroll up/zoom in
        if (fov - 0.3f <= 0.0f) {
            fov = 0.0f;
        }
        else {
            fov -= 0.3f;
        }
    }
    else if (event.GetWheelRotation() < 0) { // scroll down/zoom out
        if (fov + 0.3f >= 180.0f) {
            fov = 180.0f;
        }
        else {
            fov += 0.3f;
        }
    }
    cameraPerspective = glm::perspective(glm::radians(fov), m_swapchainExtent.width / (float)m_swapchainExtent.height, 0.1f, 100.0f);
}
#include "vulkan_my_obj.h"
/**
 * Handles wxWidgets mouse down events.
 */
void VulkanCanvas::onMouseDown(wxMouseEvent& event) {
    SetFocus();

    dragStartX = event.GetPosition().x;
    dragStartY = event.GetPosition().y;
    dragEndX = dragStartX;
    dragEndY = dragStartY;
    xDraggingDiff = 0.0f;
    yDraggingDiff = 0.0f;

    if (event.LeftIsDown()) { // OBJECT PICK
        glm::vec3 ray_world = castRay(event.GetPosition().x, event.GetPosition().y);
        Ray ray;
        std::vector<SceneNode*> gameObjs = sceneNodesRoot->getChildren();

        for (int o = 0; o < gameObjs.size(); o++) {
            SceneNode* curObj = gameObjs[o];
            BBMyObj * curBB = (BBMyObj*)(((MyObj*)(curObj->getGameObject()))->getBBMyObj()); // get BB

            glm::vec3 ray_local = glm::vec4(ray_world, 1.0) * glm::transpose(glm::inverse(((VulkanMyObj*)(curBB->myOwnerObj))->uboRotate)); // apply inverse model matrix
            glm::vec4 cam_local = glm::vec4(camPosition, 1.0) * glm::transpose(glm::inverse(((VulkanMyObj*)(curBB->myOwnerObj))->ubo.model)); // apply inverse model matrix

            ray.origin = cam_local;
            ray.direction = ray_local;

            bool intersects = rayBoxIntersect(curBB, ray); // ray-object intersection (ver1131)  - TEST WITH OBJ MODELS ONLY
            if (intersects) {
                curObj->getGameObject()->setMouseSelectOn(true);
                if (curSelectedObj != nullptr && curSelectedObj != curObj->getGameObject()) {
                    curSelectedObj->setMouseSelectOn(false);
                }
                curSelectedObj = curObj->getGameObject();
                objDragOn = true;
                std::cout << "//---------------------------------------------------------------" << std::endl;
                std::cout << "VulkanCanvas::onMouseDown(): OBJECT PICKED: " << curObj->getName() << std::endl;
                std::cout << "//---------------------------------------------------------------" << std::endl;
            }
        }
    }
    else if (event.MiddleIsDown() && keys.shiftKeyPressed) { // PANNING
        panningOn = true;
    }
    else if (event.MiddleIsDown()) { // ROTATE
        rotationOn = true;
    }
}

//==============================================================================
//==============================================================================
//==============================================================================
// mouse/object picking
//==============================================================================
//==============================================================================
//==============================================================================
glm::vec3 VulkanCanvas::castRay(double screenX, double screenY) {
    float screen_width = m_swapchainExtent.width;
    float screen_height = m_swapchainExtent.height;

    //step 1: Normalized Device Coordinates (3d)
    //float x = (2.0f * dragStartX) / screen_width - 1.0f;
    float x = (2.0f * screenX) / screen_width - 1.0f;
    //float y = 1.0f - (2.0f * dragStartY) / screen_height;
    float y = 1.0f - (2.0f * screenY) / screen_height;
    float z = 1.0f;
    glm::vec3 ray_nds = glm::vec3(x, y, z);

    // step 2: Homogeneous Clip Space (4d)
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

    // step 3: Eye Space
    // apply the inverse projection(perspective) matrix
    glm::vec4 ray_eye = glm::inverse(cameraPerspective) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

    // step 4: World Coordinates
    // apply the inverse view matrix
    glm::vec3 ray_world = glm::vec3((inverse(cameraView) * ray_eye).x, (inverse(cameraView) * ray_eye).y,
        (inverse(cameraView) * ray_eye).z);
    ray_world = glm::normalize(ray_world); // normalise the vector

    return ray_world;
}


bool VulkanCanvas::twoDimBoxAndRay(float x1, float y1, float x2, float y2, float r0x, float r0y, float rx, float ry) {
    //std::cout << "VulkanCanvas::twoDimBoxAndRay(...): x1 = " << x1 << " | y1 = " << y1 << " | x2 = " << x2 << " | y2 = " << y2
    //          << " | r0x = " << r0x << " | r0y = " << r0y << " | rx = " << rx << " | ry = " << ry << std::endl;
    //----------------------------------------------------
    // consists of 2 vertical, 2 horizontal lines
    // right side vertical line : vr
    // left side vertical line : vl
    // top side horizontal line : ht
    // bottom side horizontal line : hb
    //----------------------------------------------------
    // right side line of 2D bounding box.
    float vra = 1;
    float vrb = 0;
    //float vrc = -1;
    float vrc = -x1;
    // left side line of 2D bounding box.
    float vla = 1;
    float vlb = 0;
    //float vlc = 1;
    float vlc = -x2;
    // top side line of 2D bounding box.
    float hta = 0;
    float htb = 1;
    //float htc = -1;
    float htc = -y1;
    // bottom side line of 2D bounding box.
    float hba = 0;
    float hbb = 1;
    //float hbc = 1;
    float hbc = -y2;

    // https://math.libretexts.org/Bookshelves/Calculus/CLP-3_Multivariable_Calculus_(Feldman_Rechnitzer_and_Yeager)/01%3A_Vectors_and_Geometry_in_Two_and_Three_Dimensions/1.03%3A_Equations_of_Lines_in_2d
    //r0x, r0y = origin, camera position
    //rx, ry = direction.x, direction.y <- direction vector
    //float r1x = r0x + rx;
    //float r1y = r0y + ry;
    //r0x + (r1x * y - r1x *r0y) - x = 0
    //1x + -r0x - r1x * y + r1x * r0y;
    //float ra = 1;
    //float rb = -r1x;
    //float rc = r1x * r0y - r0x;

    //method 1: vector equations
    //method 2: y = mx + b --> (y-y0) = m(x-x0)
    //  m = ry/rx, x0 = r0x, y0 = r0y
    //  (y-r0y) = (ry/rx)*(x-r0x) = ((ry/rx)*x - (ry/rx)*r0x))
    //  (ry/rx)*x - (ry/rx)*r0x - y + r0y = 0
    //  multiply rx/ry:
    //  x - (rx/ry)*y -r0x + (rx/ry)*r0y = 0
    float ra = 1;
    float rb = -rx/ry;
    float rc = -r0x + (r0y * rx) / ry;

    //----------------------------------------------------
    // point of intersection
    // x = (b1c2 - b2c1) / (a1b2 - a2b1)
    // y = (a2c1 - a1c2) / (a1b2 - a2b1)
    // 1 = ray, 2 = line (ver or hor)
    //----------------------------------------------------
    //x: (-1(-1) - 0(0)) / ((1)(0) - (1)(-1)) = 1
    //y : (1(0) - 1(-1)) / ((1)(0) - (1)(-1)) = 1

    // line vr
    float intersectVRX = (rb * vrc - vrb * rc) / (ra * vrb - vra * rb); // = 1 or x1
    float intersectVRY = (vra * rc - ra * vrc) / (ra * vrb - vra * rb);
    // line vl
    float intersectVLX = (rb * vlc - vlb * rc) / (ra * vlb - vla * rb); // = -1 or x2
    float intersectVLY = (vla * rc - ra * vlc) / (ra * vlb - vla * rb);
    // line ht
    float intersectHTX = (rb * htc - htb * rc) / (ra * htb - hta * rb);
    float intersectHTY = (hta * rc - ra * htc) / (ra * htb - hta * rb); // = 1 or y1
    // line hb
    float intersectHBX = (rb * hbc - hbb * rc) / (ra * hbb - hba * rb);
    float intersectHBY = (hba * rc - ra * hbc) / (ra * hbb - hba * rb); // = -1 or y2

    //----------------------------------------------------
    // check if intersection points are inside box
    //----------------------------------------------------
    float bbMaxX = x1;
    float bbMaxY = y1;
    float bbMinX = x2;
    float bbMinY = y2;

    bool vr = intersectVRY <= bbMaxY && intersectVRY >= bbMinY;
    bool vl = intersectVLY <= bbMaxY && intersectVLY >= bbMinY;

    bool ht = intersectHTX <= bbMaxX && intersectHTX >= bbMinX;
    bool hb = intersectHBX <= bbMaxX && intersectHBX >= bbMinX;

    //std::cout << "VulkanCanvas::twoDimBoxAndRay(...): vr = " << vr << " | vl = " << vl << " | ht = " << ht << "| hb = " << hb << std::endl;

    if ((vr && vl) || (vr && ht) || (vr && hb) ||
        (vl && ht) || (vl && hb) || (ht && hb)) {
        return true;
    }
    return false;
}

bool VulkanCanvas::rayBoxIntersect(BBMyObj* bb, Ray ray) {
    //----------------------------------------------------
    // get intersection points
    //----------------------------------------------------
    bool xy = twoDimBoxAndRay(bb->max.x, bb->max.y, bb->min.x, bb->min.y, ray.origin.x, ray.origin.y, ray.direction.x, ray.direction.y); // xy (+z)
    bool zy = twoDimBoxAndRay(bb->max.z, bb->max.y, bb->min.z, bb->min.y, ray.origin.z, ray.origin.y, ray.direction.z, ray.direction.y); // zy (+x)
    bool xz = twoDimBoxAndRay(bb->max.x, bb->max.z, bb->min.x, bb->min.z, ray.origin.x, ray.origin.z, ray.direction.x, ray.direction.z); // xz (+y)
    //----------------------------------------------------
    // check if intersection points are inside bb
    //----------------------------------------------------
    if (xy && zy && xz) {
        return true;
    }
    return false;
}

//==========================================================================
//==========================================================================
//==========================================================================
// UI/2D
//==========================================================================
//==========================================================================
//==========================================================================

void VulkanCanvas::initUI() {
    fps = 0;
    fpsText = new wxStaticText(this, wxID_ANY, "FPS: ", wxPoint(50, 50), wxSize(20, 20), wxALIGN_CENTRE_HORIZONTAL);
    fpsText->SetForegroundColour(wxColor(255, 255, 255));
    fpsText->SetBackgroundColour(wxColor(0, 0, 0));
    fpsText->SetFont(wxFont(35, wxMODERN, wxNORMAL, wxBOLD));

    initText();
}

void VulkanCanvas::updateUI() {
    /*timer.GetFrameDelta();*/
    fps = timer.fps; // per milisecond

    fpsText->SetLabelText("FPS: " + std::to_string(fps));
    vkWaitForFences(m_logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
}

void VulkanCanvas::drawHUD() {
    // code from https://learnopengl.com/In-Practice/2D-Game/Rendering-Sprites
    //glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f); // 2D projection matrix
    // end

    //glBegin(GL_QUADS);
    //glColor3f(1.0f, 0.0f, 0.0);
    //glVertex2f(0.0, 0.0);
    //glVertex2f(10.0, 0.0);
    //glVertex2f(10.0, 10.0);
    //glVertex2f(0.0, 10.0);
    //glEnd();
}

//==============================================================================
//==============================================================================
//==============================================================================
// 2D Text
//==============================================================================
//==============================================================================
//==============================================================================
void VulkanCanvas::CreateTextGraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) { // text pso
    auto vertShaderCode = ReadFile(vertexShaderFile);
    auto fragShaderCode = ReadFile(fragmentShaderFile);

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    CreateShaderModule(vertShaderCode, vertShaderModule);
    CreateShaderModule(fragShaderCode, fragShaderModule);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = CreatePipelineShaderStageCreateInfo(
        VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule, "main");
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = CreatePipelineShaderStageCreateInfo(
        VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule, "main");
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    //VkPipelineVertexInputStateCreateInfo vertexInputInfo = CreatePipelineVertexInputStateCreateInfo();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertexInputInfo.vertexBindingDescriptionCount = 0;
    //vertexInputInfo.vertexAttributeDescriptionCount = 0;

    //auto bindingDescription = Vertex::getBindingDescription();
    auto bindingDescription = TextVertex::getBindingDescription();
    //VkVertexInputBindingDescription bindingDescription{};
    //bindingDescription.binding = 0;
    ////bindingDescription.stride = sizeof(Vertex);
    //bindingDescription.stride = sizeof(TextVertex);
    //bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    //auto attributeDescriptions = Vertex::getAttributeDescriptions();
    auto attributeDescriptions = TextVertex::getAttributeDescriptions();

    //vertexInputInfo.vertexBindingDescriptionCount = 2; // error
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    //VkPipelineInputAssemblyStateCreateInfo inputAssembly = CreatePipelineInputAssemblyStateCreateInfo(
    //    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = CreatePipelineInputAssemblyStateCreateInfo(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VK_FALSE);
    VkViewport viewport = CreateViewport();
    VkRect2D scissor = CreateScissor();
    VkPipelineViewportStateCreateInfo viewportState = CreatePipelineViewportStateCreateInfo(
        viewport, scissor);
    VkPipelineRasterizationStateCreateInfo rasterizer = CreatePipelineRasterizationStateCreateInfo();
    VkPipelineMultisampleStateCreateInfo multisampling = CreatePipelineMultisampleStateCreateInfo();

    //chai30: start
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    //depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    //chai30: end

    VkPipelineColorBlendAttachmentState colorBlendAttachment = CreatePipelineColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo colorBlending = CreatePipelineColorBlendStateCreateInfo(
        colorBlendAttachment);
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = CreateTextPipelineLayoutCreateInfo();

    //VkResult result = vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    VkResult result = vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &textPipelineLayout);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "VulkanCanvas::CreateTextGraphicsPipeline(): Failed to create pipeline layout:");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = CreateTextGraphicsPipelineCreateInfo(shaderStages,
        vertexInputInfo, inputAssembly, viewportState, rasterizer, multisampling, depthStencil, colorBlending);
    std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    pipelineInfo.pDynamicState = &dynamicState;

    //result = vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipaeline);
    result = vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &textPSO);
    // vkDestroyShaderModule calls below must be placed before possible throw of exception
    vkDestroyShaderModule(m_logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_logicalDevice, vertShaderModule, nullptr);
    if (result != VK_SUCCESS) {
        throw VulkanException(result, "VulkanCanvas::CreateTextGraphicsPipeline(): Failed to create graphics pipeline:");
    }
    //std::cout << "VulkanCanvas::CreateTextGraphicsPipeline(): textPSO created!" << textPSO << std::endl;
}

void VulkanCanvas::initText() {
    //-----------------------------------------------------------------------------
    // Load Font
    //-----------------------------------------------------------------------------
    arialFont = LoadFont("Arial.fnt", m_swapchainExtent.width, m_swapchainExtent.height);
    createTexture(fontImageFilename, &textTextureImage, &textTextureImageMemory, &textTextureImageView, &textTextureImageSampler); // createTextureImage() + createTextureImageView() + createTextureSampler()

    //--------------------------------------------------------------------------
    // descriptor pool
    //--------------------------------------------------------------------------
    createTextDescriptorPool(&textDescriptorPool);

    //--------------------------------------------------------------------------
    // descriptor sets
    //--------------------------------------------------------------------------
    createTextDescriptorSets(&textDescriptorPool, &textDescriptorSets, &textTextureImageView, &textTextureImageSampler);
    arialFont.srvHandle = textDescriptorSets;

    //-----------------------------------------------------------------------------
    // create text vertex buffer committed resources
    //-----------------------------------------------------------------------------
    createTextVertexBuffer(&textVertexBuffer, &textVertexBufferMemory); //sally
    //createTextVertexBuffer(textVertexBuffer, &textVertexBufferMemory); //sally
}

Font VulkanCanvas::LoadFont(const std::string& filename, int windowWidth, int windowHeight) {
    std::wifstream fs;
    try {
        //std::cout << "VulkanCanvas::LoadFont(): filename: " << filename << '\n';
        fs.open(filename);
    }
    catch (std::exception& e) {
        std::cout << "An exception occurred. Exception Nr. " << e.what() << '\n';
    }

    Font font;
    std::wstring tmp;
    int startpos;

    // extract font name
    fs >> tmp >> tmp; // info face="Arial"
    startpos = tmp.find(L"\"") + 1;
    font.name = tmp.substr(startpos, tmp.size() - startpos - 1);

    // get font size
    fs >> tmp; // size=73
    startpos = tmp.find(L"=") + 1;
    font.size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
    fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 

    // get padding
    fs >> tmp; // padding=5,5,5,5 
    startpos = tmp.find(L"=") + 1;
    tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

    // get up padding
    startpos = tmp.find(L",") + 1;
    font.toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get right padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    startpos = tmp.find(L",") + 1;
    font.rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get down padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    startpos = tmp.find(L",") + 1;
    font.bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get left padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    font.leftpadding = std::stoi(tmp) / (float)windowWidth;

    fs >> tmp; // spacing=0,0

    // get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
    fs >> tmp >> tmp; // common lineHeight=95
    startpos = tmp.find(L"=") + 1;
    font.lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

    // get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
    fs >> tmp; // base=68
    startpos = tmp.find(L"=") + 1;
    font.baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

    // get texture width
    fs >> tmp; // scaleW=512
    startpos = tmp.find(L"=") + 1;
    font.textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // get texture height
    fs >> tmp; // scaleH=512
    startpos = tmp.find(L"=") + 1;
    font.textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // get pages, packed, page id
    fs >> tmp >> tmp; // pages=1 packed=0
    fs >> tmp >> tmp; // page id=0

    // get texture filename
    std::wstring wtmp;
    fs >> wtmp; // file="Arial.png"
    startpos = wtmp.find(L"\"") + 1;
    font.fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

    // get number of characters
    fs >> tmp >> tmp; // chars count=97
    startpos = tmp.find(L"=") + 1;
    font.numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // initialize the character list
    font.CharList = new FontChar[font.numCharacters];

    for (int c = 0; c < font.numCharacters; ++c) {
        // get unicode id
        fs >> tmp >> tmp; // char id=0
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get x
        fs >> tmp; // x=392
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureWidth;

        // get y
        fs >> tmp; // y=340
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureHeight;

        // get width
        fs >> tmp; // width=47
        startpos = tmp.find(L"=") + 1;
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        font.CharList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
        font.CharList[c].twidth = (float)std::stoi(tmp) / (float)font.textureWidth;

        // get height
        fs >> tmp; // height=57
        startpos = tmp.find(L"=") + 1;
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        font.CharList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
        font.CharList[c].theight = (float)std::stoi(tmp) / (float)font.textureHeight;

        // get xoffset
        fs >> tmp; // xoffset=-6
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

        // get yoffset
        fs >> tmp; // yoffset=16
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

        // get xadvance
        fs >> tmp; // xadvance=65
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

        // get page
        // get channel
        fs >> tmp >> tmp; // page=0    chnl=0
    }

    // get number of kernings
    fs >> tmp >> tmp; // kernings count=96
    startpos = tmp.find(L"=") + 1;
    font.numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // initialize the kernings list
    font.KerningsList = new FontKerning[font.numKernings];

    for (int k = 0; k < font.numKernings; ++k) {
        // get first character
        fs >> tmp >> tmp; // kerning first=87
        startpos = tmp.find(L"=") + 1;
        font.KerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get second character
        fs >> tmp; // second=45
        startpos = tmp.find(L"=") + 1;
        font.KerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get amount
        fs >> tmp; // amount=-1
        startpos = tmp.find(L"=") + 1;
        int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
        font.KerningsList[k].amount = (float)t / (float)windowWidth;
    }

    return font;
}

//int countTextVertexBufferCreation = 0;

void VulkanCanvas::RenderText(VkCommandBuffer commandBuffer, Font font, std::string text, glm::vec2 pos, glm::vec2 scale, glm::vec2 padding, glm::vec4 color) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, textPSO);

    // clear the depth buffer so we can draw over everything
    //commandList->ClearDepthStencilView(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    //clearValues[0].color = { {1.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    // we only need 4 vertices per quad rather than 6 if we were to use a triangle list topology
    //commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // in createTextGraphicsPipeline()

    // bind the text srv. We will assume the correct descriptor heap and table are currently bound and set
    //commandList->SetGraphicsRootDescriptorTable(1, font.srvHandle);
    const VkDescriptorSet* curDescSet = &(*(std::vector<VkDescriptorSet>*)(&font.srvHandle))[currentFrame];
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, textPipelineLayout, 0, 1, curDescSet, 0, nullptr);

    text = "Hello: 123";

    int numCharacters = 0;
    //static int numCharacters = 0;

    //if (countTextVertexBufferCreation < 1) {
        float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
        float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;
        float x = topLeftScreenX;
        float y = topLeftScreenY;
        float horrizontalPadding = (font.leftpadding + font.rightpadding) * padding.x;
        float verticalPadding = (font.toppadding + font.bottompadding) * padding.y;

        // cast the gpu virtual address to a textvertex, so we can directly store our vertices there
        //TextVertex* vert = (TextVertex*)textVBGPUAddress[frameIndex]; // original
        std::vector<TextVertex> textVertices;

        wchar_t lastChar = -1; // no last character to start with

        //color = glm::vec4(0.0f, 255.0f, 0.0f, 1.0f);
        color = glm::vec4(0.0f, 0.0f, 255.0f, 1.0f);

        for (int i = 0; i < text.size(); ++i) {
            wchar_t c = text[i];
            FontChar* fc = font.GetChar(c);

            if (fc == nullptr) // character not in font char set
                continue;

            if (c == L'\0') // end of string
                break;

            if (c == L'\n') { // new line
                x = topLeftScreenX;
                y -= (font.lineHeight + verticalPadding) * scale.y;
                continue;
            }

            // don't overflow the buffer. In your app if this is true, you can implement a resize of your text vertex buffer
            if (numCharacters >= maxNumTextCharacters)
                break;

            float kerning = 0.0f;
            if (i > 0)
                kerning = font.GetKerning(lastChar, c);

            TextVertex curTextVertex = TextVertex(
                x + ((fc->xoffset + kerning) * scale.x),
                y - (fc->yoffset * scale.y) - 1.7f,
                fc->width * scale.x,
                fc->height * scale.y,
                fc->u,
                fc->v,
                fc->twidth,
                fc->theight,
                color.x,
                color.y,
                color.z,
                color.w
            );
            textVertices.push_back(curTextVertex);

            //std::cout << "VulkanCanvas::RenderText():================================================================================================================" << std::endl;
            //std::cout << "VulkanCanvas::RenderText(): pos.x = " << curTextVertex.pos.x << " | pos.y = " << curTextVertex.pos.y << " | pos.w = " << curTextVertex.pos.z << " | pos.h = " << curTextVertex.pos.w << std::endl;
            //std::cout << "VulkanCanvas::RenderText(): texCoord.x = " << curTextVertex.texCoord.x << " | texCoord.y = " << curTextVertex.texCoord.y << " | texCoord.w = " << curTextVertex.texCoord.z << " | texCoord.h = " << curTextVertex.texCoord.w << std::endl;
            //std::cout << "VulkanCanvas::RenderText(): color.x = " << color.x << " | color.y = " << color.y << " | color.z = " << color.z << " | color.w = " << color.w << std::endl;
            //std::cout << "VulkanCanvas::RenderText():================================================================================================================" << std::endl;

            numCharacters++;

            // remove horrizontal padding and advance to next char position
            x += (fc->xadvance - horrizontalPadding) * scale.x;
            lastChar = c;
        //}

        fillTextVertexBuffer(textVertices, &textVertexBuffer, &textVertexBufferMemory); //sally
    }

    // set the text vertex buffer
    //commandList->IASetVertexBuffers(0, 1, &textVertexBufferView[frameIndex]);
    VkBuffer textVertexBuffers[] = { (VkBuffer)(textVertexBuffer) };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, textVertexBuffers, offsets);

    // we are going to have 4 vertices per character (trianglestrip to make quad), and each instance is one character
    //commandList->DrawInstanced(4, numCharacters, 0, 0);
    vkCmdDraw(commandBuffer, 4, numCharacters, 0, 0);

    //countTextVertexBufferCreation++;
}

void VulkanCanvas::md5Test() {
    VulkanMD5Model *md5Model = new VulkanMD5Model();
    md5Model->LoadModel(md5MeshFilenames[0]);
    md5Model->LoadAnim(md5AnimFilenames[0]);
    vulkanMD5Models.push_back(md5Model);
}

void VulkanCanvas::setFilesPanel(FilesPanel* filesPanel) {
    this->filesPanel = filesPanel;
}

void VulkanCanvas::setHierarchyPanel(HierarchyPanel *hierarchyPanel) {
    this->hierarchyPanel = hierarchyPanel;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Boundary Highlight
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
