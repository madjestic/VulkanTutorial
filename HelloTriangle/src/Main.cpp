#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <set>

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT( VkInstance instance
									 , const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
									 , const VkAllocationCallbacks* pAllocator
									 , VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


// 888    8888888888888888     888      .d88888b.888888888888888888b. 8888888       d8888888b    888 .d8888b. 888     8888888888       d88888888888b. 8888888b. 888     8888888 .d8888b.        d8888888888888888888888 .d88888b. 888b    888 
// 888    888888       888     888     d88P" "Y88b   888    888   Y88b  888        d888888888b   888d88P  Y88b888     888             d88888888   Y88b888   Y88b888       888  d88P  Y88b      d88888    888      888  d88P" "Y88b8888b   888 
// 888    888888       888     888     888     888   888    888    888  888       d88P88888888b  888888    888888     888            d88P888888    888888    888888       888  888    888     d88P888    888      888  888     88888888b  888 
// 88888888888888888   888     888     888     888   888    888   d88P  888      d88P 888888Y88b 888888       888     8888888       d88P 888888   d88P888   d88P888       888  888           d88P 888    888      888  888     888888Y88b 888 
// 888    888888       888     888     888     888   888    8888888P"   888     d88P  888888 Y88b888888  88888888     888          d88P  8888888888P" 8888888P" 888       888  888          d88P  888    888      888  888     888888 Y88b888 
// 888    888888       888     888     888     888   888    888 T88b    888    d88P   888888  Y88888888    888888     888         d88P   888888       888       888       888  888    888  d88P   888    888      888  888     888888  Y88888 
// 888    888888       888     888     Y88b. .d88P   888    888  T88b   888   d8888888888888   Y8888Y88b  d88P888     888        d8888888888888       888       888       888  Y88b  d88P d8888888888    888      888  Y88b. .d88P888   Y8888 
// 888    88888888888888888888888888888 "Y88888P"    888    888   T88b8888888d88P     888888    Y888 "Y8888P88888888888888888888d88P     888888       888       888888888888888 "Y8888P" d88P     888    888    8888888 "Y88888P" 888    Y888 

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow*              window;
	bool                     framebufferResized = false;

	VkInstance               instance;
	VkDebugUtilsMessengerEXT debugMessenger;
  
	VkPhysicalDevice         physicalDevice = VK_NULL_HANDLE;
	VkDevice                 device;
	VkQueue					 graphicsQueue;
	VkQueue					 presentQueue;

 	VkSurfaceKHR             surface;

    //   GGGG  LL      FFFFFFF WW      WW 
    //  GG  GG LL      FF      WW      WW 
    // GG      LL      FFFF    WW   W  WW 
    // GG   GG LL      FF       WW WWW WW 
    //  GGGGGG LLLLLLL FF        WW   WW  
	
  	void initWindow() {
  		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		
		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwSetKeyCallback(window, keyCallback);
	}

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
		app -> framebufferResized = true;
	}

	static void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int modifiers) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	void initVulkan() {
		createInstance();
		setupDebugCallback();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			// processInput(window);
			glfwPollEvents();
		}
	}

	void cleanup() {
		vkDestroyDevice(device, nullptr);
    
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}

    //  CCCCC  RRRRRR  EEEEEEE   AAA   TTTTTTT EEEEEEE IIIII NN   NN  SSSSS  TTTTTTT   AAA   NN   NN  CCCCC  EEEEEEE 
    // CC    C RR   RR EE       AAAAA    TTT   EE       III  NNN  NN SS        TTT    AAAAA  NNN  NN CC    C EE      
    // CC      RRRRRR  EEEEE   AA   AA   TTT   EEEEE    III  NN N NN  SSSSS    TTT   AA   AA NN N NN CC      EEEEE   
    // CC    C RR  RR  EE      AAAAAAA   TTT   EE       III  NN  NNN      SS   TTT   AAAAAAA NN  NNN CC    C EE      
    //  CCCCC  RR   RR EEEEEEE AA   AA   TTT   EEEEEEE IIIII NN   NN  SSSSS    TTT   AA   AA NN   NN  CCCCC  EEEEEEE 		
  
	void createInstance() {
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo          = {};
		appInfo.sType                      = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName           = "Hello Triangle";
		appInfo.applicationVersion         = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName                = "No Engine";
		appInfo.engineVersion              = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion                 = VK_API_VERSION_1_0;
                                       
		VkInstanceCreateInfo createInfo    = {};
		createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo        = &appInfo;

		auto extensions                    = getRequiredExtensions();
		createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount     = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames   = validationLayers.data();
		} else {
			createInfo.enabledLayerCount     = 0;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	void createSurface(){
		if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
			throw std::runtime_error("failed to create window surface!");
		}
	};

    // DDDDD   EEEEEEE VV     VV IIIII  CCCCC  EEEEEEE  SSSSS  
    // DD  DD  EE      VV     VV  III  CC    C EE      SS      
    // DD   DD EEEEE    VV   VV   III  CC      EEEEE    SSSSS  
    // DD   DD EE        VV VV    III  CC    C EE           SS 
    // DDDDDD  EEEEEEE    VVV    IIIII  CCCCC  EEEEEEE  SSSSS  
	

	void createLogicalDevice(){
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.has_value(), indices.presentFamily.has_value()};

		float queuePriority = 1.0f;

		for(uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
    
		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo    = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount  = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos     = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = 0;

		if(enableValidationLayers) {
			createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount   = 0;
		}

		if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS){
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.has_value(), 0, &graphicsQueue);
	};

	void pickPhysicalDevice(){
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!\n");
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU, SUKA!\n");
		}
	}

	bool isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		return indices.isComplete() && extensionsSupported;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}
		
		return requiredExtensions.empty();
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
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
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if(queueFamily.queueCount > 0 && presentSupport){
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	void setupDebugCallback() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	bool checkValidationLayerSupport() {
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

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
};

// 888b     d888       d88888888888888b    888 
// 8888b   d8888      d88888  888  8888b   888 
// 88888b.d88888     d88P888  888  88888b  888 
// 888Y88888P888    d88P 888  888  888Y88b 888 
// 888 Y888P 888   d88P  888  888  888 Y88b888 
// 888  Y8P  888  d88P   888  888  888  Y88888 
// 888   "   888 d8888888888  888  888   Y8888 
// 888       888d88P     8888888888888    Y888 

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
