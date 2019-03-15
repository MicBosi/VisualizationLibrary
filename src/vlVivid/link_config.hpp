/*=============================================================================

  vlVivid is part of NifTK: A software platform for medical image computing.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See NifTK license for details.

=============================================================================*/

#ifndef VLVIVID_CONFIG_INCLUDE_ONCE
#define VLVIVID_CONFIG_INCLUDE_ONCE

#include <vlCore/config.hpp>

// VLVIVID_EXPORT macro
#if defined(_WIN32) && !defined(VL_STATIC_LINKING)
  #ifdef VLVivid_EXPORTS
    #define VLVIVID_EXPORT __declspec(dllexport)
  #else
    #define VLVIVID_EXPORT __declspec(dllimport)
  #endif
#else
  #define VLVIVID_EXPORT
#endif

#endif // VLVIVID_CONFIG_INCLUDE_ONCE
