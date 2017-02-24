project(inverter)
cmake_minimum_required(VERSION 2.8)

set(VTK_DIR "/home/charly/softwares/VTK/VTK_BIN_DIR/bin")
set(ITK_DIR "/home/charly/softwares/ITK/ITK_BIN_DIR/bin")
find_package(VTK REQUIRED)
find_package(ITK REQUIRED)

include(${ITK_USE_FILE})
include(${VTK_USE_FILE})

Message(${ITK_USE_FILE})
Message(${ITK_LIBRARIES})

link_directories(/usr/local/lib)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(inverter ${VTK_LIBRARIES})
target_link_libraries(inverter ${ITK_LIBRARIES})
