################################################################################
#                                                                              #
#  Copyright (c) 2005-2020, Michele Bosi, Thiago Bastos                        #
#  All rights reserved.                                                        #
#                                                                              #
#  This file is part of Visualization Library                                  #
#  http://visualizationlibrary.org                                             #
#  http://visualizationlibrary.org                                             #
#                                                                              #
#  Released under the OSI approved Simplified BSD License                      #
#  http://www.opensource.org/licenses/bsd-license.php                          #
#                                                                              #
################################################################################

# Current Version
set(VL_VERSION_MAJOR "2")
set(VL_VERSION_MINOR "2")
set(VL_VERSION_PATCH "0")
set(VL_VERSION "${VL_VERSION_MAJOR}.${VL_VERSION_MINOR}")
set(VL_VERSION_FULL "${VL_VERSION_MAJOR}.${VL_VERSION_MINOR}.${VL_VERSION_PATCH}")

################################################################################
# Initialization
################################################################################

project(Visualization_Library_SDK)

# Must be called after project!
cmake_minimum_required(VERSION 3.0)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP4>)

################################################################################
# Global Build Settings (config.hpp.in)
################################################################################

set(VL_USER_DATA_OBJECT 0 CACHE BOOL "Enable vl::Object user data.")
set(VL_USER_DATA_ACTOR 0 CACHE BOOL "Enable vl::Object user data.")
set(VL_USER_DATA_TRANSFORM 0 CACHE BOOL "Enable vl::Object user data.")
set(VL_USER_DATA_SHADER 0 CACHE BOOL "Enable vl::Object user data.")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(VL_PLATFORM_MACOSX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(VL_PLATFORM_LINUX 1)
	option(VL_UNIX_INSTALL_MODE "Set to ON to install VL into default UNIX path structure." OFF)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(VL_PLATFORM_WINDOWS 1)
else()
	message(FATAL_ERROR "Unable to detect platform!")
endif()
message(STATUS "System detected: \"${CMAKE_SYSTEM_NAME}\"")

# OpenGL, OpenGL ES 1 or OpenGL ES 2 mode
set(VL_OPENGL_MODE "OPENGL" CACHE STRING "Only value supported for now is OPENGL")
if( VL_OPENGL_MODE STREQUAL "OPENGL")
	set(VL_OPENGL 1)
	message(STATUS "Configuring for OpenGL 1.x/2.x/3.x/4.x")
else()
	message(FATAL_ERROR "Invalid VL_OPENGL_MODE! Valid modes are: OPENGL")
endif()

################################################################################

# Dynamic vs Static Linking
option(VL_DYNAMIC_LINKING "Set to ON to build VL for dynamic linking, or OFF for static." ON)
if(VL_DYNAMIC_LINKING)
	add_definitions(-DVL_DYNAMIC_LINKING)
	set(VL_SHARED_OR_STATIC "SHARED")
else()
	add_definitions(-DVL_STATIC_LINKING)
	set(VL_SHARED_OR_STATIC "STATIC")
endif()

# Common Dirs
set(VL_DATA_DIR "${CMAKE_SOURCE_DIR}/data")
set(3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/src/external")

# Header Install Dir
if(VL_UNIX_INSTALL_MODE)
	set(VL_INCLUDE_INSTALL_DIR "include/vl")
else()
	set(VL_INCLUDE_INSTALL_DIR "include")
endif()

# Add our dir to the CMake modules path and include our InternalMacros file
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )
include( InternalMacros )

# Debug postfix for all libraries.
set(CMAKE_DEBUG_POSTFIX "-d")

# Default output locations for the various target types.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

# Shared include paths for all subprojects
include_directories( "src" "src/gui" "${CMAKE_BINARY_DIR}/src" "${3RDPARTY_DIR}/Khronos" )

# High Warning Level
if(MSVC10)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc /MP")
	add_definitions(-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
elseif(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
	add_definitions(-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
else()
	set(CMAKE_CXX_FLAGS "-W -Wall") # see also: -W -Wall -Wwrite-strings -Wcast-qual -Wconversion -Wshadow
endif()

if(WIN32)
	add_definitions(-DUNICODE)
endif()

if(MSVC)
	set(WINVER "0x0600" CACHE STRING "WINVER version (see MSDN documentation)")
	add_definitions(-DWINVER=${WINVER})
	add_definitions(-D_WIN32_WINNT=${WINVER})
endif()

# Required Dependencies

if( VL_OPENGL_MODE STREQUAL "OPENGL")
	set(OpenGL_GL_PREFERENCE GLVND)
	find_package(OpenGL REQUIRED)
	set(VL_OPENGL_LIBRARIES ${OPENGL_LIBRARIES})
endif()

################################################################################
# Packaging
################################################################################

set(CPACK_PACKAGE_NAME					"${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR			"${VL_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR			"${VL_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH			"${VL_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY	"A lightweight C++ OpenGL middleware for 2D and 3D graphics.")

if(WIN32)
	set(CPACK_GENERATOR "ZIP")
else()
	set(CPACK_GENERATOR "TBZ2")
endif()

include( CPack )

################################################################################
# Subdirectories
################################################################################

add_subdirectory("docs")
add_subdirectory("data")
add_subdirectory("src")

################################################################################
# Install Rules
################################################################################

file(GLOB VL_MK_FILES "*.md")

if(VL_UNIX_INSTALL_MODE)
  install(FILES ${VL_MK_FILES} DESTINATION "share/vl")
	install(FILES "cmake/FindVL.cmake" DESTINATION "share/cmake/Modules")
else()
	install(FILES ${VL_MK_FILES} DESTINATION ".")
	install(FILES "cmake/FindVL.cmake" DESTINATION "cmake")
endif()
