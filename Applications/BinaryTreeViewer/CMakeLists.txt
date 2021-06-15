project(BinaryTreeViewer)

include_directories(
	../../DataStructures/include
	${THIRD_PARTY_DIR}/raylib/src
	${THIRD_PARTY_DIR}/raygui/src
)

add_executable(${PROJECT_NAME} Main.cpp)

target_link_libraries(${PROJECT_NAME} raylib)
