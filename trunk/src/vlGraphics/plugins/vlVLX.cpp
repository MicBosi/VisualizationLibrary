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
ref<ResourceDatabase> vl::loadVLT(const String& path)
{
  ref<VirtualFile> file = vl::locateFile(path);
  return loadVLT(file.get());
}
//-----------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadVLT(VirtualFile* file)
{
  // mic fixme: debug only
  Time timer;
  timer.start();

  VLXSerializer serializer;
  ref<ResourceDatabase> res_db = serializer.loadVLT(file)->as<ResourceDatabase>();

  if (serializer.error())
    Log::error( Say("vl::loadVLT : VLXSerializer reported: %s.\n") << serializer.errorString() );

  Log::debug( Say("VLT import = %n\n") << timer.elapsed() );

  return res_db;
}
//-----------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadVLB(const String& path)
{
  ref<VirtualFile> file = vl::locateFile(path);
  return loadVLB(file.get());
}
//-----------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadVLB(VirtualFile* file)
{
  // mic fixme: debug only
  Time timer;
  timer.start();

  VLXSerializer serializer;
  ref<ResourceDatabase> res_db = serializer.loadVLB(file)->as<ResourceDatabase>();

  if (serializer.error())
    Log::error( Say("vl::loadVLB : VLXSerializer reported: %s.\n") << serializer.errorString() );

  Log::debug( Say("VLB import = %n\n") << timer.elapsed() );

  return res_db;
}
//-----------------------------------------------------------------------------
bool vl::saveVLT(const String& path, const ResourceDatabase* res_db)
{
  ref<DiskFile> file = new DiskFile(path);
  return saveVLT(file.get(), res_db);
}
//-----------------------------------------------------------------------------
bool vl::saveVLT(VirtualFile* file, const ResourceDatabase* res_db)
{
  VL_CHECK(res_db);
  if (!res_db)
    return false;

  // mic fixme: debug only
  Time timer;
  timer.start();

  VLXSerializer serializer;
  serializer.saveVLT( file, res_db );

  if (serializer.error())
    Log::error( Say("vl::saveVLT : VLXSerializer reported: %s.\n") << serializer.errorString() );

  Log::debug( Say("VLT export = %n\n") << timer.elapsed() );

  return serializer.error() == VLXSerializer::NoError;
}
//-----------------------------------------------------------------------------
bool vl::saveVLB(const String& path, const ResourceDatabase* res_db)
{
  ref<DiskFile> file = new DiskFile(path);
  return saveVLB(file.get(), res_db);
}
//-----------------------------------------------------------------------------
bool vl::saveVLB(VirtualFile* file, const ResourceDatabase* res_db)
{
  VL_CHECK(res_db);
  if (!res_db)
    return false;

  // mic fixme: debug only
  Time timer;
  timer.start();

  VLXSerializer serializer;
  serializer.saveVLB( file, res_db );

  if (serializer.error())
    Log::error( Say("vl::saveVLB : VLXSerializer reported: %s.\n") << serializer.errorString() );

  Log::debug( Say("VLB export = %n\n") << timer.elapsed() );

  return serializer.error() == VLXSerializer::NoError;
}
//-----------------------------------------------------------------------------
bool vl::isVLT(const String& path)
{
  ref<VirtualFile> file = vl::locateFile(path);
  return isVLT(file.get());
}
//-----------------------------------------------------------------------------
bool vl::isVLT(VirtualFile* file)
{
  if (!file)
    return false;
  char vlx[12] = { 0 };
  memset(vlx, 0, sizeof(vlx));
  file->close();
  file->open(OM_ReadOnly);
  file->read(vlx, sizeof(vlx));
  file->close();
  return memcmp(vlx, "VLX version=", sizeof(vlx)) == 0;
}
//-----------------------------------------------------------------------------
bool vl::isVLB(const String& path)
{
  ref<VirtualFile> file = vl::locateFile(path);
  return isVLT(file.get());
}
//-----------------------------------------------------------------------------
bool vl::isVLB(VirtualFile* file)
{
  if (!file)
    return false;
  unsigned char vlx_identifier[] = { 0xAB, 'V', 'L', 'X', 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };
  unsigned char vlx[sizeof(vlx_identifier)];
  memset(vlx, 0, sizeof(vlx));
  file->close();
  file->open(OM_ReadOnly);
  file->read(vlx, sizeof(vlx));
  file->close();
  return memcmp(vlx, vlx_identifier, sizeof(vlx_identifier)) == 0;
}
//-----------------------------------------------------------------------------
