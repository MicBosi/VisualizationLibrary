/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
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

#include <vlGraphics/plugins/vlVLX.hpp>
#include <vlCore/Time.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadVLX(const String& path)
{
  ref<DiskFile> file = new DiskFile(path);
  return loadVLX(file.get());
}
//-----------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadVLX(VirtualFile* file)
{
  // mic fixme: debug only
  Time timer;
  timer.start();

  VLX_Serializer serializer;
  ref<ResourceDatabase> res_db = serializer.loadText(file)->as<ResourceDatabase>();

  printf("Export time = %f\n", timer.elapsed());

  return res_db;
}
//-----------------------------------------------------------------------------
bool vl::writeVLX(const String& path, const ResourceDatabase* res_db)
{
  ref<DiskFile> file = new DiskFile(path);
  return writeVLX(file.get(), res_db);
}
//-----------------------------------------------------------------------------
bool vl::writeVLX(VirtualFile* file, const ResourceDatabase* res_db)
{
  // mic fixme: debug only
  Time timer;
  timer.start();

  VLX_Serializer serializer;
  bool ok = serializer.saveText( file, res_db );

  printf("Import time = %f\n", timer.elapsed());

  if (ok)
    Log::error("LoadWriterVLX: serialization error.\n");

  return ok;
}
//-----------------------------------------------------------------------------
