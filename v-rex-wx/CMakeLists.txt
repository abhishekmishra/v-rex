cmake_minimum_required ( VERSION 3.9 )

include(CheckIncludeFile)

project ( v-rex-wx C )
include(FindMotif)

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
	src/dummy.c
)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	message(STATUS ">>> Linux, using pkg-config since vcpkg is broken")
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(json-c REQUIRED IMPORTED_TARGET json-c)
	pkg_check_modules(curl REQUIRED IMPORTED_TARGET libcurl)
	pkg_check_modules(archive REQUIRED IMPORTED_TARGET libarchive)
endif()

add_executable( ${PROJECT_NAME} ${V-REX-WX_SOURCES} )
target_include_directories(${PROJECT_NAME} PRIVATE ${YOUR_DIRECTORY})

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

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

set(CMAKE_FIND_USE_PACKAGE_REGISTRY ON)

find_package(coll CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC coll::coll)

find_package(clibdocker CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC clibdocker::clibdocker)

check_include_file("getopt.h" HAVE_GETOPT)

if(NOT HAVE_GETOPT)
	# To find and use linenoise, as the vcpkg build does not have cmake config
	find_path(GETOPT_INCLUDE_DIR getopt.h)
	find_library(GETOPT_LIBRARY getopt)
	include_directories(${GETOPT_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} PUBLIC ${GETOPT_LIBRARY})
endif (NOT HAVE_GETOPT)

# See https://gitlab.kitware.com/cmake/community/wikis/doc/cpack/Packaging-With-CPack
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "V-REX-WX: A Docker Command Line Client")
SET(CPACK_PACKAGE_VENDOR "Abhishek Mishra <abhishekmishra3@gmail.com>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "V-REX-WX")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  FILE(GLOB files "${CMAKE_CURRENT_BINARY_DIR}/Debug/*")
  INSTALL(FILES ${files} DESTINATION bin)
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "Debug\\\\V-REX-WX.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "V-REX-WX")
  SET(CPACK_NSIS_HELP_LINK "https:\\\\\\\\github.com/abhishekmishra/V-REX-WX")
  SET(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\github.com/abhishekmishra/V-REX-WX")
  SET(CPACK_NSIS_CONTACT "abhishekmishra3@gmail.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "V-REX-WX")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "V-REX-WX" "V-REX-WX")
INCLUDE(CPack)