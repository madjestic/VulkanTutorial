#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>


//  CCCCC   OOOOO  NN   NN  SSSSS  TTTTTTT   AAA   NN   NN TTTTTTT  SSSSS  
// CC    C OO   OO NNN  NN SS        TTT    AAAAA  NNN  NN   TTT   SS      
// CC      OO   OO NN N NN  SSSSS    TTT   AA   AA NN N NN   TTT    SSSSS  
// CC    C OO   OO NN  NNN      SS   TTT   AAAAAAA NN  NNN   TTT        SS 
//  CCCCC   OOOO0  NN   NN  SSSSS    TTT   AA   AA NN   NN   TTT    SSSSS  

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


// HH   HH EEEEEEE LL      PPPPPP  EEEEEEE RRRRRR   SSSSS  
// HH   HH EE      LL      PP   PP EE      RR   RR SS      
// HHHHHHH EEEEE   LL      PPPPPP  EEEEE   RRRRRR   SSSSS  
// HH   HH EE      LL      PP      EE      RR  RR       SS 
// HH   HH EEEEEEE LLLLLLL PP      EEEEEEE RR   RR  SSSSS



static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	std::cout << fileSize << std::endl;
	file.close();

	return buffer;
}

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

void DestroyDebugUtilsMessengerEXT( VkInstance instance
								  , VkDebugUtilsMessengerEXT debugMessenger
								  , const VkAllocationCallbacks* pAllocator) {
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

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR        capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>   presentModes;
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
 	VkSurfaceKHR             surface;
  
	VkPhysicalDevice         physicalDevice = VK_NULL_HANDLE;
	VkDevice                 device;
	VkQueue					 graphicsQueue;
	VkQueue					 presentQueue;

	VkSwapchainKHR           swapChain;
	std::vector<VkImage>     swapChainImages;
	VkFormat                 swapChainImageFormat;
	VkExtent2D               swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;

	VkRenderPass             renderPass;
	VkPipelineLayout         pipelineLayout;
	VkPipeline               graphicsPipeline;

    //   GGGG  LL      FFFFFFF WW      WW IIIII NN   NN IIIII TTTTTTT WW      WW IIIII NN   NN DDDDD    OOOOO  WW      WW 
    //  GG  GG LL      FF      WW      WW  III  NNN  NN  III    TTT   WW      WW  III  NNN  NN DD  DD  OO   OO WW      WW 
    // GG      LL      FFFF    WW   W  WW  III  NN N NN  III    TTT   WW   W  WW  III  NN N NN DD   DD OO   OO WW   W  WW 
    // GG   GG LL      FF       WW WWW WW  III  NN  NNN  III    TTT    WW WWW WW  III  NN  NNN DD   DD OO   OO  WW WWW WW 
    //  GGGGGG LLLLLLL FF        WW   WW  IIIII NN   NN IIIII   TTT     WW   WW  IIIII NN   NN DDDDDD   OOOO0    WW   WW  
    //  	
		
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

    // IIIII NN   NN IIIII TTTTTTT  VV     VV UU   UU LL      KK  KK   AAA   NN   NN 
    //  III  NNN  NN  III    TTT    VV     VV UU   UU LL      KK KK   AAAAA  NNN  NN 
    //  III  NN N NN  III    TTT     VV   VV  UU   UU LL      KKKK   AA   AA NN N NN 
    //  III  NN  NNN  III    TTT      VV VV   UU   UU LL      KK KK  AAAAAAA NN  NNN 
    // IIIII NN   NN IIIII   TTT       VVV     UUUUU  LLLLLLL KK  KK AA   AA NN   NN 
	
	void initVulkan() {
		createInstance();
		setupDebugCallback();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);
		
		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView,  nullptr);
          }
		
		vkDestroySwapchainKHR(device, swapChain, nullptr);
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

		VkApplicationInfo appInfo            = {};
		appInfo.sType                        = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName             = "Hello Triangle";
		appInfo.applicationVersion           = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName                  = "No Engine";
		appInfo.engineVersion                = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion                   = VK_API_VERSION_1_0;
                                       	     
		VkInstanceCreateInfo createInfo      = {};
		createInfo.sType                     = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo          = &appInfo;
										     
		auto extensions                      = getRequiredExtensions();
		createInfo.enabledExtensionCount     = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames   = extensions.data();

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

	void pickPhysicalDevice() {
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


	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies      = { indices.graphicsFamily.value()
											          , indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for(uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex        = queueFamily;
			queueCreateInfo.queueCount              = 1;
			queueCreateInfo.pQueuePriorities        = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
    
		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo           = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount         = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos            = queueCreateInfos.data();
		createInfo.pEnabledFeatures             = &deviceFeatures;
		createInfo.enabledExtensionCount        = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames      = deviceExtensions.data();

		if(enableValidationLayers) {
			createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount   = 0;
		}

		if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS){
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(),  0, &presentQueue);
	};

	bool isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported   = checkDeviceExtensionSupport(device);

		bool swapChainAdequate     = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.isComplete() && extensionsSupported && swapChainAdequate;
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

    //  SSSSS  WW      WW   AAA   PPPPPP   CCCCC  HH   HH   AAA   IIIII NN   NN 
    // SS      WW      WW  AAAAA  PP   PP CC    C HH   HH  AAAAA   III  NNN  NN 
    //  SSSSS  WW   W  WW AA   AA PPPPPP  CC      HHHHHHH AA   AA  III  NN N NN 
    //      SS  WW WWW WW AAAAAAA PP      CC    C HH   HH AAAAAAA  III  NN  NNN 
    //  SSSSS    WW   WW  AA   AA PP       CCCCC  HH   HH AA   AA IIIII NN   NN

	void createSwapChain(){
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR   presentMode   = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D         extent        = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType		             = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface		             = surface;
		createInfo.minImageCount             = imageCount;
		createInfo.imageFormat	             = surfaceFormat.format;
		createInfo.imageColorSpace           = surfaceFormat.colorSpace;
		//createInfo.imageExtent	             = extent;
		createInfo.imageArrayLayers          = 1;
		createInfo.imageUsage                = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
								   	         
		QueueFamilyIndices indices           = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[]        = { indices.graphicsFamily.value(),
									         	  indices.presentFamily.value()};
		if (indices.graphicsFamily          != indices.presentFamily) {
			createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        } else {
			createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;       // Optional
			createInfo.pQueueFamilyIndices   = nullptr; //Optional
		}

		createInfo.preTransform  			 = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha			 = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode               = presentMode;
		createInfo.clipped                   = VK_TRUE;
		createInfo.oldSwapchain              = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}
	}

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if(formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if(presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}
		

		return details;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
			return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
		}

		for (const auto &availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

    // PPPPPP  RRRRRR  EEEEEEE  SSSSS  EEEEEEE NN   NN TTTTTTT   AAA   TTTTTTT IIIII  OOOOO  NN   NN 
    // PP   PP RR   RR EE      SS      EE      NNN  NN   TTT    AAAAA    TTT    III  OO   OO NNN  NN 
    // PPPPPP  RRRRRR  EEEEE    SSSSS  EEEEE   NN N NN   TTT   AA   AA   TTT    III  OO   OO NN N NN 
    // PP      RR  RR  EE           SS EE      NN  NNN   TTT   AAAAAAA   TTT    III  OO   OO NN  NNN 
    // PP      RR   RR EEEEEEE  SSSSS  EEEEEEE NN   NN   TTT   AA   AA   TTT   IIIII  OOOO0  NN   NN 
	
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
		
		for (const auto &availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			} else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				bestMode = availablePresentMode;
			}
		}
		
		return bestMode;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			VkExtent2D actualExtent = { WIDTH, HEIGHT };

			actualExtent.width  = std::max(capabilities.minImageExtent.width,  std::min(capabilities.maxImageExtent.width,  actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return  actualExtent;
		}
	}

    // IIIII MM    MM   AAA     GGGG  EEEEEEE  VV     VV IIIII EEEEEEE WW      WW  SSSSS  
    //  III  MMM  MMM  AAAAA   GG  GG EE       VV     VV  III  EE      WW      WW SS      
    //  III  MM MM MM AA   AA GG      EEEEE     VV   VV   III  EEEEE   WW   W  WW  SSSSS  
    //  III  MM    MM AAAAAAA GG   GG EE         VV VV    III  EE       WW WWW WW      SS 
    // IIIII MM    MM AA   AA  GGGGGG EEEEEEE     VVV    IIIII EEEEEEE   WW   WW   SSSSS

	void createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType	= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image	= swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format	= swapChainImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel   = 0;
			createInfo.subresourceRange.levelCount     = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount     = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image views!");
			}
		}
	}
	
    //   GGGG  RRRRRR    AAA   PPPPPP  HH   HH IIIII  CCCCC   SSSSS  PPPPPP  IIIII PPPPPP  EEEEEEE LL      IIIII NN   NN EEEEEEE 
    //  GG  GG RR   RR  AAAAA  PP   PP HH   HH  III  CC    C SS      PP   PP  III  PP   PP EE      LL       III  NNN  NN EE      
    // GG      RRRRRR  AA   AA PPPPPP  HHHHHHH  III  CC       SSSSS  PPPPPP   III  PPPPPP  EEEEE   LL       III  NN N NN EEEEE   
    // GG   GG RR  RR  AAAAAAA PP      HH   HH  III  CC    C      SS PP       III  PP      EE      LL       III  NN  NNN EE      
    //  GGGGGG RR   RR AA   AA PP      HH   HH IIIII  CCCCC   SSSSS  PP      IIIII PP      EEEEEEE LLLLLLL IIIII NN   NN EEEEEEE

	void createGraphicsPipeline() {
		auto vertShaderCode = readFile("shaders/vert.spv");
		auto fragShaderCode = readFile("shaders/frag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName  = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName  = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount   = 0;
		vertexInputInfo.pVertexAttributeDescriptions    = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions    = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x          = 0.0f;
		viewport.y          = 0.0f;
		viewport.width      = (float) swapChainExtent.width;
		viewport.height     = (float) swapChainExtent.height;
		viewport.minDepth   = 0.0f;
		viewport.maxDepth   = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports    = &viewport;
		viewportState.scissorCount  = 1;
		viewportState.pScissors     = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable        = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth               = 1.0f;
		rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable         = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp          = 0.0f;
		rasterizer.depthBiasSlopeFactor    = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable   = VK_FALSE;
		multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading      = 1.0f;
		multisampling.pSampleMask           = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable      = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachement = {};
		colorBlendAttachement.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT
			                                      | VK_COLOR_COMPONENT_G_BIT
			                                      | VK_COLOR_COMPONENT_B_BIT
			                                      | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachement.blendEnable         = VK_FALSE;
		
		// colorBlendAttachement.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		// colorBlendAttachement.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		// colorBlendAttachement.colorBlendOp        = VK_BLEND_OP_ADD;
		
		// colorBlendAttachement.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		// colorBlendAttachement.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		// colorBlendAttachement.alphaBlendOp        = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable   = VK_FALSE;
		// colorBlending.logicOp         = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments    = &colorBlendAttachement;
		// colorBlending.blendConstants[0] = 0.0f;
		// colorBlending.blendConstants[1] = 0.0f;
		// colorBlending.blendConstants[2] = 0.0f;
		// colorBlending.blendConstants[3] = 0.0f;

		// VkDynamicState dynamicStates[] = {
		// 	VK_DYNAMIC_STATE_VIEWPORT,
		// 	VK_DYNAMIC_STATE_LINE_WIDTH
		// };

		// VkPipelineDynamicStateCreateInfo dynamicState = {};
		// dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		// dynamicState.dynamicStateCount = 2;
		// dynamicState.pDynamicStates = dynamicStates;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType			      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		// pipelineLayoutInfo.setLayoutCount	      = 0;
		// pipelineLayoutInfo.pSetLayouts		      = nullptr;
		// pipelineLayoutInfo.pushConstantRangeCount = 0;
		// pipelineLayoutInfo.pPushConstantRanges    = nullptr;

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create a pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount          = 2;
		pipelineInfo.pStages             = shaderStages;
		pipelineInfo.pVertexInputState   = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState      = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState   = &multisampling;
		pipelineInfo.pDepthStencilState  = nullptr;
		pipelineInfo.pColorBlendState    = &colorBlending;
		pipelineInfo.pDynamicState       = nullptr;

		pipelineInfo.layout              = pipelineLayout;
		pipelineInfo.renderPass          = renderPass;
		pipelineInfo.subpass             = 0;

		pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex   = -1;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}

	VkShaderModule createShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize                 = code.size();
		createInfo.pCode                    = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
		return shaderModule;
	}

	void createRenderPass() {
		VkAttachmentDescription colorAttachement = {};
		colorAttachement.format  = swapChainImageFormat;
		colorAttachement.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachement.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachement.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachement.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachement.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		
		colorAttachement.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachement.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments    = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments    = &colorAttachement;
		renderPassInfo.subpassCount    = 1;
		renderPassInfo.pSubpasses      = &subpass;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
        }
	}
	
    // MM    MM IIIII  SSSSS   CCCCC  
    // MMM  MMM  III  SS      CC    C 
    // MM MM MM  III   SSSSS  CC      
    // MM    MM  III       SS CC    C 
    // MM    MM IIIII  SSSSS   CCCCC  
	

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

			VkBool32 presentSupport    = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if(queueFamily.queueCount  > 0 && presentSupport){
				indices.presentFamily  = i;
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
		createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
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

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity
													   , VkDebugUtilsMessageTypeFlagsEXT             messageType
													   , const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
													   , void*                                       pUserData) {
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
	freopen( "output.log", "w", stdout);
	std::cout << "Output message:" << std::endl;
	
	HelloTriangleApplication app;

	try {
		app.run();
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return EXIT_FAILURE;
	}
}
