#############################################################################
#                                                                           #
#  Visualization Library                                                    #
#  http://www.visualizationlibrary.com                                      #
#                                                                           #
#  Copyright 2005-2009 Michele Bosi, all rights are reserved                #
#                                                                           #
#  This file is part of Visualization Library.                              #
#  It may only be used, modified, and distributed under the terms of the    #
#  license specified in the LICENSE.TXT file. By continuing to use,         #
#  modify or distribute this file you indicate that you have read the       #
#  license and understand and accept it fully.                              #
#                                                                           #
#  Visualization Library is distributed in the hope that it will be useful, #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#  LICENSE.TXT file for more details.                                       #
#                                                                           #
#############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(${VLZLIB_NAME})

add_library(${VLZLIB_NAME} STATIC adler32.c compress.c crc32.c deflate.c gzio.c infback.c inffast.c inflate.c inftrees.c trees.c uncompr.c zutil.c)
set_target_properties(${VLZLIB_NAME} PROPERTIES DEBUG_POSTFIX "-d")

install(TARGETS ${VLZLIB_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
