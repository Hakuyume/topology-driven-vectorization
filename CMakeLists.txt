cmake_minimum_required(VERSION 3.2)
project(vectorize)

add_definitions("-Wall -std=c++11 -O2")

add_executable(${PROJECT_NAME} main.cpp clustering.cpp)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
