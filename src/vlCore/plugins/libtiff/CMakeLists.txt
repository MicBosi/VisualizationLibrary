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
# TIFF Library
################################################################################

set(TIFF_DIR "${3RDPARTY_DIR}/libtiff")
set(TIFF_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/config")

include(CheckSymbolExists)

################################################################################
# Generate tif_config.h and tiffconf.h from the upstream CMake templates
################################################################################

# Codecs: everything that needs no extra library (besides zlib and libjpeg)
set(CCITT_SUPPORT 1)
set(PACKBITS_SUPPORT 1)
set(LZW_SUPPORT 1)
set(THUNDER_SUPPORT 1)
set(NEXT_SUPPORT 1)
set(LOGLUV_SUPPORT 1)
set(MDI_SUPPORT 1)
set(ZIP_SUPPORT 1)
set(PIXARLOG_SUPPORT 1)
set(JPEG_SUPPORT 1)
set(CHECK_JPEG_YCBCR_SUBSAMPLING 1)
set(SUBIFD_SUPPORT 1)
set(DEFAULT_EXTRASAMPLE_AS_ALPHA 1)
set(STRIPCHOP_DEFAULT 1)
set(STRIP_SIZE_DEFAULT 8192)
set(TIFF_MAX_DIR_COUNT 1048576)
set(CXX_SUPPORT 1)
# Not supported: OJPEG, JBIG, LERC, LZMA, ZSTD, WEBP, libdeflate, 8/12 bit JPEG

set(HAVE_ASSERT_H ${VL_HAVE_ASSERT_H})
set(HAVE_FCNTL_H ${VL_HAVE_FCNTL_H})
set(HAVE_IO_H ${VL_HAVE_IO_H})
set(HAVE_STRINGS_H ${VL_HAVE_STRINGS_H})
set(HAVE_SYS_TYPES_H ${VL_HAVE_SYS_TYPES_H})
set(HAVE_UNISTD_H ${VL_HAVE_UNISTD_H})
set(HAVE_IEEEFP 1)
if(NOT WIN32)
	check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
endif()

set(SIZEOF_SIZE_T ${VL_SIZEOF_SIZE_T})
set(WORDS_BIGENDIAN ${VL_BIG_ENDIAN})
if(VL_BIG_ENDIAN)
	set(HOST_BIG_ENDIAN 1)
else()
	set(HOST_BIG_ENDIAN 0)
endif()

set(TIFF_INT8_T int8_t)
set(TIFF_INT16_T int16_t)
set(TIFF_INT32_T int32_t)
set(TIFF_INT64_T int64_t)
set(TIFF_UINT8_T uint8_t)
set(TIFF_UINT16_T uint16_t)
set(TIFF_UINT32_T uint32_t)
set(TIFF_UINT64_T uint64_t)
if(VL_SIZEOF_SIZE_T EQUAL 8)
	set(TIFF_SSIZE_T int64_t)
else()
	set(TIFF_SSIZE_T int32_t)
endif()

set(PACKAGE_NAME "LibTIFF Software")
set(PACKAGE_TARNAME "tiff")
set(PACKAGE_BUGREPORT "tiff@lists.osgeo.org")
set(PACKAGE_URL "https://libtiff.gitlab.io/libtiff/")

if(WIN32)
	set(USE_WIN32_FILEIO 1)
	set(TIFF_PLATFORM_SOURCE "${TIFF_DIR}/tif_win32.c")
else()
	set(TIFF_PLATFORM_SOURCE "${TIFF_DIR}/tif_unix.c")
endif()

configure_file("${TIFF_DIR}/tif_config.h.cmake.in" "${TIFF_CONFIG_DIR}/tif_config.h" @ONLY)
configure_file("${TIFF_DIR}/tiffconf.h.cmake.in" "${TIFF_CONFIG_DIR}/tiffconf.h" @ONLY)

################################################################################
# TIFF Library
################################################################################

add_library(TIFF STATIC
	"${TIFF_DIR}/tif_aux.c"
	"${TIFF_DIR}/tif_close.c"
	"${TIFF_DIR}/tif_codec.c"
	"${TIFF_DIR}/tif_color.c"
	"${TIFF_DIR}/tif_compress.c"
	"${TIFF_DIR}/tif_dir.c"
	"${TIFF_DIR}/tif_dirinfo.c"
	"${TIFF_DIR}/tif_dirread.c"
	"${TIFF_DIR}/tif_dirwrite.c"
	"${TIFF_DIR}/tif_dumpmode.c"
	"${TIFF_DIR}/tif_error.c"
	"${TIFF_DIR}/tif_extension.c"
	"${TIFF_DIR}/tif_fax3.c"
	"${TIFF_DIR}/tif_fax3sm.c"
	"${TIFF_DIR}/tif_flush.c"
	"${TIFF_DIR}/tif_getimage.c"
	"${TIFF_DIR}/tif_hash_set.c"
	"${TIFF_DIR}/tif_jbig.c"
	"${TIFF_DIR}/tif_jpeg.c"
	"${TIFF_DIR}/tif_jpeg_12.c"
	"${TIFF_DIR}/tif_lerc.c"
	"${TIFF_DIR}/tif_luv.c"
	"${TIFF_DIR}/tif_lzma.c"
	"${TIFF_DIR}/tif_lzw.c"
	"${TIFF_DIR}/tif_next.c"
	"${TIFF_DIR}/tif_ojpeg.c"
	"${TIFF_DIR}/tif_open.c"
	"${TIFF_DIR}/tif_packbits.c"
	"${TIFF_DIR}/tif_pixarlog.c"
	"${TIFF_DIR}/tif_predict.c"
	"${TIFF_DIR}/tif_print.c"
	"${TIFF_DIR}/tif_read.c"
	"${TIFF_DIR}/tif_strip.c"
	"${TIFF_DIR}/tif_swab.c"
	"${TIFF_DIR}/tif_thunder.c"
	"${TIFF_DIR}/tif_tile.c"
	"${TIFF_DIR}/tif_version.c"
	"${TIFF_DIR}/tif_warning.c"
	"${TIFF_DIR}/tif_webp.c"
	"${TIFF_DIR}/tif_write.c"
	"${TIFF_DIR}/tif_zip.c"
	"${TIFF_DIR}/tif_zstd.c"
	"${TIFF_DIR}/tif_stream.cxx"
	"${TIFF_PLATFORM_SOURCE}"
)

# Same rules as upstream: only tif_open.c and the platform file may use _TIFFmalloc/_TIFFfree
target_compile_definitions(TIFF PRIVATE TIFF_DO_NOT_USE_NON_EXT_ALLOC_FUNCTIONS)
set_property(SOURCE "${TIFF_DIR}/tif_open.c" "${TIFF_PLATFORM_SOURCE}" APPEND PROPERTY COMPILE_DEFINITIONS ALLOW_TIFF_NON_EXT_ALLOC_FUNCTIONS)

target_include_directories(TIFF PUBLIC "${TIFF_CONFIG_DIR}" "${TIFF_DIR}")
target_link_libraries(TIFF PRIVATE ZLib JPG)

VL_THIRDPARTY_TARGET(TIFF)

VL_PROJECT_ADD(VLCORE INCLUDE_DIRECTORIES "${TIFF_CONFIG_DIR}" "${TIFF_DIR}" LINK_LIBRARIES TIFF)
