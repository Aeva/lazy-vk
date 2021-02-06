#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include "error_codes.hpp"


struct VulkanWindow
{
	VulkanWindow(std::string Name, uint32_t Width, uint32_t Height, bool UseValidation)
	{
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_VIDEO) == 0)
		{
			Window = SDL_CreateWindow(
				Name.c_str(),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				Width, Height,
				SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
		}

		VkApplicationInfo AppInfo = {};
		AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		AppInfo.pNext = nullptr;
		AppInfo.pApplicationName = Name.c_str();
		AppInfo.applicationVersion = 1;
		AppInfo.pEngineName = "Aeva's Dream Machine";
		AppInfo.engineVersion = 0;
		AppInfo.apiVersion = VK_API_VERSION_1_0;

		std::vector<const char*> InstanceExtensionNames;
		{
			uint32_t Count;
			SDL_Vulkan_GetInstanceExtensions(Window, &Count, nullptr);
			InstanceExtensionNames.resize(Count);
			SDL_Vulkan_GetInstanceExtensions(Window, &Count, InstanceExtensionNames.data());
		}

		const char* ValidationLayers[] = {
			"VK_LAYER_KHRONOS_validation",
		};

		{
			VkInstanceCreateInfo CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			CreateInfo.pNext = nullptr;
			CreateInfo.flags = 0;
			CreateInfo.pApplicationInfo = &AppInfo;
			CreateInfo.enabledExtensionCount = InstanceExtensionNames.size();
			CreateInfo.ppEnabledExtensionNames = InstanceExtensionNames.data();
			if (UseValidation)
			{
				CreateInfo.enabledLayerCount = 1;
				CreateInfo.ppEnabledLayerNames = ValidationLayers;
			}
			else
			{
				CreateInfo.enabledLayerCount = 0;
				CreateInfo.ppEnabledLayerNames = nullptr;
			}
			if (FAILED(vkCreateInstance, &CreateInfo, nullptr, &Instance))
			{
				Instance = VK_NULL_HANDLE;
			}
		}

		if (Instance != VK_NULL_HANDLE)
		{
			std::vector<VkPhysicalDevice> Adapters;
			{
				uint32_t AdapterCount = 0;
				vkEnumeratePhysicalDevices(Instance, &AdapterCount, nullptr);
				Adapters.resize(AdapterCount);
				vkEnumeratePhysicalDevices(Instance, &AdapterCount, Adapters.data());
			}
			if (Adapters.size() == 0)
			{
				std::cout << "No GPUs found.\n";
			}
			else
			{
				// Use the first available discrete adapter.
				for (VkPhysicalDevice& Candidate : Adapters)
				{
					vkGetPhysicalDeviceProperties(Candidate, &AdapterProperties);
					if (AdapterProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					{
						Adapter = Candidate;
						goto FoundAdapter;
					}
				}
				// Or the first available integrated adapter.
				for (VkPhysicalDevice& Candidate : Adapters)
				{
					vkGetPhysicalDeviceProperties(Candidate, &AdapterProperties);
					if (AdapterProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
					{
						Adapter = Candidate;
						goto FoundAdapter;
					}
				}
				// Yolo.
				Adapter = Adapters[0];
				vkGetPhysicalDeviceProperties(Adapter, &AdapterProperties);
			FoundAdapter:
				vkGetPhysicalDeviceMemoryProperties(Adapter, &AdapterMemoryProperties);
			}
		}

		if (Adapter != VK_NULL_HANDLE)
		{
			if (!SDL_Vulkan_CreateSurface(Window, Instance, &Surface))
			{
				Surface = VK_NULL_HANDLE;
				std::cout << "Failed to create window surface.\n";
			}
		}

		if (Surface != VK_NULL_HANDLE)
		{
			std::vector<VkSurfaceFormatKHR> SurfaceFormats;
			{
				uint32_t SurfaceFormatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(Adapter, Surface, &SurfaceFormatCount, nullptr);
				SurfaceFormats.resize(SurfaceFormatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(Adapter, Surface, &SurfaceFormatCount, SurfaceFormats.data());
				SurfaceFormat = SurfaceFormats[0];
			}
			std::vector<VkQueueFamilyProperties> QueueFamilies;
			{
				uint32_t QueueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(Adapter, &QueueFamilyCount, nullptr);
				QueueFamilies.resize(QueueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(Adapter, &QueueFamilyCount, QueueFamilies.data());
			}
			for (uint32_t i = 0; i < QueueFamilies.size(); ++i)
			{
				VkQueueFamilyProperties& QueueFamily = QueueFamilies[i];
				VkBool32 SupportsPresent;
				vkGetPhysicalDeviceSurfaceSupportKHR(Adapter, i, Surface, &SupportsPresent);
				const uint32_t Match = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
				if ((QueueFamily.queueFlags & Match) == Match && SupportsPresent)
				{
					QueueFamilyIndex = i;
					break;
				}
			}
		}

		if (QueueFamilyIndex != -1)
		{
			float QueuePriority = 0.0;
			VkDeviceQueueCreateInfo QueueInfo;
			QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QueueInfo.pNext = nullptr;
			QueueInfo.flags = 0;
			QueueInfo.queueFamilyIndex = QueueFamilyIndex;
			QueueInfo.queueCount = 1;
			QueueInfo.pQueuePriorities = &QueuePriority;

			std::vector<const char*> DeviceExtensions;
			DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			VkDeviceCreateInfo DeviceInfo;
			DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			DeviceInfo.pNext = nullptr;
			DeviceInfo.flags = 0;
			DeviceInfo.queueCreateInfoCount = 1;
			DeviceInfo.pQueueCreateInfos = &QueueInfo;
			DeviceInfo.enabledExtensionCount = uint32_t(DeviceExtensions.size());
			DeviceInfo.ppEnabledExtensionNames = DeviceExtensions.data();
			DeviceInfo.enabledLayerCount = 0;
			DeviceInfo.ppEnabledLayerNames = nullptr;
			DeviceInfo.pEnabledFeatures = nullptr;
			if (FAILED(vkCreateDevice, Adapter, &DeviceInfo, nullptr, &Device))
			{
				Device = VK_NULL_HANDLE;
			}
		}

		if (Device != VK_NULL_HANDLE)
		{
			vkGetDeviceQueue(Device, uint32_t(QueueFamilyIndex), 0, &GraphicsQueue);
			SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			SwapchainCreateInfo.pNext = nullptr;
			SwapchainCreateInfo.flags = 0;
			SwapchainCreateInfo.surface = Surface;
			SwapchainCreateInfo.minImageCount = 2;
			SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
			SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
			SwapchainCreateInfo.imageExtent.width = Width;
			SwapchainCreateInfo.imageExtent.height = Height;
			SwapchainCreateInfo.imageArrayLayers = 1;
			SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			SwapchainCreateInfo.queueFamilyIndexCount = 1;
			SwapchainCreateInfo.pQueueFamilyIndices = (uint32_t*)&QueueFamilyIndex;
			SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			SwapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
			SwapchainCreateInfo.clipped = true;
			SwapchainCreateInfo.oldSwapchain = nullptr;
			if (FAILED(vkCreateSwapchainKHR, Device, &SwapchainCreateInfo, nullptr, &Swapchain))
			{
				Swapchain = VK_NULL_HANDLE;
			}
		}

		if (Swapchain != VK_NULL_HANDLE)
		{
			{
				uint32_t SwapchainCount = 0;
				vkGetSwapchainImagesKHR(Device, Swapchain, &SwapchainCount, nullptr);
				SwapchainImages.resize(SwapchainCount);
				vkGetSwapchainImagesKHR(Device, Swapchain, &SwapchainCount, SwapchainImages.data());
			}
			for (VkImage& SwapchainImage : SwapchainImages)
			{
				{
					VkImageViewCreateInfo CreateInfo = {};
					CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					CreateInfo.pNext = nullptr;
					CreateInfo.flags = 0;
					CreateInfo.image = SwapchainImage;
					CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
					CreateInfo.format = SurfaceFormat.format;
					CreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
					CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					CreateInfo.subresourceRange.baseMipLevel = 0;
					CreateInfo.subresourceRange.levelCount = 1;
					CreateInfo.subresourceRange.baseArrayLayer = 0;
					CreateInfo.subresourceRange.layerCount = 1;
					VkImageView SwapchainView;
					vkCreateImageView(Device, &CreateInfo, nullptr, &SwapchainView);
					SwapchainViews.push_back(SwapchainView);
				}
				{
					VkCommandPoolCreateInfo CreateInfo = {};
					CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
					CreateInfo.pNext = nullptr;
					CreateInfo.queueFamilyIndex = QueueFamilyIndex;
					CreateInfo.flags = 0;
					VkCommandPool CommandPool;
					vkCreateCommandPool(Device, &CreateInfo, nullptr, &CommandPool);
					CommandPools.push_back(CommandPool);
				}
			}
			{
				VkFenceCreateInfo CreateInfo;
				CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				CreateInfo.pNext = nullptr;
				CreateInfo.flags = 0;
				vkCreateFence(Device, &CreateInfo, nullptr, &FrameFence);
			}
			{
				VkSemaphoreCreateInfo CreateInfo;
				CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				CreateInfo.pNext = nullptr;
				CreateInfo.flags = 0;
				vkCreateSemaphore(Device, &CreateInfo, nullptr, &SwapchainSemaphore);
			}
		}

		Shutdown = Swapchain == VK_NULL_HANDLE || GraphicsQueue == VK_NULL_HANDLE;
	}
	bool PumpEvents()
	{
		return !(SDL_QuitRequested() || Shutdown);
		{
			return false;
		}
	}
	void AdvanceFrame()
	{
		if (FAILED(vkAcquireNextImageKHR, Device, Swapchain, UINT64_MAX, SwapchainSemaphore, VK_NULL_HANDLE, &FrameOffset))
		{
			Shutdown = true;
		}
	}
	virtual ~VulkanWindow()
	{
		if (FrameFence != VK_NULL_HANDLE)
		{
			vkWaitForFences(Device, 1, &FrameFence, VK_TRUE, 1e+9);
			vkDestroyFence(Device, FrameFence, nullptr);
		}
		if (SwapchainSemaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(Device, SwapchainSemaphore, nullptr);
		}
		if (OldCommandBuffers.size())
		{
			vkFreeCommandBuffers(Device, LastCommandPool, uint32_t(OldCommandBuffers.size()), OldCommandBuffers.data());
			OldCommandBuffers.clear();
		}
		for (VkCommandPool& CommandPool : CommandPools)
		{
			vkDestroyCommandPool(Device, CommandPool, nullptr);
		}
		for (VkImageView& SwapchainView : SwapchainViews)
		{
			vkDestroyImageView(Device, SwapchainView, nullptr);
		}
		if (Swapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(Device, Swapchain, nullptr);
		}
		if (Device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(Device, nullptr);
		}
		if (Surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(Instance, Surface, nullptr);
		}
		if (Instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(Instance, nullptr);
		}
		if (Window != nullptr)
		{
			SDL_DestroyWindow(Window);
		}
		SDL_Quit();
	}

	bool Shutdown = true;
	SDL_Window* Window = nullptr;
	VkInstance Instance = VK_NULL_HANDLE;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	VkSurfaceFormatKHR SurfaceFormat;

	VkPhysicalDevice Adapter = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties AdapterProperties;
	VkPhysicalDeviceMemoryProperties AdapterMemoryProperties;
	VkDevice Device = VK_NULL_HANDLE;
	int32_t QueueFamilyIndex = -1;
	VkQueue GraphicsQueue = VK_NULL_HANDLE;

	VkSwapchainCreateInfoKHR SwapchainCreateInfo;
	VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
	std::vector<VkImage> SwapchainImages;
	std::vector<VkImageView> SwapchainViews;
	std::vector<VkCommandPool> CommandPools;
	VkCommandPool LastCommandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> OldCommandBuffers;
	uint32_t FrameOffset = 0;
	VkFence FrameFence = VK_NULL_HANDLE;
	VkSemaphore SwapchainSemaphore = VK_NULL_HANDLE;
};
