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

#include <vlCore/LoadWriterManager.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
const ResourceLoadWriter* LoadWriterManager::findLoader(VirtualFile* file) const 
{
  if (file->path().empty())
    Log::warning("findLoader(VirtualFile* file): cannot check the file extension, file->path() is empty!\n");
  return findLoader(file->path());
}
//-----------------------------------------------------------------------------
const ResourceLoadWriter* LoadWriterManager::findWriter(VirtualFile* file) const 
{
  if (file->path().empty())
    Log::warning("findWriter(VirtualFile* file): cannot check the file extension, file->path() is empty!\n");
  return findWriter(file->path());
}
//-----------------------------------------------------------------------------
ref<ResourceDatabase> LoadWriterManager::loadResource(const String& path, bool quick) const 
{
  ref<VirtualFile> file = locateFile(path);
  if (file)
    return loadResource(file.get(), quick);
  else
    return NULL;
}
//-----------------------------------------------------------------------------
ref<ResourceDatabase> LoadWriterManager::loadResource(VirtualFile* file, bool quick) const 
{
  const ResourceLoadWriter* loadwriter = findLoader(file);
  if (loadwriter)
  {
    ref<ResourceDatabase> db;
    if (quick)
    {
      // caching the data in the memory provides a huge performance boost
      ref<MemoryFile> memfile = new MemoryFile;
      memfile->allocateBuffer(file->size());
      file->open(OM_ReadOnly);
      file->read(memfile->ptr(),file->size());
      file->close();
      memfile->setPath(file->path());
      db = loadwriter->loadResource(memfile.get());
    }
    else
      db = loadwriter->loadResource(file);
    // load callbacks
    for(int i=0; db && i<loadCallbacks()->size(); ++i)
      const_cast<LoadCallback*>(loadCallbacks()->at(i))->operator()(db.get());
    return db;
  }
  else
  {
    Log::error( Say("no ResourceLoadWriter registered to load '%s'.\n") << file->path() );
    return NULL; 
  }
}
//-----------------------------------------------------------------------------
bool LoadWriterManager::writeResource(const String& path, ResourceDatabase* resource) const
{
  const ResourceLoadWriter* loadwriter = findWriter(path);
  if (loadwriter)
  {
    // write callbacks
    for(int i=0; i<writeCallbacks()->size(); ++i)
      const_cast<WriteCallback*>(writeCallbacks()->at(i))->operator()(resource);
    // write resource
    return loadwriter->writeResource(path,resource);
  }
  else
  {
    Log::error( Say("no ResourceLoadWriter registered to write '%s'.\n") << path );
    return false; 
  }
}
//-----------------------------------------------------------------------------
bool LoadWriterManager::writeResource(VirtualFile* file, ResourceDatabase* resource) const
{ 
  const ResourceLoadWriter* loadwriter = findWriter(file);
  if (loadwriter)
  {
    // write callbacks
    for(int i=0; i<writeCallbacks()->size(); ++i)
      const_cast<WriteCallback*>(writeCallbacks()->at(i))->operator()(resource);
    // write resource
    return loadwriter->writeResource(file,resource);
  }
  else
  {
    Log::error( Say("no ResourceLoadWriter registered to write '%s'.\n") << file->path() );
    return false; 
  }
}
//-----------------------------------------------------------------------------
const ResourceLoadWriter* LoadWriterManager::findLoader(const String& path) const 
{
  String ext = path.extractFileExtension(false).toLowerCase();
  for(int i=0; i<loadWriters()->size(); ++i)
    if (loadWriters()->at(i)->canLoad(ext))
      return loadWriters()->at(i);

  return NULL;
}
//-----------------------------------------------------------------------------
const ResourceLoadWriter* LoadWriterManager::findWriter(const String& path) const 
{
  String ext = path.extractFileExtension(false).toLowerCase();
  for(int i=0; i<loadWriters()->size(); ++i)
    if (loadWriters()->at(i)->canWrite(ext))
      return loadWriters()->at(i);

  return NULL;
}
//-----------------------------------------------------------------------------
void LoadWriterManager::registerLoadWriter(ResourceLoadWriter* load_writer)
{
  ref<ResourceLoadWriter> lowr = load_writer;
  LoadWriterManager::loadWriters()->erase( load_writer );
  LoadWriterManager::loadWriters()->push_back( load_writer );
}
//-----------------------------------------------------------------------------
bool vl::canLoad(const String& path)  { return defLoadWriterManager()->canLoad(path);  }
//-----------------------------------------------------------------------------
bool vl::canWrite(const String& path) { return defLoadWriterManager()->canWrite(path); }
//-----------------------------------------------------------------------------
bool vl::canLoad(VirtualFile* file)   { return defLoadWriterManager()->canLoad(file);  }
//-----------------------------------------------------------------------------
bool vl::canWrite(VirtualFile* file)  { return defLoadWriterManager()->canWrite(file); }
//-----------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadResource(const String& path, bool quick) { return defLoadWriterManager()->loadResource(path,quick); }
//-----------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadResource(VirtualFile* file, bool quick)  { return defLoadWriterManager()->loadResource(file,quick); }
//-----------------------------------------------------------------------------
bool vl::writeResource(const String& path, ResourceDatabase* resource) { return defLoadWriterManager()->writeResource(path, resource); }
//-----------------------------------------------------------------------------
bool vl::writeResource(VirtualFile* file, ResourceDatabase* resource) { return defLoadWriterManager()->writeResource(file, resource); }
//-----------------------------------------------------------------------------


