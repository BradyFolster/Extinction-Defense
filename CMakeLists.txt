cmake_minimum_required(VERSION 3.20)

project(ExtinctionDefense LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Version supplied by GitHub Actions.
# Local development builds fall back to 0.0.0-dev.
set(
    EXTINCTION_DEFENSE_VERSION
    "0.0.0-dev"
    CACHE STRING
    "Extinction Defense version"
)

option(
    EXTINCTION_DEFENSE_BUILD_TOOLS
    "Build developer tools"
    ON
)


# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
#
# GitHub Actions builds use vcpkg.
#
# Your normal Ubuntu development build can continue using the system packages
# you already have installed through pkg-config.

if(DEFINED VCPKG_TARGET_TRIPLET)

    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)

else()

    find_package(PkgConfig REQUIRED)
    find_package(nlohmann_json REQUIRED)

    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED IMPORTED_TARGET SDL2_mixer)

endif()


# ---------------------------------------------------------------------------
# Game
# ---------------------------------------------------------------------------

set(
    EXTINCTION_DEFENSE_SOURCES

    src/main.cpp
    src/core/app.cpp
    src/map/map_data.cpp
    src/tower/tower.cpp
    src/player/player.cpp
    src/enemy/enemy.cpp
    src/wave/wave.cpp
    src/assets/asset_manager.cpp
)

# WIN32 prevents a console window from opening alongside the game
# in Windows release builds.
if(WIN32)
    add_executable(
        ExtinctionDefense
        WIN32
        ${EXTINCTION_DEFENSE_SOURCES}
    )
else()
    add_executable(
        ExtinctionDefense
        ${EXTINCTION_DEFENSE_SOURCES}
    )
endif()

target_include_directories(
    ExtinctionDefense
    PRIVATE
    include
)

target_compile_definitions(
    ExtinctionDefense
    PRIVATE
    EXTINCTION_DEFENSE_VERSION="${EXTINCTION_DEFENSE_VERSION}"
)


# ---------------------------------------------------------------------------
# Linking
# ---------------------------------------------------------------------------

if(DEFINED VCPKG_TARGET_TRIPLET)

    target_link_libraries(
        ExtinctionDefense
        PRIVATE

        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>

        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>

        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>

        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>

        nlohmann_json::nlohmann_json
    )

else()

    target_link_libraries(
        ExtinctionDefense
        PRIVATE

        PkgConfig::SDL2
        PkgConfig::SDL2_IMAGE
        PkgConfig::SDL2_TTF
        PkgConfig::SDL2_MIXER

        nlohmann_json::nlohmann_json
    )

endif()


# ---------------------------------------------------------------------------
# Windows icon
# ---------------------------------------------------------------------------

if(WIN32)

    set(
        WINDOWS_ICON
        "${CMAKE_SOURCE_DIR}/assets/images/icon.ico"
    )

    if(EXISTS "${WINDOWS_ICON}")

        set(
            WINDOWS_RESOURCE_FILE
            "${CMAKE_BINARY_DIR}/ExtinctionDefense.rc"
        )

        file(
            WRITE
            "${WINDOWS_RESOURCE_FILE}"
            "IDI_ICON1 ICON \"${WINDOWS_ICON}\"\n"
        )

        target_sources(
            ExtinctionDefense
            PRIVATE
            "${WINDOWS_RESOURCE_FILE}"
        )

    endif()

endif()


# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    target_compile_options(
        ExtinctionDefense
        PRIVATE

        -Wall
        -Wextra
        -Wpedantic
    )

endif()


# ---------------------------------------------------------------------------
# Developer tools
# ---------------------------------------------------------------------------

if(EXTINCTION_DEFENSE_BUILD_TOOLS)

    add_executable(
        png_to_map_json

        tools/png_to_map_json.cpp
    )

    target_include_directories(
        png_to_map_json
        PRIVATE
        include
    )

    if(DEFINED VCPKG_TARGET_TRIPLET)

        target_link_libraries(
            png_to_map_json
            PRIVATE

            $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
            $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>

            $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>

            $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>

            nlohmann_json::nlohmann_json
        )

    else()

        target_link_libraries(
            png_to_map_json
            PRIVATE

            PkgConfig::SDL2
            PkgConfig::SDL2_IMAGE
            PkgConfig::SDL2_TTF

            nlohmann_json::nlohmann_json
        )

    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

        target_compile_options(
            png_to_map_json
            PRIVATE

            -Wall
            -Wextra
            -Wpedantic
        )

    endif()

endif()