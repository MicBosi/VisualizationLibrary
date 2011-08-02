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

#if !defined(LoadCOLLADA_INCLUDE_ONCE)
#define LoadCOLLADA_INCLUDE_ONCE

#include <vlGraphics/link_config.hpp>
#include <vlCore/VirtualFile.hpp>
#include <vlCore/ResourceLoadWriter.hpp>
#include <vlCore/ResourceDatabase.hpp>

namespace vl
{
//---------------------------------------------------------------------------
// LoadWriterCOLLADA
//---------------------------------------------------------------------------
  /**
   * The LoadWriterCOLLADA class is a ResourceLoadWriter capable of reading COLLADA files.
   */
  class LoadWriterCOLLADA: public ResourceLoadWriter
  {
    VL_INSTRUMENT_CLASS(vl::LoadWriterCOLLADA, ResourceLoadWriter)

  public:
    class LoadOptions: public Object
    {
    public:
      enum TransparencyOption 
      { 
        TransparencyKeep,   //!<< Keep the <transparency> value as it is.
        TransparencyInvert, //!<< Transparency becomes 1.0 - <transparency>.
        TransparencyAuto    //!<< Transparency is inverted if <authoring_tool> contains the string "Google" or reports ColladaMax or ColladaMaya version less than 3.03.
      };

    public:
      LoadOptions()
      {
        mInvertTransparency = TransparencyAuto;
        mUseAlwaysMipmapping = true;
        mFlattenTransformHierarchy = true;
        mComputeMissingNormals = true;
        mFixBadNormals = true;
        mExtractSkins = true;
      }

      //! If true then the <node>'s transform hierachy is flattened and baked inside the Actor::transform(), otherwise the full transform tree is exported in the resource database.
      void setFlattenTransformHierarchy(bool flatten) { mFlattenTransformHierarchy = flatten; }

      //! If true then the <node>'s transform hierachy is flattened and baked inside the Actor::transform(), otherwise the full transform tree is exported in the resource database.
      bool flattenTransformHierarchy() const { return mFlattenTransformHierarchy; }

      //! If true then TPF_LINEAR_MIPMAP_NEAREST filtering is used when a non-mipmapped filter is specified
      void setUseAlwaysMipmapping(bool use) { mUseAlwaysMipmapping = use; }

      //! If true then TPF_LINEAR_MIPMAP_NEAREST filtering is used when a non-mipmapped filter is specified
      bool useAlwaysMipmapping() const { return mUseAlwaysMipmapping; }

      //! Invert the value of the <transparency> tag
      void setInvertTransparency(TransparencyOption invert) { mInvertTransparency = invert; }

      //! Invert the value of the <transparency> tag
      TransparencyOption invertTransparency() const { return mInvertTransparency; }

      //! Compute normals for those objects that don't have
      void setComputeMissingNormals(bool compute) { mComputeMissingNormals = compute; }

      //! Compute normals for those objects that don't have
      bool computeMissingNormals() const { return mComputeMissingNormals; }

      //! Fix normals that are flipped compared to the polygon winding order
      void setFixBadNormals(bool fix) { mFixBadNormals = fix; }

      //! Fix normals that are flipped compared to the polygon winding order
      bool fixBadNormals() const { return mFixBadNormals; }

      //! If set to true the skinned geometries will be also exported.
      void setExtractSkins(bool extract) { mExtractSkins = extract; }

      //! If set to true the skinned geometries will be also exported.
      bool extractSkins() const { return mExtractSkins; }

    protected:
      TransparencyOption mInvertTransparency;
      bool mFlattenTransformHierarchy;
      bool mUseAlwaysMipmapping;
      bool mComputeMissingNormals;
      bool mFixBadNormals;
      bool mExtractSkins;
    };

  public:
    static VLGRAPHICS_EXPORT ref<ResourceDatabase> load(const String& path, const LoadOptions* options);

    static VLGRAPHICS_EXPORT ref<ResourceDatabase> load(VirtualFile* file, const LoadOptions* options);

    LoadWriterCOLLADA(): ResourceLoadWriter("|dae|", "|dae|") 
    { 
      mLoadOptions = new LoadOptions;
    }

    ref<ResourceDatabase> loadResource(const String& path) const 
    {
      return load(path, loadOptions());
    }

    ref<ResourceDatabase> loadResource(VirtualFile* file) const
    {
      return load(file, loadOptions());
    }

    //! Not supported yet.
    bool writeResource(const String& /*path*/, ResourceDatabase* /*resource*/) const
    {
      return false;
    }

    //! Not supported yet.
    bool writeResource(VirtualFile* /*file*/, ResourceDatabase* /*resource*/) const
    {
      return false;
    }

    // --- options ---

    const LoadOptions* loadOptions() const { return mLoadOptions.get(); }

    LoadOptions* loadOptions() { return mLoadOptions.get(); }

  protected:
    ref<LoadOptions> mLoadOptions;
  };
//---------------------------------------------------------------------------
}

#endif
