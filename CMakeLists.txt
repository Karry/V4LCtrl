cmake_minimum_required(VERSION 3.1)

project(v4lctrl C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON) # build should fail when compiler don't support standard defined by CMAKE_CXX_STANDARD

find_package(Qt5Core 5.6)
find_package(Qt5Gui 5.6)
find_package(Qt5Widgets 5.6)

add_subdirectory(v4lctrl)
add_subdirectory(v4lctrlcli)

if(Qt5Core_FOUND AND Qt5Gui_FOUND AND Qt5Widgets_FOUND)
    add_subdirectory(v4lctrlgui)
    message(STATUS "Gui will be build")
else()
    message(WARNING "Gui cannot be build")
endif()

