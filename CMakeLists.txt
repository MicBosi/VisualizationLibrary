################################################################################
#                                                                              #
#  Copyright (c) 2005-2011, Michele Bosi, Thiago Bastos                        #
#  All rights reserved.                                                        #
#                                                                              #
#  This file is part of Visualization Library                                  #
#  http://www.visualizationlibrary.com                                         #
#                                                                              #
#  Released under the OSI approved Simplified BSD License                      #
#  http://www.opensource.org/licenses/bsd-license.php                          #
#                                                                              #
################################################################################

cmake_minimum_required(VERSION 2.6)

################################################################################
# Global VL Settings
################################################################################

# Current Version
set(VLVERSION_MAJOR "2011")
set(VLVERSION_MINOR "05")
set(VLVERSION_PATCH "1140")
set(VLVERSION "${VLVERSION_MAJOR}.${VLVERSION_MINOR}")

# Common Dirs
set(VL_DATA_DIR "${CMAKE_SOURCE_DIR}/data")
set(3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/src/3rdparty")

################################################################################
# Initialization
################################################################################

project(Visualization_Library_SDK-${VLVERSION_MAJOR}.${VLVERSION_MINOR})

# Add our dir to the CMake modules path and include our InternalMacros file
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )
include( InternalMacros )

################################################################################
# Global Build Settings
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

# Debug postfix for all libraries.
set(CMAKE_DEBUG_POSTFIX "-d")

# Default output locations for the various target types.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

# Shared include paths for all subprojects
include_directories( "src" "${CMAKE_BINARY_DIR}/src" "${3RDPARTY_DIR}/glew" )

# High Warning Level
if(MSVC10)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc /MP")
elseif(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
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
find_package(OpenGL REQUIRED)

################################################################################
# Packaging
################################################################################

set(CPACK_PACKAGE_NAME					"${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR			"${VLVERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR			"${VLVERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH			"${VLVERSION_PATCH}")
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

install(FILES "LICENSE.TXT" "README.TXT" DESTINATION ".")
install(FILES "cmake/FindVL.cmake" DESTINATION "cmake")
