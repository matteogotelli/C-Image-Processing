cmake_minimum_required(VERSION 4.0)
project(Image_processing)

set(CMAKE_CXX_STANDARD 23)

add_executable(Image_processing my_main.cpp
        stb_image.h
        stb_image_write.h
        ImageG.hpp
        ImageG.cpp
        Kernel.hpp
        Kernel.cpp
        main.cpp
)
