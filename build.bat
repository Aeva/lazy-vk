clang++ ^
    -D SDL_MAIN_HANDLED ^
    -I "%VULKAN_SDK%\Include" ^
    -L "%VULKAN_SDK%\Lib" ^
    -I "external\SDL2-2.0.14\include" ^
    -L "external\SDL2-2.0.14\lib\x64" ^
    -l "vulkan-1.lib" ^
    -l "SDL2.lib" ^
    -fuse-ld=lld ^
    hail_eris.cpp ^
    -o hail_eris.exe