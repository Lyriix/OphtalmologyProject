
cmake_minimum_required(VERSION 2.8)

project(enfaceProjection)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2 -std=c++11")

set(VTK_DIR "/home/charly/softwares/VTK/VTK_BIN_DIR/bin")
set(ITK_DIR "/home/charly/softwares/ITK/ITK_BIN_DIR/bin")
#set(OPENCV_DIR "/home/charly/softwares/opencv/")

find_package(VTK REQUIRED)
find_package(ITK REQUIRED)
find_package(OpenCV REQUIRED)

include(${ITK_USE_FILE})
include(${VTK_USE_FILE})
#include(${OPENCV_USE_FILE})

Message(${ITK_USE_FILE})
Message(${ITK_LIBRARIES})

link_directories(/usr/local/lib)


file(
	GLOB
	source_files
	src/*.[ch]pp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(enfaceProjection ${VTK_LIBRARIES})
target_link_libraries(enfaceProjection ${ITK_LIBRARIES})
target_link_libraries(enfaceProjection ${OpenCV_LIBS})
