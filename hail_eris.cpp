
#include "vulkan_window.hpp"


int main()
{
	auto App = VulkanWindow("Eris is my copilot", 800, 600, true);
	while (App.PumpEvents())
	{
		//App.AdvanceFrame();
	}
	return 0;
}
