cmake_minimum_required(VERSION 3.4)
project(Lesson4)

#########################################################
# FIND OPENGL
#########################################################
find_package(OPENGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#########################################################
# FIND GLEW
#########################################################
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

set(SOURCE_FILES lesson4.cpp
        ../src/Timer.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})