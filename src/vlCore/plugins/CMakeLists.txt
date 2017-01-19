################################################################################
#                                                                              #
#  Copyright (c) 2005-2011, Michele Bosi, Thiago Bastos                        #
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
# Compile-time Plugins for VLCore
################################################################################

include_directories("../src/3rdparty/freetype")
include_directories("../src/3rdparty/jpeg-6b")
include_directories("../src/3rdparty/libpng")
include_directories("../src/3rdparty/libtiff")
include_directories("../src/3rdparty/zlib")

# List of "2D IO" plugins
set(VLCORE_PLUGINS "BMP" "DAT" "DDS" "DICOM" "JPG" "PNG" "TGA" "TIFF")
set(INSTALL_DIR "${VL_INCLUDE_INSTALL_DIR}/vlCore/plugins")

# All plugins are enabled by default, except for DICOM
set(VL_IO_2D_DICOM_OPTION "Enable DICOM support (requires GDCM)" OFF)

# Process plugins
VL_PROCESS_PROJECT_PLUGINS(VLCORE VL_IO_2D_ ${INSTALL_DIR} ${VLCORE_PLUGINS})

################################################################################
# Add external/3rd-party dependencies as necessary
################################################################################

if(VL_IO_2D_PNG)
	add_subdirectory("libpng")
endif()

if(VL_IO_2D_TIFF)
	add_subdirectory("libtiff")
endif()

if(VL_IO_2D_JPG OR VL_IO_2D_TIFF)
    add_subdirectory("jpeg")
endif()

if(VL_IO_2D_DICOM)
	FIND_PACKAGE(GDCM REQUIRED)

        MESSAGE(STATUS, ${GDCM_INCLUDE_DIRS})
        MESSAGE(STATUS, ${GDCM_LIBRARY_DIRS})
        
        if(WIN32)
            file(GLOB _gdcm_libs "${GDCM_LIBRARY_DIRS}/gdcm*.lib")
        else()
            file(GLOB _gdcm_libs "${GDCM_LIBRARY_DIRS}/libgdcm*.so*")
        endif()

        MESSAGE(STATUS, ${_gdcm_libs})
        
        set(VL_IO_2D_GDCM_LIB_DIR ${GDCM_LIBRARY_DIRS} CACHE PATH "GDCM library directory")
	set(VL_IO_2D_GDCM_LIBRARIES ${_gdcm_libs} CACHE LIST "GDCM libraries (semicolon separated)")
	set(VL_IO_2D_GDCM_INC_DIR ${GDCM_INCLUDE_DIRS} CACHE PATH "GDCM include directory")
	
	VL_PROJECT_ADD(VLCORE
		INCLUDE_DIRECTORIES "${VL_IO_2D_GDCM_INC_DIR}"
		LINK_LIBRARIES "${VL_IO_2D_GDCM_LIBRARIES}"
	)
else()
	unset(VL_IO_2D_GDCM_LIB_DIR CACHE)
	unset(VL_IO_2D_GDCM_LIBRARIES CACHE)
	unset(VL_IO_2D_GDCM_INC_DIR CACHE)
endif()

# VLCore requires ZLib (not really a plugin, it's here for convenience)
add_subdirectory("zlib")

