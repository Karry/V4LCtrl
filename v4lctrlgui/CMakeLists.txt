
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CLI_HEADERS
    cfunct.h
    controlwrappers.h
    errordialog.h
    mainwindow.h
    qidslider.h
    selectdevicedialog.h
    v4lctrlgui.h)

set(CLI_SOURCES
    cfunct.cpp
    controlwrappers.cpp
    errordialog.cpp
    main.cpp
    qidslider.cpp
    selectdevicedialog.cpp
    v4lctrlgui.cpp)

qt5_add_resources(RESOURCE_FILES application.qrc)

qt5_wrap_ui(UI_SOURCES
    errordialog.ui
    mainwindow.ui
    selectdevice.ui)

MESSAGE(STATUS "UI_SOURCES: ${UI_SOURCES}")

set_property(SOURCE qrc_res.cpp PROPERTY SKIP_AUTOMOC ON)

add_executable(v4lctrlgui ${CLI_HEADERS} ${CLI_SOURCES} ${RESOURCE_FILES} ${UI_SOURCES})

target_include_directories(v4lctrlgui PRIVATE ${CMAKE_CURRENT_BINARY_DIR}) # UI headers

target_link_libraries(v4lctrlgui
    v4lctrl
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets)

set_target_properties(v4lctrlgui PROPERTIES
    CXX_STANDARD 17)
