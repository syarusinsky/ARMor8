# Example Audio Plugin CMakeLists.txt

# To get started on a new plugin, copy this entire folder (containing this file and C++ sources) to
# a convenient location, and then start making modifications.

# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.

cmake_minimum_required(VERSION 3.22)

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(ARMOR8-VST VERSION 0.0.1)

# If you've installed JUCE somehow (via a package manager, or directly using the CMake install
# target), you'll need to tell this project that it depends on the installed copy of JUCE. If you've
# included JUCE directly in your source tree (perhaps as a submodule), you'll need to tell CMake to
# include that subdirectory as part of the build.

# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
add_subdirectory(${JUCE_DIRECTORY} ./juce)                    # If you've put JUCE in a subdirectory called JUCE

# If you are building a VST2 or AAX plugin, CMake needs to be told where to find these SDKs on your
# system. This setup should be done before calling `juce_add_plugin`.

juce_set_vst2_sdk_path(/usr/src/vst2sdk/)
# juce_set_aax_sdk_path(...)

# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (AudioPluginExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_plugin(ARMor8VST
    VERSION 0.0.1                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    # COMPANY_NAME ...                          # Specify the name of the plugin's author
    IS_SYNTH TRUE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT TRUE               # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT TRUE              # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD FALSE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE RMDC               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Arm8                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS VST3 VST                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    VST3_AUTO_MANIFEST FALSE
    PRODUCT_NAME "ARMor8")        # The name of the final executable, which can differ from the target name

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

juce_generate_juce_header(ARMor8VST)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

target_sources(ARMor8VST
    PRIVATE
        ../../../Source/PluginEditor.cpp
        ../../../Source/PluginProcessor.cpp
        ../../../../src/ARMor8VoiceManager.cpp
        ../../../../src/ARMor8UiManager.cpp
        ../../../../src/ARMor8VoiceManager.cpp
        ../../../../src/ARMor8Voice.cpp
        ../../../../src/ARMor8Operator.cpp
        ../../../../src/ARMor8Filter.cpp
        ../../../../src/ARMor8PresetUpgrader.cpp
        ../../../../src/IARMor8LCDRefreshEventListener.cpp
        ../../../../src/IARMor8ParameterEventListener.cpp
        ../../../../src/IARMor8PresetEventListener.cpp
        ../../../../src/ScrollableMenuModel.cpp
        ../../../../lib/SAL/src/ADSREnvelopeGenerator.cpp
        ../../../../lib/SAL/src/AntiAliasingFilter.cpp
        ../../../../lib/SAL/src/SampleRateConverter.cpp
        ../../../../lib/SAL/src/AudioBuffer.cpp
        ../../../../lib/SAL/src/ExponentialResponse.cpp
        ../../../../lib/SAL/src/Limiter.cpp
        ../../../../lib/SAL/src/LinearResponse.cpp
        ../../../../lib/SAL/src/MidiHandler.cpp
        ../../../../lib/SAL/src/PolyBLEPOsc.cpp
        ../../../../lib/SAL/src/PresetManager.cpp
        ../../../../lib/SAL/src/IPotEventListener.cpp
        ../../../../lib/SAL/src/IButtonEventListener.cpp
        ../../../../lib/SAL/src/IKeyEventListener.cpp
        ../../../../lib/SAL/src/IMidiEventListener.cpp
        ../../../../lib/SAL/src/IPitchEventListener.cpp
	../../../../lib/SAL/src/ISalSysexEventListener.cpp
        ../../../../lib/SAL/src/KeyEventServer.cpp
        ../../../../lib/SAL/src/OnePoleFilter.cpp
        ../../../../lib/DevLib/src/CPPFile.cpp
        ../../../../lib/DevLib/src/IAllocator.cpp
        ../../../../lib/SIGL/src/ColorProfile.cpp
        ../../../../lib/SIGL/src/FrameBuffer.cpp
        ../../../../lib/SIGL/src/Font.cpp
        ../../../../lib/SIGL/src/Sprite.cpp
        ../../../../lib/SIGL/src/Surface.cpp
        ../../../../lib/SIGL/src/SoftwareGraphics.cpp)

target_include_directories(ARMor8VST
    PRIVATE
        ../../../../include
        ../../../../assets
        ../../../../lib/SAL/include
        ../../../../lib/DevLib/include
        ../../../../lib/SIGL/include
        ../../../../lib/STM32h745zi-HAL/llpd/include)

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(ARMor8VST
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_INCLUDE_ZLIB_CODE=1
        JUCE_USE_FLAC=1
        JUCE_USE_EXTERNAL_FLAC_CODEC=0
        JUCE_INCLUDE_JPEGLIB_CODE=1
        JUCE_USE_JPEGLIB=1
        JUCE_USE_COREIMAGE_LOADER=0
        CMAKE_DISABLE_FIND_PACKAGE_JPEG=TRUE # otherwise mingw's jpeglib is used, we want to use juce's
        JUCE_INCLUDE_PNGLIB_CODE=1
        JUCE_USE_PNG=1
        CMAKE_DISABLE_FIND_PACKAGE_PNG=TRUE
        JUCE_WINDOWS=1
        JUCE_MINGW=1
        JUCE_LINUX=0
        DO_NOT_SET_USING_NAMESPACE_JUCE
        SOFTWARE_RENDERING
	VST_BUILD)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

# juce_add_binary_data(AudioPluginData SOURCES ...)

# ensure our plugin is statically linked and contains everything needed to run it
set(MINGW_STATIC_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
set_target_properties(ARMor8VST PROPERTIES LINK_FLAGS "${MINGW_STATIC_FLAGS}")
set_target_properties(ARMor8VST_VST3 PROPERTIES LINK_FLAGS "${MINGW_STATIC_FLAGS}")
set_target_properties(ARMor8VST_VST PROPERTIES LINK_FLAGS "${MINGW_STATIC_FLAGS}")

# set standard
set(CMAKE_CXX_STANDARD 17)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_link_libraries(ARMor8VST
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_data_structures
        juce::juce_events
        juce::juce_core
        # windows (mingw) stuff
        gdi32
        user32
        shell32
        ole32
        oleaut32
        uuid
        winmm
        shlwapi
        imm32
        version
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
