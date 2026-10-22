cmake_minimum_required(VERSION 3.21)

set(FSSTUDIO_SOURCES application.cpp
                     audioplayer.cpp
                     datamodel.cpp
                     exporter.cpp
                     filedialog.cpp
                     main.cpp
                     resourceview.cpp
                     sprite-file-sdl3.cpp
                     spriteimage.cpp)

set(FSSTUDIO_HEADERS application.h
                     audioplayer.h
                     datamodel.h
                     exporter.h
                     filedialog.h
                     resourceview.h
                     spriteimage.h)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${FSSTUDIO_SOURCES} ${FSSTUDIO_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE freeserf_data imgui SDL3::SDL3)
if(SDL3_mixer_FOUND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE FSSTUDIO_HAVE_MIXER)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_mixer::SDL3_mixer)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "FreeSerf Studio"
    MACOSX_BUNDLE_GUI_IDENTIFIER "org.freeserf.fsstudio"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
)

install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION "." RUNTIME DESTINATION ".")

# General MIDI SoundFont for the MIDI music of the DOS data, as in freeserf:
# FluidSynth needs one. TimGM6mb (GPL-2) is downloaded at configure time and
# installed next to the program (into the bundle resources on macOS). The
# vendored SDL3_mixer has no FluidSynth, so it is not needed there.
option(FSSTUDIO_SOUNDFONT "Download and install the TimGM6mb SoundFont" ON)
if(FSSTUDIO_SOUNDFONT AND SDL3_mixer_FOUND AND SDL3_FOUND)
    set(SOUNDFONT_URL "https://raw.githubusercontent.com/musescore/musescore-old/master/mscore/share/sound/TimGM6mb.sf2")
    set(SOUNDFONT_SHA256 "c5378b62028c920cb11e4803327983fee2f2cdff5dc89c708e39da417e51c854")
    set(SOUNDFONT_FILE "${CMAKE_CURRENT_BINARY_DIR}/TimGM6mb.sf2")
    if(EXISTS "${SOUNDFONT_FILE}")
        file(SHA256 "${SOUNDFONT_FILE}" SOUNDFONT_HASH)
    endif()
    if(NOT SOUNDFONT_HASH STREQUAL SOUNDFONT_SHA256)
        message(STATUS "Downloading SoundFont TimGM6mb")
        file(DOWNLOAD "${SOUNDFONT_URL}" "${SOUNDFONT_FILE}" STATUS SOUNDFONT_STATUS)
        file(SHA256 "${SOUNDFONT_FILE}" SOUNDFONT_HASH)
        if(NOT SOUNDFONT_HASH STREQUAL SOUNDFONT_SHA256)
            message(WARNING "Could not download the SoundFont (${SOUNDFONT_STATUS}), "
                            "MIDI music will not be played")
            file(REMOVE "${SOUNDFONT_FILE}")
        endif()
    endif()

    if(EXISTS "${SOUNDFONT_FILE}")
        set(SOUNDFONT_FILES "${SOUNDFONT_FILE}"
            "${PROJECT_SOURCE_DIR}/freeserf/cmake/soundfont/TimGM6mb.txt"
            "${PROJECT_SOURCE_DIR}/freeserf/cmake/soundfont/GPL-2.txt")
        if(APPLE)
            target_sources(${PROJECT_NAME} PRIVATE ${SOUNDFONT_FILES})
            set_source_files_properties(${SOUNDFONT_FILES} PROPERTIES
                                        MACOSX_PACKAGE_LOCATION Resources)
        else()
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E copy_if_different
                                       "${SOUNDFONT_FILE}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
            install(FILES ${SOUNDFONT_FILES} DESTINATION ".")
        endif()
    endif()
endif()


# A statically linked SDL3 needs no deployment. A shared system SDL3 is
# copied next to the executable (Windows) or into the bundle (macOS).
if(WIN32)
    install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> DESTINATION ".")
elseif(APPLE AND FSSTUDIO_SDL_SHARED)
    install(CODE "
        include(BundleUtilities)
        set(bundle \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app\")
        fixup_bundle(\"\${bundle}\" \"\" \"\")
        # fixup_bundle invalidates the signature, sign the bundle again
        execute_process(COMMAND codesign --force --deep --sign - \"\${bundle}\"
                        RESULT_VARIABLE result)
        if(NOT result EQUAL 0)
            message(FATAL_ERROR \"Failed to sign the bundle\")
        endif()
    ")
endif()

set(CPACK_PACKAGE_VERSION_MAJOR  ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR  ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH  ${PROJECT_VERSION_PATCH})
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WIN_ARCH_STRING x64)
    else()
        set(WIN_ARCH_STRING x86)
    endif()
    set(CPACK_GENERATOR "NSIS")
    if("${CMAKE_BUILD_TYPE}" STREQUAL "")
        set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${WIN_ARCH_STRING}-Develop")
    else()
        set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${WIN_ARCH_STRING}-${CMAKE_BUILD_TYPE}")
    endif()
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME};${PROJECT_NAME}")
    # install VC redistributables
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
        set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE)
    endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
    include(InstallRequiredSystemLibraries)
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}")
else()
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}")
endif()
include(CPack)
