#include "VuApp.h"
#include "VuInfo.h"
#include "VuDebug.h"
#include <vector>
#include <algorithm>

#include <SDL2/SDL_syswm.h>

using namespace NeiVu;
using namespace std;

VuApp::VuApp() {

	try{
		createInstance();
		createDevice();
		createCommandPool();
		createSurface();
		createSwapchain();

	}	catch (system_error er) {
	std:cout << "error " << er.what()<<"\n";
	}
}



void VuApp::createInstance() {
	if (verbose) {
		printInstanceExtensions();
		printLayers();
	}
	vector<const char*> enabledExtensions;
	vector<const char*> enabledLayers;

	enabledExtensions.push_back("VK_KHR_surface");
#ifdef WIN32
	enabledExtensions.push_back("VK_KHR_win32_surface");
#endif
	//@todo linux

	if (debugVulkan) {
		auto la = vk::enumerateInstanceLayerProperties();
		auto ex = vk::enumerateInstanceExtensionProperties();

		auto names = vector<string>();
		for (auto &l : la)names.push_back(l.layerName);
		for (auto &e : ex)names.push_back(e.extensionName);

		if (isLayerPresent("VK_LAYER_LUNARG_standard_validation")) {
			enabledLayers.push_back("VK_LAYER_LUNARG_standard_validation");
		}

		if (isExtensionPresent("VK_EXT_debug_report")) {
			enabledExtensions.push_back("VK_EXT_debug_report");
		}
	}

	auto ici = vk::InstanceCreateInfo();
	ici.enabledExtensionCount = enabledExtensions.size();
	ici.ppEnabledExtensionNames = enabledExtensions.data();
	ici.enabledLayerCount = enabledLayers.size();
	ici.ppEnabledLayerNames = enabledLayers.data();


	instance = vk::createInstance(ici);
	assert(instance);

	if (debugVulkan) {
		enableDebugCallbacks(instance);
	}

}

void NeiVu::VuApp::createDevice() {
	if (verbose)printDevices(instance);

	auto physicalDevices = instance.enumeratePhysicalDevices();
	if (physicalDevices.size() > 1) cout << "Multiple vulkan devices not supported! Selected first one.";

	physicalDevice = physicalDevices[0];
	auto queueProps = physicalDevice.getQueueFamilyProperties();

	for (int i = 0; i < queueProps.size(); i++) {
		auto &q = queueProps[i];
		if (q.queueFlags & (vk::QueueFlagBits::eTransfer | vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)) {
			universalQueueIndex = i;
			break;
		}
	}
	assert(universalQueueIndex >= 0 && "Universal queue not found.");

	float priorities[1] = { 1.0 };
	vk::DeviceQueueCreateInfo dqci;
	dqci.queueCount = 1;
	dqci.queueFamilyIndex = universalQueueIndex;
	dqci.pQueuePriorities = { priorities };

	vector<const char*> enabledLayers;
	if (debugVulkan && isLayerPresent(physicalDevice,"VK_LAYER_LUNARG_standard_validation"))
		enabledLayers.push_back("VK_LAYER_LUNARG_standard_validation");

	vector<const char*> enabledExtensions;
	enabledExtensions.push_back("VK_KHR_swapchain");
	enabledExtensions.push_back("VK_NV_glsl_shader");

	vk::DeviceCreateInfo dci;
	dci.queueCreateInfoCount = 1;
	dci.pQueueCreateInfos = &dqci;
	dci.enabledExtensionCount = enabledExtensions.size();
	dci.ppEnabledExtensionNames = enabledExtensions.data();
	dci.enabledLayerCount = enabledLayers.size();
	dci.ppEnabledLayerNames = enabledLayers.data();

	device = physicalDevice.createDevice(dci);
	assert(device);

	queue = device.getQueue(universalQueueIndex, 0);
}

void NeiVu::VuApp::createCommandPool(){
	vk::CommandPoolCreateInfo cpci;
	cpci.queueFamilyIndex = universalQueueIndex;
	commandPool = device.createCommandPool(cpci, 0);
}

void NeiVu::VuApp::createSurface(){
#ifdef WIN32
	vk::Win32SurfaceCreateInfoKHR wsci;
	wsci.hinstance = (HINSTANCE)GetModuleHandle(NULL);
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(mainWindow->getWindowHandle(), &info);
	wsci.hwnd = (HWND)info.info.win.window;

	surface = instance.createWin32SurfaceKHR(wsci);
	assert(surface);
	auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
	surfaceFormat = surfaceFormats[0];

	assert(physicalDevice.getSurfaceSupportKHR(universalQueueIndex, surface));
	
#else
	assert("No pinguins" && 0);
#endif
}

void NeiVu::VuApp::createSwapchain(){
	auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	if (verbose) {
		std::cout << "surfaceCapabilities:\n";
		std::cout << "  currentExtent " << surfaceCapabilities.currentExtent.height << "," << surfaceCapabilities.currentExtent.width << endl;
		std::cout << "  minImageCount " << surfaceCapabilities.minImageCount << endl;
		std::cout << "  maxImageCount " << surfaceCapabilities.maxImageCount << endl;
		std::cout << "  supportedTransforms " << vk::to_string(surfaceCapabilities.supportedTransforms) << endl;
		std::cout << "  currentTransform " << vk::to_string(surfaceCapabilities.currentTransform) << endl;
	}
}
