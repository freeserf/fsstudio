cmake_minimum_required(VERSION 3.21)

project(FSStudio VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR} freeserf)

# SDL3 3.4 or newer is required (built-in PNG support). The system library is
# used when available, otherwise SDL3 is built from source and linked
# statically.
option(FSSTUDIO_VENDORED_SDL "Build SDL3 from source instead of using the system library" OFF)
if(NOT FSSTUDIO_VENDORED_SDL)
    find_package(SDL3 3.4 CONFIG QUIET)
endif()
if(SDL3_FOUND)
    message(STATUS "Using system SDL3 ${SDL3_VERSION}")
    # a shared system SDL3 has to be deployed with the application
    if(TARGET SDL3::SDL3-shared)
        set(FSSTUDIO_SDL_SHARED ON)
    endif()
else()
    message(STATUS "Building SDL3 from source")
    include(FetchContent)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SDL3
                         GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
                         GIT_TAG release-3.4.18
                         GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(SDL3)
endif()

# SDL3_mixer decodes the music for playback (MOD with libxmp, MIDI with
# FluidSynth). Without it the music can only be saved. The vendored build
# has MOD playback only: FluidSynth is not vendored by SDL3_mixer.
option(FSSTUDIO_MUSIC "Play music with SDL3_mixer" ON)
if(FSSTUDIO_MUSIC)
    if(SDL3_FOUND)
        find_package(SDL3_mixer 3.2 CONFIG QUIET)
        if(SDL3_mixer_FOUND)
            message(STATUS "Using system SDL3_mixer ${SDL3_mixer_VERSION}")
        else()
            message(STATUS "SDL3_mixer not found, music playback is disabled")
        endif()
    else()
        message(STATUS "Building SDL3_mixer from source")
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(SDLMIXER_VENDORED ON CACHE BOOL "" FORCE)
        set(SDLMIXER_DEPS_SHARED OFF CACHE BOOL "" FORCE)
        set(SDLMIXER_STRICT ON CACHE BOOL "" FORCE)
        set(SDLMIXER_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDLMIXER_TESTS OFF CACHE BOOL "" FORCE)
        set(SDLMIXER_EXAMPLES OFF CACHE BOOL "" FORCE)
        foreach(codec FLAC GME MP3 OPUS VORBIS_STB VORBIS_VORBISFILE WAVPACK
                      MIDI_FLUIDSYNTH MIDI_TIMIDITY)
            set(SDLMIXER_${codec} OFF CACHE BOOL "" FORCE)
        endforeach()
        set(SDLMIXER_MOD_XMP ON CACHE BOOL "" FORCE)
        FetchContent_Declare(SDL3_mixer
                             GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
                             GIT_TAG release-3.2.4
                             GIT_SHALLOW TRUE
                             GIT_SUBMODULES external/libxmp)
        FetchContent_MakeAvailable(SDL3_mixer)
        set(SDL3_mixer_FOUND ON)
    endif()
endif()

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(imgui STATIC ${IMGUI_DIR}/imgui.cpp
                         ${IMGUI_DIR}/imgui_draw.cpp
                         ${IMGUI_DIR}/imgui_tables.cpp
                         ${IMGUI_DIR}/imgui_widgets.cpp
                         ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
                         ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
                         ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC SDL3::SDL3)

set(FREESERF_DATA_SOURCES ./freeserf/src/data.cc
                          ./freeserf/src/data-source-dos.cc
                          ./freeserf/src/data-source-amiga.cc
                          ./freeserf/src/data-source-custom.cc
                          ./freeserf/src/data-source-legacy.cc
                          ./freeserf/src/data-source-mixed.cc
                          ./freeserf/src/tpwm.cc
                          ./freeserf/src/sfx2wav.cc
                          ./freeserf/src/xmi2mid.cc
                          ./freeserf/src/data-source.cc
                          ./freeserf/src/log.cc
                          ./freeserf/src/configfile.cc
                          ./freeserf/src/buffer.cc
                          ./freeserf/src/pcm2wav.cc
                          ./freeserf/src/debug.cc)

set(FREESERF_DATA_HEADERS ./freeserf/src/data.h
                          ./freeserf/src/misc.h
                          ./freeserf/src/freeserf_endian.h
                          ./freeserf/src/data-source-dos.h
                          ./freeserf/src/data-source-amiga.h
                          ./freeserf/src/data-source-custom.h
                          ./freeserf/src/data-source-legacy.h
                          ./freeserf/src/data-source-mixed.h
                          ./freeserf/src/tpwm.h
                          ./freeserf/src/sfx2wav.h
                          ./freeserf/src/xmi2mid.h
                          ./freeserf/src/data-source.h
                          ./freeserf/src/log.h
                          ./freeserf/src/configfile.h
                          ./freeserf/src/buffer.h
                          ./freeserf/src/pcm2wav.h
                          ./freeserf/src/debug.h)

add_library(freeserf_data STATIC ${FREESERF_DATA_SOURCES} ${FREESERF_DATA_HEADERS})

add_subdirectory(src)
