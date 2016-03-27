cmake_minimum_required(VERSION 3.4)
project(Lesson1)

configure_file(hello.bmp hello.bmp COPYONLY)

set(SOURCE_FILES lesson1.cpp
        ../src/Timer.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})