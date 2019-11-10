cmake_minimum_required ( VERSION 3.9 )
project ( v-rex-wx C CXX )
include(CheckIncludeFile)

# Set the project source dir (just convention)
set( V-REX-WX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( V-REX-WX_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

# It's useful to setup the current version of our code in the build system
# using a `semver` style
set ( V-REX-WX_VERSION_MAJOR 0 )
set ( V-REX-WX_VERSION_MINOR 1 )
set ( V-REX-WX_VERSION_PATCH 0 )

# Include Directories
# In GCC, this will invoke the "-I" command
include_directories( src )

# Lists
# Setup the list of source files
set( V-REX-WX_SOURCES
	src/v-rex-main.cpp
)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	message(STATUS ">>> Linux, using pkg-config since vcpkg is broken")
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(json-c REQUIRED IMPORTED_TARGET json-c)
	pkg_check_modules(curl REQUIRED IMPORTED_TARGET libcurl)
	pkg_check_modules(archive REQUIRED IMPORTED_TARGET libarchive)
endif()

add_executable( ${PROJECT_NAME} WIN32 ${V-REX-WX_SOURCES} )
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

set(CMAKE_FIND_USE_PACKAGE_REGISTRY ON)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(${PROJECT_NAME} PUBLIC json-c)
	target_link_libraries(${PROJECT_NAME} PUBLIC curl)
	target_link_libraries(${PROJECT_NAME} PUBLIC archive)
else()
	find_package(json-c CONFIG REQUIRED)
	target_link_libraries(${PROJECT_NAME} PUBLIC json-c::json-c)

	find_package(CURL CONFIG REQUIRED)
	target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)
endif()

find_package(coll CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC coll::coll)

find_package(clibdocker CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC clibdocker::clibdocker)

set (wxWidgets_ROOT_DIR "D:\\wxWidgets-3.1.3")
set (wxWidgets_LIB_DIR "D:\\wxWidgets-3.1.3\\lib\\vc_x64_lib")
find_package(wxWidgets REQUIRED
	COMPONENTS core qa html xml aui adv stc webview base scintilla)
include(${wxWidgets_USE_FILE})
target_link_libraries(${PROJECT_NAME} PUBLIC ${wxWidgets_LIBRARIES})