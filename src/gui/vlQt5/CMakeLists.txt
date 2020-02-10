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
# VLQt5 Library
################################################################################

project(VLQt5)

# Gather VLQt5 source files
file(GLOB VLQt5_SRC "*.cpp")
file(GLOB VLQt5_INC "*.hpp")

add_library(VLQt5 ${VL_SHARED_OR_STATIC} ${VLQt5_SRC} ${VLQt5_INC})
VL_DEFAULT_TARGET_PROPERTIES(VLQt5)

target_link_libraries(VLQt5 VLMain Qt5::Widgets Qt5::OpenGL)

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLQt5)

# VLQt5 headers
install(FILES ${VLQt5_INC} DESTINATION "${VL_INCLUDE_INSTALL_DIR}/vlQt5")
