cmake_minimum_required(VERSION 3.4)
project(SDLTutorials)

# Use our modified Find* modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${PROJECT_NAME}_SOURCE_DIR}/cmake")
#set(BIN_DIR ${${PROJECT_NAME}_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#########################################################
# FIND SDL2
#########################################################
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

include_directories(include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#add_executable(${PROJECT_NAME} Lesson1/main.cpp)
#target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

add_subdirectory(Lesson1)
add_subdirectory(Lesson2)
add_subdirectory(Lesson3)
add_subdirectory(Lesson4)