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
#include <vlCore/String.hpp>
#include <string>
#include <map>

namespace vl
{
  class VirtualFile;
  /** Translates an arbitrary set of vl::Object (and subclasses) into VLB and VLT format. */
  class VLXSerializer: public Object
  {
    VL_INSTRUMENT_CLASS(vl::VLXSerializer, Object)

  public:
    typedef enum { NoError, ImportError, ExportError, ReadError, WriteError } EError;

  public:
    VLXSerializer(): mError(NoError), mIDCounter(0)
    {
      setRegistry( defVLXRegistry() );
    }

    VLCORE_EXPORT bool saveVLT(const String& path, const Object* obj, bool start_fresh=true);

    VLCORE_EXPORT bool saveVLT(VirtualFile* file, const Object* obj, bool start_fresh=true);

    VLCORE_EXPORT bool saveVLB(const String& path, const Object* obj, bool start_fresh=true);

    VLCORE_EXPORT bool saveVLB(VirtualFile* file, const Object* obj, bool start_fresh=true);

    VLCORE_EXPORT ref<Object> loadVLT(const String& path, bool start_fresh=true);

    VLCORE_EXPORT ref<Object> loadVLT(VirtualFile* file, bool start_fresh=true);

    VLCORE_EXPORT ref<Object> loadVLB(const String& path, bool start_fresh=true);

    VLCORE_EXPORT ref<Object> loadVLB(VirtualFile* file, bool start_fresh=true);

    VLCORE_EXPORT Object* importVLX(const VLXStructure* st);

    VLCORE_EXPORT VLXStructure* exportVLX(const Object* obj);

    VLCORE_EXPORT bool canExport(const Object* obj) const;

    VLCORE_EXPORT bool canImport(const VLXStructure* st) const;

    VLCORE_EXPORT void registerImportedStructure(const VLXStructure* st, Object* obj);

    VLCORE_EXPORT void registerExportedObject(const Object* obj, VLXStructure* st);

    VLCORE_EXPORT Object* getImportedStructure(const VLXStructure* st);

    VLCORE_EXPORT VLXStructure* getExportedObject(const Object* obj);
    
    //! The VLXRegistry used by the serializer, by default set to vl::defVLXRegistry().
    VLXRegistry* registry() { return mRegistry.get(); }

    //! The VLXRegistry used by the serializer, by default set to vl::defVLXRegistry().
    const VLXRegistry* registry() const { return mRegistry.get(); }
    
    //! The VLXRegistry used by the serializer, by default set to vl::defVLXRegistry().
    void setRegistry(const VLXRegistry* registry) { mRegistry = registry; }

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

    void reset()
    {
      mError = NoError;
      mIDCounter = 0;
      mImportedStructures.clear();
      mExportedObjects.clear();
    }

    VLCORE_EXPORT std::string generateID(const char* prefix);

    //! Sets a serialization error.
    void setError(EError err) { mError = err; }

    //! The last signaled error
    EError error() const { return mError; }

    VLCORE_EXPORT void signalImportError(const String& str);

    VLCORE_EXPORT void signalExportError(const String& str);

  private:
    EError mError;
    int mIDCounter;
    std::map< ref<VLXStructure>, ref<Object> > mImportedStructures; // structure --> object
    std::map< ref<Object>, ref<VLXStructure> > mExportedObjects;    // object --> structure
    std::map< std::string, VLXValue > mMetadata; // metadata to import or to export
    ref<VLXRegistry> mRegistry;
  };
}

#endif
