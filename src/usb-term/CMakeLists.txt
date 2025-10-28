cmake_minimum_required(VERSION 3.5)

project(usb-term VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# Find the PkgConfig module
find_package(PkgConfig REQUIRED)

# Use PkgConfig to find libusb-1.0 and create an imported target
pkg_check_modules(libusb REQUIRED IMPORTED_TARGET libusb-1.0)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp mainwindow.h mainwindow.ui
        usb_ids.c
        usbcon.cpp
        connectiondialog.cpp connectiondialog.h connectiondialog.ui
        hex_dump.cpp
        inputform.cpp inputform.h inputform.ui
        outputform.cpp outputform.h outputform.ui
        text_parser.cpp
        resource.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(usb-term
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET usb-term APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(usb-term SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(usb-term
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(usb-term PRIVATE ../)
target_link_libraries(usb-term PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
#Link your executable against the libusb imported target
target_link_libraries(usb-term PRIVATE PkgConfig::libusb)

set_target_properties(usb-term PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(usb-term)
endif()
