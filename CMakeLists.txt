cmake_minimum_required(VERSION 3.26)
project(Capture VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(utils)

if(WIN32)
    add_executable(Windows-Capture src/windows-capture.cpp)
    target_link_libraries(Windows-Capture 
        PRIVATE 
        ws2_32
        capturelib
    )
    target_link_options(Windows-Capture 
        PRIVATE 
        -static-libgcc 
        -static-libstdc++ 
        -static
    )
    target_compile_options(Windows-Capture 
        PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic 
        -Wconversion 
        -Wshadow
    )
else()
    add_executable(Mac-Listen src/mac-listen.cpp)
    target_link_libraries(Mac-Listen
        PRIVATE
        "-framework ApplicationServices"
        capturelib
    )
    target_compile_options(Mac-Listen
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wno-unused-parameter
    )
    set(APP_BUNDLE_DIR "${CMAKE_BINARY_DIR}/Mac-Listen.app/Contents/MacOS")
    add_custom_command(TARGET Mac-Listen POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Mac-Listen.app/Contents/MacOS"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:Mac-Listen>" "${APP_BUNDLE_DIR}/Mac-Listen"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/Info.plist"
            "${CMAKE_BINARY_DIR}/Mac-Listen.app/Contents/Info.plist"
    )
endif()