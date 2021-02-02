
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan_core.h>
#include <iostream>
#include <vector>
#include <string>


bool HandleVkResult(VkResult Result, const char* FunctionName)
{
	if (Result == VK_SUCCESS)
	{
		return false;
	}
	std::cout << "" << FunctionName << " failed with error code ";
	switch (Result)
	{
	case VK_NOT_READY:
		std::cout << "VK_NOT_READY";
		break;
	case VK_TIMEOUT:
		std::cout << "VK_TIMEOUT";
		break;
	case VK_EVENT_SET:
		std::cout << "VK_EVENT_SET";
		break;
	case VK_EVENT_RESET:
		std::cout << "VK_EVENT_RESET";
		break;
	case VK_INCOMPLETE:
		std::cout << "VK_INCOMPLETE";
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		std::cout << "VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		std::cout << "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
	case VK_ERROR_INITIALIZATION_FAILED:
		std::cout << "VK_ERROR_INITIALIZATION_FAILED";
		break;
	case VK_ERROR_DEVICE_LOST:
		std::cout << "VK_ERROR_DEVICE_LOST";
		break;
	case VK_ERROR_MEMORY_MAP_FAILED:
		std::cout << "VK_ERROR_MEMORY_MAP_FAILED";
		break;
	case VK_ERROR_LAYER_NOT_PRESENT:
		std::cout << "VK_ERROR_LAYER_NOT_PRESENT";
		break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		std::cout << "VK_ERROR_EXTENSION_NOT_PRESENT";
		break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
		std::cout << "VK_ERROR_FEATURE_NOT_PRESENT";
		break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		std::cout << "VK_ERROR_INCOMPATIBLE_DRIVER";
		break;
	case VK_ERROR_TOO_MANY_OBJECTS:
		std::cout << "VK_ERROR_TOO_MANY_OBJECTS";
		break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		std::cout << "VK_ERROR_FORMAT_NOT_SUPPORTED";
		break;
	case VK_ERROR_FRAGMENTED_POOL:
		std::cout << "VK_ERROR_FRAGMENTED_POOL";
		break;
	case VK_ERROR_UNKNOWN:
		std::cout << "VK_ERROR_UNKNOWN";
		break;
	case VK_ERROR_OUT_OF_POOL_MEMORY:
		std::cout << "VK_ERROR_OUT_OF_POOL_MEMORY";
		break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
		std::cout << "VK_ERROR_INVALID_EXTERNAL_HANDLE";
		break;
	case VK_ERROR_FRAGMENTATION:
		std::cout << "VK_ERROR_FRAGMENTATION";
		break;
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
		std::cout << "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
		break;
	case VK_ERROR_SURFACE_LOST_KHR:
		std::cout << "VK_ERROR_SURFACE_LOST_KHR";
		break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		std::cout << "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		break;
	case VK_SUBOPTIMAL_KHR:
		std::cout << "VK_SUBOPTIMAL_KHR";
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		std::cout << "VK_ERROR_OUT_OF_DATE_KHR";
		break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		std::cout << "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
		std::cout << "VK_ERROR_VALIDATION_FAILED_EXT";
		break;
	case VK_ERROR_INVALID_SHADER_NV:
		std::cout << "VK_ERROR_INVALID_SHADER_NV";
		break;
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
		std::cout << "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
		break;
	case VK_ERROR_NOT_PERMITTED_EXT:
		std::cout << "VK_ERROR_NOT_PERMITTED_EXT";
		break;
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
		std::cout << "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
		break;
	case VK_THREAD_IDLE_KHR:
		std::cout << "VK_THREAD_IDLE_KHR";
		break;
	case VK_THREAD_DONE_KHR:
		std::cout << "VK_THREAD_DONE_KHR";
		break;
	case VK_OPERATION_DEFERRED_KHR:
		std::cout << "VK_OPERATION_DEFERRED_KHR";
		break;
	case VK_OPERATION_NOT_DEFERRED_KHR:
		std::cout << "VK_OPERATION_NOT_DEFERRED_KHR";
		break;
	case VK_PIPELINE_COMPILE_REQUIRED_EXT:
		std::cout << "VK_PIPELINE_COMPILE_REQUIRED_EXT";
		break;
	default:
		std::cout << int32_t(Result);
	}
	std::cout << "\n";
	return true;
}


#define CHECK_RESULT(vkCall, ...) HandleVkResult(vkCall(__VA_ARGS__), #vkCall)


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
			if (CHECK_RESULT(vkCreateInstance, &CreateInfo, nullptr, &Instance))
			{
				Instance = VK_NULL_HANDLE;
			}
		}
		if (Instance != VK_NULL_HANDLE)
		{
			if (!SDL_Vulkan_CreateSurface(Window, Instance, &Surface))
			{
				Surface = VK_NULL_HANDLE;
				std::cout << "Failed to create window surface.\n";
			}
		}
	}
	bool PumpEvents()
	{
		return !SDL_QuitRequested();
	}
	virtual ~VulkanWindow()
	{
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
	SDL_Window* Window = nullptr;
	VkInstance Instance = VK_NULL_HANDLE;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	VkSurfaceFormatKHR SurfaceFormat;
};
