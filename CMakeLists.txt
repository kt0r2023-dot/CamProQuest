cmake_minimum_required(VERSION 3.19)
project(campro LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(LIB_NAME campro)

if(DEFINED ENV{ANDROID_NDK_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake" CACHE STRING "Android toolchain file")
endif()

set(ANDROID_ABI arm64-v8a)
set(ANDROID_PLATFORM android-26)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/lib/beatsaber-hook/shared
  ${CMAKE_SOURCE_DIR}/lib/questui/shared
)

add_library(${LIB_NAME} SHARED src/main.cpp)
target_compile_definitions(${LIB_NAME} PRIVATE QUEST_PLUGIN)
target_compile_options(${LIB_NAME} PRIVATE -fPIC -Wno-unused-parameter)
set_target_properties(${LIB_NAME} PROPERTIES OUTPUT_NAME "libcampro")
