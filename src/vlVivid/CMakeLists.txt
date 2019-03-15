################################################################################
#
#  vlVivid is part of NifTK: A software platform for medical image computing.
#
#  Copyright (c) University College London (UCL). All rights reserved.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#  See NifTK license for details.
#
################################################################################

################################################################################
# VLVivid Library
################################################################################

project(VLVIVID)

# Gather VLMolecule source files
file(GLOB VLVIVID_SRC "*.cpp")
file(GLOB VLVIVID_INC "*.hpp")

add_library(VLVivid ${VL_SHARED_OR_STATIC} ${VLVIVID_SRC} ${VLVIVID_INC})
VL_DEFAULT_TARGET_PROPERTIES(VLVivid)

target_link_libraries(VLVivid VLCore VLGraphics VLVolume)

################################################################################
# Install Rules
################################################################################

VL_INSTALL_TARGET(VLVivid)

# VLVivid headers
install(FILES ${VLVIVID_INC} DESTINATION "${VL_INCLUDE_INSTALL_DIR}/vlVivid")
