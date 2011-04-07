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

PROJECT(${VLJPG_NAME})

#add_definitions()

add_library(${VLJPG_NAME} STATIC
	jmemnobs.c
	jcomapi.c jutils.c jerror.c jmemmgr.c
	jcapimin.c jcapistd.c jctrans.c jcparam.c jdatadst.c 
	jcinit.c jcmaster.c jcmarker.c jcmainct.c jcprepct.c 
	jccoefct.c jccolor.c jcsample.c jchuff.c jcphuff.c 
	jcdctmgr.c jfdctfst.c jfdctflt.c jfdctint.c
	jdapimin.c jdapistd.c jdtrans.c jdatasrc.c 
	jdmaster.c jdinput.c jdmarker.c jdhuff.c jdphuff.c 
	jdmainct.c jdcoefct.c jdpostct.c jddctmgr.c jidctfst.c 
	jidctflt.c jidctint.c jidctred.c jdsample.c jdcolor.c 
	jquant1.c jquant2.c jdmerge.c
)
set_target_properties(${VLJPG_NAME} PROPERTIES DEBUG_POSTFIX "-d")

install(TARGETS ${VLJPG_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
