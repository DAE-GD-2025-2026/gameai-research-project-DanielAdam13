cmake_minimum_required(VERSION 3.26)
project(gameEngine
        VERSION 0.1.0
        DESCRIPTION "Reusable SDL3 + ImGui game engine"
        LANGUAGES C CXX)

#----------------------------------------------------------------------
# Build settings
#----------------------------------------------------------------------
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

include(FetchContent)

#----------------------------------------------------------------------
# Warning helper, exported for games to apply to their targets
#----------------------------------------------------------------------
function(ge_apply_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Werror)
    endif()
endfunction()

#----------------------------------------------------------------------
# Dear ImGui (docking branch)
#----------------------------------------------------------------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG        docking
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

#----------------------------------------------------------------------
# GLM
#----------------------------------------------------------------------
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
    FetchContent_Declare(
        glm
        URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
        DOWNLOAD_NO_PROGRESS ON
        DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(glm)
endif()

#----------------------------------------------------------------------
# SDL3
#   - Windows: download the prebuilt VC devel zip (gives SDL3::SDL3)
#   - Linux/macOS: build from source (gives SDL3-shared)
#----------------------------------------------------------------------
find_package(SDL3 3.4 CONFIG QUIET)
if(NOT SDL3_FOUND)
    if(WIN32)
        FetchContent_Declare(
            SDL3
            URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
            DOWNLOAD_NO_PROGRESS ON
            DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
        )
        FetchContent_MakeAvailable(SDL3)
        list(PREPEND CMAKE_PREFIX_PATH "${sdl3_SOURCE_DIR}")
        find_package(SDL3 CONFIG REQUIRED)
    else()
        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG        release-3.4.0
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
        )
        FetchContent_MakeAvailable(SDL3)
    endif()
endif()

# Pick the canonical SDL3 target. The Windows VC zip exports SDL3::SDL3.
# Building from source exports SDL3-shared / SDL3-static without namespace.
if(TARGET SDL3::SDL3)
    set(GAMEENGINE_SDL3_TARGET SDL3::SDL3)
elseif(TARGET SDL3::SDL3-shared)
    set(GAMEENGINE_SDL3_TARGET SDL3::SDL3-shared)
elseif(TARGET SDL3-shared)
    set(GAMEENGINE_SDL3_TARGET SDL3-shared)
elseif(TARGET SDL3::SDL3-static)
    set(GAMEENGINE_SDL3_TARGET SDL3::SDL3-static)
elseif(TARGET SDL3-static)
    set(GAMEENGINE_SDL3_TARGET SDL3-static)
else()
    message(FATAL_ERROR "[gameEngine] No usable SDL3 target found.")
endif()
message(STATUS "[gameEngine] SDL3 link target: ${GAMEENGINE_SDL3_TARGET}")

#----------------------------------------------------------------------
# SDL3_ttf
#----------------------------------------------------------------------
find_package(SDL3_ttf 3.2.2 CONFIG QUIET)
if(NOT SDL3_ttf_FOUND)
    if(WIN32)
        FetchContent_Declare(
            SDL3_ttf
            URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
            DOWNLOAD_NO_PROGRESS ON
            DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
        )
        FetchContent_MakeAvailable(SDL3_ttf)
        list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}")
        find_package(SDL3_ttf CONFIG REQUIRED)
    else()
        FetchContent_Declare(
            SDL3_ttf
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
            GIT_TAG        release-3.2.2
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
        )
        set(SDL3TTF_INSTALL OFF)
        FetchContent_MakeAvailable(SDL3_ttf)
    endif()
endif()

if(TARGET SDL3_ttf::SDL3_ttf)
    set(GAMEENGINE_SDL3_TTF_TARGET SDL3_ttf::SDL3_ttf)
elseif(TARGET SDL3_ttf::SDL3_ttf-shared)
    set(GAMEENGINE_SDL3_TTF_TARGET SDL3_ttf::SDL3_ttf-shared)
elseif(TARGET SDL3_ttf-shared)
    set(GAMEENGINE_SDL3_TTF_TARGET SDL3_ttf-shared)
elseif(TARGET SDL3_ttf::SDL3_ttf-static)
    set(GAMEENGINE_SDL3_TTF_TARGET SDL3_ttf::SDL3_ttf-static)
elseif(TARGET SDL3_ttf-static)
    set(GAMEENGINE_SDL3_TTF_TARGET SDL3_ttf-static)
else()
    message(FATAL_ERROR "[gameEngine] No usable SDL3_ttf target found.")
endif()
message(STATUS "[gameEngine] SDL3_ttf link target: ${GAMEENGINE_SDL3_TTF_TARGET}")

#----------------------------------------------------------------------
# VLD (Visual Leak Detector) — Windows only, optional
#----------------------------------------------------------------------
if(WIN32)
    find_package(VLD CONFIG QUIET)
endif()

#----------------------------------------------------------------------
# Build Dear ImGui as a static library tied to SDL3's renderer backend
#----------------------------------------------------------------------
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC ${GAMEENGINE_SDL3_TARGET})

#----------------------------------------------------------------------
# The engine itself
#----------------------------------------------------------------------
add_subdirectory(source)

#----------------------------------------------------------------------
# Helper: ge_install_runtime(<target>)
#
# Post-build glue for a game executable:
#   * Copies SDL3/SDL3_ttf/SDL3_mixer DLLs next to the exe (Windows)
#   * Copies frameworks into the .app bundle (macOS)
#   * Copies <CMAKE_SOURCE_DIR>/resources next to the exe
#   * Sets Visual Studio startup project + working directory
#----------------------------------------------------------------------
function(ge_install_runtime target)
    if(WIN32)
        set_property(DIRECTORY ${CMAKE_SOURCE_DIR}
            PROPERTY VS_STARTUP_PROJECT ${target})
        set_target_properties(${target} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${target}>")

        # Collect any SDL runtime targets that actually exist as DLLs.
        set(_sdl_runtime "")
        foreach(t IN ITEMS
            SDL3::SDL3 SDL3::SDL3-shared SDL3-shared
            SDL3_ttf::SDL3_ttf SDL3_ttf::SDL3_ttf-shared SDL3_ttf-shared
            SDL3_mixer::SDL3_mixer SDL3_mixer::SDL3_mixer-shared SDL3_mixer-shared)
            if(TARGET ${t})
                get_target_property(_type ${t} TYPE)
                if(_type STREQUAL "SHARED_LIBRARY")
                    list(APPEND _sdl_runtime ${t})
                endif()
            endif()
        endforeach()

        foreach(dll IN LISTS _sdl_runtime)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${dll}>"
                    "$<TARGET_FILE_DIR:${target}>"
                VERBATIM)
        endforeach()

    elseif(APPLE)
        set_target_properties(${target} PROPERTIES
            MACOSX_BUNDLE TRUE
            INSTALL_RPATH "@executable_path/../Frameworks"
            BUILD_WITH_INSTALL_RPATH TRUE)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_BUNDLE_DIR:${target}>/Contents/Frameworks")
        foreach(t IN ITEMS SDL3::SDL3-shared SDL3-shared
                          SDL3_ttf::SDL3_ttf-shared SDL3_ttf-shared)
            if(TARGET ${t})
                add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "$<TARGET_FILE:${t}>"
                        "$<TARGET_BUNDLE_DIR:${target}>/Contents/Frameworks/")
            endif()
        endforeach()
    endif()

    # Copy resources next to the executable (or into the .app bundle).
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources")
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/resources"
                "$<TARGET_FILE_DIR:${target}>/resources")
    endif()
endfunction()
