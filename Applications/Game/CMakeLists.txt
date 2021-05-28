project(Game)

include_directories(
	include # Local include directory
	${CMAKE_SOURCE_DIR}/DataStructures/include
	./modules/Utilities
	./modules/EntityComponentSystem
	${THIRD_PARTY_DIRECTORY}/Physac/src
)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")

add_subdirectory(modules/Utilities)
add_subdirectory(modules/EntityComponentSystem)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME}
						DataStructures
						raylib
						GameUtilities
						EntityComponentSystem
					 )

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Headers" FILES ${HEADER_FILES})

set(ASSET_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

if(MSVC)
	set(ASSET_OUTPUT_DIR "$(OutDir)/assets")
endif()

# Copy assets directory to binary output directory
add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/assets
	${ASSET_OUTPUT_DIR}
)