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

#ifndef VLXRegistry_INCLUDE_ONCE
#define VLXRegistry_INCLUDE_ONCE

#include <vlX/link_config.hpp>
#include <vlX/ClassWrapper.hpp>
#include <string>
#include <map>

namespace vlX
{
  /** Registry of vl::ClassWrapper objects, used by vl::VLXSerializer, see also vl::defVLXRegistry(). */
  class Registry: public vl::Object
  {
    VL_INSTRUMENT_CLASS(vlX::Registry, vl::Object)

  public:
    void registerClassWrapper(const vl::TypeInfo& type, ClassWrapper* wrapper)
    {
      std::string tag = std::string("<") + type.name() + ">";
      mExportRegistry[type] = wrapper;
      mImportRegistry[tag]  = wrapper;
    }

    std::map< std::string, vl::ref<ClassWrapper> >& importRegistry() { return mImportRegistry; }
    std::map< vl::TypeInfo, vl::ref<ClassWrapper> >& exportRegistry() { return mExportRegistry; }

    const std::map< std::string, vl::ref<ClassWrapper> >& importRegistry() const { return mImportRegistry; }
    const std::map< vl::TypeInfo, vl::ref<ClassWrapper> >& exportRegistry() const { return mExportRegistry; }

  private:
    std::map< std::string, vl::ref<ClassWrapper> > mImportRegistry;     // <tag> --> ClassWrapper
    std::map< vl::TypeInfo, vl::ref<ClassWrapper> > mExportRegistry; // TypeInfo --> ClassWrapper
  };

  VLX_EXPORT extern Registry* defVLXRegistry();
  VLX_EXPORT extern void setDefVLXRegistry(Registry* reg);
}

#endif
