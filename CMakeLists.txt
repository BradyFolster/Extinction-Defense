cmake_minimum_required(VERSION 3.20)

project(ExtinctionDefense LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
find_package(nlohmann_json REQUIRED)

pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)

add_executable(ExtinctionDefense
    src/main.cpp
    src/core/app.cpp
    src/map/map_data.cpp
    src/tower/tower.cpp
    src/player/player.cpp
    src/enemy/enemy.cpp
    src/wave/wave.cpp
)

add_executable(png_to_map_json
    tools/png_to_map_json.cpp
)

target_include_directories(ExtinctionDefense PRIVATE include)
target_include_directories(png_to_map_json PRIVATE include)

target_link_libraries(ExtinctionDefense PRIVATE
    PkgConfig::SDL2
    PkgConfig::SDL2_IMAGE
    nlohmann_json::nlohmann_json
)

target_link_libraries(png_to_map_json PRIVATE
    PkgConfig::SDL2
    PkgConfig::SDL2_IMAGE
    nlohmann_json::nlohmann_json
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ExtinctionDefense PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(png_to_map_json PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()