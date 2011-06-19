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

# Khronos OpenGL headers
set(KHRONOS_DIR "${3RDPARTY_DIR}/Khronos")
set(KHRONOS_INC ${KHRONOS_DIR}/GL/gl3.h ${KHRONOS_DIR}/GL/glext.h ${KHRONOS_DIR}/GL/glxext.h ${KHRONOS_DIR}/GL/wglext.h)

add_library(VLGraphics ${VL_SHARED_OR_STATIC} ${VLGRAPHICS_SRC} ${VLGRAPHICS_INC} ${KHRONOS_INC} ${_SOURCES})
VL_DEFAULT_TARGET_PROPERTIES(VLGraphics)

target_link_libraries(VLGraphics VLCore ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${_LIBS})

################################################################################
# Source Groups
################################################################################

source_group("plugins" FILES ${_SOURCES})

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLGraphics)

# VLGraphics headers
install(FILES ${VLGRAPHICS_INC} DESTINATION "include/vlGraphics")

# Khronos headers
install(DIRECTORY "${KHRONOS_DIR}/GL" DESTINATION "include")
