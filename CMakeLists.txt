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
pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED IMPORTED_TARGET SDL2_mixer)

add_executable(ExtinctionDefense
    src/main.cpp
    src/core/app.cpp
    src/map/map_data.cpp
    src/tower/tower.cpp
    src/player/player.cpp
    src/enemy/enemy.cpp
    src/wave/wave.cpp
    src/assets/asset_manager.cpp
)

set(EXTINCTION_DEFENSE_WINDOWS_ICON "" CACHE FILEPATH "Windows .ico file for ExtinctionDefense.exe")
if (WIN32 AND EXTINCTION_DEFENSE_WINDOWS_ICON AND EXISTS "${EXTINCTION_DEFENSE_WINDOWS_ICON}")
    set(EXTINCTION_DEFENSE_ICON_RC "${CMAKE_CURRENT_BINARY_DIR}/ExtinctionDefenseIcon.rc")
    file(WRITE "${EXTINCTION_DEFENSE_ICON_RC}" "IDI_ICON1 ICON \"${EXTINCTION_DEFENSE_WINDOWS_ICON}\"\n")
    target_sources(ExtinctionDefense PRIVATE "${EXTINCTION_DEFENSE_ICON_RC}")
endif()

add_executable(png_to_map_json
    tools/png_to_map_json.cpp
)

target_include_directories(ExtinctionDefense PRIVATE include)
target_include_directories(png_to_map_json PRIVATE include)

target_link_libraries(ExtinctionDefense PRIVATE
    PkgConfig::SDL2
    PkgConfig::SDL2_IMAGE
    PkgConfig::SDL2_TTF
    PkgConfig::SDL2_MIXER
    nlohmann_json::nlohmann_json
)

target_link_libraries(png_to_map_json PRIVATE
    PkgConfig::SDL2
    PkgConfig::SDL2_IMAGE
    PkgConfig::SDL2_TTF
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