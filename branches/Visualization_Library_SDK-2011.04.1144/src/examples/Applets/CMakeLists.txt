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

project(VLAPPLETS)

# Gather VLApplets source files
file(GLOB VLAPPLETS_SRC "*.cpp" "App_BlindTests/*.cpp")
file(GLOB VLAPPLETS_INC "*.hpp" "App_BlindTests/*.hpp")

add_library(VLApplets STATIC ${VLAPPLETS_INC} ${VLAPPLETS_SRC})
VL_DEFAULT_TARGET_PROPERTIES(VLApplets)
