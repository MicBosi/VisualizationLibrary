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

project(EXAMPLES)

if( VL_GUI_QT4_EXAMPLES OR
    VL_GUI_QT5_EXAMPLES OR
    VL_GUI_QT6_EXAMPLES OR
    VL_GUI_MFC_EXAMPLES OR
    VL_GUI_WIN32_EXAMPLES OR
    VL_GUI_WXWIDGETS_EXAMPLES OR
    VL_GUI_GLES_EXAMPLES OR
    VL_GUI_GLFW_EXAMPLES OR
    VL_GUI_GLUT_EXAMPLES OR
    VL_GUI_SDL_EXAMPLES )

  ################################################################################
  # Compile all Applets
  ################################################################################

  add_subdirectory("Applets")

  ################################################################################
  # Build the Example/Test executables
  ################################################################################

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

  # Common libraries
  set(VL_LIBS_EXAMPLE VLMain)
  set(VL_LIBS_TESTS VLMolecule VLVG VLVolume VLMain)

  if(VL_GUI_SDL_EXAMPLES)
      # Darwin
      if( ${CMAKE_SYSTEM} MATCHES "Darwin" )
          set(SDLMAIN_M_PATH "Specify path to SDLMain.m" CACHE FILEPATH "Path to SDLMain.m")
      endif()

      include_directories(${SDL_INCLUDE_DIR})

      # Example
      add_executable(vlSDL_example SDL_example.cpp Applets/App_RotatingCube.hpp ${SDLMAIN_M_PATH})
      target_link_libraries(vlSDL_example VLSDL ${VL_LIBS_EXAMPLE} ${SDLMAIN_LIBRARY} ${SDL_LIBRARY})
      VL_INSTALL_TARGET(vlSDL_example)

      # Tests
      add_executable(vlSDL_tests SDL_tests.cpp ${SDLMAIN_M_PATH})
      target_link_libraries(vlSDL_tests VLApplets VLSDL ${VL_LIBS_TESTS} ${SDLMAIN_LIBRARY} ${SDL_LIBRARY})
      VL_INSTALL_TARGET(vlSDL_tests)
  endif()

  if(VL_GUI_GLUT_EXAMPLES)
      include_directories(${GLUT_INCLUDE_DIR})

      # Example
      add_executable(vlGLUT_example GLUT_example.cpp Applets/App_RotatingCube.hpp)
      target_link_libraries(vlGLUT_example VLGLUT ${VL_LIBS_EXAMPLE} ${GLUT_LIBRARIES})
      VL_INSTALL_TARGET(vlGLUT_example)

      # Tests
      add_executable(vlGLUT_tests GLUT_tests.cpp)
      target_link_libraries(vlGLUT_tests VLApplets VLGLUT ${VL_LIBS_TESTS} ${GLUT_LIBRARIES})
      VL_INSTALL_TARGET(vlGLUT_tests)
  endif()

  if(VL_GUI_GLFW_EXAMPLES)
      include_directories(${GLFW_INCLUDE_DIR})

      # Example
      add_executable(vlGLFW_example GLFW_example.cpp Applets/App_RotatingCube.hpp)
      target_link_libraries(vlGLFW_example VLGLFW ${VL_LIBS_EXAMPLE} ${GLFW_LIBRARIES})
      VL_INSTALL_TARGET(vlGLFW_example)

      # Tests
      add_executable(vlGLFW_tests GLFW_tests.cpp)
      target_link_libraries(vlGLFW_tests VLApplets VLGLFW ${VL_LIBS_TESTS} ${GLFW_LIBRARIES})
      VL_INSTALL_TARGET(vlGLFW_tests)
  endif()

  if(VL_GUI_QT4_EXAMPLES)
  	include(${QT_USE_FILE})

  	# Example
  	add_executable(vlQt4_example WIN32 Qt4_example.cpp Applets/App_RotatingCube.hpp)
  	target_link_libraries(vlQt4_example VLQt4 ${VL_LIBS_EXAMPLE} ${QT_LIBRARIES})
  	VL_INSTALL_TARGET(vlQt4_example)

  	# Tests
  	add_executable(vlQt4_tests WIN32 Qt4_tests.cpp )
  	target_link_libraries(vlQt4_tests VLApplets VLQt4 ${VL_LIBS_TESTS} ${QT_LIBRARIES})
  	VL_INSTALL_TARGET(vlQt4_tests)
  endif()

  if(VL_GUI_QT5_EXAMPLES)
    # Example
    add_executable(vlQt5_example WIN32 Qt5_example.cpp Applets/App_RotatingCube.hpp)
    target_link_libraries(vlQt5_example VLQt5 ${VL_LIBS_EXAMPLE} Qt5::Widgets Qt5::OpenGL)
    VL_INSTALL_TARGET(vlQt5_example)

    # Tests
    add_executable(vlQt5_tests WIN32 Qt5_tests.cpp )
    target_link_libraries(vlQt5_tests VLApplets VLQt5 ${VL_LIBS_TESTS} Qt5::Widgets Qt5::OpenGL)
    VL_INSTALL_TARGET(vlQt5_tests)
  endif()

  if(VL_GUI_QT6_EXAMPLES)
    # Example
    add_executable(vlQt6_example WIN32 Qt6_example.cpp Applets/App_RotatingCube.hpp)
    target_link_libraries(vlQt6_example VLQt6 ${VL_LIBS_EXAMPLE} Qt6::Widgets Qt6::OpenGLWidgets)
    VL_INSTALL_TARGET(vlQt6_example)

    # Tests
    add_executable(vlQt6_tests WIN32 Qt6_tests.cpp )
    target_link_libraries(vlQt6_tests VLApplets VLQt6 ${VL_LIBS_TESTS} Qt6::Widgets Qt6::OpenGLWidgets)
    VL_INSTALL_TARGET(vlQt6_tests)
  endif()

  if (VL_GUI_WIN32_EXAMPLES)
  	# Direct
  	add_executable(vlWin32_direct WIN32 Win32_direct.cpp)
  	target_link_libraries(vlWin32_direct VLWin32 ${VL_LIBS_EXAMPLE})
  	VL_INSTALL_TARGET(vlWin32_direct)

  	# Example
  	add_executable(vlWin32_example WIN32 Win32_example.cpp Applets/App_RotatingCube.hpp)
  	target_link_libraries(vlWin32_example VLWin32 ${VL_LIBS_EXAMPLE})
  	VL_INSTALL_TARGET(vlWin32_example)

  	# Tests
  	add_executable(vlWin32_tests WIN32 Win32_tests.cpp )
  	target_link_libraries(vlWin32_tests VLApplets VLWin32 ${VL_LIBS_TESTS})
  	VL_INSTALL_TARGET(vlWin32_tests)
  endif()

  if (VL_GUI_MFC_EXAMPLES)
  	# Example
  	add_executable(vlMFC_example WIN32 MFC_example/MFC_example.cpp MFC_example/StdAfx.cpp )
  	target_link_libraries(vlMFC_example VLApplets VLMFC ${VL_LIBS_EXAMPLE})
  	set_target_properties(vlMFC_example PROPERTIES COMPILE_FLAGS -D_AFXDLL)
  	set_target_properties(vlMFC_example PROPERTIES LINK_FLAGS /ENTRY:wWinMainCRTStartup)
  	VL_INSTALL_TARGET(vlMFC_example)

  	# MDI Example
  	file(GLOB MFC_MDI_SRC "MFC_MDI_example/*.cpp")
  	file(GLOB MFC_MDI_INC "MFC_MDI_example/*.hpp")
  	file(GLOB MFC_MDI_RC "MFC_MDI_example/*.rc")
  	add_executable(vlMFC_MDI_example WIN32 ${MFC_MDI_SRC} ${MFC_MDI_INC} ${MFC_MDI_RC})
  	target_link_libraries(vlMFC_MDI_example VLApplets VLMFC ${VL_LIBS_TESTS})
  	set_target_properties(vlMFC_MDI_example PROPERTIES COMPILE_FLAGS -D_AFXDLL)
  	set_target_properties(vlMFC_MDI_example PROPERTIES LINK_FLAGS /ENTRY:wWinMainCRTStartup)
  	VL_INSTALL_TARGET(vlMFC_MDI_example)

  	# Tests
  	add_executable(vlMFC_tests WIN32 MFC_tests/MFC_tests.cpp MFC_tests/StdAfx.cpp )
  	target_link_libraries(vlMFC_tests VLApplets VLMFC ${VL_LIBS_TESTS})
  	set_target_properties(vlMFC_tests PROPERTIES COMPILE_FLAGS -D_AFXDLL)
  	set_target_properties(vlMFC_tests PROPERTIES LINK_FLAGS /ENTRY:wWinMainCRTStartup)
  	VL_INSTALL_TARGET(vlMFC_tests)
  endif()

  if(VL_GUI_WXWIDGETS_EXAMPLES)
      include(${wxWidgets_USE_FILE})

      # Example
      add_executable(vlWX_example WIN32 WX_example.cpp Applets/App_RotatingCube.hpp)
      target_link_libraries(vlWX_example VLWX ${VL_LIBS_EXAMPLE} ${wxWidgets_LIBRARIES})
      VL_INSTALL_TARGET(vlWX_example)

      # Tests
      add_executable(vlWX_tests WIN32 WX_tests.cpp )
      target_link_libraries(vlWX_tests VLApplets VLWX ${VL_LIBS_TESTS} ${wxWidgets_LIBRARIES})
      VL_INSTALL_TARGET(vlWX_tests)
  endif()

endif()
