################################################################################
#                                                                              #
#  Copyright (c) 2005-2011, Michele Bosi                                       #
#  All rights reserved.                                                        #
#                                                                              #
#  This file is part of Visualization Library                                  #
#  http://www.visualizationlibrary.org                                         #
#                                                                              #
#  Released under the OSI approved Simplified BSD License                      #
#  http://www.opensource.org/licenses/bsd-license.php                          #
#                                                                              #
################################################################################

################################################################################
# COLLADA DOM Library
################################################################################

# See also http://www.collada.org/mediawiki/index.php/DOM_guide:_Setting_up

set(COLLADA_DOM_ROOT "NOTFOUND" CACHE PATH "Root of COLLADA DOM installation")

# mic fixme: 
# for now only support DLL/Windows
# todo: support Static/Windows & Static/DLL/Linux
# todo: support Debug/Release versions
# COLLADA should be disabled by default
# use find file to double check that the files are there where they are supposed to be
if (COLLADA_DOM_ROOT)
	if(MSVC8)
		set(COLLADA_DOM_LIB "${COLLADA_DOM_ROOT}/build/vc8-1.4-d/libcollada14dom22-d.lib")
	elseif(MSVC9)
		set(COLLADA_DOM_LIB "${COLLADA_DOM_ROOT}/build/vc9-1.4-d/libcollada14dom22-d.lib")
	elseif(MSVC10)
		set(COLLADA_DOM_LIB "${COLLADA_DOM_ROOT}/build/vc10-1.4-d/libcollada14dom22-d.lib")
	endif()

	VL_PROJECT_ADD(VLGRAPHICS 
		DEFINITIONS "-DDOM_INCLUDE_LIBXML" "-DPCRE_STATIC" "-DBOOST_ALL_NO_LIB" "-DNO_BOOST"
		INCLUDE_DIRECTORIES "${COLLADA_DOM_ROOT}/include/" "${COLLADA_DOM_ROOT}/include/1.4" "${COLLADA_DOM_ROOT}/external-libs/boost"
		LINK_LIBRARIES ${COLLADA_DOM_LIB}) 
endif()
