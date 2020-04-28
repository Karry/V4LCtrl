
add_library(v4lctrl libv4lctrl.c libv4lctrl.h)

target_include_directories(v4lctrl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
