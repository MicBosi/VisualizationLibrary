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

#ifndef VLXSerializer_INCLUDE_ONCE
#define VLXSerializer_INCLUDE_ONCE

#include <vlX/Registry.hpp>
#include <vlX/Value.hpp>
#include <vlCore/String.hpp>
#include <string>
#include <map>

namespace vlX
{
  class VirtualFile;
  /** Translates an arbitrary set of vl::Object (and subclasses) into VLB and VLT format. */
  class VLX_EXPORT VLXSerializer: public vl::Object
  {
    VL_INSTRUMENT_CLASS(vlX::Serializer, vl::Object)

  public:
    typedef enum { NoError, ImportError, ExportError, ReadError, WriteError } EError;

  public:
    VLXSerializer(): mError(NoError), mIDCounter(0)
    {
      setRegistry( defVLXRegistry() );
    }

    const char* errorString() const;

    bool saveVLT(const vl::String& path, const vl::Object* obj, bool start_fresh=true);

    bool saveVLT(vl::VirtualFile* file, const vl::Object* obj, bool start_fresh=true);

    bool saveVLB(const vl::String& path, const vl::Object* obj, bool start_fresh=true);

    bool saveVLB(vl::VirtualFile* file, const vl::Object* obj, bool start_fresh=true);

    vl::ref<vl::Object> loadVLT(const vl::String& path, bool start_fresh=true);

    vl::ref<vl::Object> loadVLT(vl::VirtualFile* file, bool start_fresh=true);

    vl::ref<vl::Object> loadVLB(const vl::String& path, bool start_fresh=true);

    vl::ref<vl::Object> loadVLB(vl::VirtualFile* file, bool start_fresh=true);

    vl::Object* importVLX(const VLXStructure* st);

    VLXStructure* exportVLX(const vl::Object* obj);

    bool canExport(const vl::Object* obj) const;

    bool canImport(const VLXStructure* st) const;

    void registerImportedStructure(const VLXStructure* st, Object* obj);

    void registerExportedObject(const vl::Object* obj, VLXStructure* st);

    vl::Object* getImportedStructure(const VLXStructure* st);

    VLXStructure* getExportedObject(const vl::Object* obj);

    //! The Registry used by the serializer, by default set to vl::defVLXRegistry().
    Registry* registry() { return mRegistry.get(); }

    //! The Registry used by the serializer, by default set to vl::defVLXRegistry().
    const Registry* registry() const { return mRegistry.get(); }

    //! The Registry used by the serializer, by default set to vl::defVLXRegistry().
    void setRegistry(const Registry* registry) { mRegistry = registry; }

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

    std::string generateID(const char* prefix);

    //! Sets a serialization error.
    void setError(EError err) { mError = err; }

    //! The last signaled error
    EError error() const { return mError; }

    void signalImportError(const vl::String& str);

    void signalExportError(const vl::String& str);

    //! The URL of the document used to resolve document-relative file paths
    void setDocumentURL(const vl::String& location) { mDocumentURL = location; }

    //! The URL of the document used to resolve document-relative file paths
    const vl::String& documentURL() const { return mDocumentURL; }

    //! If the given path starts with "this:" then the "this:" prefix is replaced with the documentURL(), otherwise the path is left unchanged.
    void resolvePath(std::string& path);

    //! Sets a serialization directive that can be used by ClassWrapper objects to program the serialization process.
    //! Directives are essentially a way to pass options to ClassWrapper objects, which can read them from the VLXSerializer they are using.
    void setDirective(const char* directive, const char* value) { mDirectives[directive] = value; }

    //! Removes a serialization directive.
    void eraseDirective(const char* directive) { mDirectives.erase(directive); }

    //! Returns the value of a serialization directive.
    const std::string& directive(const char* directive) const
    {
      static const std::string no_directive = "NO_SUCH_DIRECTIVE";
      std::map<std::string, std::string>::const_iterator it = mDirectives.find(directive);
      if (it != mDirectives.end())
        return it->second;
      else
        return no_directive;
    }

    //! Returns true if the given directive has been set.
    bool hasDirective(const char* directive) { return mDirectives.find(directive) != mDirectives.end(); }

    //! Erases all previously set directives
    void eraseAllDirectives() { mDirectives.clear(); }

  private:
    vl::String mDocumentURL;
    std::map<std::string, std::string> mDirectives;
    EError mError;
    int mIDCounter;
    std::map< vl::ref<VLXStructure>, vl::ref<vl::Object> > mImportedStructures; // structure --> object
    std::map< vl::ref<vl::Object>, vl::ref<VLXStructure> > mExportedObjects;    // object --> structure
    std::map< std::string, VLXValue > mMetadata; // metadata to import or to export
    vl::ref<Registry> mRegistry;
  };
}

#endif
