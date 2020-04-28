
set(CLI_HEADERS
    struct-dump.h
    struct-v4l2.h
    v4lctrlcli.h)

set(CLI_SOURCES
    struct-dump.c
    struct-v4l2.c
    v4lctrlcli.c)

add_executable(v4lctrlcli ${CLI_HEADERS} ${CLI_SOURCES})

target_link_libraries(v4lctrlcli
    v4lctrl)

set_target_properties(v4lctrlcli PROPERTIES
    C_STANDARD 11)
