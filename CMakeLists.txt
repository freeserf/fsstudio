cmake_minimum_required(VERSION 3.16)

project(FSStudio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR} freeserf)

set(FREESERF_DATA_SOURCES ./freeserf/src/data.cc
                          ./freeserf/src/data-source-dos.cc
                          ./freeserf/src/data-source-amiga.cc
                          ./freeserf/src/data-source-custom.cc
                          ./freeserf/src/data-source-legacy.cc
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
