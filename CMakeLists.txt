# 3.24.1 is bundled in Visual Studio 2022 v17.4
# 3.24.1 is also bundled in CLion as of 2023
cmake_minimum_required(VERSION 3.24.1)

set(PROJECT_NAME "HeadTrackerOSCBridge")
set(PRODUCT_NAME "Head Tracker OSC Bridge")
set(COMPANY_NAME "nvsonic")
set(BUNDLE_ID "com.nvsonic.htoscbridge")
set(CURRENT_VERSION "4.0")

project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

add_subdirectory(JUCE)

juce_add_gui_app(${PROJECT_NAME}
    VERSION ...                       # Set this if the app version is different to the project version
    # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
    # ICON_SMALL ...
    # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
    ${COMPANY_NAME} ...                  # Specify the name of the app's author
    ${PRODUCT_NAME})     # The name of the final executable, which can differ from the target name


# juce_generate_juce_header(${PROJECT_NAME}) # recommended to avoid

target_sources(${PROJECT_NAME}
    PRIVATE
        source/Main.cpp
        source/MainComponent.h
        source/MainComponent.cpp
        source/Bridge.h
        source/Bridge.cpp
        source/BinauralHeadView.h
        source/rs232.h
        source/rs232-linux.c
        source/rs232-win.c
        source/SMLookAndFeel.h
        source/WavefrontObjParser.h
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:HeadTrackerOSCBridge,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:HeadTrackerOSCBridge,JUCE_VERSION>"
)

juce_add_binary_data(resources SOURCES
    resources/male_head.obj
    resources/Tbold.ttf
    resources/segoeui.ttf
    resources/axis.png
    resources/osc.png
    resources/serial.png
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        resources
        juce::juce_core
        juce::juce_gui_basics
        juce::juce_events
        juce::juce_osc
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)