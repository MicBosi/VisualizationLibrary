################################################################################
#                                                                              #
#  Copyright (c) 2005-2020, Michele Bosi, Thiago Bastos                        #
#  All rights reserved.                                                        #
#                                                                              #
#  This file is part of Visualization Library                                  #
#  http://visualizationlibrary.org                                             #
#                                                                              #
#  Released under the OSI approved Simplified BSD License                      #
#  http://www.opensource.org/licenses/bsd-license.php                          #
#                                                                              #
################################################################################

################################################################################
# VLGLFW Library
################################################################################

project(VLGLFW)

# Gather VLGLFW source files
file(GLOB VLGLFW_SRC "*.cpp")
file(GLOB VLGLFW_INC "*.hpp")

include_directories(${GLFW_INCLUDE_DIR})
add_library(VLGLFW ${VL_SHARED_OR_STATIC} ${VLGLFW_SRC} ${VLGLFW_INC})
VL_DEFAULT_TARGET_PROPERTIES(VLGLFW)

target_link_libraries(VLGLFW VLMain ${GLFW_glfw3dll_LIB})

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLGLFW)

# VLGLFW headers
install(FILES ${VLGLFW_INC} DESTINATION "${VL_INCLUDE_INSTALL_DIR}/vlGLFW")
