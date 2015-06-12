project(RPE_tools)

cmake_minimum_required(VERSION 2.6)

# Override some CMAKE defaults for our own purposes...
set(EXECUTABLE_OUTPUT_PATH deploy)
set(LIBRARY_OUTPUT_PATH deploy)

# Define some knobs that the users will want out of us...
option(USE_TINYTRHEAD "Turn on included TinyThread++ support instead of the native C++11 support" FALSE)
option(BUILD_DYNAMIC "Turn on dynamic (.so) building" TRUE)
option(GCC_BUILD "Support GCC Atomics" TRUE)

# Enforce C++11 semantics- RPE Tools are C++11 entities and require this.
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

set(LIBRARY_SOURCES src/POpen.cpp)
# Check CMake settings- if the developer sets the rules up
if(USE_TINYTRHEAD)
    # Add TinyThread++ support to the library and tell the compile to
    # use it...
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_TINYTHREAD")
    set(LIBRARY_SOURCES ${LIBRARY_SOURCES} src/tinythread.cpp)
endif(USE_TINYTRHEAD)
if(GCC_BUILD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_TTHREAD_HAS_ATOMIC_BUILTINS_")
endif(GCC_BUILD)

# Set up the library proper...
if(BUILD_DYNAMIC)
    set(BUILD_TYPE SHARED)
else (BUILD_DYNAMIC)
    set(BUILD_TYPE STATIC)
endif(BUILD_DYNAMIC)
include_directories(include)
add_library(rpetools ${BUILD_TYPE} ${LIBRARY_SOURCES})

# Set up the test app.
add_executable(TestRPE src/TestRPE.cpp)
target_link_libraries(TestRPE rpetools pthread)

# Set up install rules...
install(TARGETS rpetools DESTINATION lib)
install(DIRECTORY include DESTINATION /usr)