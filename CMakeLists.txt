cmake_minimum_required (VERSION 2.6)
project (sm-scheduler)

file(GLOB_RECURSE SMS_SRC_LIST "sources/*.cpp")
file(GLOB_RECURSE SMS_HEADER_LIST "include/*.hpp" "src/*.hpp")

#set(CMAKE_CXX_STANDARD 11)
if (UNIX)
    find_package(Threads REQUIRED)
    list(APPEND CMAKE_CXX_FLAGS "-std=c++0x -pthread")
endif (UNIX)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(sms ${SMS_HEADER_LIST} ${SMS_SRC_LIST})
 
# add the executable
add_executable(sms-pingpong-demo demo/main.cpp)

target_link_libraries (sms-pingpong-demo sms)
