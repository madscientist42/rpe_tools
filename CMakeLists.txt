cmake_minimum_required(VERSION 2.6)

project(RPE_tools C CXX)

# Override some CMAKE defaults for our own purposes...
set(EXECUTABLE_OUTPUT_PATH deploy)
set(LIBRARY_OUTPUT_PATH deploy)

# Define some knobs that the users will want out of us...
option(USE_TINYTRHEAD "Turn on included TinyThread++ support instead of the native C++11 support" FALSE)
option(BUILD_DYNAMIC "Turn on dynamic (.so) building" TRUE)
option(USE_BOOST "Turn on BOOST support" TRUE)

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

# Force C++ 11 support...
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

# Set the ground rules for files being linked, as needed...
find_package (Threads)

# Check CMake settings- if the developer sets one of the options, we
# want to adjust the parameters...
if(USE_TINYTRHEAD)
    # Add TinyThread++ support to the library and tell the compiler to
    # use it in the build
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_TINYTHREAD")
    set(LIBRARY_SOURCES ${LIBRARY_SOURCES} src/tinythread.cpp)
endif(USE_TINYTRHEAD)
if(USE_BOOST)
    # Tell the compiler that BOOST has been specified.  As needed,
    # add it to the .so build as a link target.  Currently, only
    # a couple of things use BOOST. 
    find_package(Boost COMPONENTS system thread atomic chrono REQUIRED)    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST")
    list(APPEND TEST_APP_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_ATOMIC_LIBRARY} ${Boost_CHRONO_LIBRARY})    
endif(USE_BOOST)
if (NOT(USE_BOOST) AND NOT(USE_TINYTHREAD))
    # Force C++11 standard compliance if it's not turned on. 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (NOT(USE_BOOST) AND NOT(USE_TINYTHREAD))
  

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
install(DIRECTORY include DESTINATION /usr)
