cmake_minimum_required(VERSION 3.1)

project(v4lctrl C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON) # build should fail when compiler don't support standard defined by CMAKE_CXX_STANDARD

find_package(Qt5Core 5.6)
find_package(Qt5Gui 5.6)
find_package(Qt5Widgets 5.6)

option(BUILD_SHARED_LIBS "Shared libraries " ON)  # SHARED lib default
option(BUILD_CLI "Build v4lctrlcli" ON)
option(BUILD_GUI "Build v4lctrlgui" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

add_subdirectory(v4lctrl)

if (BUILD_CLI)
    add_subdirectory(v4lctrlcli)
endif()

if(BUILD_GUI)
    if(Qt5Core_FOUND AND Qt5Gui_FOUND AND Qt5Widgets_FOUND)
        add_subdirectory(v4lctrlgui)
        message(STATUS "Gui will be build")
    else()
        message(WARNING "Gui cannot be build")
    endif()
endif()
