# Lazy Vulkan

This is a base code for Vulkan experiments, which can either be [compiled like a
normal C++ program](#instructions-for-the-ordinary-compiled-c-variant),
or [dynamically interpreted in python](#instructions-for-the-magic-python-variant).

# Instructions for the Ordinary Compiled C++ Variant
## Windows + Clang 11.0.0

 1. Install [clang 11.0.0](https://releases.llvm.org/download.html).
 2. Install the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)

Now you can compile the project with `build.bat` and run it via `hail_eris.exe`!

## Windows + Visual Studio 2019
TODO!

## Linux
TODO!

# Instructions for the ✨Magic✨ Python Variant
## Windows + Clang
TODO?

## Windows + MSBuild Tools 2017
 1. Install [MSBuild Tools 2017](https://aka.ms/vs/15/release/vs_buildtools.exe) with the following:
    * Workload `Visual C++ build tools`
 2. Install [Python 3](https://www.python.org/downloads/), and add it to PATH
 3. Install [cppyy](https://cppyy.readthedocs.io/en/latest/installation.html)

Now you can run the project via `python hail_eris.py`!

## Windows + Visual Studio 2017
 1. Install Visual Studio 2017 with the following:
    * Workload `Desktop development with C++`
 2. Install [Python 3](https://www.python.org/downloads/), and add it to PATH
 3. Install [cppyy](https://cppyy.readthedocs.io/en/latest/installation.html)

Now you can run the project via `python hail_eris.py`!

## Windows + Visual Studio 2019
 1. Install Visual Studio 2019 with the following:
    * Workload `Desktop development with C++`
    * Component  `MSVC v141 - VS 2017 C++ x64/x86 build tools (v14.16)`
 2. Install [Python 3](https://www.python.org/downloads/), and add it to PATH
 3. Run cmd.exe and enter the following to setup cppyy and build the correct PCH:
    * `cd "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"`
    * `vcvars64.bat -vcvars_ver=14.16`
    * `pip install cppyy`
    * `python -c "import cppyy"`

Now you can run the project via `python hail_eris.py`!

## Linux
TODO!
