cmake_minimum_required ( VERSION 3.9 )

include(CheckIncludeFile)

project ( v-rex-motif C )
include(FindMotif)

# Set the project source dir (just convention)
set( V-REX-MOTIF_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( V-REX-MOTIF_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

# It's useful to setup the current version of our code in the build system
# using a `semver` style
set ( V-REX-MOTIF_VERSION_MAJOR 0 )
set ( V-REX-MOTIF_VERSION_MINOR 1 )
set ( V-REX-MOTIF_VERSION_PATCH 0 )

# Include Directories
# In GCC, this will invoke the "-I" command
include_directories( src )

# Lists
# Setup the list of source files
set( V-REX-MOTIF_SOURCES
  src/container_list_window.c
  src/container_log_window.c
# src/container_stats_window.c
  src/docker_server_window.c
  src/events_window.c
  src/images_list_window.c
  src/interactions_window.c
  src/main.c
  src/messages_window.c
  src/network_list_window.c
  src/ps_window.c
  src/statusbar_window.c
  src/theme.c
  src/volumes_list_window.c
  src/vrex_util.c

  src/container_list_window.h
  src/container_log_window.h
# src/container_stats_window.h
  src/docker_server_window.h
  src/events_window.h
  src/images_list_window.h
  src/interactions_window.h
  src/messages_window.h
  src/network_list_window.h
  src/ps_window.h
  src/statusbar_window.h
  src/theme.h
  src/volumes_list_window.h
  src/vrex_util.h
)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	message(STATUS ">>> Linux, using pkg-config since vcpkg is broken")
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(json-c REQUIRED IMPORTED_TARGET json-c)
	pkg_check_modules(curl REQUIRED IMPORTED_TARGET libcurl)
	pkg_check_modules(archive REQUIRED IMPORTED_TARGET libarchive)
endif()

add_executable( ${PROJECT_NAME} ${V-REX-MOTIF_SOURCES} )
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

# see inclusion of pthread https://stackoverflow.com/a/29871891/9483968
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

# Motif must be linked before Xt
# see https://babbage.cs.qc.cuny.edu/courses/GUIDesign/motif-faq.html#314
message(STATUS "MOTIF_FOUND = ${MOTIF_FOUND}")
include_directories(${MOTIF_INCLUDE_DIR})
link_directories(${MOTIF_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${MOTIF_LIBRARIES})

# need to link to -lXbae -lXm -lXt -lSM -lICE -lX11 -lpthread -lXmHTML
# see https://stackoverflow.com/questions/34014970/what-difference-lx11-option-vs-target-link-libraries-in-cmake
# for X11 linking via cmake
find_package(X11 REQUIRED)

message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "Xt found = ${X11_Xt_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

# check_include_file("XmHTML/XmHTML.h" HAVE_XMHTML)

# if(NOT HAVE_XMHTML)
# 	# To find and use XMHTML
# 	find_path(XMHTML_INCLUDE_DIR "XmHTML/XmHTML.h")
# 	find_library(XMHTML_LIBRARY XmHTML)
# 	include_directories(${XMHTML_INCLUDE_DIR})
# 	target_link_libraries(${PROJECT_NAME} PUBLIC ${XMHTML_LIBRARY})
# endif (NOT HAVE_XMHTML)


# include X11
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${X11_LIBRARIES})

# include Xmu
include_directories(${X11_Xmu_INCLUDE_PATH})
link_directories(${X11_Xmu_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC ${X11_Xmu_LIB})

# include SM
include_directories(${X11_SM_INCLUDE_PATH})
link_directories(${X11_SM_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC ${X11_SM_LIB})

# include Xt
include_directories(${X11_Xt_INCLUDE_PATH})
link_directories(${X11_Xt_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC ${X11_Xt_LIB})

check_include_file("Xbae/Matrix.h" HAVE_XBAE)
message(STATUS "HAVE_XBAE = ${HAVE_XBAE}")
# To find and use Xbae
find_path(XBAE_INCLUDE_DIR "Xbae/Matrix.h")
find_library(XBAE_LIBRARY Xbae)
include_directories(${XBAE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${XBAE_LIBRARY})

# See https://gitlab.kitware.com/cmake/community/wikis/doc/cpack/Packaging-With-CPack
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "V-REX-MOTIF: A Docker Command Line Client")
SET(CPACK_PACKAGE_VENDOR "Abhishek Mishra <abhishekmishra3@gmail.com>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "V-REX-MOTIF")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  FILE(GLOB files "${CMAKE_CURRENT_BINARY_DIR}/Debug/*")
  INSTALL(FILES ${files} DESTINATION bin)
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "Debug\\\\V-REX-MOTIF.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "V-REX-MOTIF")
  SET(CPACK_NSIS_HELP_LINK "https:\\\\\\\\github.com/abhishekmishra/V-REX-MOTIF")
  SET(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\github.com/abhishekmishra/V-REX-MOTIF")
  SET(CPACK_NSIS_CONTACT "abhishekmishra3@gmail.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "V-REX-MOTIF")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "V-REX-MOTIF" "V-REX-MOTIF")
INCLUDE(CPack)
