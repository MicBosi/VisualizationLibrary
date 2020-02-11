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
# Doxygen Documentation
################################################################################

project(DOCS)

option(VL_BUILD_DOCS "Build VL documentation" OFF)

if (VL_BUILD_DOCS)
    set(DOXYGEN_SKIP_DOT TRUE)
    find_package(Doxygen)

    if (DOXYGEN_FOUND)
      set( DOXYFILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile" )
      configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" "${DOXYFILE_PATH}" @ONLY )

      ADD_CUSTOM_TARGET(Docs
        COMMENT "Generating API documentation with Doxygen..."
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYFILE_PATH}"
      )

      # Does not work
      # set_property( DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "html-out/html" )

      option(VL_INSTALL_DOCS "Install VL documentation" OFF)
      if(VL_INSTALL_DOCS)
        if(VL_UNIX_INSTALL_MODE)
          # install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/html-in/" DESTINATION "share/vl/docs/")
          # install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html-out/" DESTINATION "share/vl/docs/html")
          install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/html-out/" DESTINATION "share/vl/docs/")
        else()
          # install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/html-in/" DESTINATION "docs/")
          # install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html-out/" DESTINATION "docs/html")
          install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/html-out/" DESTINATION "docs/")
        endif()
      endif()
    endif()
endif()

