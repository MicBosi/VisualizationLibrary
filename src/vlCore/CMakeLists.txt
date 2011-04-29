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

################################################################################
# VLCore Library
################################################################################

project(VLCORE)

# Handle compile-time plugins for VLCore
add_subdirectory("plugins")

# Gather VLCore source files
file(GLOB VLCORE_SRC "*.cpp")
file(GLOB VLCORE_INC "*.hpp")

# Generate the 'vlCore/version.hpp' file
configure_file("version.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/version.hpp")
list(APPEND VLCORE_INC "${CMAKE_CURRENT_BINARY_DIR}/version.hpp")

# Handle extras added by plugins
VL_PROJECT_GET(VLCORE _SOURCES _DEFINITIONS _INCLUDE_DIRS _LIBS)
add_definitions(${_DEFINITIONS})
include_directories(${_INCLUDE_DIRS})

add_library(VLCore ${VL_SHARED_OR_STATIC} ${VLCORE_SRC} ${VLCORE_INC} ${_SOURCES})
VL_DEFAULT_TARGET_PROPERTIES(VLCore)

# Because of GLEW, VLCore must also link with OpenGL
target_link_libraries(VLCore ${_LIBS})

################################################################################
# Source Groups
################################################################################

source_group("plugins" FILES ${_SOURCES})

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLCore)

# VLCore headers
install(FILES ${VLCORE_INC} DESTINATION "include/vlCore")
