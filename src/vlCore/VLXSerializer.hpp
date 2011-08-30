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

#ifndef VLXSerializer_INCLUDE_ONCE
#define VLXSerializer_INCLUDE_ONCE

#include <vlCore/VLXRegistry.hpp>
#include <vlCore/VLXValue.hpp>
#include <string>
#include <map>

namespace vl
{
  class VLXSerializer: public Object
  {
  public:
    typedef enum { NoError, ImportError, ExportError } EError;

  public:
    VLXSerializer(): mUIDCounter(0), mError(NoError) 
    {
      setRegistry( defVLXRegistry() );
    }

    void signalImportError(const String& str) 
    { 
      // signal only the first one
      if (!error())
      {
        Log::error( str );
        setError( VLXSerializer::ImportError );
      }
    }

    void signalExportError(const String& str)
    { 
      // signal only the first one
      if (!error())
      {
        Log::error( str );
        setError( VLXSerializer::ExportError ); 
      }
    }

    std::string generateUID(const char* prefix)
    {
      std::stringstream strstr;
      strstr << "#" << prefix << "id" << getNewUID();
      return strstr.str();
    }

    Object* importVLX(const VLXStructure* st)
    {
      if (error())
        return NULL;

      Object* obj = getImportedStructure(st);
      if (obj)
        return obj;
      else
      {
        std::map< std::string, ref<VLXIO> >::iterator it = registry()->importRegistry().find(st->tag());
        if (it != registry()->importRegistry().end())
        {
          VLXIO* serializer = it->second.get_writable();
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

    VLXStructure* exportVLX(const Object* obj)
    {
      if (error())
        return NULL;

      VLXStructure* st = getExportedObject(obj);
      if (st)
        return st;
      else
      {
        std::map< const TypeInfo*, ref<VLXIO> >::iterator it = registry()->exportRegistry().find(obj->classType());
        if (it != registry()->exportRegistry().end())
        {
          VLXIO* serializer = it->second.get_writable();
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

    bool canExport(const Object* obj) const 
    { 
      if (!registry())
        return false;
      else
        return registry()->exportRegistry().find(obj->classType()) != registry()->exportRegistry().end(); 
    }

    bool canImport(const VLXStructure* st) const 
    { 
      if (!registry())
        return false;
      else
        return registry()->importRegistry().find(st->tag()) != registry()->importRegistry().end(); 
    }

    void registerImportedStructure(const VLXStructure* st, Object* obj) 
    {
      VL_CHECK( mImportedStructures.find(st) == mImportedStructures.end() )
      mImportedStructures[st] = obj;
    }

    void registerExportedObject(const Object* obj, VLXStructure* st)
    {
      VL_CHECK(mExportedObjects.find(obj) == mExportedObjects.end())
      mExportedObjects[obj] = st;
    }

    Object* getImportedStructure(const VLXStructure* st)
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

    VLXStructure* getExportedObject(const Object* obj)
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
    
    bool saveVLT(const String& path, const Object* obj, bool start_fresh=true);

    bool saveVLT(VirtualFile* file, const Object* obj, bool start_fresh=true);

    bool saveVLB(const String& path, const Object* obj, bool start_fresh=true);

    bool saveVLB(VirtualFile* file, const Object* obj, bool start_fresh=true);

    ref<Object> loadVLT(const String& path, bool start_fresh=true);

    ref<Object> loadVLT(VirtualFile* file, bool start_fresh=true);

    ref<Object> loadVLB(const String& path, bool start_fresh=true);

    ref<Object> loadVLB(VirtualFile* file, bool start_fresh=true);

    int getNewUID() { return ++mUIDCounter; }

    EError error() const { return mError; }
    void setError(EError err) { mError = err; }

    //! The VLXRegistry used by the serializer, by default set to vl::defVLXRegistry().
    VLXRegistry* registry() { return mRegistry.get(); }

    //! The VLXRegistry used by the serializer, by default set to vl::defVLXRegistry().
    const VLXRegistry* registry() const { return mRegistry.get(); }
    
    //! The VLXRegistry used by the serializer, by default set to vl::defVLXRegistry().
    void setRegistry(const VLXRegistry* registry) { mRegistry = registry; }

    void reset()
    {
      mError = NoError;
      mUIDCounter = 0;
      mImportedStructures.clear();
      mExportedObjects.clear();
    }

    //! The metadata to be imported or exported.
    std::map< std::string, VLXValue >& metadata() { return mMetadata; }

    //! The metadata to be imported or exported.
    const std::map< std::string, VLXValue >& metadata() const { return mMetadata; }

    //! Returns the value of the given metadata key or NULL if no such metadata was found.
    VLXValue* getMetadata(const char* key)
    {
      std::map< std::string, VLXValue >::iterator it = metadata().find(key);
      if (it == metadata().end())
        return NULL;
      else
        return &it->second;
    }

    //! Returns the value of the given metadata key or NULL if no such metadata was found.
    const VLXValue* getMetadata(const char* key) const
    {
      std::map< std::string, VLXValue >::const_iterator it = metadata().find(key);
      if (it == metadata().end())
        return NULL;
      else
        return &it->second;
    }

  private:
    EError mError;
    int mUIDCounter;
    std::map< ref<VLXStructure>, ref<Object> > mImportedStructures; // structure --> object
    std::map< ref<Object>, ref<VLXStructure> > mExportedObjects;    // object --> structure
    std::map< std::string, VLXValue > mMetadata; // metadata to import or to export
    ref<VLXRegistry> mRegistry;
  };
}

#endif
