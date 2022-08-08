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
# VLSDL2 Library
################################################################################

project(VLSDL2)

# Gather VLSDL2 source files
file(GLOB VLSDL2_SRC "*.cpp")
file(GLOB VLSDL2_INC "*.hpp")
# ${SDL2_LIBRARIES}, ${SDL2_INCLUDE_DIRS}
include_directories(${SDL2_INCLUDE_DIRS})

add_library(VLSDL2 ${VL_SHARED_OR_STATIC} ${VLSDL2_SRC} ${VLSDL2_INC})
VL_DEFAULT_TARGET_PROPERTIES(VLSDL2)

target_link_libraries(VLSDL2 VLMain ${SDL2_LIBRARIES})

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLSDL2)

# VLSDL2 headers
install(FILES ${VLSDL2_INC} DESTINATION "${VL_INCLUDE_INSTALL_DIR}/vlSDL2")
