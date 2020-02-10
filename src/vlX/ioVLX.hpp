/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*                                                                                    */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*                                                                                    */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*                                                                                    */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   */
/*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            */
/*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  */
/*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    */
/*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      */
/*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    */
/*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      */
/*                                                                                    */
/**************************************************************************************/

#ifndef LoadWriterVLX_INCLUDE_ONCE
#define LoadWriterVLX_INCLUDE_ONCE

#include <vlX/ClassWrapper.hpp>
#include <vlX/Registry.hpp>
#include <vlX/Serializer.hpp>
#include <vlX/Value.hpp>
#include <vlX/vlxutils.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/DiskFile.hpp>

namespace vlX
{
  //-----------------------------------------------------------------------------

  VLX_EXPORT vl::ref<vl::ResourceDatabase> loadVLT(vl::VirtualFile* file);
  VLX_EXPORT vl::ref<vl::ResourceDatabase> loadVLT(const vl::String& path);
  VLX_EXPORT vl::ref<vl::ResourceDatabase> loadVLB(vl::VirtualFile* file);
  VLX_EXPORT vl::ref<vl::ResourceDatabase> loadVLB(const vl::String& path);
  VLX_EXPORT bool saveVLT(vl::VirtualFile* file, const vl::ResourceDatabase*);
  VLX_EXPORT bool saveVLT(const vl::String& file, const vl::ResourceDatabase*);
  VLX_EXPORT bool saveVLB(vl::VirtualFile* file, const vl::ResourceDatabase*);
  VLX_EXPORT bool saveVLB(const vl::String& file, const vl::ResourceDatabase*);
  VLX_EXPORT bool isVLT(vl::VirtualFile* file);
  VLX_EXPORT bool isVLT(const vl::String& file);
  VLX_EXPORT bool isVLB(vl::VirtualFile* file);
  VLX_EXPORT bool isVLB(const vl::String& file);

  //---------------------------------------------------------------------------
  // LoadWriterVLX
  //---------------------------------------------------------------------------
  /**
   * A vl::ResourceLoadWriter capable of reading Visualization Library's VLT and VLB files.
   */
  class LoadWriterVLX: public vl::ResourceLoadWriter
  {
    VL_INSTRUMENT_CLASS(vl::LoadWriterVLX, vl::ResourceLoadWriter)

  public:
    LoadWriterVLX(): vl::ResourceLoadWriter("|vlt|vlb|", "|vlt|vlb|") {}

    vl::ref<vl::ResourceDatabase> loadResource(const vl::String& path) const
    {
      if (isVLT(path))
        return loadVLT(path);
      else
      if (isVLB(path))
        return loadVLB(path);
      else
        return NULL;
    }

    vl::ref<vl::ResourceDatabase> loadResource(vl::VirtualFile* file) const
    {
      if (isVLT(file))
        return loadVLT(file);
      else
      if (isVLB(file))
        return loadVLB(file);
      else
        return NULL;
    }

    bool writeResource(const vl::String& path, vl::ResourceDatabase* res_db) const
    {
      if (path.extractFileExtension().toLowerCase() == "vlt")
        return saveVLT(path, res_db);
      else
      if (path.extractFileExtension().toLowerCase() == "vlb")
        return saveVLB(path, res_db);
      else
        return false;
    }

    bool writeResource(vl::VirtualFile* file, vl::ResourceDatabase* res_db) const
    {
      if (file->path().extractFileExtension().toLowerCase() == "vlt")
        return saveVLT(file, res_db);
      else
      if (file->path().extractFileExtension().toLowerCase() == "vlb")
        return saveVLB(file, res_db);
      else
        return false;
    }
  };
//-----------------------------------------------------------------------------
}

#endif
