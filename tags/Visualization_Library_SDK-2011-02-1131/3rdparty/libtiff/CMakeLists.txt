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

PROJECT(${VLTIFF_NAME})

include_directories("../zlib")
include_directories("../jpeg-6b")

#-DOJPEG_SUPPORT not supported
add_definitions( -DCCITT_SUPPORT -DPACKBITS_SUPPORT -DLZW_SUPPORT -DTHUNDER_SUPPORT -DNEXT_SUPPORT -DLOGLUV_SUPPORT -DZIP_SUPPORT 
                 -DPIXARLOG_SUPPORT -DDEFAULT_EXTRASAMPLE_AS_ALPHA -DSTRIPCHOP_DEFAULT=TIFF_STRIPCHOP -DSTRIP_SIZE_DEFAULT=8192 
                 -DAVOID_WIN32_FILEIO -DJPEG_SUPPORT -DCHECK_JPEG_YCBCR_SUBSAMPLING )

#if(WIN32)
#	set(PLATFORM_SPECIFIC tif_win32.c)
#endif()
#if(APPLE)
#	set(PLATFORM_SPECIFIC tif_apple.c)
#endif()
#if(UNIX or APPLE)
	set(PLATFORM_SPECIFIC tif_unix.c) # this one works for all platforms
#endif()

add_library(${VLTIFF_NAME} STATIC 
	${PLATFORM_SPECIFIC}
	tif_aux.c
	tif_close.c
	tif_codec.c
	tif_color.c
	tif_compress.c
	tif_dir.c
	tif_dirinfo.c
	tif_dirread.c
	tif_dirwrite.c
	tif_dumpmode.c
	tif_error.c
	tif_extension.c
	tif_fax3.c
	tif_fax3sm.c
	tif_getimage.c
	tif_jpeg.c
	#tif_ojpeg.c
	tif_flush.c
	tif_luv.c
	tif_lzw.c
	tif_next.c
	tif_open.c
	tif_packbits.c
	tif_pixarlog.c
	tif_predict.c
	tif_print.c
	tif_read.c
	tif_stream.cxx
	tif_swab.c
	tif_strip.c
	tif_thunder.c
	tif_tile.c
	tif_version.c
	tif_warning.c
	tif_write.c
	tif_zip.c
)
set_target_properties(${VLTIFF_NAME} PROPERTIES DEBUG_POSTFIX "-d")

install(TARGETS ${VLTIFF_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
