# Toolchain stuff to build vst3 for linux. To use, call...
# cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake ..
# from the build dir

# unfortunately since arch linux installs a pared down version of juce, I'm grabbing it from
# github, checking out the correct tag for my arch linux version, and then using those files
# to supplement when stuff is missing like jpegint.h. Currently running juce version 7.0.12
set(JUCE_DIRECTORY $ENV{HOME}/Downloads/JUCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
