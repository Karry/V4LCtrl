
add_library(v4lctrl libv4lctrl.c libv4lctrl.h)

target_include_directories(v4lctrl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(v4lctrl PRIVATE -DV4LCTRL_EXPORT)

set_target_properties(v4lctrl PROPERTIES
    C_STANDARD 11)
