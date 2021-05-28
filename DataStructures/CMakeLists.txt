project(DataStructures)

include_directories(include) # Local include directory

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Headers" FILES ${HEADER_FILES})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})