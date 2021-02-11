
import time
import os
import gc
import cppyy

cppyy.load_library("SDL2.dll")
cppyy.load_library("vulkan-1")
cppyy.add_include_path("external")
cppyy.add_include_path("external/SDL2-2.0.14/include")
cppyy.include("vulkan_window.hpp")
cpp = cppyy.gbl


__live_objects = []


def register(thing):
    __live_objects.append(thing)
    return thing


def teardown():
    while len(__live_objects) > 0:
        __live_objects.pop()


def main():
    app = register(cpp.VulkanWindow("Eris is my copilot", 800, 600, True))
    calls = 0
    while app.PumpEvents():
        app.AdvanceFrame();
        if calls == 0:
            # first call to app.AdvanceFrame() should be safe
            calls += 1
        else:
            # second call to app.AdvanceFrame() should crash for normal vulkan reasons
            break;
        time.sleep(0.1)


if __name__ == "__main__":
    main()
    gc.collect()
    teardown()
