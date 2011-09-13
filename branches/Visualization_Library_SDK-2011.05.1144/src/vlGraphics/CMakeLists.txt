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
# VLGraphics Library
################################################################################

project(VLGRAPHICS)

# Handle compile-time plugins for VLGraphics
add_subdirectory("plugins")

# Gather VLGraphics source files
file(GLOB VLGRAPHICS_SRC "*.cpp")
file(GLOB VLGRAPHICS_INC "*.hpp")

# Handle extras added by plugins
VL_PROJECT_GET(VLGRAPHICS _SOURCES _DEFINITIONS _INCLUDE_DIRS _LIBS)
add_definitions(${_DEFINITIONS})
include_directories(${_INCLUDE_DIRS})

# VLGraphics must embed and export GLEW
set(GLEW_DIR "${3RDPARTY_DIR}/glew")
set(GLEW_SRC "${GLEW_DIR}/glew.c")
add_definitions(-DGLEW_BUILD)

add_library(VLGraphics ${VL_SHARED_OR_STATIC} ${VLGRAPHICS_SRC} ${VLGRAPHICS_INC} ${GLEW_SRC} ${_SOURCES})
VL_DEFAULT_TARGET_PROPERTIES(VLGraphics)

target_link_libraries(VLGraphics VLCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${_LIBS})

################################################################################
# Source Groups
################################################################################

source_group("glew" FILES ${GLEW_SRC})
source_group("plugins" FILES ${_SOURCES})

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLGraphics)

# VLGraphics headers
install(FILES ${VLGRAPHICS_INC} DESTINATION "include/vlGraphics")

# GLEW headers
install(DIRECTORY "${GLEW_DIR}/GL" DESTINATION "include")
