project(IPC_Entities)

include_directories(
	include # Local include directory
	${THIRD_PARTY_DIR}/raygui/src
	)

file(GLOB EDITOR_FILES Editor/*)
file(GLOB DISPLAY_FILES Display/*)

add_executable(${PROJECT_NAME}_Editor ${EDITOR_FILES})
add_executable(${PROJECT_NAME}_Display ${DISPLAY_FILES})

target_link_libraries(${PROJECT_NAME}_Editor raylib)
target_link_libraries(${PROJECT_NAME}_Display raylib)
