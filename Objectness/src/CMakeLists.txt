project(Objectness)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++11")


set(ITK_DIR "/home/charly/softwares/ITK/")

find_package(ITK REQUIRED)

include(${ITK_USE_FILE})
INCLUDE_DIRECTORIES("/home/charly/softwares/ITK/InsightToolkit-4.10.1/Modules/Nonunit/Review/include")

Message(${ITK_USE_FILE})
Message(${ITK_LIBRARIES})


link_directories(/usr/local/lib)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(Objectness ${ITK_LIBRARIES})
