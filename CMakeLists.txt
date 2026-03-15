cmake_minimum_required(VERSION 3.20)

project(ExtinctionDefense LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ExtinctionDefense
    src/main.cpp
    src/core/app.cpp
)

target_include_directories(ExtinctionDefense PRIVATE include)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

target_link_libraries(ExtinctionDefense PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ExtinctionDefense PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()