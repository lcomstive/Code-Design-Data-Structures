project(CDDS_Tests)

include_directories(
	include # Local include directory
	${THIRD_PARTY_DIR}/catch2
	${CMAKE_SOURCE_DIR}/DataStructures/include
	)

file(GLOB TEST_FILES "./*.cpp")
file(GLOB TEST_FILE_HEADERS "./*.hpp")

add_executable(${PROJECT_NAME} ${TEST_FILES} ${TEST_FILE_HEADERS})
