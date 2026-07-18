cmake_minimum_required(VERSION 3.16)
project(CamPro)

set(POSSIBLE_QPM_DIRS
  "${CMAKE_BINARY_DIR}/qpm"
  "${CMAKE_SOURCE_DIR}/qpm"
  "${CMAKE_SOURCE_DIR}/.qpm"
  "${CMAKE_BINARY_DIR}/.qpm"
)
foreach(_dir IN LISTS POSSIBLE_QPM_DIRS)
  if(EXISTS "${_dir}")
    list(APPEND CMAKE_MODULE_PATH "${_dir}")
  endif()
endforeach()

find_package(qpm REQUIRED)
qpm_resolve()

add_library(CamPro SHARED 
    src/main.cpp
)

target_link_libraries(CamPro PRIVATE beatsaber-hook)
