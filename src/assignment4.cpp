#include <cstring>
#include <optional>
#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <iostream>
#include <vector>

namespace vulk
{

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"
};
const bool enableValidationLayers = true;

bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

class Instance;
class PhysicalDevice;
class LogicalDevice;
class Queue;

class Instance
{
    VkInstance instance;

    friend PhysicalDevice;

public:
    Instance()
    {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error(
                "validations layers requested, but not available\n"
            );
        }
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        // turn on validation layer
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance\n");
        } else {
            std::cout << "Instance created\n";
        }
    }
    ~Instance()
    {
        vkDestroyInstance(instance, nullptr);
        std::cout << "Instance destroyed\n";
    }
};

struct QueueFamilyIndexes {
    std::optional<uint32_t> graphicsFamilyIndex;
    QueueFamilyIndexes(VkPhysicalDevice physicalDevice)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice,
            &queueFamilyCount,
            nullptr
        );
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice,
            &queueFamilyCount,
            queueFamilies.data()
        );
        {
            for (auto const& f : queueFamilies) {
                uint32_t i = 0;
                if (f.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    graphicsFamilyIndex = i;
                }
                i++;
            }
        }
        std::cout << "QueueFamilyIndexes created\n";
    }
};

class PhysicalDevice
{
    VkPhysicalDevice physicalDevice;
    uint32_t graphicsFamilyIndex;

    friend LogicalDevice;

public:
    bool isDeviceSuitable(VkPhysicalDevice device) { return true; }
    QueueFamilyIndexes availableQueueFamilies()
    {
        QueueFamilyIndexes queueFamilyIndexes(physicalDevice);
        return queueFamilyIndexes;
    }
    PhysicalDevice(Instance& instance)
    {
        physicalDevice = VK_NULL_HANDLE;
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance.instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(
            instance.instance,
            &deviceCount,
            devices.data()
        );
        for (auto const& d : devices) {
            if (isDeviceSuitable(d)) {
                physicalDevice = d;
                break;
            }
        }
        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        std::cout << "PhysicalDevice created\n";
    }
};

class LogicalDevice
{
    VkDevice device;

    friend Queue;

public:
    LogicalDevice(PhysicalDevice& physical_device)
    {

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        QueueFamilyIndexes qFamilyIndexes =
            physical_device.availableQueueFamilies();

        VkDeviceQueueCreateInfo qCreateInfo{};
        qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        if (!qFamilyIndexes.graphicsFamilyIndex.has_value()) {
            throw std::runtime_error(
                "LogicalDevice required graphics family index"
            );
        }

        qCreateInfo.queueFamilyIndex =
            qFamilyIndexes.graphicsFamilyIndex.value();
        qCreateInfo.queueCount = 1;

        float qPriority = 1.0f;
        qCreateInfo.pQueuePriorities = &qPriority;

        createInfo.pQueueCreateInfos = &qCreateInfo;
        createInfo.queueCreateInfoCount = 1;

        VkPhysicalDeviceFeatures deviceFeatures{};
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();

        VkResult result = vkCreateDevice(
            physical_device.physicalDevice,
            &createInfo,
            nullptr,
            &device
        );
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }
        std::cout << "LogicalDevice created\n";
    }
    ~LogicalDevice()
    {
        vkDestroyDevice(device, nullptr);
        std::cout << "LogicalDevice destroyed\n";
    }
};

struct Queue {
    VkQueue q;

    Queue(LogicalDevice& device, PhysicalDevice& pDevice, uint32_t qIndex)
    {
        vkGetDeviceQueue(
            device.device,
            pDevice.availableQueueFamilies().graphicsFamilyIndex.value(),
            qIndex,
            &q
        );
    }
};

}; // namespace vulk

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(600, 600, "Vulk", nullptr, nullptr);

    uint32_t extensionCount{};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extensionCount,
        extensions.data()
    );
    std::cout << "available extensions:\n";
    for (auto const& e : extensions) {
        std::cout << "\t" << e.extensionName << "\n";
    }

    vulk::Instance instance{};
    vulk::PhysicalDevice physical_device(instance);
    vulk::LogicalDevice device(physical_device);
    vulk::Queue graphics_q(device, physical_device, 0);

    /*
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    */

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
