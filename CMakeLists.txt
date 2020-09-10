cmake_minimum_required(VERSION 3.10)
include(FetchContent)
project(powerwall)

# For easy build debugging
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Default build type is Debug
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
else ()
    add_definitions(-DNDEBUG)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

# Set the default output directory.
if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/demo/addons/${CMAKE_PROJECT_NAME}/bin/x11")
endif (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)

# Create the default output directory if necessary.
if (NOT (EXISTS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}))
    file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endif (NOT (EXISTS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}))

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# vrpn
# Forces building vrpn client, this is disabled by default if vrpn is included as a subproject
set(VRPN_BUILD_CLIENT_LIBRARY ON CACHE INTERNAL "")
FetchContent_Declare(
        vrpn
        GIT_REPOSITORY https://github.com/vrpn/vrpn.git
        GIT_TAG        version_07.34
        PATCH_COMMAND  
          COMMAND echo "" > ${PROJECT_SOURCE_DIR}/_deps/vrpn-src/cmake/cmake-4.0.0-modules/RequireOutOfSourceBuild.cmake
)
FetchContent_MakeAvailable(vrpn)

# pthreads for vrpn
find_package(Threads REQUIRED)


set(GODOT_COMPILE_FLAGS)
set(GODOT_COMPILE_FLAGS "-fPIC -g -Wwrite-strings")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wchar-subscripts -Wcomment -Wdisabled-optimization")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wformat -Wformat=2 -Wformat-security -Wformat-y2k")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wimport -Winit-self -Winline -Winvalid-pch")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wlong-long -Wmissing-braces -Wmissing-format-attribute")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wpointer-arith")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wredundant-decls -Wreturn-type -Wsequence-point")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wswitch -Wswitch-enum -Wtrigraphs")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused-label")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wunused-value -Wvariadic-macros -Wvolatile-register-var -Wno-error=attributes")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -fno-omit-frame-pointer -O0")
set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wno-ignored-attributes")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -fno-omit-frame-pointer -O0")
else ()
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -O3")
endif (CMAKE_BUILD_TYPE MATCHES Debug)

# Get sources
file(GLOB_RECURSE SOURCES src/*.c**)
file(GLOB_RECURSE HEADERS src/*.h**)

add_library(${PROJECT_NAME}
        SHARED
        ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        src
        )

set(GODOT_HEADERS_DIR "/Users/ascadian/Projects/ZPP/godot_powerwall/modules/gdnative/include" CACHE PATH "found in $godot_src_dir/modules/gdnative/include")
#set(GODOT_HEADERS_DIR "/home/stud22/godot/modules/gdnative/include")
#set(GODOT_HEADERS_DIR "${CMAKE_SOURCE_DIR}/godot_headers")

# Put godot headers as SYSTEM PUBLIC to exclude warnings from irrelevant headers
target_include_directories(${PROJECT_NAME}
        SYSTEM PUBLIC
        ${GODOT_HEADERS_DIR}
        ${VRPN_INCLUDE_DIRS}
        )

target_link_libraries(${PROJECT_NAME} vrpn quat Threads::Threads )

# Add the compile flags
set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS ${GODOT_COMPILE_FLAGS})
