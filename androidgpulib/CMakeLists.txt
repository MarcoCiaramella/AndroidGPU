cmake_minimum_required(VERSION 3.4.1)

add_library(
        androidgpu
        SHARED
        src/main/cpp/asset.c
        src/main/cpp/vulkan_wrapper.c
        src/main/cpp/compiler.c
        src/main/cpp/androidgpu.c )

target_include_directories(
        androidgpu # target
        PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/shaderc/include # include dir
)

target_link_libraries(
        androidgpu # target
        log  # lib to link
        android # lib to link
        stdc++ # lib to link
        ${CMAKE_SOURCE_DIR}/libs/shaderc/libs/c++_static/${ANDROID_ABI}/libshaderc.a # lib to link
)
