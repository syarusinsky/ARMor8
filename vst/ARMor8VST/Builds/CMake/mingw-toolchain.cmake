# Toolchain stuff to build vst3 for windows. To use, call...
# cmake -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake ..
# from the build dir

# unfortunately since arch linux installs a pared down version of juce, I'm grabbing it from
# github, checking out the correct tag for my arch linux version, and then using those files
# to supplement when stuff is missing like jpegint.h. Currently running juce version 7.0.12
set(JUCE_DIRECTORY $ENV{HOME}/Downloads/JUCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the system name to Windows
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify the cross-compiler prefix (adjust if your system uses a different one)
set(CMAKE_CROSSCOMPILER_PREFIX x86_64-w64-mingw32-)

# Specify the C and C++ compilers
set(CMAKE_C_COMPILER ${CMAKE_CROSSCOMPILER_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${CMAKE_CROSSCOMPILER_PREFIX}g++)
set(CMAKE_RC_COMPILER ${CMAKE_CROSSCOMPILER_PREFIX}windres)
set(CMAKE_AR ${CMAKE_CROSSCOMPILER_PREFIX}ar)
set(CMAKE_STRIP ${CMAKE_CROSSCOMPILER_PREFIX}strip)

# Specify the build tools
set(CMAKE_SYSROOT /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LINK ONLY)
