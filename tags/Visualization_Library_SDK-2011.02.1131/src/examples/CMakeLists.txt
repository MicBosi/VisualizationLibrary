######################################################################################
#                                                                                    #
#  Visualization Library                                                             #
#  http://www.visualizationlibrary.com                                               #
#                                                                                    #
#  Copyright (c) 2005-2010, Michele Bosi                                             #
#  All rights reserved.                                                              #
#                                                                                    #
#  Redistribution and use in source and binary forms, with or without modification,  #
#  are permitted provided that the following conditions are met:                     #
#                                                                                    #
#  - Redistributions of source code must retain the above copyright notice, this     #
#  list of conditions and the following disclaimer.                                  #
#                                                                                    #
#  - Redistributions in binary form must reproduce the above copyright notice, this  #
#  list of conditions and the following disclaimer in the documentation and/or       #
#  other materials provided with the distribution.                                   #
#                                                                                    #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   #
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     #
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            #
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  #
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    #
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      #
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    #
#  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           #
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     #
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      #
#                                                                                    #
######################################################################################

cmake_minimum_required(VERSION 2.6)

project(Examples)

include_directories(".")

if(MINGW)
	SET(MINGW32_LIB "libmingw32.a")
	SET(MINGW32_EXTRA_LIB "Winmm.lib")
endif()

if(GUI_MFC_EXAMPLES)
	add_subdirectory("MFC_example")
	add_subdirectory("MFC_MDI_example")
	add_subdirectory("MFC_tests")
endif()

if(${CMAKE_SYSTEM} MATCHES "Darwin")
	find_library(APP_KIT AppKit)
endif()

if(GUI_WIN32_EXAMPLES)
	# Direct
	add_executable(Win32_direct WIN32 Win32_direct.cpp)
	target_link_libraries(
		Win32_direct
		${VLWIN32_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${OPENGL_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
	)
	# Example
	add_executable(Win32_example WIN32 Win32_example.cpp Applets/App_RotatingCube.hpp)
	target_link_libraries(
		Win32_example
		${VLWIN32_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${OPENGL_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
	)
	# Tests
	add_executable(Win32_tests WIN32 Win32_tests.cpp)
	target_link_libraries(
		Win32_tests
		${VLWIN32_NAME}
		${VLAPPLETS_NAME}
		${VLVG_NAME} ${VLMOLECULE_NAME} ${VLVOLUME_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${OPENGL_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
	)
  # Install directives
  install(TARGETS Win32_tests Win32_example
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()

if(GUI_WXWIDGETS_EXAMPLES)
	# Example
	include_directories(${WXWIDGETS_INCLUDE_DIR})
	link_directories(${WXWIDGETS_LIBRARY_DIR})
	if (WIN32)
		add_executable(WX_example WIN32 WX_example.cpp Applets/App_RotatingCube.hpp)
	else()
		add_executable(WX_example WX_example.cpp Applets/App_RotatingCube.hpp)
	endif()
	target_link_libraries(
		WX_example
		${VLWX_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${WXWIDGETS_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${OPENGL_LIBRARIES}
	)
	# Tests
	if (WIN32)
		add_executable(WX_tests WIN32 WX_tests.cpp)
	else()
		add_executable(WX_tests WX_tests.cpp)
	endif()
	target_link_libraries(
		WX_tests
		${VLWX_NAME}
		${VLAPPLETS_NAME}
		${VLVG_NAME} ${VLMOLECULE_NAME} ${VLVOLUME_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${WXWIDGETS_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${OPENGL_LIBRARIES}
	)
  # Install directives
  install(TARGETS WX_tests WX_example
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()

if(GUI_GLUT_EXAMPLES)
	# Example
	include_directories(${GLUT_INCLUDE_DIR})
	add_executable(GLUT_example GLUT_example.cpp Applets/App_RotatingCube.hpp)
	target_link_libraries(
		GLUT_example
		${VLGLUT_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${GLUT_glut_LIBRARY}
		${OPENGL_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${APP_KIT}
	)
	# Tests
	add_executable(GLUT_tests GLUT_tests.cpp)
	target_link_libraries(
		GLUT_tests
		${VLGLUT_NAME}
		${VLAPPLETS_NAME}
		${VLVG_NAME} ${VLMOLECULE_NAME} ${VLVOLUME_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${GLUT_glut_LIBRARY}
		${OPENGL_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${APP_KIT}
	)
  # Install directives
  install(TARGETS GLUT_tests GLUT_example
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif(GUI_GLUT_EXAMPLES)

if(GUI_SDL_EXAMPLES)
  # Darwin
  if( ${CMAKE_SYSTEM} MATCHES "Darwin" )
      set(SDLMAIN_M_PATH "Specify path to SDLMain.m" CACHE FILEPATH "Path to SDLMain.m")
  endif()
	# Example
	include_directories(${SDL_INCLUDE_DIR})
  add_executable(SDL_example WIN32 SDL_example.cpp Applets/App_RotatingCube.hpp ${SDLMAIN_M_PATH})
	target_link_libraries(
		SDL_example
		${VLSDL_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${MINGW32_LIB}
		${SDLMAIN_LIBRARY}
		${SDL_LIBRARY}
		${OPENGL_LIBRARIES}
		${MINGW32_EXTRA_LIB}
	)
	# Tests
  add_executable(SDL_tests WIN32 SDL_tests.cpp ${SDLMAIN_M_PATH})
	target_link_libraries(
		SDL_tests
		${VLSDL_NAME}
		${VLAPPLETS_NAME}
		${VLVG_NAME} ${VLMOLECULE_NAME} ${VLVOLUME_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${MINGW32_LIB}
		${SDLMAIN_LIBRARY}
		${SDL_LIBRARY}
		${OPENGL_LIBRARIES}
		${MINGW32_EXTRA_LIB}
	)
  # Install directives
  install(TARGETS SDL_tests SDL_example
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()

if(GUI_QT4_EXAMPLES)
	# Example
	add_executable(Qt4_example WIN32 Qt4_example.cpp Applets/App_RotatingCube.hpp)
	target_link_libraries(
		Qt4_example
		${VLQT4_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${OPENGL_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${QT_LIBRARIES}
		${APP_KIT}
	)

	# Tests
	add_executable(Qt4_tests WIN32 Qt4_tests.cpp )

	# doesn't seem to work
	#if(MSVC)
	#	set_target_properties(Qt4_tests PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-")
	#endif()

	target_link_libraries(
		Qt4_tests
		${VLQT4_NAME}
		${VLAPPLETS_NAME}
		${VLVG_NAME} ${VLMOLECULE_NAME} ${VLVOLUME_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${OPENGL_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${QT_LIBRARIES}
		${APP_KIT}
	)

  # Install directives
  install(TARGETS Qt4_example Qt4_tests
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()

# note this is currently not used as all the Cocoa related classes has been deleted 
# in the hope that someday they will be reimplemented in full quality.
if(GUI_COCOA_EXAMPLES)
	# Example
	add_executable(Cocoa_example Cocoa_example.mm Applets/App_RotatingCube.hpp)

	FIND_LIBRARY(CORE_VIDEO CoreVideo)

	target_link_libraries(
		Cocoa_example
		${VLCOCOA_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${OPENGL_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${APP_KIT}
		${CORE_VIDEO}
	)

	# Tests
	add_executable(Cocoa_tests Cocoa_tests.mm)

	target_link_libraries(
		Cocoa_tests
		${VLCOCOA_NAME}
		${VLAPPLETS_NAME}
		${VLVG_NAME} ${VLMOLECULE_NAME} ${VLVOLUME_NAME}
		${VLGRAPHICS_NAME} ${VLCORE_NAME}
		${OPENGL_LIBRARIES}
		${VLFREETYPE_NAME} #FreeTypeLibrary
		${VL_MODULE_LIBRARIES}
		${VLZLIB_NAME} #ZLibLibrary
		${APP_KIT}
		${CORE_VIDEO}
	)

  # Install directives
  install(TARGETS Cocoa_example Cocoa_tests
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()

