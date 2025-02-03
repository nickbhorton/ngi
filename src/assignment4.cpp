#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

namespace vulk
{
class WindowSurface;
class PhysicalDevice;
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
    friend PhysicalDevice;

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

    friend PhysicalDevice;

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

class PhysicalDevice
{
    uint32_t present_family;
    uint32_t graphics_family;

public:
    PhysicalDevice(Instance& instance, WindowSurface& surface)
    {
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
        std::cout << "physical device count " << deviceCount << "\n";
        const char* deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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
        }
    }
};

}; // namespace vulk

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
    vulk::WindowSurface sufrace(instance, window);
    vulk::PhysicalDevice pdevice(instance);
}
