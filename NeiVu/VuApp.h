#pragma once
#include <BaseApp.h>


#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.hpp>

namespace NeiVu {
	class VuApp;
};

class NeiVu::VuApp: public BaseApp {
public:
	VuApp();



protected:
	bool debugVulkan = true;
	bool verbose = true;

	int universalQueueIndex = -1;

	vk::Instance instance;
	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::Queue queue;
	vk::CommandPool commandPool;
	vk::SurfaceKHR surface;
	vk::SurfaceFormatKHR surfaceFormat;
	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImages;

	vk::CommandPool commandPool;
	vk::CommandBuffer commandBuffer;
private:
	void createInstance();
	void createDevice();
	void createCommands();
	void createSurface();
	void createSwapchain();
};
