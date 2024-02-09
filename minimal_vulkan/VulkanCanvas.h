#pragma once

#include <set>

#include "wx/wx.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define FRAMEBUFFER_COUNT 3
#include <vulkan/vulkan.h>

#include "obj_loader.h"
#include "files_panel.h";
#include "hierarchy_panel.h";

const int MAX_FRAMES_IN_FLIGHT = 2;

struct QueueFamilyIndices {
    int graphicsFamily = -1;
    int presentFamily = -1;

    bool IsComplete() {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex;
//class ObjLoader;
class VulkanMD5Model;
class SceneNode;
class VulkanRenderInfo;
class BBMyObj;

class VulkanCanvas : public wxWindow {
public:
    VulkanCanvas(wxWindow* pParent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = "VulkanCanvasName");

    virtual ~VulkanCanvas() noexcept;

private:

    void InitializeVulkan(std::vector<const char*> extensions);
    void CreateInstance(const VkInstanceCreateInfo& createInfo);
    void CreateWindowSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();

    //void CreateSwapChain(const wxSize& size);
    void createSwapChain(); //chai30
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device); //chai30
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    void CreateImageViews();
    void CreateRenderPass();
    void createDescriptorSetLayout();
    void createTextDescriptorSetLayout();
    void CreateGraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

    void CreateFrameBuffers();
    void CreateCommandPool();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties); //sally19
    void CreateCommandBuffers();

    void createColorResources(); //chai30
    void createDepthResources(); //chai30
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features); //chai30
    VkFormat findDepthFormat(); //chai30
    bool hasStencilComponent(VkFormat format); //chai30
    //void createTextureImage(); //chai30
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels); //chai30
    VkSampleCountFlagBits getMaxUsableSampleCount(); //chai30
    //void createTextureImageView(); //chai30
    //void createTextureSampler(); //chai30
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels); //chai30
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory); //chai30
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels); //chai30
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height); //chai30
    void loadModel(); //chai31
public:
    void loadModel(std::string modelPath, std::string animPath, bool hasAnim);
private:
    //void createVertexBuffer(); //chai30
    //void createIndexBuffer(); //chai30
    //void createUniformBuffers(); //chai31
    //void createDescriptorPool(); //chai31
    //void createDescriptorSets(); //chai31
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory); //chai31
    VkCommandBuffer beginSingleTimeCommands(); //chai30
    void endSingleTimeCommands(VkCommandBuffer commandBuffer); //chai30
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size); //chai30
    void createCommandBuffers(); //chai30
    //void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex); //chai30
    void createSyncObjects(); //chai30
    void updateUniformBuffer(uint32_t currentImage); //chai30

    void RecreateSwapchain();
    //void drawFrame();

    //void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    //void createSyncObjects(); //sally15
    //void CreateSemaphores();
    void cleanupSwapChain(); //sally17
    VkWin32SurfaceCreateInfoKHR CreateWin32SurfaceCreateInfo() const noexcept;
    VkDeviceQueueCreateInfo CreateDeviceQueueCreateInfo(int queueFamily) const noexcept;
    VkApplicationInfo CreateApplicationInfo(const std::string& appName,
        const int32_t appVersion = VK_MAKE_VERSION(1, 0, 0),
        const std::string& engineName = "No Engine",
        const int32_t engineVersion = VK_MAKE_VERSION(1, 0, 0),
        const int32_t apiVersion = VK_API_VERSION_1_0) const noexcept;
    VkInstanceCreateInfo CreateInstanceCreateInfo(const VkApplicationInfo& appInfo,
        const std::vector<const char*>& extensionNames,
        const std::vector<const char*>& layerNames) const noexcept;
    std::vector<VkDeviceQueueCreateInfo> CreateQueueCreateInfos(
        const std::set<int>& uniqueQueueFamilies) const noexcept;
    VkDeviceCreateInfo CreateDeviceCreateInfo(
        const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
        const VkPhysicalDeviceFeatures& deviceFeatures) const noexcept;
    VkSwapchainCreateInfoKHR CreateSwapchainCreateInfo(
        const SwapChainSupportDetails& swapChainSupport,
        const VkSurfaceFormatKHR& surfaceFormat,
        uint32_t imageCount,
        const VkExtent2D& extent);
    VkImageViewCreateInfo CreateImageViewCreateInfo(uint32_t swapchainImage) const noexcept;
    VkAttachmentDescription CreateAttachmentDescription() const noexcept;
    VkAttachmentReference CreateAttachmentReference() const noexcept;
    VkSubpassDescription CreateSubpassDescription(const VkAttachmentReference& attachmentRef) const noexcept;
    VkSubpassDependency CreateSubpassDependency() const noexcept;
    VkRenderPassCreateInfo CreateRenderPassCreateInfo(
        const VkAttachmentDescription& colorAttachment,
        const VkSubpassDescription& subPass,
        const VkSubpassDependency& dependency) const noexcept;
    VkPipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(
        VkShaderStageFlagBits stage, VkShaderModule& module, const char* entryName) const noexcept;
    VkPipelineVertexInputStateCreateInfo CreatePipelineVertexInputStateCreateInfo() const noexcept;
    VkPipelineInputAssemblyStateCreateInfo CreatePipelineInputAssemblyStateCreateInfo(
        const VkPrimitiveTopology& topology, uint32_t restartEnable) const noexcept;
    VkViewport CreateViewport() const noexcept;
    VkRect2D CreateScissor() const noexcept;
    VkPipelineViewportStateCreateInfo CreatePipelineViewportStateCreateInfo(
        const VkViewport& viewport, const VkRect2D& scissor) const noexcept;
    VkPipelineRasterizationStateCreateInfo CreatePipelineRasterizationStateCreateInfo() const noexcept;
    VkPipelineMultisampleStateCreateInfo CreatePipelineMultisampleStateCreateInfo() const noexcept;
    VkPipelineColorBlendAttachmentState CreatePipelineColorBlendAttachmentState() const noexcept;
    VkPipelineColorBlendStateCreateInfo CreatePipelineColorBlendStateCreateInfo(
        const VkPipelineColorBlendAttachmentState& colorBlendAttachment) const noexcept;
    VkPipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo() const noexcept;
    VkPipelineLayoutCreateInfo CreateTextPipelineLayoutCreateInfo() const noexcept;
    VkGraphicsPipelineCreateInfo CreateGraphicsPipelineCreateInfo(
        const VkPipelineShaderStageCreateInfo shaderStages[],
        const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
        const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
        const VkPipelineViewportStateCreateInfo& viewportState,
        const VkPipelineRasterizationStateCreateInfo& rasterizer,
        const VkPipelineMultisampleStateCreateInfo& multisampling,
        const VkPipelineDepthStencilStateCreateInfo& depthStencil,
        const VkPipelineColorBlendStateCreateInfo& colorBlending) const noexcept;
    
    VkGraphicsPipelineCreateInfo CreateBoundaryGraphicsPipelineCreateInfo(
        const VkPipelineShaderStageCreateInfo shaderStages[],
        const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
        const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
        const VkPipelineViewportStateCreateInfo& viewportState,
        const VkPipelineRasterizationStateCreateInfo& rasterizer,
        const VkPipelineMultisampleStateCreateInfo& multisampling,
        const VkPipelineDepthStencilStateCreateInfo& depthStencil,
        const VkPipelineColorBlendStateCreateInfo& colorBlending) const noexcept;

    VkGraphicsPipelineCreateInfo CreateTextGraphicsPipelineCreateInfo(
        const VkPipelineShaderStageCreateInfo shaderStages[],
        const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
        const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
        const VkPipelineViewportStateCreateInfo& viewportState,
        const VkPipelineRasterizationStateCreateInfo& rasterizer,
        const VkPipelineMultisampleStateCreateInfo& multisampling,
        const VkPipelineDepthStencilStateCreateInfo& depthStencil,
        const VkPipelineColorBlendStateCreateInfo& colorBlending) const noexcept;
    
    VkShaderModuleCreateInfo CreateShaderModuleCreateInfo(
        const std::vector<char>& code) const noexcept;
    VkFramebufferCreateInfo CreateFramebufferCreateInfo(
        const VkImageView& attachments) const noexcept;
    VkCommandPoolCreateInfo CreateCommandPoolCreateInfo(QueueFamilyIndices& queueFamilyIndices) const noexcept;
    VkCommandBufferBeginInfo CreateCommandBufferBeginInfo() const noexcept;
    VkRenderPassBeginInfo CreateRenderPassBeginInfo(size_t swapchainBufferNumber,
        const VkClearValue& clearValue) const noexcept;
    VkSemaphoreCreateInfo CreateSemaphoreCreateInfo() const noexcept;
    VkPresentInfoKHR CreatePresentInfoKHR(uint32_t& imageIndex) const noexcept;
    bool IsDeviceSuitable(const VkPhysicalDevice& device) const;
    QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device) const;
    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device) const;
    SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device) const;
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const noexcept;
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const noexcept;
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const wxSize& size) const noexcept;
    static std::vector<char> ReadFile(const std::string& filename);
    void CreateShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule) const;
    virtual void OnPaint(wxPaintEvent& event);
    virtual void OnResize(wxSizeEvent& event);
    void OnPaintException(const std::string& msg);

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // member fields
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT debugMessenger; //chai30
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; //chai30
    VkDevice m_logicalDevice;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_swapchainImages;
    VkFormat m_swapchainImageFormat;
public:
    VkExtent2D m_swapchainExtent;
    VkPipelineLayout m_pipelineLayout;
    VkPipelineCache m_pipelineCache; //sally 1127
    
private:
    std::vector<VkImageView> m_swapchainImageViews;
    std::vector<VkFramebuffer> m_swapchainFramebuffers;

    VkRenderPass m_renderPass;
    VkDescriptorSetLayout descriptorSetLayout; //chai30
    VkDescriptorSetLayout textDescriptorSetLayout; //chai30

public:
    VkPipeline m_graphicsPipeline; // general 3D pipeline

private:
    VkCommandPool m_commandPool;

    VkImage colorImage; //chai30
    VkDeviceMemory colorImageMemory; //chai30
    VkImageView colorImageView; //chai30

    VkImage depthImage; //chai30
    VkDeviceMemory depthImageMemory; //chai30
    VkImageView depthImageView; //chai30

    uint32_t mipLevels; //chai30

    std::vector<VkCommandBuffer> m_commandBuffers;
    VulkanRenderInfo *renderInfo; // chai1094

    std::vector<VkSemaphore> m_imageAvailableSemaphores; //sally16
    std::vector<VkSemaphore> m_renderFinishedSemaphores; //sally16
    //VkSemaphore m_imageAvailableSemaphore;
    //VkSemaphore m_renderFinishedSemaphore;
    std::vector<VkFence> inFlightFences; //sally16
public:
    uint32_t currentFrame = 0; //sally16
private:
    bool framebufferResized = false; //sally17

    bool m_vulkanInitialized;

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // OnIdle
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
public:
    bool render_loop_on;
    void onIdle(wxIdleEvent& evt);
    void activateRenderLoop(bool on);

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // ch30
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    //     static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // sally init/load obj
    //==========================================================================
    //==========================================================================
    //==========================================================================
    std::vector<ObjLoader*> objLoaders;

    void createVertexBufferAndIndexBuffer(std::vector<Vertex> vertices, std::vector<MyFace> myFaces, VkBuffer *pVertexBuffer,
                                          VkBuffer *pIndexBuffer, VkDeviceMemory *pVertexBufferMemory, VkDeviceMemory *pIndexBufferMemory); // called by VulkanObjLoader::createVertexBufferAndIndexBuffer()
    void updateIndexBuffer(std::vector<MyFace> myFaces, VkBuffer* pIndexBuffer, VkDeviceMemory* pIndexBufferMemory); // called by BBMyObj::update()

    void createTexture(std::string textureFilename, VkImage* curTextureImage, VkDeviceMemory* curTextureImageMemory, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler); // called by VulkanObjLoader::createTexture()

    void createUniformBuffers(std::vector<VkBuffer> *curUniformBuffers, std::vector<VkDeviceMemory> *curUniformBuffersMemory, std::vector<void*> *curUniformBuffersMapped); // called by VulkanObjLoader::createUniformBuffers()

    void createDescriptorPool(VkDescriptorPool *curDescriptorPool);
    void createTextDescriptorPool(VkDescriptorPool* curDescriptorPool);
    void createDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, std::vector<VkBuffer>* curUniformBuffers, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler);

    //void createCommandBuffers(std::vector<VkCommandBuffer> *curCommandBuffers);
    VkCommandBufferAllocateInfo CreateCommandBufferAllocateInfo(std::vector<VkCommandBuffer>* curCommandBuffers) const noexcept; // not called?????????????

    void drawFrame(double deltaTime);
    void recordCommandBuffer(double deltaTime, VulkanRenderInfo *renderInfo, uint32_t imageIndex, uint32_t currentFrame);

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // camera
    //==========================================================================
    //==========================================================================
    //==========================================================================
    enum CameraType { lookat, firstperson };
    CameraType type = CameraType::firstperson;

    //bool mouseLookOn = true;
    bool mouseLookOn = false;

    float rotationSpeed = 1.0f;
    float movementSpeed = 1.0f;

    bool updated = false;
    bool flipY = false;

    float fov;
    float znear, zfar;

    glm::vec4 camViewPos;
    glm::vec3 camRotation;
    glm::vec3 camPosition;
    glm::vec3 camTarget;

    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forward;

    void initCamera(); // called by constructor

    void setMovementSpeed(float movementSpeed);
    void setRotationSpeed(float rotationSpeed);

    void updateCamera(double delta); // called by drawFrame()
    void updateViewMatrix(); // called by updateCamera()

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // mouse + key events
    //==========================================================================
    //==========================================================================
    //==========================================================================
    float mouseX;
    float mouseY;
    //--------------------------------------------------------------------------
    // keyboard
    //--------------------------------------------------------------------------
    struct {
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
        bool shiftKeyPressed = false;
        bool fKeyPressed = false;
    } keys;

    //--------------------------------------------------------------------------
    // panning/rotation/zoom
    //--------------------------------------------------------------------------
    bool rotationOn;
    bool panningOn;
    bool zoomOn;

    //-------------------------------------------------------------------------- panning
    float pixelRatio;
    float dragStartX;
    float dragStartY;
    float dragEndX;
    float dragEndY;
    float xDraggingDiff;
    float yDraggingDiff;

    //-------------------------------------------------------------------------- rotation
    float rotDeltaAngleX; // a movement from left to right = 2*PI = 360 deg
    float rotDeltaAngleY;  // a movement from top to bottom = PI = 180 deg
    float rotXAngle;
    float rotYAngle;
    
    //--------------------------------------------------------------------------
    // event handlers
    //--------------------------------------------------------------------------
    void onKeyDown(wxKeyEvent& event);
    void onKeyUp(wxKeyEvent& event);

    void onMouseMoved(wxMouseEvent& event);
    void onMouseDown(wxMouseEvent& event);
    void onMouseReleased(wxMouseEvent& event);
    void onMMBReleased(wxMouseEvent& event);
    void onMouseWheel(wxMouseEvent& event);

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // UI/2D
    //==========================================================================
    //==========================================================================
    //==========================================================================
    Timer timer; // to count fps
    int fps;
    wxStaticText *fpsText;

    void initUI();
    void updateUI();

    void drawHUD();

    //--------------------------------------------------------------------------
    // text
    //--------------------------------------------------------------------------
    VkImage textTextureImage;
    VkDeviceMemory textTextureImageMemory;
    VkImageView textTextureImageView;
    VkSampler textTextureImageSampler;

    VkDescriptorPool textDescriptorPool;
    VkDescriptorSet textDescriptorSet;
    std::vector<VkDescriptorSet> textDescriptorSets;

    std::string fontImageFilename = "Arial.png";

    //int frameBufferCount = 3;

    void createTextDescriptorSets(VkDescriptorPool* curDescriptorPool, std::vector<VkDescriptorSet>* curDescriptorSets, VkImageView* curTextureImageView, VkSampler* curTextureImageSampler);
    void createTextVertexBuffer(VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory); // don't need to give text vertices as we are just creating space
    void fillTextVertexBuffer(std::vector<TextVertex> textVertices, VkBuffer* pTextVertexBuffer, VkDeviceMemory* pTextVertexBufferMemory);

    void CreateTextGraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile); // text pso

    void initText();
    
    VkPipeline textPSO; // pso containing a pipeline state (m_graphicsPipeline = general PSO)
    VkPipelineLayout textPipelineLayout;

    Font arialFont; // this will store our arial font information
    
    int maxNumTextCharacters = 1024; // the maximum number of characters you can render during a frame. This is just used to make sure

    VkBuffer textVertexBuffer;
    VkDeviceMemory textVertexBufferMemory;
    //VkBufferView textVertexBufferView[FRAMEBUFFER_COUNT]; // ?????????????????????????????????????????????
    //uint8_t* textVBGPUAddress[FRAMEBUFFER_COUNT]; // this is a pointer to each of the text constant buffers
    
    Font LoadFont(const std::string& filename, int windowWidth, int windowHeight); // load a font
    void RenderText(VkCommandBuffer commandBuffer, Font font, std::string text, glm::vec2 pos, glm::vec2 scale = glm::vec2(1.0f, 1.0f), glm::vec2 padding = glm::vec2(0.5f, 0.0f), glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    //void RenderText(uint32_t imageIndex, VkCommandBuffer commandBuffer, Font font, std::wstring text, glm::vec2 pos, glm::vec2 scale = glm::vec2(1.0f, 1.0f), glm::vec2 padding = glm::vec2(0.5f, 0.0f), glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // MD5 Models (Animated)
    //==========================================================================
    //==========================================================================
    //==========================================================================
    const std::vector<std::string> md5MeshFilenames = {
        //"models/boblampclean.md5mesh",
    };

    const std::vector<std::string> md5AnimFilenames = {
        "models/boblampclean.md5anim",
    };

    std::vector<VulkanMD5Model*> vulkanMD5Models;

    void md5Test();
    
    void createMD5VertexBufferAndIndexBuffer(std::vector<Vertex> vertices, std::vector<MyFace> indices, VkBuffer* pVertexBuffer, VkBuffer* pIndexBuffer,
                                       VkDeviceMemory* pVertexBufferMemory, VkDeviceMemory* pIndexBufferMemory);
    void fillMD5VertexBufferAndIndexBuffer(std::vector<Vertex> vertices, std::vector<MyFace> indices, VkBuffer* pVertexBuffer, VkBuffer* pIndexBuffer,
                                           VkDeviceMemory* pVertexBufferMemory, VkDeviceMemory* pIndexBufferMemory);

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // SceneNodes/GameObjects
    //==========================================================================
    //==========================================================================
    //==========================================================================
    //std::vector<SceneNode*> sceneNodes;
    FilesPanel *filesPanel;
    HierarchyPanel *hierarchyPanel;
    SceneNode *sceneNodesRoot;

    wxStaticText *curSelectedFile;

    void setFilesPanel(FilesPanel *filesPanel);
    void setHierarchyPanel(HierarchyPanel *hierarchyPanel);

    //--------------------------------------------------------------------------
    // ray/object picking
    //--------------------------------------------------------------------------
    struct Ray {
        glm::vec3 origin; // rayOrigin (camera position)
        glm::vec3 direction; // rayDir (normalized)
    };

    bool objDragOn = false;

    glm::vec3 castRay(double screenX, double screenY);
    bool twoDimBoxAndRay(float x1, float y1, float x2, float y2, float r0x, float r0y, float rx, float ry); //helper fn of rayBoxIntersect
    bool rayBoxIntersect(BBMyObj *bb, Ray ray);

    //--------------------------------------------------------------------------
    // boundary highlight
    //--------------------------------------------------------------------------
    VkPipeline outlinePSO; // pso containing a pipeline state (m_graphicsPipeline = general PSO)
    GameObject *curSelectedObj;

    void CreateOutlineGraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile, const std::string& outlineVertexShaderFile, const std::string& outlineFragmentShaderFile);
};
