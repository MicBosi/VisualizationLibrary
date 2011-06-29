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

project(EXAMPLES)

################################################################################
# Compile all Applets
################################################################################

add_subdirectory("Applets")

################################################################################
# Build the Example/Test executables
################################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Common libraries
set(VL_LIBS_EXAMPLE VLGraphics VLCore)
set(VL_LIBS_TESTS VLMolecule VLVG VLVolume VLGraphics VLCore)

if(VL_GUI_SDL_EXAMPLES)
    # Darwin
    if( ${CMAKE_SYSTEM} MATCHES "Darwin" )
        set(SDLMAIN_M_PATH "Specify path to SDLMain.m" CACHE FILEPATH "Path to SDLMain.m")
    endif()

    include_directories(${SDL_INCLUDE_DIR})

    # Example
    add_executable(SDL_example SDL_example.cpp Applets/App_RotatingCube.hpp ${SDLMAIN_M_PATH})
    target_link_libraries(SDL_example VLSDL ${VL_LIBS_EXAMPLE} ${SDLMAIN_LIBRARY} ${SDL_LIBRARY} ${OPENGL_LIBRARIES})
    VL_INSTALL_TARGET(SDL_example)

    # Tests
    add_executable(SDL_tests SDL_tests.cpp ${SDLMAIN_M_PATH})
    target_link_libraries(SDL_tests VLApplets VLSDL ${VL_LIBS_TESTS} ${SDLMAIN_LIBRARY} ${SDL_LIBRARY} ${OPENGL_LIBRARIES})
    VL_INSTALL_TARGET(SDL_tests)
endif()

if(VL_GUI_GLUT_EXAMPLES)
    include_directories(${GLUT_INCLUDE_DIR})

    # Example
    add_executable(GLUT_example GLUT_example.cpp Applets/App_RotatingCube.hpp)
    target_link_libraries(GLUT_example VLGLUT ${VL_LIBS_EXAMPLE} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})
    VL_INSTALL_TARGET(GLUT_example)

    # Tests
    add_executable(GLUT_tests GLUT_tests.cpp)
    target_link_libraries(GLUT_tests VLApplets VLGLUT ${VL_LIBS_TESTS} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})
    VL_INSTALL_TARGET(GLUT_tests)
endif()

if(VL_GUI_QT4_EXAMPLES)
	include(${QT_USE_FILE})

	# Example
	add_executable(Qt4_example WIN32 Qt4_example.cpp Applets/App_RotatingCube.hpp)
	target_link_libraries(Qt4_example VLQt4 ${VL_LIBS_EXAMPLE} ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
	VL_INSTALL_TARGET(Qt4_example)

	# Tests
	add_executable(Qt4_tests WIN32 Qt4_tests.cpp )
	target_link_libraries(Qt4_tests VLApplets VLQt4 ${VL_LIBS_TESTS} ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
	VL_INSTALL_TARGET(Qt4_tests)
endif()

if (VL_GUI_WIN32_EXAMPLES)
	# Direct
	add_executable(Win32_direct WIN32 Win32_direct.cpp)
	target_link_libraries(Win32_direct VLWin32 ${VL_LIBS_EXAMPLE} ${OPENGL_LIBRARIES})
	VL_INSTALL_TARGET(Win32_direct)

	# Example
	add_executable(Win32_example WIN32 Win32_example.cpp Applets/App_RotatingCube.hpp)
	target_link_libraries(Win32_example VLWin32 ${VL_LIBS_EXAMPLE} ${OPENGL_LIBRARIES})
	VL_INSTALL_TARGET(Win32_example)

	# Tests
	add_executable(Win32_tests WIN32 Win32_tests.cpp )
	target_link_libraries(Win32_tests VLApplets VLWin32 ${VL_LIBS_TESTS} ${OPENGL_LIBRARIES})
	VL_INSTALL_TARGET(Win32_tests)
endif()

if (VL_GUI_MFC_EXAMPLES)
	# Example
	add_executable(MFC_example WIN32 MFC_example/MFC_example.cpp MFC_example/StdAfx.cpp )
	target_link_libraries(MFC_example VLApplets VLMFC ${VL_LIBS_EXAMPLE} ${OPENGL_LIBRARIES})
	set_target_properties(MFC_example PROPERTIES COMPILE_FLAGS -D_AFXDLL)
	set_target_properties(MFC_example PROPERTIES LINK_FLAGS /ENTRY:wWinMainCRTStartup)
	VL_INSTALL_TARGET(MFC_example)

	# MDI Example
	file(GLOB MFC_MDI_SRC "MFC_MDI_example/*.cpp")
	file(GLOB MFC_MDI_INC "MFC_MDI_example/*.hpp")
	file(GLOB MFC_MDI_RC "MFC_MDI_example/*.rc")
	add_executable(MFC_MDI_example WIN32 ${MFC_MDI_SRC} ${MFC_MDI_INC} ${MFC_MDI_RC})
	target_link_libraries(MFC_MDI_example VLApplets VLMFC ${VL_LIBS_TESTS} ${OPENGL_LIBRARIES})
	set_target_properties(MFC_MDI_example PROPERTIES COMPILE_FLAGS -D_AFXDLL)
	set_target_properties(MFC_MDI_example PROPERTIES LINK_FLAGS /ENTRY:wWinMainCRTStartup)
	VL_INSTALL_TARGET(MFC_MDI_example)

	# Tests
	add_executable(MFC_tests WIN32 MFC_tests/MFC_tests.cpp MFC_tests/StdAfx.cpp )
	target_link_libraries(MFC_tests VLApplets VLMFC ${VL_LIBS_TESTS} ${OPENGL_LIBRARIES})
	set_target_properties(MFC_tests PROPERTIES COMPILE_FLAGS -D_AFXDLL)
	set_target_properties(MFC_tests PROPERTIES LINK_FLAGS /ENTRY:wWinMainCRTStartup)
	VL_INSTALL_TARGET(MFC_tests)
endif()

if(VL_GUI_WXWIDGETS_EXAMPLES)
    include(${wxWidgets_USE_FILE})

    # Example
    add_executable(WX_example WIN32 WX_example.cpp Applets/App_RotatingCube.hpp)
    target_link_libraries(WX_example VLWX ${VL_LIBS_EXAMPLE} ${wxWidgets_LIBRARIES} ${OPENGL_LIBRARIES})
    VL_INSTALL_TARGET(WX_example)

    # Tests
    add_executable(WX_tests WIN32 WX_tests.cpp )
    target_link_libraries(WX_tests VLApplets VLWX ${VL_LIBS_TESTS} ${wxWidgets_LIBRARIES} ${OPENGL_LIBRARIES})
    VL_INSTALL_TARGET(WX_tests)
endif()

if (VL_GLES1_EXAMPLES)
	# Example
	add_executable(GLES1_example GLES1_example.cpp)
	target_link_libraries(GLES1_example VLEGL ${VL_LIBS_EXAMPLE} ${VL_GLES_LIBRARIES})
	VL_INSTALL_TARGET(GLES1_example)

	# Tests
	add_executable(GLES1_tests GLES1_tests.cpp)
	target_link_libraries(GLES1_tests VLApplets VLEGL ${VL_LIBS_TESTS} ${VL_GLES_LIBRARIES})
	VL_INSTALL_TARGET(GLES1_tests)
endif()
