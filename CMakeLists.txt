cmake_minimum_required(VERSION 2.6)

project(RPE_tools C CXX)

# Override some CMAKE defaults for our own purposes...
set(EXECUTABLE_OUTPUT_PATH deploy)
set(LIBRARY_OUTPUT_PATH deploy)

# Define some knobs that the users will want out of us...
option(BUILD_DYNAMIC "Turn on dynamic (.so) building" TRUE)

# Define the library's components...  Unless you're using TinyThread++
# or one of the piece-parts that is not pure header definition, you
# don't need to link to this under all circumstances.
set(LIBRARY_SOURCES src/POpen.cpp)

# IF you've got Linux...we have a bit of GPIO magic to work with
# as well...so add it to the library sources when CMake detects
# Linux as a build target...
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    option(PROVIDE_SysFSGPIO "Turn on SysFSGPIO support" FALSE)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(PROVIDE_SysFSGPIO)
	set(LIBRARY_SOURCES ${LIBRARY_SOURCES} src/SysFSGPIO.cpp)
endif(PROVIDE_SysFSGPIO)


# Set the ground rules for files being linked, as needed...
find_package (Threads)

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
target_link_libraries(TestRPE rpetools ${TEST_APP_LIBS} ${CMAKE_THREAD_LIBS_INIT})


# Set up install rules...
install(TARGETS rpetools DESTINATION lib)
install(DIRECTORY include DESTINATION .)
