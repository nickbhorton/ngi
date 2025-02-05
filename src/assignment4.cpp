#include <cstring>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

//  Define cube
//  vlakkies typedefs
#define NFRAMES 2
#define RGBA_FORMAT VK_FORMAT_R8G8B8A8_SRGB // RGBA format

typedef struct {
    float x, y;
} vec2;
typedef struct {
    float x, y, z;
} vec3;
typedef struct {
    float x, y, z, w;
} vec4;
typedef struct {
    vec3 xyz, nml, rgb;
    vec2 st;
} Vertex;
typedef struct {
    float model[16], view[16], proj[16], norm[16]; // Transformation matrices
    vec4 pos, Ca, Cd, Cs;                          // Light properties
    vec4 Ks;                                       // Material properties
    float Ns;                                      //
} UniformBufferObject;

namespace vulk
{
class WindowSurface;
class Device;
class SwapChain;
}; // namespace vulk

class Glfw
{
public:
    Glfw() { glfwInit(); }
    ~Glfw() { glfwTerminate(); }
};

class Window
{
    GLFWwindow* window;

    friend vulk::WindowSurface;
    friend vulk::SwapChain;

public:
    Window(int width, int height, std::string const& title)
        : window{glfwCreateWindow(width, height, title.c_str(), NULL, NULL)}
    {
    }
    ~Window() { glfwDestroyWindow(window); }
};

namespace vulk
{

const char* ErrString(VkResult err)
{
    switch (err) {
    case VK_SUCCESS:
        return "VK_SUCCESS";
    case VK_NOT_READY:
        return "VK_NOT_READY";
    case VK_TIMEOUT:
        return "VK_TIMEOUT";
    case VK_EVENT_SET:
        return "VK_EVENT_SET";
    case VK_EVENT_RESET:
        return "VK_EVENT_RESET";
    case VK_INCOMPLETE:
        return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST:
        return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS:
        return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL:
        return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_UNKNOWN:
        return "VK_ERROR_UNKNOWN";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    case VK_ERROR_FRAGMENTATION:
        return "VK_ERROR_FRAGMENTATION";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
    case VK_ERROR_SURFACE_LOST_KHR:
        return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR:
        return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return "VK_ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV:
        return "VK_ERROR_INVALID_SHADER_NV";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
#ifdef VK_API_VERSION_1_3_0
    case VK_PIPELINE_COMPILE_REQUIRED:
        return "VK_PIPELINE_COMPILE_REQUIRED";
    case VK_ERROR_NOT_PERMITTED_KHR:
        return "VK_ERROR_NOT_PERMITTED_KHR";
    case VK_THREAD_IDLE_KHR:
        return "VK_THREAD_IDLE_KHR";
    case VK_THREAD_DONE_KHR:
        return "VK_THREAD_DONE_KHR";
    case VK_OPERATION_DEFERRED_KHR:
        return "VK_OPERATION_DEFERRED_KHR";
    case VK_OPERATION_NOT_DEFERRED_KHR:
        return "VK_OPERATION_NOT_DEFERRED_KHR";
#endif
    default:
        return "Unknown VkResult";
    }
}

class Instance
{
    VkInstance instance;

    friend WindowSurface;
    friend Device;

public:
    Instance()
    {
        VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Vulkan Textured Cube",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_0,
        };

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions =
            glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        VkInstanceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount = glfwExtensionCount,
            .ppEnabledExtensionNames = glfwExtensions,
        };
        VkResult ios = vkCreateInstance(&createInfo, NULL, &instance);
        if (ios) {
            std::cerr << "Instance() fail " << ErrString(ios) << "\n";
            throw 1;
        }
    }
    ~Instance() { vkDestroyInstance(instance, NULL); }
};

class WindowSurface
{
    VkSurfaceKHR surface;
    Instance& bound_instance;

    friend Device;
    friend SwapChain;

public:
    WindowSurface(Instance& instance, Window& window) : bound_instance(instance)
    {
        VkResult ios = glfwCreateWindowSurface(
            bound_instance.instance,
            window.window,
            NULL,
            &surface
        );
        if (ios) {
            std::cerr << "WindowSurface() fail " << ErrString(ios) << "\n";
            throw 1;
        }
    }
    ~WindowSurface()
    {
        vkDestroySurfaceKHR(bound_instance.instance, surface, NULL);
    }
};

struct Device {
    uint32_t presentation_family;
    uint32_t graphics_family;

    VkDevice device;
    VkPhysicalDevice physical_device;
    VkSurfaceCapabilitiesKHR capabilities;

    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentation_modes;

    VkQueue graphics_q;
    VkQueue presentation_q;

    VkCommandPool commandPool;

    friend SwapChain;

    Device(Instance& instance, WindowSurface& surface)
        : formats{}, presentation_modes{}, device{}, physical_device{},
          capabilities{}, graphics_q{}, presentation_q{}
    {
        const char* deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        uint32_t physical_device_count{};
        vkEnumeratePhysicalDevices(
            instance.instance,
            &physical_device_count,
            NULL
        );
        if (physical_device_count == 0) {
            std::cerr << "PhysicalDevice() fail: "
                      << "there are no physical devices that support vulkan"
                      << "\n";
            throw 1;
        }
        std::vector<VkPhysicalDevice> physical_devices{physical_device_count};
        vkEnumeratePhysicalDevices(
            instance.instance,
            &physical_device_count,
            &physical_devices[0]
        );

        physical_device = VK_NULL_HANDLE;
        for (uint32_t i = 0;
             i < physical_devices.size() && physical_device == VK_NULL_HANDLE;
             i++) {
            uint32_t queueFamilyCount{};
            vkGetPhysicalDeviceQueueFamilyProperties(
                physical_devices[i],
                &queueFamilyCount,
                NULL
            );
            std::vector<VkQueueFamilyProperties> queueFamilies{queueFamilyCount
            };
            vkGetPhysicalDeviceQueueFamilyProperties(
                physical_devices[i],
                &queueFamilyCount,
                &queueFamilies[0]
            );
            VkBool32 suitable = VK_FALSE;
            for (int32_t j = 0; j < queueFamilyCount && !suitable; j++) {
                VkBool32 presentSupport = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(
                    physical_devices[i],
                    j,
                    surface.surface,
                    &presentSupport
                );
                VkPhysicalDeviceFeatures supportedFeatures;
                vkGetPhysicalDeviceFeatures(
                    physical_devices[i],
                    &supportedFeatures
                );
                if (presentSupport &&
                    queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                    supportedFeatures.samplerAnisotropy) {
                    suitable = VK_TRUE;
                    presentation_family = j;
                    graphics_family = j;
                }
            }

            //  Check that all required device extentions are supported
            // std::cout << "physical device count " << deviceCount << "\n";
            if (suitable) {
                //  Get device extensions supported
                uint32_t extensionCount;
                vkEnumerateDeviceExtensionProperties(
                    physical_devices[i],
                    NULL,
                    &extensionCount,
                    NULL
                );
                std::vector<VkExtensionProperties> availableExtensions{
                    extensionCount
                };
                vkEnumerateDeviceExtensionProperties(
                    physical_devices[i],
                    NULL,
                    &extensionCount,
                    &availableExtensions[0]
                );

                //  Check if device extentions match
                for (int k = 0;
                     k < sizeof(deviceExtensions) / sizeof(char*) && suitable;
                     k++) {
                    VkBool32 match = VK_FALSE;
                    for (int i = 0; i < extensionCount && !match; i++)
                        if (!strcmp(
                                deviceExtensions[k],
                                availableExtensions[i].extensionName
                            )) {
                            match = VK_TRUE;
                        }
                    if (!match) {
                        suitable = VK_FALSE;
                    }
                }
            }

            //  Get capabilities if device is suitable
            if (suitable) {
                //  Get device capabilities
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                    physical_devices[i],
                    surface.surface,
                    &capabilities
                );
                //  Get formats
                uint32_t formatCount{};
                vkGetPhysicalDeviceSurfaceFormatsKHR(
                    physical_devices[i],
                    surface.surface,
                    &formatCount,
                    NULL
                );
                if (formatCount) {
                    for (size_t k = 0; k < formatCount; k++) {
                        formats.push_back({});
                    }
                    vkGetPhysicalDeviceSurfaceFormatsKHR(
                        physical_devices[i],
                        surface.surface,
                        &formatCount,
                        &formats[0]
                    );
                }
                //  Get presentation modes
                uint32_t presentModeCount{};
                vkGetPhysicalDeviceSurfacePresentModesKHR(
                    physical_devices[i],
                    surface.surface,
                    &presentModeCount,
                    NULL
                );
                if (presentModeCount) {
                    for (size_t k = 0; k < presentModeCount; k++) {
                        presentation_modes.push_back({});
                    }
                    vkGetPhysicalDeviceSurfacePresentModesKHR(
                        physical_devices[i],
                        surface.surface,
                        &presentModeCount,
                        &presentation_modes[0]
                    );
                }
                //  Select device if formats and presentation modes are
                //  acceptable
                if (formats.size() && presentation_modes.size()) {
                    physical_device = physical_devices[i];
                }
            }
        }
        if (physical_device == VK_NULL_HANDLE) {
            std::cerr
                << "Device() fail: "
                << "there are no physical devices that meet specificiation."
                << "\n";
            throw 1;
        }

        //  Create logical device
        uint32_t infoCount = (graphics_family != presentation_family) ? 2 : 1;
        float queuePriority = 1;
        uint32_t uniqueQueueFamilies[] = {graphics_family, presentation_family};
        VkDeviceQueueCreateInfo queueCreateInfos[2];
        for (int i = 0; i < infoCount; i++) {
            VkDeviceQueueCreateInfo queueCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = uniqueQueueFamilies[i],
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,
            };
            queueCreateInfos[i] = queueCreateInfo;
        }
        VkPhysicalDeviceFeatures deviceFeatures = {
            .samplerAnisotropy = VK_TRUE
        };
        VkDeviceCreateInfo deviceInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = infoCount,
            .pQueueCreateInfos = queueCreateInfos,
            .enabledExtensionCount = sizeof(deviceExtensions) / sizeof(char*),
            .ppEnabledExtensionNames = deviceExtensions,
            .pEnabledFeatures = &deviceFeatures,
        };
        if (vkCreateDevice(physical_device, &deviceInfo, NULL, &device)) {
            std::cerr << "Device() fail: " << "failed to create a device\n";
            throw 1;
        }

        //  Get graphics and presentation queues
        vkGetDeviceQueue(device, graphics_family, 0, &graphics_q);
        vkGetDeviceQueue(device, presentation_family, 0, &presentation_q);

        //  Create command pool
        VkCommandPoolCreateInfo poolInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = graphics_family,
        };

        if (vkCreateCommandPool(device, &poolInfo, NULL, &commandPool)) {
            std::cerr << "Device() fail: " << "failed to create a command pool"
                      << "\n";
            throw 1;
        }
    }
    ~Device() { vkDestroyDevice(device, NULL); }
};

uint32_t findMemoryType(
    VkPhysicalDevice& physical_device,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties
)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memProperties);
    for (int k = 0; k < memProperties.memoryTypeCount; k++)
        if ((typeFilter & (1 << k)) &&
            (memProperties.memoryTypes[k].propertyFlags & properties) ==
                properties)
            return k;
    std::cerr << "Failed to find suitable memory type\n";
    return 0;
}

void CreateImage(
    VkDevice& device,
    VkPhysicalDevice& physical_device,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage* image,
    VkDeviceMemory* imageMemory
)
{
    const VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {.width = width, .height = height, .depth = 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    int ios = vkCreateImage(device, &imageInfo, NULL, image);
    if (ios) {
        std::cerr << "Failed to create %dx%d image: %s\n"
                  << width << height << ios << "\n";
    }

    //  Allocate memory for image
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *image, &memRequirements);
    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(
            physical_device,
            memRequirements.memoryTypeBits,
            properties
        ),
    };
    if (vkAllocateMemory(device, &allocInfo, NULL, imageMemory))
        std::cerr << "Failed to allocate image memory\n";

    //  Bind memory to image
    vkBindImageMemory(device, *image, *imageMemory, 0);
}

VkImageView CreateImageView(
    VkDevice& device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspectFlags
)
{
    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange =
            {.aspectMask = aspectFlags,
             .baseMipLevel = 0,
             .levelCount = 1,
             .baseArrayLayer = 0,
             .layerCount = 1}
    };
    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, NULL, &imageView)) {
        std::cerr << "SwapChain::create_image_view fail: "
                  << "could not create image view\n";
        throw 1;
    }
    return imageView;
}

class SwapChain
{
    uint32_t swap_count;

    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR presentation_mode = VK_PRESENT_MODE_FIFO_KHR;
    VkExtent2D extent;

    VkSwapchainKHR swap_chain;

    VkFormat swapChainImageFormat;
    VkFormat depthFormat;
    VkExtent2D swapChainExtent;
    VkRenderPass renderPass;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    std::vector<VkImageView> swapChainImageViews;

    std::vector<VkFramebuffer> swapChainFramebuffers;

    Device& device;

public:
    SwapChain(Window& window, WindowSurface& surface, Device& device)
        : surface_format{}, presentation_mode{VK_PRESENT_MODE_FIFO_KHR},
          device(device)
    {
        //  Set surface format
        for (int k = 0; k < device.formats.size(); k++)
            if (device.formats[k].format == VK_FORMAT_B8G8R8A8_SRGB &&
                device.formats[k].colorSpace ==
                    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                surface_format = device.formats[k];

        //  Set present mode
        for (int k = 0; k < device.presentation_modes.size(); k++)
            if (device.presentation_modes[k] == VK_PRESENT_MODE_MAILBOX_KHR)
                presentation_mode = device.presentation_modes[k];

        //  Set  extent
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            device.physical_device,
            surface.surface,
            &device.capabilities
        );
        if (device.capabilities.currentExtent.width != UINT32_MAX) {
            extent = device.capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window.window, &width, &height);
            if (width < device.capabilities.minImageExtent.width)
                width = device.capabilities.minImageExtent.width;
            if (width > device.capabilities.maxImageExtent.width)
                width = device.capabilities.maxImageExtent.width;
            if (height < device.capabilities.minImageExtent.height)
                height = device.capabilities.minImageExtent.height;
            if (height > device.capabilities.maxImageExtent.height)
                height = device.capabilities.maxImageExtent.height;
            extent.width = width;
            extent.height = height;
        }

        //  Set number of images
        uint32_t imageCount = device.capabilities.minImageCount + 1;
        if (device.capabilities.maxImageCount > 0 &&
            imageCount > device.capabilities.maxImageCount)
            imageCount = device.capabilities.maxImageCount;

        //  Create swap chain
        VkSwapchainCreateInfoKHR chainInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = surface.surface,
            .minImageCount = imageCount,
            .imageFormat = surface_format.format,
            .imageColorSpace = surface_format.colorSpace,
            .imageExtent = extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        };
        uint32_t queueFamilyIndices[] = {
            device.graphics_family,
            device.presentation_family
        };
        if (device.graphics_family != device.presentation_family) {
            chainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            chainInfo.queueFamilyIndexCount = 2;
            chainInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            chainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        chainInfo.preTransform = device.capabilities.currentTransform;
        chainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        chainInfo.presentMode = presentation_mode;
        chainInfo.clipped = VK_TRUE;
        chainInfo.oldSwapchain = VK_NULL_HANDLE;
        if (vkCreateSwapchainKHR(
                device.device,
                &chainInfo,
                NULL,
                &swap_chain
            )) {
            std::cerr << "SwapChain fail: " << "failed to create swapchain\n";
        }

        //  Create swap chain images
        vkGetSwapchainImagesKHR(device.device, swap_chain, &swap_count, NULL);
        std::vector<VkImage> swapChainImages{swap_count};
        vkGetSwapchainImagesKHR(
            device.device,
            swap_chain,
            &swap_count,
            &swapChainImages[0]
        );
        swapChainImageFormat = surface_format.format;
        swapChainExtent = extent;

        //  Create image views
        for (size_t i = 0; i < swap_count; i++) {
            swapChainImageViews.push_back({});
        }
        for (int i = 0; i < swap_count; i++) {
            swapChainImageViews[i] = CreateImageView(
                device.device,
                swapChainImages[i],
                swapChainImageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT
            );
        }

        //
        //  Create render pass if it does not exist
        //
        if (true) {
            //  Color buffer attachment
            VkAttachmentDescription colorAttachment = {
                .format = swapChainImageFormat,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            };
            //  Find optimal depth format
            VkFormatFeatureFlags features =
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
            depthFormat = VK_FORMAT_UNDEFINED;
            VkFormat formats[] = {
                VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D32_SFLOAT
            };
            for (int k = 0; k < sizeof(formats) / sizeof(VkFormat); k++) {
                VkFormatProperties props;
                vkGetPhysicalDeviceFormatProperties(
                    device.physical_device,
                    formats[k],
                    &props
                );
                if ((props.optimalTilingFeatures & features) == features)
                    depthFormat = formats[k];
            }
            if (depthFormat == VK_FORMAT_UNDEFINED) {
                std::cerr << "Failed to find supported depth format\n";
            }
            //  Depth buffer attachment
            VkAttachmentDescription depthAttachment = {
                .format = depthFormat,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            };
            VkAttachmentReference colorAttachmentRef = {
                .attachment = 0,
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };
            VkAttachmentReference depthAttachmentRef = {
                .attachment = 1,
                .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            VkSubpassDescription subpass = {
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .colorAttachmentCount = 1,
                .pColorAttachments = &colorAttachmentRef,
                .pDepthStencilAttachment = &depthAttachmentRef,
            };

            VkSubpassDependency dependency = {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                 VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            };

            VkAttachmentDescription attachments[] = {
                colorAttachment,
                depthAttachment
            };
            VkRenderPassCreateInfo renderPassInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount =
                    sizeof(attachments) / sizeof(VkAttachmentDescription),
                .pAttachments = attachments,
                .subpassCount = 1,
                .pSubpasses = &subpass,
                .dependencyCount = 1,
                .pDependencies = &dependency,
            };
            if (vkCreateRenderPass(
                    device.device,
                    &renderPassInfo,
                    NULL,
                    &renderPass
                ))
                std::cerr << "Failed to create render pass\n";
        }

        //  Create depth buffers
        CreateImage(
            device.device,
            device.physical_device,
            swapChainExtent.width,
            swapChainExtent.height,
            depthFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &depthImage,
            &depthImageMemory
        );
        depthImageView = CreateImageView(
            device.device,
            depthImage,
            depthFormat,
            VK_IMAGE_ASPECT_DEPTH_BIT
        );

        //
        //  Create frame buffers
        //
        for (size_t i = 0; i < swap_count; i++) {
            swapChainFramebuffers.push_back({});
        }
        for (int i = 0; i < swap_count; i++) {
            VkImageView attachments[] = {
                swapChainImageViews[i],
                depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = renderPass,
                .attachmentCount = sizeof(attachments) / sizeof(VkImageView),
                .pAttachments = attachments,
                .width = swapChainExtent.width,
                .height = swapChainExtent.height,
                .layers = 1,
            };

            if (vkCreateFramebuffer(
                    device.device,
                    &framebufferInfo,
                    NULL,
                    &swapChainFramebuffers[i]
                ))
                std::cerr << "Failed to create framebuffer\n";
        }
    }
    ~SwapChain()
    {
        vkDestroyImageView(device.device, depthImageView, NULL);
        vkDestroyImage(device.device, depthImage, NULL);
        vkFreeMemory(device.device, depthImageMemory, NULL);
        for (int k = 0; k < swap_count; k++)
            vkDestroyFramebuffer(device.device, swapChainFramebuffers[k], NULL);
        for (int k = 0; k < swap_count; k++)
            vkDestroyImageView(device.device, swapChainImageViews[k], NULL);
        vkDestroySwapchainKHR(device.device, swap_chain, NULL);
    }
};

//
//  Create a buffer and allocate memory
//
void CreateBuffer(
    VkPhysicalDevice& physical_device,
    VkDevice& device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer* buffer,
    VkDeviceMemory* bufferMemory
)
{
    //  Create buffer
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    int ios = vkCreateBuffer(device, &bufferInfo, NULL, buffer);
    if (ios)
        std::cerr << "Failed to create buffer: " << ErrString((VkResult)ios)
                  << "\n";

    //  Get memory requirements for buffer
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

    //  Allocate memory for buffer
    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(
            physical_device,
            memRequirements.memoryTypeBits,
            properties
        ),
    };
    if (vkAllocateMemory(device, &allocInfo, NULL, bufferMemory))
        std::cerr << "Failed to allocate buffer memory\n";

    //  Bind buffer and memory
    vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}

//
//  Copy buffer
//
void CopyBuffer(
    VkDevice& device,
    VkQueue& graphicsQueue,
    VkCommandPool& commandPool,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size
)
{
    //  Create command buffer
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    //  Set up one time copy
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    //  Set copy command
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VkBufferCopy copyRegion = {.size = size};
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    //  Perform copy
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    //  Free up command buffer
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

//
//  Create vertex buffer
//

struct VertexBuffer {
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VertexBuffer(
        VkDevice& device,
        VkPhysicalDevice& physical_device,
        VkQueue& graphics_q,
        VkCommandPool& commandPool
    )
    {
        const Vertex vertices[] = {
            //  Front
            {{-1, -1, +1}, {0, 0, +1}, {1, 0, 0}, {0, 0}},
            {{+1, -1, +1}, {0, 0, +1}, {1, 0, 0}, {1, 0}},
            {{+1, +1, +1}, {0, 0, +1}, {1, 0, 0}, {1, 1}},
            {{+1, +1, +1}, {0, 0, +1}, {1, 0, 0}, {1, 1}},
            {{-1, +1, +1}, {0, 0, +1}, {1, 0, 0}, {0, 1}},
            {{-1, -1, +1}, {0, 0, +1}, {1, 0, 0}, {0, 0}},
            //  Back
            {{+1, -1, -1}, {0, 0, -1}, {0, 0, 1}, {0, 0}},
            {{-1, -1, -1}, {0, 0, -1}, {0, 0, 1}, {1, 0}},
            {{-1, +1, -1}, {0, 0, -1}, {0, 0, 1}, {1, 1}},
            {{-1, +1, -1}, {0, 0, -1}, {0, 0, 1}, {1, 1}},
            {{+1, +1, -1}, {0, 0, -1}, {0, 0, 1}, {0, 1}},
            {{+1, -1, -1}, {0, 0, -1}, {0, 0, 1}, {0, 0}},
            //  Right
            {{+1, -1, +1}, {+1, 0, 0}, {1, 1, 0}, {0, 0}},
            {{+1, -1, -1}, {+1, 0, 0}, {1, 1, 0}, {1, 0}},
            {{+1, +1, -1}, {+1, 0, 0}, {1, 1, 0}, {1, 1}},
            {{+1, +1, -1}, {+1, 0, 0}, {1, 1, 0}, {1, 1}},
            {{+1, +1, +1}, {+1, 0, 0}, {1, 1, 0}, {0, 1}},
            {{+1, -1, +1}, {+1, 0, 0}, {1, 1, 0}, {0, 0}},
            //  Left
            {{-1, -1, -1}, {-1, 0, 0}, {0, 1, 0}, {0, 0}},
            {{-1, -1, +1}, {-1, 0, 0}, {0, 1, 0}, {1, 0}},
            {{-1, +1, +1}, {-1, 0, 0}, {0, 1, 0}, {1, 1}},
            {{-1, +1, +1}, {-1, 0, 0}, {0, 1, 0}, {1, 1}},
            {{-1, +1, -1}, {-1, 0, 0}, {0, 1, 0}, {0, 1}},
            {{-1, -1, -1}, {-1, 0, 0}, {0, 1, 0}, {0, 0}},
            //  Top
            {{-1, +1, +1}, {0, +1, 0}, {0, 1, 1}, {0, 0}},
            {{+1, +1, +1}, {0, +1, 0}, {0, 1, 1}, {1, 0}},
            {{+1, +1, -1}, {0, +1, 0}, {0, 1, 1}, {1, 1}},
            {{+1, +1, -1}, {0, +1, 0}, {0, 1, 1}, {1, 1}},
            {{-1, +1, -1}, {0, +1, 0}, {0, 1, 1}, {0, 1}},
            {{-1, +1, +1}, {0, +1, 0}, {0, 1, 1}, {0, 0}},
            //  Bottom
            {{-1, -1, -1}, {0, -1, 0}, {1, 0, 1}, {0, 0}},
            {{+1, -1, -1}, {0, -1, 0}, {1, 0, 1}, {1, 0}},
            {{+1, -1, +1}, {0, -1, 0}, {1, 0, 1}, {1, 1}},
            {{+1, -1, +1}, {0, -1, 0}, {1, 0, 1}, {1, 1}},
            {{-1, -1, +1}, {0, -1, 0}, {1, 0, 1}, {0, 1}},
            {{-1, -1, -1}, {0, -1, 0}, {1, 0, 1}, {0, 0}},
        };

        VkDeviceSize bufferSize = sizeof(vertices);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(
            physical_device,
            device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &stagingBuffer,
            &stagingBufferMemory
        );

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices, bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        CreateBuffer(
            physical_device,
            device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &vertexBuffer,
            &vertexBufferMemory
        );

        CopyBuffer(
            device,
            graphics_q,
            commandPool,
            stagingBuffer,
            vertexBuffer,
            bufferSize
        );

        vkDestroyBuffer(device, stagingBuffer, NULL);
        vkFreeMemory(device, stagingBufferMemory, NULL);
    }
};

//
//  Reverse n bytes
//
static void Reverse(void* x, const int n)
{
    char* ch = (char*)x;
    for (int k = 0; k < n / 2; k++) {
        char tmp = ch[k];
        ch[k] = ch[n - 1 - k];
        ch[n - 1 - k] = tmp;
    }
}

//
//  Start one time commands
//
VkCommandBuffer
BeginSingleTimeCommands(VkDevice& device, VkCommandPool& commandPool)
{
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

//
//  End one time commands
//
void EndSingleTimeCommands(
    VkDevice& device,
    VkCommandPool& commandPool,
    VkQueue& graphicsQueue,
    VkCommandBuffer& commandBuffer
)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void TransitionImageLayout(
    VkDevice& device,
    VkCommandPool& commandPool,
    VkQueue& graphicsQueue,
    VkImage image,
    VkFormat format,
    VkImageLayout oldLayout,
    VkImageLayout newLayout
)
{
    VkCommandBuffer commandBuffer =
        BeginSingleTimeCommands(device, commandPool);

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        }
    };

    VkPipelineStageFlags sourceStage = 0;
    VkPipelineStageFlags destinationStage = 0;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else
        std::cerr << "Unsupported layout transition\n";

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0,
        NULL,
        0,
        NULL,
        1,
        &barrier
    );

    EndSingleTimeCommands(device, commandPool, graphicsQueue, commandBuffer);
}

struct TextureImage {
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkSampler textureSampler;
    VkImageView textureImageView;

    Device& device;

    TextureImage(Device& device, const char* file) : device(device)
    {
        //  Open file
        FILE* f = fopen(file, "r");
        if (!f)
            std::cerr << "Cannot open file" << file << "\n";
        //  Check image magic
        unsigned short magic;
        if (fread(&magic, 2, 1, f) != 1)
            std::cerr << "Cannot read magic from" << file << "\n";
        if (magic != 0x4D42 && magic != 0x424D)
            std::cerr << "Image magic not BMP in" << file << "\n";
        //  Read header
        unsigned int dx, dy, off, k; // Image dimensions, offset and compression
        unsigned short nbp, bpp;     // Planes and bits per pixel
        if (fseek(f, 8, SEEK_CUR) || fread(&off, 4, 1, f) != 1 ||
            fseek(f, 4, SEEK_CUR) || fread(&dx, 4, 1, f) != 1 ||
            fread(&dy, 4, 1, f) != 1 || fread(&nbp, 2, 1, f) != 1 ||
            fread(&bpp, 2, 1, f) != 1 || fread(&k, 4, 1, f) != 1)
            std::cerr << "Cannot read header from" << file << "\n";
        //  Reverse bytes on big endian hardware (detected by backwards magic)
        if (magic == 0x424D) {
            Reverse(&off, 4);
            Reverse(&dx, 4);
            Reverse(&dy, 4);
            Reverse(&nbp, 2);
            Reverse(&bpp, 2);
            Reverse(&k, 4);
        }
        //  Get physical device properties
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device.physical_device, &properties);
        unsigned int max = properties.limits.maxImageDimension2D;
        //  Check image parameters
        if (dx < 1 || dx > max)
            std::cerr << file << " image width " << dx << " out of range 1-"
                      << max << "\n";
        if (dy < 1 || dy > max)
            std::cerr << file << " image height " << dy << " out of range 1-"
                      << max << "\n";
        if (nbp != 1)
            std::cerr << file << " bit planes is not 1: " << nbp << "\n";
        if (bpp != 24)
            std::cerr << file << " bits per pixel is not 24: " << bpp << "\n";
        if (k != 0)
            std::cerr << file << " compressed files not supported\n";

        //  Allocate image memory
        VkDeviceSize imageSize = 4 * dx * dy;
        unsigned char* image = (unsigned char*)malloc(imageSize);
        if (!image)
            std::cerr << "Cannot allocate " << imageSize
                      << " bytes of memory for image " << file << "\n";

        //  Seek to and read image
        if (fseek(f, off, SEEK_SET) || fread(image, 3 * dx * dy, 1, f) != 1)
            std::cerr << "Error reading data from image " << file << "\n";
        fclose(f);
        //  Map BGR to RGBA (RGBA is better supported)
        for (int i = dx * dy - 1; i >= 0; i--) {
            unsigned char R = image[3 * i + 2];
            unsigned char G = image[3 * i + 1];
            unsigned char B = image[3 * i + 0];
            image[4 * i + 0] = R;
            image[4 * i + 1] = G;
            image[4 * i + 2] = B;
            image[4 * i + 3] = 255;
        }

        //  Create staging buffer
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(
            device.physical_device,
            device.device,
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &stagingBuffer,
            &stagingBufferMemory
        );
        //  Copy image to buffer
        void* data;
        vkMapMemory(device.device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, image, imageSize);
        vkUnmapMemory(device.device, stagingBufferMemory);
        free(image);
        //  Create Image
        CreateImage(
            device.device,
            device.physical_device,
            dx,
            dy,
            RGBA_FORMAT,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &textureImage,
            &textureImageMemory
        );
        //  Transfer buffer to image
        TransitionImageLayout(
            device.device,
            device.commandPool,
            device.graphics_q,
            textureImage,
            RGBA_FORMAT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        );
        VkCommandBuffer commandBuffer =
            BeginSingleTimeCommands(device.device, device.commandPool);
        VkBufferImageCopy region = {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .imageOffset = {0, 0, 0},
            .imageExtent = {dx, dy, 1},
        };
        vkCmdCopyBufferToImage(
            commandBuffer,
            stagingBuffer,
            textureImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );
        EndSingleTimeCommands(
            device.device,
            device.commandPool,
            device.graphics_q,
            commandBuffer
        );
        TransitionImageLayout(
            device.device,
            device.commandPool,
            device.graphics_q,
            textureImage,
            RGBA_FORMAT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );

        //  Destroy staging buffer
        vkDestroyBuffer(device.device, stagingBuffer, NULL);
        vkFreeMemory(device.device, stagingBufferMemory, NULL);

        //  Create texture sampler
        VkSamplerCreateInfo samplerInfo = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
        };
        if (vkCreateSampler(device.device, &samplerInfo, NULL, &textureSampler))
            std::cerr << "Failed to create texture sampler\n";

        //  Create Image View
        textureImageView = CreateImageView(
            device.device,
            textureImage,
            RGBA_FORMAT,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
    }
    ~TextureImage()
    {
        vkDestroyImage(device.device, textureImage, NULL);
        vkFreeMemory(device.device, textureImageMemory, NULL);
        vkDestroySampler(device.device, textureSampler, NULL);
        vkDestroyImageView(device.device, textureImageView, NULL);
    }
};

//
//  Create shader module
//
VkShaderModule CreateShaderModule(VkDevice& device, const char* file)
{
    //  Open file
    FILE* f = fopen(file, "rb");
    if (!f)
        std::cerr << "Cannot open shader file " << file << "\n";
    //  Seek to end to determine size, then rewind
    fseek(f, 0, SEEK_END);
    int n = ftell(f);
    rewind(f);
    //  Allocate memory for the whole file
    uint32_t* code = (uint32_t*)malloc(n);
    if (!code)
        std::cerr << "Failed to malloc %d bytes for shader file " << file
                  << "\n";
    //  Snarf and close the file
    if (fread(code, n, 1, f) != 1)
        std::cerr << "Cannot read " << n << " bytes from shader file " << file
                  << "\n";
    fclose(f);
    //  Fill in shader struct
    VkShaderModuleCreateInfo shaderInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = static_cast<size_t>(n),
        .pCode = code,
    };
    //  Create shader
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &shaderInfo, NULL, &shaderModule))
        std::cerr << "Failed to create shader module\n";
    //  Free code and return
    free(code);
    return shaderModule;
}

//
//  Create graphics pipeline
//
struct GraphicsPipeline {
    VkBuffer uniformBuffers[NFRAMES];             // Uniform buffers
    VkDeviceMemory uniformBuffersMemory[NFRAMES]; // Uniform buffer memory
    void* uniformBuffersMapped[NFRAMES];          // Mapping for uniform buffer
    VkDescriptorPool descriptorPool;              // Descriptor pool
    VkDescriptorSet descriptorSets[NFRAMES];      // Descriptor sets
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;

    Device& device;

    GraphicsPipeline(Device& device, TextureImage& texture) : device(device)
    {
        //  Attributes for cube
        VkVertexInputBindingDescription bindingDescription = {
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };
        //  Layout for xyz, rgb and st
        VkVertexInputAttributeDescription attributeDescriptions[] = {
            {.location = 0,
             .format = VK_FORMAT_R32G32B32_SFLOAT,
             .offset = offsetof(Vertex, xyz)},
            {.location = 1,
             .format = VK_FORMAT_R32G32B32_SFLOAT,
             .offset = offsetof(Vertex, nml)},
            {.location = 2,
             .format = VK_FORMAT_R32G32B32_SFLOAT,
             .offset = offsetof(Vertex, rgb)},
            {.location = 3,
             .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(Vertex, st)},
        };
        //  Set input state
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount =
                sizeof(bindingDescription) /
                sizeof(VkVertexInputBindingDescription),
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount =
                sizeof(attributeDescriptions) /
                sizeof(VkVertexInputAttributeDescription),
            .pVertexAttributeDescriptions = attributeDescriptions,
        };

        //  Create uniform buffers
        for (int k = 0; k < NFRAMES; k++) {
            VkDeviceSize bufferSize = sizeof(UniformBufferObject);
            CreateBuffer(
                device.physical_device,
                device.device,
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &uniformBuffers[k],
                &uniformBuffersMemory[k]
            );
            vkMapMemory(
                device.device,
                uniformBuffersMemory[k],
                0,
                bufferSize,
                0,
                &uniformBuffersMapped[k]
            );
        }

        //  Uniform buffer layout
        VkDescriptorSetLayoutBinding uboLayoutBinding = {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags =
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = NULL,
        };
        VkDescriptorSetLayoutBinding samplerLayoutBinding = {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = NULL,
        };

        //  Create descriptor layout
        VkDescriptorSetLayoutBinding bindings[] = {
            uboLayoutBinding,
            samplerLayoutBinding
        };
        VkDescriptorSetLayoutCreateInfo layoutInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount =
                sizeof(bindings) / sizeof(VkDescriptorSetLayoutBinding),
            .pBindings = bindings,
        };
        if (vkCreateDescriptorSetLayout(
                device.device,
                &layoutInfo,
                NULL,
                &descriptorSetLayout
            ))
            std::cerr << "Failed to create descriptor set layout\n";

        //  Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &descriptorSetLayout,
        };
        if (vkCreatePipelineLayout(
                device.device,
                &pipelineLayoutInfo,
                NULL,
                &pipelineLayout
            )) {
            std::cerr << "Failed to create pipeline layout\n";
            throw 1;
        }

        //  Create descriptor pool
        VkDescriptorPoolSize poolSizes[] = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = NFRAMES},
            {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = NFRAMES},
        };

        VkDescriptorPoolCreateInfo dsPoolInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = NFRAMES,
            .poolSizeCount = sizeof(poolSizes) / sizeof(VkDescriptorPoolSize),
            .pPoolSizes = poolSizes,
        };
        if (vkCreateDescriptorPool(
                device.device,
                &dsPoolInfo,
                NULL,
                &descriptorPool
            )) {
            std::cerr << "Failed to create descriptor pool\n";
            throw 1;
        }

        //  Create descriptor sets
        VkDescriptorSetLayout layouts[NFRAMES];
        for (int k = 0; k < NFRAMES; k++)
            layouts[k] = descriptorSetLayout;
        VkDescriptorSetAllocateInfo dsAllocInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool,
            .descriptorSetCount = NFRAMES,
            .pSetLayouts = layouts,
        };
        if (vkAllocateDescriptorSets(
                device.device,
                &dsAllocInfo,
                descriptorSets
            )) {
            std::cerr << "Failed to allocate descriptor sets\n";
            throw 1;
        }
        //  Set descriptors for each frame
        for (int k = 0; k < NFRAMES; k++) {
            VkDescriptorBufferInfo bufferInfo = {
                .buffer = uniformBuffers[k],
                .offset = 0,
                .range = sizeof(UniformBufferObject),
            };
            VkDescriptorImageInfo imageInfo = {
                .sampler = texture.textureSampler,
                .imageView = texture.textureImageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };

            VkWriteDescriptorSet descriptorWrites[] = {
                {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = descriptorSets[k],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pBufferInfo = &bufferInfo,
                },
                {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = descriptorSets[k],
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = &imageInfo,
                },
            };

            vkUpdateDescriptorSets(
                device.device,
                sizeof(descriptorWrites) / sizeof(VkWriteDescriptorSet),
                descriptorWrites,
                0,
                NULL
            );
        }

        //  Select TRIANGLE LIST
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
            .sType =
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
        };

        //  Enable CULL FACE
        VkPipelineRasterizationStateCreateInfo rasterizer = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1,
        };

        //  Enable Z-buffer
        VkPipelineDepthStencilStateCreateInfo depthStencil = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        //  Disable multisampling
        VkPipelineMultisampleStateCreateInfo multisampling = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
        };

        //  Disable blending
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {
            .blendEnable = VK_FALSE,
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };

        //  Blend function copy
        VkPipelineColorBlendStateCreateInfo colorBlending = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
            .blendConstants{0, 0, 0, 0},
        };

        //  Enable viewport and scissors test
        VkPipelineViewportStateCreateInfo viewportState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1,
        };

        //  Allow viewport and scissors to change dynamically
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState),
            .pDynamicStates = dynamicStates,
        };

        //  Vertex shader module
        VkShaderModule vertShaderModule = CreateShaderModule(
            device.device,
            "res/assignment_shaders/4.vert.spv"
        );
        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertShaderModule,
            .pName = "main",
        };

        //  Fragment shader module
        VkShaderModule fragShaderModule = CreateShaderModule(
            device.device,
            "res/assignment_shaders/4.frag.spv"
        );
        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragShaderModule,
            .pName = "main",
        };

        //  Create graphics pipeline
        VkPipelineShaderStageCreateInfo shaderStages[] = {
            vertShaderStageInfo,
            fragShaderStageInfo
        };
        VkGraphicsPipelineCreateInfo pipelineInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = &depthStencil,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = pipelineLayout,
            .renderPass = renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
        };
        std::cout << "before" << std::endl;
        if (vkCreateGraphicsPipelines(
                device.device,
                VK_NULL_HANDLE,
                1,
                &pipelineInfo,
                NULL,
                &graphicsPipeline
            )) {
            std::cerr << "failed to create graphics pipeline" << std::endl;
        }
        std::cout << "after" << std::endl;

        //  Delete shader modules
        vkDestroyShaderModule(device.device, fragShaderModule, NULL);
        vkDestroyShaderModule(device.device, vertShaderModule, NULL);
    }
    ~GraphicsPipeline()
    {
        //  Destroy graphics pipeline
        vkDestroyPipeline(device.device, graphicsPipeline, NULL);
        vkDestroyPipelineLayout(device.device, pipelineLayout, NULL);
        vkDestroyRenderPass(device.device, renderPass, NULL);
    }
};

} // namespace vulk

int main()
{
    Glfw glfw{};
    if (!glfwVulkanSupported()) {
        std::cerr << "Vulkan is no supported\n";
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    Window window{600, 600, "Vulk"};
    vulk::Instance instance{};
    vulk::WindowSurface surface(instance, window);
    vulk::Device device(instance, surface);
    vulk::SwapChain swap_chain(window, surface, device);
    vulk::TextureImage texture(device, "res/img/pi.bmp");
    vulk::GraphicsPipeline(device, texture);
}
