#include <cstring>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

namespace vulk
{
class WindowSurface;
class Device;
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

class Device
{
    uint32_t present_family;
    uint32_t graphics_family;

    VkSurfaceCapabilitiesKHR device_capabilities;
    VkPhysicalDevice physical_device;
    VkDevice device;

public:
    Device(Instance& instance, WindowSurface& surface)
    {
        const char* deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        uint32_t deviceCount{};
        vkEnumeratePhysicalDevices(instance.instance, &deviceCount, NULL);
        if (deviceCount == 0) {
            std::cerr << "PhysicalDevice() fail: "
                      << "there are no physical devices that support vulkan"
                      << "\n";
            throw 1;
        }
        std::vector<VkPhysicalDevice> physical_devices{deviceCount};
        vkEnumeratePhysicalDevices(
            instance.instance,
            &deviceCount,
            &physical_devices[0]
        );

        VkPhysicalDevice physical_device{VK_NULL_HANDLE};
        for (uint32_t i = 0;
             i < deviceCount && physical_device == VK_NULL_HANDLE;
             i++) {
            uint32_t queueFamilyCount{0};
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
                    present_family = j;
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
                    &device_capabilities
                );
                //  Get formats
                uint32_t formatCount{};
                vkGetPhysicalDeviceSurfaceFormatsKHR(
                    physical_devices[i],
                    surface.surface,
                    &formatCount,
                    NULL
                );
                std::vector<VkSurfaceFormatKHR> formats{};
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
                std::vector<VkPresentModeKHR> presentModes{};
                if (presentModeCount) {
                    for (size_t k = 0; k < presentModeCount; k++) {
                        presentModes.push_back({});
                    }
                    vkGetPhysicalDeviceSurfacePresentModesKHR(
                        physical_devices[i],
                        surface.surface,
                        &presentModeCount,
                        &presentModes[0]
                    );
                }
                //  Select device if formats and presentation modes are
                //  acceptable
                if (formats.size() && presentModes.size()) {
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
        uint32_t infoCount = (graphics_family != present_family) ? 2 : 1;
        float queuePriority = 1;
        uint32_t uniqueQueueFamilies[] = {graphics_family, present_family};
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
        VkQueue graphicsQueue{};
        VkQueue presentQueue{};

        vkGetDeviceQueue(device, graphics_family, 0, &graphicsQueue);
        vkGetDeviceQueue(device, present_family, 0, &presentQueue);

        //  Create command pool
        VkCommandPoolCreateInfo poolInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = graphics_family,
        };

        VkCommandPool commandPool;

        if (vkCreateCommandPool(device, &poolInfo, NULL, &commandPool)) {
            std::cerr << "Device() fail: " << "failed to create a command pool"
                      << "\n";
            throw 1;
        }
    }
    ~Device() { vkDestroyDevice(device, NULL); }
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
    vulk::Device pdevice(instance, surface);
}
