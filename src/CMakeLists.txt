cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(FSSTUDIO_SOURCES animationview.cpp
                     audioview.cpp
                     colorlabel.cpp
                     datamodel.cpp
                     exportdialog.cpp
                     exporter.cpp
                     main.cpp
                     mainwindow.cpp
                     qpathedit.cpp
                     resourcemodel.cpp
                     resourceview.cpp
                     sourcesdialog.cpp
                     sourcesview.cpp
                     spriteview.cpp
                     sprite-file-qt.cpp
                     ../res/resources.qrc)

set(FSSTUDIO_HEADERS animationview.h
                     audioview.h
                     colorlabel.h
                     datamodel.h
                     exportdialog.h
                     exporter.h
                     mainwindow.h
                     qpathedit.h
                     resourcemodel.h
                     resourceview.h
                     sourcesdialog.h
                     sourcesview.h
                     spriteview.h)

qt_add_executable(${PROJECT_NAME} WIN32 ${FSSTUDIO_SOURCES} ${FSSTUDIO_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE freeserf_data Qt6::Core Qt6::Gui Qt6::Widgets)
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
)

if(APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND "${MACDEPLOYQT_EXECUTABLE}"
                       ARGS $<TARGET_BUNDLE_DIR:${PROJECT_NAME}> "-always-overwrite"
                       COMMENT "Execute macdeployqt to create macOS bundle")
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ".")

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
