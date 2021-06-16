project(DataStructures)

file(GLOB_RECURSE HEADER_FILES "include/*.hpp")

# Header-only library can be classed as an interface
add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} PUBLIC ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} INTERFACE include) # Local include directory