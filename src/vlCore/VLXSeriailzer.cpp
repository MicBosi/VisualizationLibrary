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

#include <vlCore/VLXSerializer.hpp>
#include <vlCore/VLXVisitorCollectID.hpp>
#include <vlCore/VLXVisitorLinker.hpp>
#include <vlCore/VLXParserVLT.hpp>
#include <vlCore/VLXParserVLB.hpp>
#include <vlCore/VLXVisitorExportToVLT.hpp>
#include <vlCore/VLXVisitorExportToVLB.hpp>
#include <vlCore/DiskFile.hpp>
#include <vlCore/version.hpp>
#include <ctime>

using namespace vl;

//-----------------------------------------------------------------------------
Object* VLXSerializer::importVLX(const VLXStructure* st)
{
  if (error())
    return NULL;

  Object* obj = getImportedStructure(st);
  if (obj)
    return obj;
  else
  {
    std::map< std::string, ref<VLXClassWrapper> >::iterator it = registry()->importRegistry().find(st->tag());
    if (it != registry()->importRegistry().end())
    {
      VLXClassWrapper* serializer = it->second.get_writable();
      VL_CHECK(serializer);
      // import structure
      ref<Object> obj = serializer->importVLX(*this, st);
      if (!obj)
      {
        setError(ImportError);
        Log::error( Say("Error importing structure '%s'.") << st->tag() );
        VL_TRAP()
      }
      return obj.get();
    }
    else
    {
      setError(ImportError);
      Log::error( Say("No serializer found for structure '%s'.") << st->tag() );
      VL_TRAP();
      return NULL;
    }
  }
}
//-----------------------------------------------------------------------------
VLXStructure* VLXSerializer::exportVLX(const Object* obj)
{
  if (error())
    return NULL;

  VLXStructure* st = getExportedObject(obj);
  if (st)
    return st;
  else
  {
    std::map< const TypeInfo*, ref<VLXClassWrapper> >::iterator it = registry()->exportRegistry().find(obj->classType());
    if (it != registry()->exportRegistry().end())
    {
      VLXClassWrapper* serializer = it->second.get_writable();
      VL_CHECK(serializer);
      // export object
      ref<VLXStructure> st = serializer->exportVLX(*this, obj);
      if (!st)
      {
        setError(ExportError);
        Log::error( Say("Error exporting '%s'.") << obj->classType()->name() );
        VL_TRAP()
      }
      return st.get();
    }
    else
    {
      setError(ExportError);
      Log::error( Say("No serializer found for '%s'.") << obj->classType()->name() );
      VL_TRAP()
      return NULL;
    }
  }
}
//-----------------------------------------------------------------------------
bool VLXSerializer::canExport(const Object* obj) const 
{ 
  if (!registry())
    return false;
  else
    return registry()->exportRegistry().find(obj->classType()) != registry()->exportRegistry().end(); 
}
//-----------------------------------------------------------------------------
bool VLXSerializer::canImport(const VLXStructure* st) const 
{ 
  if (!registry())
    return false;
  else
    return registry()->importRegistry().find(st->tag()) != registry()->importRegistry().end(); 
}
//-----------------------------------------------------------------------------
void VLXSerializer::registerImportedStructure(const VLXStructure* st, Object* obj) 
{
  VL_CHECK( mImportedStructures.find(st) == mImportedStructures.end() )
  mImportedStructures[st] = obj;
}
//-----------------------------------------------------------------------------
void VLXSerializer::registerExportedObject(const Object* obj, VLXStructure* st)
{
  VL_CHECK(mExportedObjects.find(obj) == mExportedObjects.end())
  mExportedObjects[obj] = st;
}
//-----------------------------------------------------------------------------
Object* VLXSerializer::getImportedStructure(const VLXStructure* st)
{
  std::map< ref<VLXStructure>, ref<Object> >::iterator it = mImportedStructures.find(st);
  if (it == mImportedStructures.end())
    return NULL;
  else
  {
    VL_CHECK(it->second.get_writable() != NULL)
    return it->second.get_writable();
  }
}
//-----------------------------------------------------------------------------
VLXStructure* VLXSerializer::getExportedObject(const Object* obj)
{
  std::map< ref<Object>, ref<VLXStructure> >::iterator it = mExportedObjects.find(obj);
  if (it == mExportedObjects.end())
    return NULL;
  else
  {
    VL_CHECK(it->second.get_writable() != NULL)
    return it->second.get_writable();
  }
}
//-----------------------------------------------------------------------------
void VLXSerializer::signalImportError(const String& str) 
{ 
  // signal only the first one
  if (!error())
  {
    Log::error( str );
    setError( VLXSerializer::ImportError );
  }
}
//-----------------------------------------------------------------------------
void VLXSerializer::signalExportError(const String& str)
{ 
  // signal only the first one
  if (!error())
  {
    Log::error( str );
    setError( VLXSerializer::ExportError ); 
  }
}
//-----------------------------------------------------------------------------
std::string VLXSerializer::generateID(const char* prefix)
{
  char buffer[sizeof(int)*8+1];
  return std::string("#") + prefix + "id" + itoa(++mIDCounter, buffer, 10);
}
//-----------------------------------------------------------------------------
bool VLXSerializer::saveVLT(const String& path, const Object* obj, bool start_fresh)
{
  ref<DiskFile> file = new DiskFile(path);
  return saveVLT(file.get(), obj, start_fresh);
}
//-----------------------------------------------------------------------------
bool VLXSerializer::saveVLT(VirtualFile* file, const Object* obj, bool start_fresh)
{
  if (start_fresh)
    reset();

  if (mError)
    return false;

  ref<VLXStructure> meta = new VLXStructure("<Metadata>");
  std::map< std::string, VLXValue >::iterator it = metadata().begin();
  for( ; it != metadata().end(); ++it )
  {
    if (it->first == "VL_Serializer_Version")
      continue;
    if (it->first == "VLXWriter")
      continue;
    if (it->first == "Creation_Date")
      continue;
    meta->value().push_back( VLXStructure::Value(it->first.c_str(), it->second) );
  }

  // add VL metadata
  *meta << "VL_Serializer_Version" << VLXValue( (long long) 100 );

  String auth = Say("Visualization Library %n.%n.%n") << VL_Major << VL_Minor << VL_Build;
  *meta << "VLXWriter" << VLXValue( auth.toStdString().c_str(), VLXValue::String );

  time_t rawtime;
  time( &rawtime );
  std::string str = ctime(&rawtime);
  str.resize(str.size()-1); // remove the trailing \n
  *meta << "Creation_Date" << VLXValue( str.c_str(), VLXValue::String );

  ref<VLXStructure> st = exportVLX( obj );
  if (st)
  {
    std::map< std::string, int > uid_set;
    VLXVisitorCollectID uid_collector;
    uid_collector.setIDSet(&uid_set);
    meta->acceptVisitor(&uid_collector);
    st->acceptVisitor(&uid_collector);

    VLXVisitorExportToVLT text_export_visitor;
    text_export_visitor.setIDSet(&uid_set);
    text_export_visitor.writeHeader();
    meta->acceptVisitor(&text_export_visitor);
    st->acceptVisitor(&text_export_visitor);

    file->open(vl::OM_WriteOnly);
    file->write( text_export_visitor.text().c_str(), text_export_visitor.text().size() );
    file->close();

    return mError == NoError;
  }
  else
    return false;
}
//-----------------------------------------------------------------------------
bool VLXSerializer::saveVLB(const String& path, const Object* obj, bool start_fresh)
{
  ref<DiskFile> file = new DiskFile(path);
  return saveVLT(file.get(), obj, start_fresh);
}
//-----------------------------------------------------------------------------
bool VLXSerializer::saveVLB(VirtualFile* file, const Object* obj, bool start_fresh)
{
  if (start_fresh)
    reset();

  if (mError)
    return false;

  ref<VLXStructure> meta = new VLXStructure("<Metadata>");
  std::map< std::string, VLXValue >::iterator it = metadata().begin();
  for( ; it != metadata().end(); ++it )
  {
    if (it->first == "VL_Serializer_Version")
      continue;
    if (it->first == "VLXWriter")
      continue;
    if (it->first == "Creation_Date")
      continue;
    meta->value().push_back( VLXStructure::Value(it->first.c_str(), it->second) );
  }

  // add VL metadata
  *meta << "VL_Serializer_Version" << VLXValue( (long long) 100 );

  String auth = Say("Visualization Library %n.%n.%n") << VL_Major << VL_Minor << VL_Build;
  *meta << "VLXWriter" << VLXValue( auth.toStdString().c_str(), VLXValue::String );

  time_t rawtime;
  time( &rawtime );
  std::string str = ctime(&rawtime);
  str.resize(str.size()-1); // remove the trailing \n
  *meta << "Creation_Date" << VLXValue( str.c_str(), VLXValue::String );

  ref<VLXStructure> st = exportVLX( obj );
  if (st)
  {
    std::map< std::string, int > uid_set;
    VLXVisitorCollectID uid_collector;
    uid_collector.setIDSet(&uid_set);
    meta->acceptVisitor(&uid_collector);
    st->acceptVisitor(&uid_collector);

    VLXVisitorExportToVLB bin_export_visitor(file);
    bin_export_visitor.setIDSet(&uid_set);
    bin_export_visitor.writeHeader();
    meta->acceptVisitor(&bin_export_visitor);
    st->acceptVisitor(&bin_export_visitor);
    file->close();

    return mError == NoError;
  }
  else
    return false;
}
//-----------------------------------------------------------------------------
ref<Object> VLXSerializer::loadVLT(const String& path, bool start_fresh)
{
  ref<VirtualFile> file = vl::locateFile(path);
  return loadVLT(file.get(), start_fresh);
}
//-----------------------------------------------------------------------------
ref<Object> VLXSerializer::loadVLT(VirtualFile* file, bool start_fresh)
{
  if (start_fresh)
    reset();

  if (mError)
    return NULL;

  VLXParserVLT parser;
  parser.tokenizer()->setInputFile( file );

  bool ok = parser.parse();
  file->close();

  if (!ok)
  {
    setError(ImportError);
    return NULL;
  }

  // copy metadata over
  metadata() = parser.metadata();

  if (!parser.link())
  {
    setError(ImportError);
    return NULL;
  }

  if (parser.structures().empty())
    return NULL;
  else
    return importVLX( parser.structures()[0].get() ); // note that we ignore the other structures
}
//-----------------------------------------------------------------------------
ref<Object> VLXSerializer::loadVLB(const String& path, bool start_fresh)
{
  ref<VirtualFile> file = vl::locateFile(path);
  return loadVLB(file.get(), start_fresh);
}
//-----------------------------------------------------------------------------
ref<Object> VLXSerializer::loadVLB(VirtualFile* file, bool start_fresh)
{
  if (start_fresh)
    reset();

  if (mError)
    return NULL;

  VLXParserVLB parser;
  parser.setInputFile( file );

  bool ok = parser.parse();
  file->close();

  if (!ok)
  {
    setError(ImportError);
    return NULL;
  }

  // copy metadata over
  metadata() = parser.metadata();

  if (!parser.link())
  {
    setError(ImportError);
    return NULL;
  }

  if (parser.structures().empty())
    return NULL;
  else
    return importVLX( parser.structures()[0].get() ); // note that we ignore the other structures
}
//-----------------------------------------------------------------------------
