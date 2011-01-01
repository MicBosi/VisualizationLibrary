/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
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

#ifndef ResourceDatabase_INCLUDE_ONCE
#define ResourceDatabase_INCLUDE_ONCE

#include <vlCore/Object.hpp>
#include <vlCore/String.hpp>
#include <vector>
#include <algorithm>

namespace vl
{
  class VirtualFile;

  /**
   * The ResourceDatabase class contains and manipulates a set of resources. It works with 
   * any kind of resources derived from vl::Object, even user-customized ones.
  */
  class ResourceDatabase: public Object
  {
  public:
    virtual const char* className() { return "ResourceDatabase"; }

    const std::vector< ref<Object> >& resources() const { return mResources; }
    std::vector< ref<Object> >& resources() { return mResources; }

    template<class T>
    unsigned count() const
    {
      unsigned count=0;
      for(unsigned i=0; i<mResources.size(); ++i)
      {
        ref<T> r = dynamic_cast<T*>(mResources[i].get());
        if (r)
          ++count;
      }
      return count;
    }

    template<class T>
    T* get(int j) const
    {
      int count=0;
      for(unsigned i=0; i<mResources.size(); ++i)
      {
        ref<T> r = dynamic_cast<T*>(mResources[i].get());
        if (r)
        {
          if (count == j)
            return r.get();
          else
            ++count;
        }
      }
      return NULL;
    }

    template<class T>
    T* next(int& cur_pos) const
    {
      for(unsigned i=cur_pos; i<mResources.size(); ++i)
      {
        ref<T> r = dynamic_cast<T*>(mResources[i].get());
        if (r)
        {
          cur_pos = i+1;
          return r.get();
        }
      }
      return NULL;
    }

    template<class T>
    void extract( std::vector< ref<T> >& resources, bool clear_vector=true )
    {
      if (clear_vector)
        resources.clear();

      for( unsigned i=mResources.size(); i--; )
      {
        ref<T> r = dynamic_cast<T*>(mResources[i].get());
        if (r)
        {
          resources.push_back(r);
          mResources.erase(mResources.begin()+i);
        }
      }

      std::reverse(resources.begin(), resources.end());
    }

    template<class T>
    void get( std::vector< ref<T> >& resources, bool clear_vector=true )
    {
      if (clear_vector)
        resources.clear();

      for( unsigned i=0; i<mResources.size(); ++i )
      {
        ref<T> r = dynamic_cast<T*>(mResources[i].get());
        if (r)
          resources.push_back(r);
      }
    }

  protected:
    std::vector< ref<Object> > mResources;
  };
//-----------------------------------------------------------------------------
  //! Loads the specified resource. Equivalent to VisualizationLibrary::loadWriterManager()->loadResource(const String& path, bool quick)
  ref<ResourceDatabase> loadResource(const String& path, bool quick=true);
  //! Loads the specified resource. Equivalent to VisualizationLibrary::loadWriterManager()->loadResource(VirtualFile* file)
  ref<ResourceDatabase> loadResource(VirtualFile* file, bool quick=true);
  //! Returns true if there is a ResourceLoadWriter registered to load the specified path or extension
  bool canLoad(const String& path);
  //! Returns true if there is a ResourceLoadWriter registered to load the specified file
  bool canLoad(VirtualFile* file);
  //! Returns true if there is a ResourceLoadWriter registered to write the specified path or extension
  bool canWrite(const String& path);
  //! Returns true if there is a ResourceLoadWriter registered to write the specified file
  bool canWrite(VirtualFile* file);
//-----------------------------------------------------------------------------
}

#endif
