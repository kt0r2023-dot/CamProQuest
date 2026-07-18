cmake_minimum_required(VERSION 3.16)
project(CamPro)

find_package(qpm REQUIRED)
qpm_resolve()

add_library(CamPro SHARED 
    src/main.cpp
)

target_link_libraries(CamPro PRIVATE beatsaber-hook)
