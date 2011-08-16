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

#ifndef LoadWriterSRF_INCLUDE_ONCE
#define LoadWriterSRF_INCLUDE_ONCE

#include <vlCore/SRF_Tools.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlGraphics/Shader.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/ClipPlane.hpp>
#include <vlGraphics/Camera.hpp>
#include <vlGraphics/DrawElements.hpp>
#include <vlGraphics/MultiDrawElements.hpp>
#include <vlGraphics/DrawArrays.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/DiskFile.hpp>

#define VL_SRF_CHECK_ERROR(Condition) \
  if (!(Condition))                   \
  {                                   \
  Log::error( Say("Line %n : condition failed : %s\n") << value.lineNumber() << #Condition ); \
    return NULL;                      \
  }

namespace vl
{
  static const int VL_SERIALIZER_VERSION = 100;

  class SRFSerializer
  {
  public:
    typedef enum
    {
      NoError,
      ExportError,
      ImportError
    } EError;

    SRFSerializer()
    {
      mError = NoError;
      mUIDCounter = 0;
    }

    EError error() const { return mError; }

    void resetError() { mError = NoError; }

    //-----------------------------------------------------------------------------
    // EXPORT
    //-----------------------------------------------------------------------------

    void registerExportedStructure(const Object* vl_obj, const SRF_Structure* srf_obj)
    {
      VL_CHECK( mVL_To_SRF.find(vl_obj) == mVL_To_SRF.end() );
      mVL_To_SRF[vl_obj] = srf_obj;
    }

    SRF_Structure* vlToSRF(const Object* srf_obj)
    {
      std::map< ref<Object>, ref<SRF_Structure> >::iterator it = mVL_To_SRF.find(srf_obj);
      if (it != mVL_To_SRF.end())
        return it->second.get();
      else
        return NULL;
    }

    const std::map< ref<Object>, ref<SRF_Structure> >& vlToSRF() const { return mVL_To_SRF; }

    //-----------------------------------------------------------------------------

    std::string generateUID(const char* prefix)
    {
      return String::printf("#%sid%d", prefix, ++mUIDCounter).toStdString();
    }

    SRF_Value export_ResourceDatabase(const ResourceDatabase* res_db)
    {
      SRF_Value value;
      if (vlToSRF(res_db))
      {
        value.setUID( vlToSRF(res_db)->uid().c_str() );
        return value;
      }

      value.setStructure( new SRF_Structure("<ResourceDatabase>", generateUID("resourcedb_")) );
      registerExportedStructure(res_db, value.getStructure());

      value.getStructure()->value().push_back( SRF_Structure::Value("SerializerVersion", (long long)VL_SERIALIZER_VERSION) );

      SRF_Structure::Value srf_resource_list("Resources", new SRF_List);
      value.getStructure()->value().push_back( srf_resource_list );

      for(size_t i=0; i<res_db->resources().size(); ++i)
      {
        SRF_Value resource;

        if(res_db->resources()[i]->isOfType(Geometry::Type()))
          resource = export_Geometry(res_db->resources()[i]->as<Geometry>());
        else
        if(res_db->resources()[i]->isOfType(Effect::Type()))
          resource = export_Effect(res_db->resources()[i]->as<Effect>());
        else
        if(res_db->resources()[i]->isOfType(Shader::Type()))
          resource = export_Shader(res_db->resources()[i]->as<Shader>());
        else
        if(res_db->resources()[i]->isOfType(Transform::Type()))
          resource = export_Transform(res_db->resources()[i]->as<Transform>());
        else
        if(res_db->resources()[i]->isOfType(Actor::Type()))
          resource = export_Actor(res_db->resources()[i]->as<Actor>());
        else
        if(res_db->resources()[i]->isOfType(Camera::Type()))
          resource = export_Camera(res_db->resources()[i]->as<Camera>());

        srf_resource_list.value().getList()->value().push_back( resource );
      }

      return value;
    }

    void export_Renderable(const Renderable* ren, SRF_Structure* renderable_subclass)
    {
      renderable_subclass->value().push_back( SRF_Structure::Value("VBOEnabled", ren->isVBOEnabled()) );
      renderable_subclass->value().push_back( SRF_Structure::Value("DisplayListEnabled", ren->isDisplayListEnabled()) );
      renderable_subclass->value().push_back( SRF_Structure::Value("AABB", export_AABB(ren->boundingBox())) );
      renderable_subclass->value().push_back( SRF_Structure::Value("Sphere", export_Sphere(ren->boundingSphere())) );
    }

    SRF_Value export_AABB(const AABB& aabb)
    {
      SRF_Value value ( new SRF_Structure("<AABB>", generateUID("aabb_")) );
      value.getStructure()->value().push_back( SRF_Structure::Value("MinCorner", aabb.minCorner()) );
      value.getStructure()->value().push_back( SRF_Structure::Value("MaxCorner", aabb.maxCorner()) );
      return value;
    }

    SRF_Value export_Sphere(const Sphere& sphere)
    {
      SRF_Value value ( new SRF_Structure("<Sphere>", generateUID("sphere_")) );
      value.getStructure()->value().push_back( SRF_Structure::Value("Center", sphere.center()) );
      value.getStructure()->value().push_back( SRF_Structure::Value("Radius", sphere.radius()) );
      return value;
    }

    SRF_Value export_Geometry(const Geometry* geom)
    {
      SRF_Value value;
      if (vlToSRF(geom))
      {
        value.setUID( vlToSRF(geom)->uid().c_str() );
        return value;
      }

      value.setStructure( new SRF_Structure( "<Geometry>", generateUID("geometry_") ) );
      registerExportedStructure(geom, value.getStructure());
      std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

      export_Renderable( geom, value.getStructure() );

      if (geom->vertexArray()) 
        values.push_back( SRF_Structure::Value("VertexArray", export_Array(geom->vertexArray())) );
    
      if (geom->normalArray()) 
        values.push_back( SRF_Structure::Value("NormalArray", export_Array(geom->normalArray())) );
    
      if (geom->colorArray()) 
        values.push_back( SRF_Structure::Value("ColorArray", export_Array(geom->colorArray())) );
    
      if (geom->secondaryColorArray()) 
        values.push_back( SRF_Structure::Value("SecondaryColorArray", export_Array(geom->secondaryColorArray())) );
    
      if (geom->fogCoordArray()) 
        values.push_back( SRF_Structure::Value("FogCoordArray", export_Array(geom->fogCoordArray())) );

      for( int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      {
        if (geom->texCoordArray(i)) 
        {
          std::string TexCoordArray = String::printf("TexCoordArray%d", i).toStdString();
          values.push_back( SRF_Structure::Value(TexCoordArray.c_str(), export_Array(geom->texCoordArray(i))) ); 
        }
      }
    
      for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      {
        if (geom->vertexAttribArray(i))
        {
          std::string VertexAttribArray = String::printf("VertexAttribArray%d", i).toStdString();
          values.push_back( SRF_Structure::Value(VertexAttribArray.c_str(), export_VertexAttribInfo(geom->vertexAttribArray(i))) );
        }
      }

      for(int i=0; i<geom->drawCalls()->size(); ++i)
        values.push_back( SRF_Structure::Value("DrawCall", export_DrawCall(geom->drawCalls()->at(i))) );

      return value;
    }

    SRF_Value export_Effect(const Effect* fx)
    {
      SRF_Value value;
      if (vlToSRF(fx))
      {
        value.setUID( vlToSRF(fx)->uid().c_str() );
        return value;
      }

      // mic fixme: this is just a stub, complete the implementation
      value.setStructure( new SRF_Structure("<Effect>", generateUID("effect_")) );
      registerExportedStructure(fx, value.getStructure());

      return value;
    }

    SRF_Value export_Shader(const Shader* sh)
    {
      SRF_Value value;
      if (vlToSRF(sh))
      {
        value.setUID( vlToSRF(sh)->uid().c_str() );
        return value;
      }

      // mic fixme: this is just a stub, complete the implementation
      value.setStructure( new SRF_Structure("<Shader>", generateUID("shader_")) );
      registerExportedStructure(sh, value.getStructure());

      return value;
    }

    SRF_Value export_Transform(const Transform* tr)
    {
      SRF_Value value;
      if (vlToSRF(tr))
      {
        value.setUID( vlToSRF(tr)->uid().c_str() );
        return value;
      }

      // mic fixme: this is just a stub, complete the implementation
      value.setStructure( new SRF_Structure("<Transform>", generateUID("transform_")) );
      registerExportedStructure(tr, value.getStructure());

      return value;
    }

    SRF_Value export_Actor(const Actor* act)
    {
      SRF_Value value;
      if (vlToSRF(act))
      {
        value.setUID( vlToSRF(act)->uid().c_str() );
        return value;
      }

      // mic fixme: this is just a stub, complete the implementation
      value.setStructure( new SRF_Structure("<Actor>", generateUID("actor_")) );
      registerExportedStructure(act, value.getStructure());

      return value;
    }

    SRF_Value export_Camera(const Camera* cam)
    {
      SRF_Value value;
      if (vlToSRF(cam))
      {
        value.setUID( vlToSRF(cam)->uid().c_str() );
        return value;
      }

      // mic fixme: this is just a stub, complete the implementation
      value.setStructure( new SRF_Structure("<Camera>", generateUID("camera_")) );
      registerExportedStructure(cam, value.getStructure());

      return value;
    }

    SRF_Value export_VertexAttribInfo(const VertexAttribInfo* info)
    {
      SRF_Value value;
      if (vlToSRF(info))
      {
        value.setUID( vlToSRF(info)->uid().c_str() );
        return value;
      }

      SRF_Structure* srf_obj = value.setStructure( new SRF_Structure("<VertexAttribInfo>", generateUID("vertexattribinfo_")) );
      registerExportedStructure(info, srf_obj);
      std::vector<SRF_Structure::Value>& values = srf_obj->value();

      values.push_back( SRF_Structure::Value("Data", export_Array(info->data())) );
      values.push_back( SRF_Structure::Value("Normalize", info->normalize()) );
      std::string interpretation;
      switch(info->interpretation())
      {
      case VAI_NORMAL:  interpretation = "VAI_NORMAL";  break;
      case VAI_INTEGER: interpretation = "VAI_INTEGER"; break;
      case VAI_DOUBLE:  interpretation = "VAI_DOUBLE";  break;
      }
      values.push_back( SRF_Structure::Value("Interpretation", SRF_Value(interpretation.c_str(), SRF_Value::Identifier)) );

      return value;
    }

    template<typename T_Array, typename T_SRF_Array>
    SRF_Value export_ArrayT(const ArrayAbstract* arr_abstract, const char* name)
    {
      const T_Array* arr = arr_abstract->as<T_Array>();
      SRF_Value value( new SRF_Structure(name, generateUID("array_")) );
      ref<T_SRF_Array> srf_array = new T_SRF_Array;
      if (arr->size())
      {
        srf_array->value().resize( arr->size() * arr->glSize() );
        typename T_SRF_Array::scalar_type* dst = &srf_array->value()[0];
        const typename T_Array::scalar_type* src = (const T_Array::scalar_type*)arr->begin();
        const typename T_Array::scalar_type* end = (const T_Array::scalar_type*)arr->end();
        for(; src<end; ++src, ++dst)
          *dst = (T_SRF_Array::scalar_type)*src;
      }
      value.getStructure()->value().push_back( SRF_Structure::Value("Value", srf_array.get() ) );
      return value;
    }

    SRF_Value export_Array(const ArrayAbstract* arr_abstract)
    {
      SRF_Value value;
      if (vlToSRF(arr_abstract))
      {
        value.setUID( vlToSRF(arr_abstract)->uid().c_str() );
        return value;
      }

      if(arr_abstract->classType() == ArrayUInt1::Type())
        value = export_ArrayT<ArrayUInt1, SRF_ArrayInt32>(arr_abstract, "<ArrayUInt1>");
      else
      if(arr_abstract->classType() == ArrayUInt2::Type())
        value = export_ArrayT<ArrayUInt2, SRF_ArrayInt32>(arr_abstract, "<ArrayUInt2>");
      else
      if(arr_abstract->classType() == ArrayUInt3::Type())
        value = export_ArrayT<ArrayUInt3, SRF_ArrayInt32>(arr_abstract, "<ArrayUInt3>");
      else
      if(arr_abstract->classType() == ArrayUInt4::Type())
        value = export_ArrayT<ArrayUInt4, SRF_ArrayInt32>(arr_abstract, "<ArrayUInt4>");
      else

      if(arr_abstract->classType() == ArrayInt1::Type())
        value = export_ArrayT<ArrayInt1, SRF_ArrayInt32>(arr_abstract, "<ArrayInt1>");
      else
      if(arr_abstract->classType() == ArrayInt2::Type())
        value = export_ArrayT<ArrayInt2, SRF_ArrayInt32>(arr_abstract, "<ArrayInt2>");
      else
      if(arr_abstract->classType() == ArrayInt3::Type())
        value = export_ArrayT<ArrayInt3, SRF_ArrayInt32>(arr_abstract, "<ArrayInt3>");
      else
      if(arr_abstract->classType() == ArrayInt4::Type())
        value = export_ArrayT<ArrayInt4, SRF_ArrayInt32>(arr_abstract, "<ArrayInt4>");
      else

      if(arr_abstract->classType() == ArrayUShort1::Type())
        value = export_ArrayT<ArrayUShort1, SRF_ArrayInt32>(arr_abstract, "<ArrayUShort1>");
      else
      if(arr_abstract->classType() == ArrayUShort2::Type())
        value = export_ArrayT<ArrayUShort2, SRF_ArrayInt32>(arr_abstract, "<ArrayUShort2>");
      else
      if(arr_abstract->classType() == ArrayUShort3::Type())
        value = export_ArrayT<ArrayUShort3, SRF_ArrayInt32>(arr_abstract, "<ArrayUShort3>");
      else
      if(arr_abstract->classType() == ArrayUShort4::Type())
        value = export_ArrayT<ArrayUShort4, SRF_ArrayInt32>(arr_abstract, "<ArrayUShort4>");
      else

      if(arr_abstract->classType() == ArrayUShort1::Type())
        value = export_ArrayT<ArrayUShort1, SRF_ArrayInt32>(arr_abstract, "<ArrayUShort1>");
      else
      if(arr_abstract->classType() == ArrayUShort2::Type())
        value = export_ArrayT<ArrayUShort2, SRF_ArrayInt32>(arr_abstract, "<ArrayUShort2>");
      else
      if(arr_abstract->classType() == ArrayUShort3::Type())
        value = export_ArrayT<ArrayUShort3, SRF_ArrayInt32>(arr_abstract, "<ArrayUShort3>");
      else
      if(arr_abstract->classType() == ArrayUShort4::Type())
        value = export_ArrayT<ArrayUShort4, SRF_ArrayInt32>(arr_abstract, "<ArrayUShort4>");
      else

      if(arr_abstract->classType() == ArrayShort1::Type())
        value = export_ArrayT<ArrayShort1, SRF_ArrayInt32>(arr_abstract, "<ArrayShort1>");
      else
      if(arr_abstract->classType() == ArrayShort2::Type())
        value = export_ArrayT<ArrayShort2, SRF_ArrayInt32>(arr_abstract, "<ArrayShort2>");
      else
      if(arr_abstract->classType() == ArrayShort3::Type())
        value = export_ArrayT<ArrayShort3, SRF_ArrayInt32>(arr_abstract, "<ArrayShort3>");
      else
      if(arr_abstract->classType() == ArrayShort4::Type())
        value = export_ArrayT<ArrayShort4, SRF_ArrayInt32>(arr_abstract, "<ArrayShort4>");
      else

      if(arr_abstract->classType() == ArrayUByte1::Type())
        value = export_ArrayT<ArrayUByte1, SRF_ArrayInt32>(arr_abstract, "<ArrayUByte1>");
      else
      if(arr_abstract->classType() == ArrayUByte2::Type())
        value = export_ArrayT<ArrayUByte2, SRF_ArrayInt32>(arr_abstract, "<ArrayUByte2>");
      else
      if(arr_abstract->classType() == ArrayUByte3::Type())
        value = export_ArrayT<ArrayUByte3, SRF_ArrayInt32>(arr_abstract, "<ArrayUByte3>");
      else
      if(arr_abstract->classType() == ArrayUByte4::Type())
        value = export_ArrayT<ArrayUByte4, SRF_ArrayInt32>(arr_abstract, "<ArrayUByte4>");
      else

      if(arr_abstract->classType() == ArrayByte1::Type())
        value = export_ArrayT<ArrayByte1, SRF_ArrayInt32>(arr_abstract, "<ArrayByte1>");
      else
      if(arr_abstract->classType() == ArrayByte2::Type())
        value = export_ArrayT<ArrayByte2, SRF_ArrayInt32>(arr_abstract, "<ArrayByte2>");
      else
      if(arr_abstract->classType() == ArrayByte3::Type())
        value = export_ArrayT<ArrayByte3, SRF_ArrayInt32>(arr_abstract, "<ArrayByte3>");
      else
      if(arr_abstract->classType() == ArrayByte4::Type())
        value = export_ArrayT<ArrayByte4, SRF_ArrayInt32>(arr_abstract, "<ArrayByte4>");
      else

      if(arr_abstract->classType() == ArrayFloat1::Type())
        value = export_ArrayT<ArrayFloat1, SRF_ArrayFloat>(arr_abstract, "<ArrayFloat1>");
      else
      if(arr_abstract->classType() == ArrayFloat2::Type())
        value = export_ArrayT<ArrayFloat2, SRF_ArrayFloat>(arr_abstract, "<ArrayFloat2>");
      else
      if(arr_abstract->classType() == ArrayFloat3::Type())
        value = export_ArrayT<ArrayFloat3, SRF_ArrayFloat>(arr_abstract, "<ArrayFloat3>");
      else
      if(arr_abstract->classType() == ArrayFloat4::Type())
        value = export_ArrayT<ArrayFloat4, SRF_ArrayFloat>(arr_abstract, "<ArrayFloat4>");
      else

      if(arr_abstract->classType() == ArrayDouble1::Type())
        value = export_ArrayT<ArrayDouble1, SRF_ArrayDouble>(arr_abstract, "<ArrayDouble1>");
      else
      if(arr_abstract->classType() == ArrayDouble2::Type())
        value = export_ArrayT<ArrayDouble2, SRF_ArrayDouble>(arr_abstract, "<ArrayDouble2>");
      else
      if(arr_abstract->classType() == ArrayDouble3::Type())
        value = export_ArrayT<ArrayDouble3, SRF_ArrayDouble>(arr_abstract, "<ArrayDouble3>");
      else
      if(arr_abstract->classType() == ArrayDouble4::Type())
        value = export_ArrayT<ArrayDouble4, SRF_ArrayDouble>(arr_abstract, "<ArrayDouble4>");
      else
      {
        Log::error("Array type not supported for export.\n");
        VL_TRAP();
      }

      registerExportedStructure(arr_abstract, value.getStructure());
      return value;
    }

    void export_DrawCallBase(const DrawCall* dcall, SRF_Structure* srf_obj)
    {
      std::string primitive_type = "PRIMITIVE_TYPE_ERROR";
      switch(dcall->primitiveType())
      {
        case PT_POINTS:                   primitive_type =  "PT_POINTS"; break;
        case PT_LINES:                    primitive_type =  "PT_LINES"; break;
        case PT_LINE_LOOP:                primitive_type =  "PT_LINE_LOOP"; break;
        case PT_LINE_STRIP:               primitive_type =  "PT_LINE_STRIP"; break;
        case PT_TRIANGLES:                primitive_type =  "PT_TRIANGLES"; break;
        case PT_TRIANGLE_STRIP:           primitive_type =  "PT_TRIANGLE_STRIP"; break;
        case PT_TRIANGLE_FAN:             primitive_type =  "PT_TRIANGLE_FAN"; break;
        case PT_QUADS:                    primitive_type =  "PT_QUADS"; break;
        case PT_QUAD_STRIP:               primitive_type =  "PT_QUAD_STRIP"; break;
        case PT_POLYGON:                  primitive_type =  "PT_POLYGON"; break;
        case PT_LINES_ADJACENCY:          primitive_type =  "PT_LINES_ADJACENCY"; break;
        case PT_LINE_STRIP_ADJACENCY:     primitive_type =  "PT_LINE_STRIP_ADJACENCY"; break;
        case PT_TRIANGLES_ADJACENCY:      primitive_type =  "PT_TRIANGLES_ADJACENCY"; break;
        case PT_TRIANGLE_STRIP_ADJACENCY: primitive_type =  "PT_TRIANGLE_STRIP_ADJACENCY"; break;
        case PT_PATCHES:                  primitive_type =  "PT_PATCHES"; break;
        case PT_UNKNOWN:                  primitive_type =  "PT_UNKNOWN"; break;
      }

      srf_obj->value().push_back( SRF_Structure::Value("PrimitiveType", SRF_Value(primitive_type.c_str(), SRF_Value::Identifier) ) );
      srf_obj->value().push_back( SRF_Structure::Value("Enabled", dcall->isEnabled()) );
      if (dcall->patchParameter())
        srf_obj->value().push_back( SRF_Structure::Value("PatchParameter", export_PatchParameter(dcall->patchParameter())) );
    }

    SRF_Value export_PatchParameter(const PatchParameter* pp)
    {
      SRF_Value value;
      if (vlToSRF(pp))
      {
        value.setUID( vlToSRF(pp)->uid().c_str() );
        return value;
      }

      SRF_Structure* srf_obj = value.setStructure( new SRF_Structure("<PatchParameter>", generateUID("patchparam_")) );
      registerExportedStructure(pp, srf_obj);
      srf_obj->value().push_back( SRF_Structure::Value("PatchVertices", (long long)pp->patchVertices()) );
      srf_obj->value().push_back( SRF_Structure::Value("PatchDefaultOuterLevel", pp->patchDefaultOuterLevel()) );
      srf_obj->value().push_back( SRF_Structure::Value("PatchDefaultInnerLevel", pp->patchDefaultInnerLevel()) );

      return value;
    }

    SRF_Value export_DrawCall(const DrawCall* dcall)
    {
      SRF_Value value;
      if (vlToSRF(dcall))
      {
        value.setUID( vlToSRF(dcall)->uid().c_str() );
        return value;
      }

      if (dcall->classType() == DrawArrays::Type())
      {
        const DrawArrays* da = dcall->as<DrawArrays>();
        SRF_Structure* srf_obj = value.setStructure( new SRF_Structure("<DrawArrays>", generateUID("drawarrays_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(da, srf_obj);
        srf_obj->value().push_back( SRF_Structure::Value("Instances", (long long)da->instances()) );
        srf_obj->value().push_back( SRF_Structure::Value("Start", (long long)da->start()) );
        srf_obj->value().push_back( SRF_Structure::Value("Count", (long long)da->count()) );
      }
      else
      if (dcall->classType() == DrawElementsUInt::Type())
      {
        const DrawElementsUInt* de = dcall->as<DrawElementsUInt>();
        SRF_Structure* srf_obj = value.setStructure( new SRF_Structure("<DrawElementsUInt>", generateUID("drawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( SRF_Structure::Value("Instances", (long long)de->instances()) );
        srf_obj->value().push_back( SRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( SRF_Structure::Value("BaseVertex", (long long)de->baseVertex()) );
        srf_obj->value().push_back( SRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == DrawElementsUShort::Type())
      {
        const DrawElementsUShort* de = dcall->as<DrawElementsUShort>();
        SRF_Structure* srf_obj = value.setStructure( new SRF_Structure("<DrawElementsUShort>", generateUID("drawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( SRF_Structure::Value("Instances", (long long)de->instances()) );
        srf_obj->value().push_back( SRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( SRF_Structure::Value("BaseVertex", (long long)de->baseVertex()) );
        srf_obj->value().push_back( SRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == DrawElementsUByte::Type())
      {
        const DrawElementsUByte* de = dcall->as<DrawElementsUByte>();
        SRF_Structure* srf_obj = value.setStructure( new SRF_Structure("<DrawElementsUByte>", generateUID("drawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( SRF_Structure::Value("Instances", (long long)de->instances()) );
        srf_obj->value().push_back( SRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( SRF_Structure::Value("BaseVertex", (long long)de->baseVertex()) );
        srf_obj->value().push_back( SRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == MultiDrawElementsUInt::Type())
      {
        const MultiDrawElementsUInt* de = dcall->as<MultiDrawElementsUInt>();
        SRF_Structure* srf_obj = value.setStructure( new SRF_Structure("<MultiDrawElementsUInt>", generateUID("multidrawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( SRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( SRF_Structure::Value("BaseVertices", export_vector_int32(de->baseVertices())) );
        srf_obj->value().push_back( SRF_Structure::Value("CountVector", export_vector_int32(de->countVector())) );
        srf_obj->value().push_back( SRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == MultiDrawElementsUShort::Type())
      {
        const MultiDrawElementsUShort* de = dcall->as<MultiDrawElementsUShort>();
        SRF_Structure* srf_obj = value.setStructure( new SRF_Structure("<MultiDrawElementsUShort>", generateUID("multidrawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( SRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( SRF_Structure::Value("BaseVertices", export_vector_int32(de->baseVertices())) );
        srf_obj->value().push_back( SRF_Structure::Value("CountVector", export_vector_int32(de->countVector())) );
        srf_obj->value().push_back( SRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == MultiDrawElementsUByte::Type())
      {
        const MultiDrawElementsUByte* de = dcall->as<MultiDrawElementsUByte>();
        SRF_Structure* srf_obj = value.setStructure( new SRF_Structure("<MultiDrawElementsUByte>", generateUID("multidrawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( SRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( SRF_Structure::Value("BaseVertices", export_vector_int32(de->baseVertices())) );
        srf_obj->value().push_back( SRF_Structure::Value("CountVector", export_vector_int32(de->countVector())) );
        srf_obj->value().push_back( SRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      {
        Log::error("DrawCall type not supported for export.\n");
        VL_TRAP();
      }

      return value;
    }

    SRF_Value export_vector_int32(const std::vector<int>& vec)
    {
      SRF_Value value;
      value.setArray( new SRF_ArrayInt32 );
      value.getArrayInt32()->value().resize( vec.size() );
      if (vec.size())
        memcpy( &value.getArrayInt32()->value()[0], &vec[0], sizeof(vec[0])*vec.size() );
      return value;
    }

    //-----------------------------------------------------------------------------
    // IMPORT FUNCTIONS
    //-----------------------------------------------------------------------------

    void registerImportedStructure(const SRF_Structure* srf_obj, const Object* vl_obj)
    {
      VL_CHECK( mSRF_To_VL.find(srf_obj) == mSRF_To_VL.end() );
      mSRF_To_VL[srf_obj] = vl_obj;
    }

    Object* srfToVL(const SRF_Structure* srf_obj)
    {
      std::map< ref<SRF_Structure>, ref<Object> >::iterator it = mSRF_To_VL.find(srf_obj);
      if (it != mSRF_To_VL.end())
        return it->second.get();
      else
        return NULL;
    }

    const std::map< ref<SRF_Structure>, ref<Object> >& srfToVL() const { return mSRF_To_VL; }

    //-----------------------------------------------------------------------------

    ResourceDatabase* import_ResourceDatabase(const SRF_Structure* srf_obj)
    {
      if (srf_obj->tag() != "<ResourceDatabase>")
        return NULL;

      // <ResourceDatabase> must have at least one "SerializerVersion" and one "Resources" keys in this order.
      if (srf_obj->value().size() < 2)
        return NULL;
      else
      {
        if (srf_obj->value()[0].key() != "SerializerVersion" || srf_obj->value()[0].value().type() != SRF_Value::Int64)
        {
          Log::error( Say("Line %n : no serializer version found.\n") << srf_obj->value()[0].value().lineNumber() );
          return NULL;
        }
        else
        if (srf_obj->value()[0].value().getInt64() != VL_SERIALIZER_VERSION )
        {
          Log::error("Unsupported serializer version.\n");
          return NULL;
        }

        if (srf_obj->value()[1].key() != "Resources" || srf_obj->value()[1].value().type() != SRF_Value::List)
        {
          Log::error( Say("Line %n : 'Resources' key/value expected.\n") << srf_obj->value()[1].value().lineNumber() );
          return NULL;
        }
      }

      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<ResourceDatabase>();

      // link the SRF to the VL object
      ref<ResourceDatabase> res_db = new ResourceDatabase;
      registerImportedStructure(srf_obj, res_db.get());

      // get the list
      const SRF_List* list = srf_obj->value()[1].value().getList();
      for(size_t i=0; i<list->value().size(); ++i)
      {
        const SRF_Value& value = list->value()[i];

        // the member of this list must be all structures, unknown ones are silently skipped

        if (value.type() != SRF_Value::Structure)
        {
          Log::error( Say("Line %n : structure expected.\n") << value.lineNumber() );
          return NULL;
        }

        const SRF_Structure* obj = value.getStructure();
      
        if (obj->tag() == "<Geometry>")
        {
          Geometry* geom = import_Geometry(obj);
          if (geom)
            res_db->resources().push_back(geom);
        }
        else
        if (obj->tag() == "<Effect>")
        {
          Effect* fx = import_Effect(obj);
          if (fx)
            res_db->resources().push_back(fx);
        }
        else
        if (obj->tag() == "<Shader>")
        {
          Shader* sh = import_Shader(obj);
          if (sh)
            res_db->resources().push_back(sh);
        }
        else
        if (obj->tag() == "<Transform>")
        {
          Transform* tr = import_Transform(obj);
          if (tr)
            res_db->resources().push_back(tr);
        }
        else
        if (obj->tag() == "<Actor>")
        {
          Actor* act = import_Actor(obj);
          if (act)
            res_db->resources().push_back(act);
        }
        else
        if (obj->tag() == "<Camera>")
        {
          Camera* cam = import_Camera(obj);
          if (cam)
            res_db->resources().push_back(cam);
        }
        else
        {
          // mic fixme: metti questo in debug
          Log::warning( Say("Line %n : skipping unknown structure '%s'.\n") << obj->lineNumber() << obj->tag() );
        }
      }

      return res_db.get();
    }

    bool import_AABB(const SRF_Structure* srf_obj, AABB& aabb)
    {
      VL_CHECK( srf_obj->tag() == "<AABB>" )

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const SRF_Value& value = srf_obj->value()[i].value();
        if (key == "MinCorner")
        {
          VL_CHECK(value.type() == SRF_Value::ArrayFloat)
          aabb.setMinCorner( value.getArrayFloat()->getFloat3() );
        }
        else
        if (key == "MaxCorner")
        {
          VL_CHECK(value.type() == SRF_Value::ArrayFloat)
          aabb.setMaxCorner( value.getArrayFloat()->getFloat3() );
        }
        else
        if (key == "Extension")
          continue;
        else
        {
          Log::error( Say("Line %n : error.\n") << value.lineNumber() );
          return false;
        }
      }

      return true;
    }

    bool import_Sphere(const SRF_Structure* srf_obj, Sphere& sphere)
    {
      VL_CHECK( srf_obj->tag() == "<Sphere>" )

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const SRF_Value& value = srf_obj->value()[i].value();
        if (key == "Center")
        {
          VL_CHECK(value.type() == SRF_Value::ArrayFloat)
          sphere.setCenter( value.getArrayFloat()->getFloat3() );
        }
        else
        if (key == "Radius")
        {
          VL_CHECK(value.type() == SRF_Value::Double)
          sphere.setRadius( (Real)value.getDouble() );
        }
        else
        if (key == "Extension")
          continue;
        else
        {
          Log::error( Say("Line %n : error.\n") << value.lineNumber() );
          return false;
        }
      }

      return true;
    }

    ArrayAbstract* import_Array(const SRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<ArrayAbstract>();

      if (!srf_obj->getValue("Value"))
      {
        Log::error( Say("Line %n : error. 'Value' expected in object '%s'. \n") << srf_obj->lineNumber() << srf_obj->tag() );
        return NULL;
      }

      const SRF_Value& value = *srf_obj->getValue("Value");
    
      ref<ArrayAbstract> arr_abstract;

      if (srf_obj->tag() == "<ArrayFloat1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayFloat);
        const SRF_ArrayFloat* srf_arr_float = value.getArrayFloat();
        ref<ArrayFloat1> arr_float1 = new ArrayFloat1; arr_abstract = arr_float1;
        arr_float1->resize( srf_arr_float->value().size() );
        memcpy(arr_float1->ptr(), srf_arr_float->ptr(), arr_float1->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayFloat2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayFloat);
        const SRF_ArrayFloat* srf_arr_float = value.getArrayFloat();
        VL_SRF_CHECK_ERROR( srf_arr_float->value().size() % 2 == 0)
        ref<ArrayFloat2> arr_float2 = new ArrayFloat2; arr_abstract = arr_float2;
        arr_float2->resize( srf_arr_float->value().size() / 2 );
        memcpy(arr_float2->ptr(), srf_arr_float->ptr(), arr_float2->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayFloat3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayFloat);
        const SRF_ArrayFloat* srf_arr_float = value.getArrayFloat();
        VL_SRF_CHECK_ERROR( srf_arr_float->value().size() % 3 == 0)
        ref<ArrayFloat3> arr_float3 = new ArrayFloat3; arr_abstract = arr_float3;
        arr_float3->resize( srf_arr_float->value().size() / 3 );
        memcpy(arr_float3->ptr(), srf_arr_float->ptr(), arr_float3->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayFloat4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayFloat);
        const SRF_ArrayFloat* srf_arr_float = value.getArrayFloat();
        VL_SRF_CHECK_ERROR( srf_arr_float->value().size() % 4 == 0)
        ref<ArrayFloat4> arr_float4 = new ArrayFloat4; arr_abstract = arr_float4;
        arr_float4->resize( srf_arr_float->value().size() / 4 );
        memcpy(arr_float4->ptr(), srf_arr_float->ptr(), arr_float4->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayDouble);
        const SRF_ArrayDouble* srf_arr_double = value.getArrayDouble();
        ref<ArrayDouble1> arr_double1 = new ArrayDouble1; arr_abstract = arr_double1;
        arr_double1->resize( srf_arr_double->value().size() );
        memcpy(arr_double1->ptr(), srf_arr_double->ptr(), arr_double1->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayDouble);
        const SRF_ArrayDouble* srf_arr_double = value.getArrayDouble();
        VL_SRF_CHECK_ERROR( srf_arr_double->value().size() % 2 == 0)
        ref<ArrayDouble2> arr_double2 = new ArrayDouble2; arr_abstract = arr_double2;
        arr_double2->resize( srf_arr_double->value().size() / 2 );
        memcpy(arr_double2->ptr(), srf_arr_double->ptr(), arr_double2->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayDouble);
        const SRF_ArrayDouble* srf_arr_double = value.getArrayDouble();
        VL_SRF_CHECK_ERROR( srf_arr_double->value().size() % 3 == 0)
        ref<ArrayDouble3> arr_double3 = new ArrayDouble3; arr_abstract = arr_double3;
        arr_double3->resize( srf_arr_double->value().size() / 3 );
        memcpy(arr_double3->ptr(), srf_arr_double->ptr(), arr_double3->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayDouble);
        const SRF_ArrayDouble* srf_arr_double = value.getArrayDouble();
        VL_SRF_CHECK_ERROR( srf_arr_double->value().size() % 4 == 0)
        ref<ArrayDouble4> arr_double4 = new ArrayDouble4; arr_abstract = arr_double4;
        arr_double4->resize( srf_arr_double->value().size() / 4 );
        memcpy(arr_double4->ptr(), srf_arr_double->ptr(), arr_double4->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayInt1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        ref<ArrayInt1> arr_int1 = new ArrayInt1; arr_abstract = arr_int1;
        arr_int1->resize( srf_arr_int->value().size() );
        memcpy(arr_int1->ptr(), srf_arr_int->ptr(), arr_int1->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayInt2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayInt2> arr_int2 = new ArrayInt2; arr_abstract = arr_int2;
        arr_int2->resize( srf_arr_int->value().size() / 2 );
        memcpy(arr_int2->ptr(), srf_arr_int->ptr(), arr_int2->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayInt3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayInt3> arr_int3 = new ArrayInt3; arr_abstract = arr_int3;
        arr_int3->resize( srf_arr_int->value().size() / 3 );
        memcpy(arr_int3->ptr(), srf_arr_int->ptr(), arr_int3->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayInt4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayInt4> arr_int4 = new ArrayInt4; arr_abstract = arr_int4;
        arr_int4->resize( srf_arr_int->value().size() / 4 );
        memcpy(arr_int4->ptr(), srf_arr_int->ptr(), arr_int4->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        ref<ArrayUInt1> arr_int1 = new ArrayUInt1; arr_abstract = arr_int1;
        arr_int1->resize( srf_arr_int->value().size() );
        memcpy(arr_int1->ptr(), srf_arr_int->ptr(), arr_int1->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayUInt2> arr_int2 = new ArrayUInt2; arr_abstract = arr_int2;
        arr_int2->resize( srf_arr_int->value().size() / 2 );
        memcpy(arr_int2->ptr(), srf_arr_int->ptr(), arr_int2->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayUInt3> arr_int3 = new ArrayUInt3; arr_abstract = arr_int3;
        arr_int3->resize( srf_arr_int->value().size() / 3 );
        memcpy(arr_int3->ptr(), srf_arr_int->ptr(), arr_int3->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayUInt4> arr_int4 = new ArrayUInt4; arr_abstract = arr_int4;
        arr_int4->resize( srf_arr_int->value().size() / 4 );
        memcpy(arr_int4->ptr(), srf_arr_int->ptr(), arr_int4->bytesUsed());
      }
      else
      if (srf_obj->tag() == "<ArrayShort1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        ref<ArrayShort1> arr_short1 = new ArrayShort1; arr_abstract = arr_short1;
        arr_short1->resize( srf_arr_int->value().size() );
        typedef ArrayShort1::scalar_type type;
        for(size_t i=0; i<arr_short1->size(); ++i)
          arr_short1->at(i) = (type)srf_arr_int->value()[i];
      }
      else
      if (srf_obj->tag() == "<ArrayShort2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayShort2> arr_short2 = new ArrayShort2; arr_abstract = arr_short2;
        arr_short2->resize( srf_arr_int->value().size() / 2 );
        typedef ArrayShort2::scalar_type type;
        for(size_t i=0; i<arr_short2->size(); ++i)
          arr_short2->at(i) = svec2((type)srf_arr_int->value()[i*2+0], (type)srf_arr_int->value()[i*2+1]);
      }
      else
      if (srf_obj->tag() == "<ArrayShort3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayShort3> arr_short3 = new ArrayShort3; arr_abstract = arr_short3;
        arr_short3->resize( srf_arr_int->value().size() / 3 );
        typedef ArrayShort3::scalar_type type;
        for(size_t i=0; i<arr_short3->size(); ++i)
          arr_short3->at(i) = svec3((type)srf_arr_int->value()[i*3+0], (type)srf_arr_int->value()[i*3+1], (type)srf_arr_int->value()[i*3+2]);
      }
      else
      if (srf_obj->tag() == "<ArrayShort4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayShort4> arr_short4 = new ArrayShort4; arr_abstract = arr_short4;
        arr_short4->resize( srf_arr_int->value().size() / 4 );
        typedef ArrayShort4::scalar_type type;
        for(size_t i=0; i<arr_short4->size(); ++i)
          arr_short4->at(i) = svec4((type)srf_arr_int->value()[i*4+0], (type)srf_arr_int->value()[i*4+1], (type)srf_arr_int->value()[i*4+2], (type)srf_arr_int->value()[i*4+3]);
      }
      else
      if (srf_obj->tag() == "<ArrayUShort1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        ref<ArrayUShort1> arr_short1 = new ArrayUShort1; arr_abstract = arr_short1;
        arr_short1->resize( srf_arr_int->value().size() );
        typedef ArrayUShort1::scalar_type type;
        for(size_t i=0; i<arr_short1->size(); ++i)
          arr_short1->at(i) = (type)srf_arr_int->value()[i];
      }
      else
      if (srf_obj->tag() == "<ArrayUShort2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayUShort2> arr_short2 = new ArrayUShort2; arr_abstract = arr_short2;
        arr_short2->resize( srf_arr_int->value().size() / 2 );
        typedef ArrayUShort2::scalar_type type;
        for(size_t i=0; i<arr_short2->size(); ++i)
          arr_short2->at(i) = usvec2((type)srf_arr_int->value()[i*2+0], (type)srf_arr_int->value()[i*2+1]);
      }
      else
      if (srf_obj->tag() == "<ArrayUShort3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayUShort3> arr_short3 = new ArrayUShort3; arr_abstract = arr_short3;
        arr_short3->resize( srf_arr_int->value().size() / 3 );
        typedef ArrayUShort3::scalar_type type;
        for(size_t i=0; i<arr_short3->size(); ++i)
          arr_short3->at(i) = usvec3((type)srf_arr_int->value()[i*3+0], (type)srf_arr_int->value()[i*3+1], (type)srf_arr_int->value()[i*3+2]);
      }
      else
      if (srf_obj->tag() == "<ArrayUShort4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayUShort4> arr_short4 = new ArrayUShort4; arr_abstract = arr_short4;
        arr_short4->resize( srf_arr_int->value().size() / 4 );
        typedef ArrayUShort4::scalar_type type;
        for(size_t i=0; i<arr_short4->size(); ++i)
          arr_short4->at(i) = usvec4((type)srf_arr_int->value()[i*4+0], (type)srf_arr_int->value()[i*4+1], (type)srf_arr_int->value()[i*4+2], (type)srf_arr_int->value()[i*4+3]);
      }
      else
      if (srf_obj->tag() == "<ArrayByte1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        ref<ArrayByte1> arr_byte1 = new ArrayByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( srf_arr_int->value().size() );
        typedef ArrayByte1::scalar_type type;
        for(size_t i=0; i<arr_byte1->size(); ++i)
          arr_byte1->at(i) = (type)srf_arr_int->value()[i];
      }
      else
      if (srf_obj->tag() == "<ArrayByte2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayByte2> arr_byte2 = new ArrayByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( srf_arr_int->value().size() / 2 );
        typedef ArrayByte2::scalar_type type;
        for(size_t i=0; i<arr_byte2->size(); ++i)
          arr_byte2->at(i) = bvec2((type)srf_arr_int->value()[i*2+0], (type)srf_arr_int->value()[i*2+1]);
      }
      else
      if (srf_obj->tag() == "<ArrayByte3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayByte3> arr_byte3 = new ArrayByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( srf_arr_int->value().size() / 3 );
        typedef ArrayByte3::scalar_type type;
        for(size_t i=0; i<arr_byte3->size(); ++i)
          arr_byte3->at(i) = bvec3((type)srf_arr_int->value()[i*3+0], (type)srf_arr_int->value()[i*3+1], (type)srf_arr_int->value()[i*3+2]);
      }
      else
      if (srf_obj->tag() == "<ArrayByte4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayByte4> arr_byte4 = new ArrayByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( srf_arr_int->value().size() / 4 );
        typedef ArrayByte4::scalar_type type;
        for(size_t i=0; i<arr_byte4->size(); ++i)
          arr_byte4->at(i) = bvec4((type)srf_arr_int->value()[i*4+0], (type)srf_arr_int->value()[i*4+1], (type)srf_arr_int->value()[i*4+2], (type)srf_arr_int->value()[i*4+3]);
      }
      else
      if (srf_obj->tag() == "<ArrayUByte1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        ref<ArrayUByte1> arr_byte1 = new ArrayUByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( srf_arr_int->value().size() );
        typedef ArrayUByte1::scalar_type type;
        for(size_t i=0; i<arr_byte1->size(); ++i)
          arr_byte1->at(i) = (type)srf_arr_int->value()[i];
      }
      else
      if (srf_obj->tag() == "<ArrayUByte2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayUByte2> arr_byte2 = new ArrayUByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( srf_arr_int->value().size() / 2 );
        typedef ArrayUByte2::scalar_type type;
        for(size_t i=0; i<arr_byte2->size(); ++i)
          arr_byte2->at(i) = ubvec2((type)srf_arr_int->value()[i*2+0], (type)srf_arr_int->value()[i*2+1]);
      }
      else
      if (srf_obj->tag() == "<ArrayUByte3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayUByte3> arr_byte3 = new ArrayUByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( srf_arr_int->value().size() / 3 );
        typedef ArrayUByte3::scalar_type type;
        for(size_t i=0; i<arr_byte3->size(); ++i)
          arr_byte3->at(i) = ubvec3((type)srf_arr_int->value()[i*3+0], (type)srf_arr_int->value()[i*3+1], (type)srf_arr_int->value()[i*3+2]);
      }
      else
      if (srf_obj->tag() == "<ArrayUByte4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInt32);
        const SRF_ArrayInt32* srf_arr_int = value.getArrayInt32();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayUByte4> arr_byte4 = new ArrayUByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( srf_arr_int->value().size() / 4 );
        typedef ArrayUByte4::scalar_type type;
        for(size_t i=0; i<arr_byte4->size(); ++i)
          arr_byte4->at(i) = ubvec4((type)srf_arr_int->value()[i*4+0], (type)srf_arr_int->value()[i*4+1], (type)srf_arr_int->value()[i*4+2], (type)srf_arr_int->value()[i*4+3]);
      }
      else
      {
        Log::error( Say("Line %n : unknown array '%s'.\n") << srf_obj->lineNumber() << srf_obj->tag() );
        return NULL;
      }

      // link the SRF to the VL object
      registerImportedStructure(srf_obj, arr_abstract.get());

      return arr_abstract.get();
    }

    EPrimitiveType import_EPrimitiveType(const std::string& str, int line_num)
    {
      if ("PT_POINTS" == str) return PT_POINTS;
      if ("PT_LINES" == str)  return PT_LINES;
      if ("PT_LINE_LOOP" == str) return PT_LINE_LOOP;
      if ("PT_LINE_STRIP" == str) return PT_LINE_STRIP;
      if ("PT_TRIANGLES" == str) return PT_TRIANGLES;
      if ("PT_TRIANGLE_STRIP" == str) return PT_TRIANGLE_STRIP;
      if ("PT_TRIANGLE_FAN" == str) return PT_TRIANGLE_FAN;
      if ("PT_QUADS" == str) return PT_QUADS;
      if ("PT_QUAD_STRIP" == str) return PT_QUAD_STRIP;
      if ("PT_POLYGON" == str) return PT_POLYGON;
      if ("PT_LINES_ADJACENCY" == str) return PT_LINES_ADJACENCY;
      if ("PT_LINE_STRIP_ADJACENCY" == str) return PT_LINE_STRIP_ADJACENCY;
      if ("PT_TRIANGLES_ADJACENCY" == str) return PT_TRIANGLES_ADJACENCY;
      if ("PT_TRIANGLE_STRIP_ADJACENCY" == str) return PT_TRIANGLES_ADJACENCY;
      if ("PT_PATCHES" == str) return PT_PATCHES;
      // if ("PT_UNKNOWN" == str) return PT_UNKNOWN;
    
      Log::error( Say("Line %n : error : unknown primitive type '%s'\n") << line_num << str);
      return PT_UNKNOWN;
    }

    DrawCall* import_DrawCall(const SRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<DrawCall>();

      ref<DrawCall> draw_call;

      if(srf_obj->tag() == "<DrawElementsUInt>" || srf_obj->tag() == "<DrawElementsUShort>" || srf_obj->tag() == "<DrawElementsUByte>")
      {
        ref<DrawElementsBase> de;

        if (srf_obj->tag() == "<DrawElementsUInt>")
          draw_call = de = new DrawElementsUInt;

        if (srf_obj->tag() == "<DrawElementsUShort>")
          draw_call = de = new DrawElementsUShort;

        if (srf_obj->tag() == "<DrawElementsUByte>")
          draw_call = de = new DrawElementsUByte;

        VL_CHECK(de)
        VL_CHECK(draw_call)

        for(size_t i=0; i<srf_obj->value().size(); ++i)
        {
          const std::string& key = srf_obj->value()[i].key();
          const SRF_Value& value = srf_obj->value()[i].value();
          if( key == "PrimitiveType" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Identifier )
            de->setPrimitiveType( import_EPrimitiveType( value.getIdentifier(), value.lineNumber() ) );
            VL_SRF_CHECK_ERROR( de->primitiveType() != PT_UNKNOWN );
          }
          else
          if( key == "Enabled" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Bool )
            de->setEnabled( value.getBool() );
          }
          else
          if( key == "Instances" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Int64 )
            de->setInstances( (int)value.getInt64() );
          }
          else
          if( key == "PrimitiveRestartEnabled" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Bool )
            de->setPrimitiveRestartEnabled( value.getBool() );
          }
          else
          if( key == "BaseVertex" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Int64 )
            de->setBaseVertex( (int)value.getInt64() );
          }
          else
          if( key == "IndexBuffer" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Structure )
            ArrayAbstract* arr_abstract = import_Array(value.getStructure());
            if(!arr_abstract)
              return NULL;

            if ( de->classType() == DrawElementsUInt::Type() )
            {
              VL_SRF_CHECK_ERROR(arr_abstract->classType() == ArrayUInt1::Type());
              de->as<DrawElementsUInt>()->setIndexBuffer( arr_abstract->as<ArrayUInt1>() );
            }
            else
            if ( de->classType() == DrawElementsUShort::Type() )
            {
              VL_SRF_CHECK_ERROR(arr_abstract->classType() == ArrayUShort1::Type());
              de->as<DrawElementsUShort>()->setIndexBuffer( arr_abstract->as<ArrayUShort1>() );
            }
            else
            if ( de->classType() == DrawElementsUByte::Type() )
            {
              VL_SRF_CHECK_ERROR(arr_abstract->classType() == ArrayUByte1::Type());
              de->as<DrawElementsUByte>()->setIndexBuffer( arr_abstract->as<ArrayUByte1>() );
            }
          }
          else
          if ( key == "Extension" )
            continue;
          else
          {
            Log::error( Say("Line %n : error.\n") << value.lineNumber() );
            return NULL;
          }
        }
      }
      else
      if(srf_obj->tag() == "<MultiDrawElementsUInt>" || srf_obj->tag() == "<MultiDrawElementsUShort>" || srf_obj->tag() == "<MultiDrawElementsUByte>")
      {
        ref<MultiDrawElementsBase> de;

        if (srf_obj->tag() == "<MultiDrawElementsUInt>")
          draw_call = de = new MultiDrawElementsUInt;

        if (srf_obj->tag() == "<MultiDrawElementsUShort>")
          draw_call = de = new MultiDrawElementsUShort;

        if (srf_obj->tag() == "<MultiDrawElementsUByte>")
          draw_call = de = new MultiDrawElementsUByte;

        VL_CHECK(de)
        VL_CHECK(draw_call)

        for(size_t i=0; i<srf_obj->value().size(); ++i)
        {
          const std::string& key = srf_obj->value()[i].key();
          const SRF_Value& value = srf_obj->value()[i].value();
          if( key == "PrimitiveType" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Identifier )
            de->setPrimitiveType( import_EPrimitiveType( value.getIdentifier(), value.lineNumber() ) );
            VL_SRF_CHECK_ERROR( de->primitiveType() != PT_UNKNOWN );
          }
          else
          if( key == "Enabled" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Bool )
            de->setEnabled( value.getBool() );
          }
          else
          if( key == "PrimitiveRestartEnabled" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Bool )
            de->setPrimitiveRestartEnabled( value.getBool() );
          }
          else
          if( key == "BaseVertices" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::ArrayInt32 )
            de->setBaseVertices( value.getArrayInt32()->value() );
          }
          else
          if( key == "CountVector" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::ArrayInt32 )
            de->countVector() = value.getArrayInt32()->value();
          }
          else
          if( key == "IndexBuffer" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Structure )
            ArrayAbstract* arr_abstract = import_Array(value.getStructure());
            if( !arr_abstract )
              return NULL;

            if ( de->classType() == MultiDrawElementsUInt::Type() )
            {
              VL_SRF_CHECK_ERROR(arr_abstract->classType() == ArrayUInt1::Type());
              de->as<MultiDrawElementsUInt>()->setIndexBuffer( arr_abstract->as<ArrayUInt1>() );
            }
            else
            if ( de->classType() == MultiDrawElementsUShort::Type() )
            {
              VL_SRF_CHECK_ERROR(arr_abstract->classType() == ArrayUShort1::Type());
              de->as<MultiDrawElementsUShort>()->setIndexBuffer( arr_abstract->as<ArrayUShort1>() );
            }
            else
            if ( de->classType() == MultiDrawElementsUByte::Type() )
            {
              VL_SRF_CHECK_ERROR(arr_abstract->classType() == ArrayUByte1::Type());
              de->as<MultiDrawElementsUByte>()->setIndexBuffer( arr_abstract->as<ArrayUByte1>() );
            }
          }
          else
          if ( key == "Extension" )
            continue;
          else
          {
            Log::error( Say("Line %n : error.\n") << value.lineNumber() );
            return NULL;
          }
        }

        // finalize setup
        de->computePointerVector();
        de->computeVBOPointerVector();
        if ( de->baseVertices().size() != de->countVector().size() )
          de->baseVertices().resize( de->countVector().size() );
      }
      else
      if(srf_obj->tag() == "<DrawArrays>")
      {
        ref<DrawArrays> da;

        draw_call = da = new DrawArrays;

        for(size_t i=0; i<srf_obj->value().size(); ++i)
        {
          const std::string& key = srf_obj->value()[i].key();
          const SRF_Value& value = srf_obj->value()[i].value();

          if( key == "PrimitiveType" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Identifier )
            da->setPrimitiveType( import_EPrimitiveType( value.getIdentifier(), value.lineNumber() ) );
            VL_SRF_CHECK_ERROR( da->primitiveType() != PT_UNKNOWN );
          }
          else
          if( key == "Enabled" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Bool )
            da->setEnabled( value.getBool() );
          }
          else
          if( key == "Instances" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Int64 )
            da->setInstances( (int)value.getInt64() );
          }
          else
          if( key == "Start" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Int64 )
            da->setStart( (int)value.getInt64() );
          }
          else
          if( key == "Count" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Int64 )
            da->setCount( (int)value.getInt64() );
          }
          else
          if ( key == "Extension" )
            continue;
          else
          {
            Log::error( Say("Line %n : error.\n") << value.lineNumber() );
            return NULL;
          }
        }
      }
      else
      {
        Log::error( Say("Line %n : error. Unknown draw call.\n") << srf_obj->lineNumber() );
        return NULL;
      }

      // link the SRF to the VL object
      registerImportedStructure(srf_obj, draw_call.get());
    
      return draw_call.get();
    }

    VertexAttribInfo* import_VertexAttribInfo(const SRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<VertexAttribInfo>();

      if (srf_obj->tag() != "<VertexAttribInfo>")
      {
        Log::error( Say("Line %n : <VertexAttribInfo> expected.\n") << srf_obj->lineNumber() );
        return NULL;
      }

      // link the SRF to the VL object
      ref<VertexAttribInfo> info = new VertexAttribInfo;
      registerImportedStructure(srf_obj, info.get());

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const SRF_Value& value = srf_obj->value()[i].value();

        if (key == "Data")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          ArrayAbstract* arr = import_Array( value.getStructure() );
          if(arr)
            info->setData(arr);
          else
            return NULL;
        }
        else
        if (key == "Normalize")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Bool) 
          info->setNormalize( value.getBool() );
        }
        else
        if (key == "Interpretation")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Identifier) 
          if (strcmp(value.getIdentifier(), "VAI_NORMAL") == 0)
            info->setInterpretation(VAI_NORMAL);
          else
          if (strcmp(value.getIdentifier(), "VAI_INTEGER") == 0)
            info->setInterpretation(VAI_INTEGER);
          else
          if (strcmp(value.getIdentifier(), "VAI_DOUBLE") == 0)
            info->setInterpretation(VAI_DOUBLE);
          else
          {
            Log::error( Say("Line %n : error.\n") << value.lineNumber() );
            return NULL;
          }
        }
        else
        if (key == "Extension")
          continue;
        else
        {
          Log::error( Say("Line %n : error.\n") << value.lineNumber() );
          return NULL;
        }
      }
    
      return info.get();
    }

    Geometry* import_Geometry(const SRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Geometry>();

      if (srf_obj->tag() != "<Geometry>")
      {
        Log::error( Say("Line %n : <Geometry> expected.\n") << srf_obj->lineNumber() );
        return NULL;
      }

      // link the SRF to the VL object
      ref<Geometry> geom = new Geometry;
      registerImportedStructure(srf_obj, geom.get());

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const SRF_Value& value = srf_obj->value()[i].value();

        if (key == "VBOEnabled")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Bool) 
          geom->setVBOEnabled( value.getBool() );
        }
        else
        if (key == "DisplayListEnabled")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Bool) 
          geom->setDisplayListEnabled( value.getBool() );
        }
        else
        if (key == "AABB")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          AABB aabb;
          if (!import_AABB(value.getStructure(), aabb))
            return NULL;
        }
        else
        if (key == "Sphere")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          Sphere sphere;
          if (!import_Sphere(value.getStructure(), sphere))
            return NULL;
        }
        else
        if (key == "VertexArray")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setVertexArray(arr);
          else
            return NULL;
        }
        else
        if (key == "NormalArray")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setNormalArray(arr);
          else
            return NULL;
        }
        else
        if (key == "ColorArray")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setColorArray(arr);
          else
            return NULL;
        }
        else
        if (key == "SecondaryColorArray")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setSecondaryColorArray(arr);
          else
            return NULL;
        }
        else
        if (key == "FogCoordArray")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setFogCoordArray(arr);
          else
            return NULL;
        }
        else
        if (strstr(key.c_str(), "TexCoordArray") == key.c_str())
        {
          const char* ch = key.c_str() + 13/*strlen("TexCoordArray")*/;
          int tex_unit = 0;
          for(; *ch; ++ch)
          {
            if (*ch>='0' && *ch<='9')
              tex_unit = tex_unit*10 + (*ch - '0');
            else
            {
              Log::error( Say("Line %n : error. ") << value.lineNumber() );
              Log::error( "TexCoordArray must end with a number!\n" );
              return NULL;
            }
          }

          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setTexCoordArray(tex_unit, arr);
          else
            return NULL;
        }
        else
        if (strstr(key.c_str(), "VertexAttribArray") == key.c_str())
        {
          const char* ch = key.c_str() + 17/*strlen("VertexAttribArray")*/;
          int attrib_location = 0;
          for(; *ch; ++ch)
          {
            if (*ch>='0' && *ch<='9')
              attrib_location = attrib_location*10 + (*ch - '0');
            else
            {
              Log::error( Say("Line %n : error. ") << value.lineNumber() );
              Log::error( "VertexAttribArray must end with a number!\n" );
              return NULL;
            }
          }
        
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          VertexAttribInfo* info_ptr = import_VertexAttribInfo(value.getStructure());
          if (info_ptr)
          {
            VertexAttribInfo info = *info_ptr;
            info.setAttribLocation(attrib_location);
            geom->setVertexAttribArray(info);
          }
          else
            return NULL;
        }
        else
        if (key == "DrawCall")
        {
          VL_SRF_CHECK_ERROR(value.type() == SRF_Value::Structure) 
          DrawCall* draw_call = import_DrawCall(value.getStructure());
          if (draw_call)
            geom->drawCalls()->push_back(draw_call);
          else
            return NULL;
        }
        else
        if (key == "Extension")
          continue;
        else
        {
          Log::error( Say("Line %n : error.\n") << value.lineNumber() );
          return NULL;
        }
      }

      return geom.get();
    }

    Effect* import_Effect(const SRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Effect>();

      return NULL;
    }

    Shader* import_Shader(const SRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Shader>();

      return NULL;
    }

    Transform* import_Transform(const SRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Transform>();

      return NULL;
    }

    Actor* import_Actor(const SRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Actor>();

      return NULL;
    }

    Camera* import_Camera(const SRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Camera>();

      return NULL;
    }

    //-----------------------------------------------------------------------------

    std::string exportToText(const ResourceDatabase* res_db)
    {
      SRF_Value value = export_ResourceDatabase(res_db);

      // do not link!

      // collect the UIDs that need to be visible
      std::set< std::string > uid_set;
      SRF_UIDCollectorVisitor uid_collector;
      uid_collector.setUIDSet(&uid_set);
      value.getStructure()->acceptVisitor(&uid_collector);

      SRF_TextExportVisitor text_export_visitor;
      text_export_visitor.setUIDSet(&uid_set);
      value.getStructure()->acceptVisitor(&text_export_visitor);

      return text_export_visitor.srfText();
    }

    //-----------------------------------------------------------------------------

    ref<ResourceDatabase> importFromText(VirtualFile* file)
    {
      SRF_Parser parser;
      parser.tokenizer()->setInputFile( file );

      if (!parser.parse())
        return NULL;

      if (!parser.link())
        return NULL;

      ref<ResourceDatabase> res_db = import_ResourceDatabase( parser.root() );
      return res_db;
    }

  protected:
    // import
    std::map< ref<SRF_Structure>, ref<Object> > mSRF_To_VL;

    // export
    std::map< ref<Object>, ref<SRF_Structure> > mVL_To_SRF;
    int mUIDCounter;

    EError mError;
  };
//-----------------------------------------------------------------------------
  VLGRAPHICS_EXPORT ref<ResourceDatabase> loadSRF(VirtualFile* file);
  VLGRAPHICS_EXPORT ref<ResourceDatabase> loadSRF(const String& path);
  VLGRAPHICS_EXPORT bool writeSRF(VirtualFile* file, const ResourceDatabase*);
  VLGRAPHICS_EXPORT bool writeSRF(const String& file, const ResourceDatabase*);
//---------------------------------------------------------------------------
// LoadWriterSRF
//---------------------------------------------------------------------------
  /**
   * The LoadWriterSRF class is a ResourceLoadWriter capable of reading Visualization Library's SRF files.
   */
  class LoadWriterSRF: public ResourceLoadWriter
  {
    VL_INSTRUMENT_CLASS(vl::LoadWriterSRF, ResourceLoadWriter)

  public:
    LoadWriterSRF(): ResourceLoadWriter("|srf|", "|srf|") {}

    ref<ResourceDatabase> loadResource(const String& path) const 
    {
      return loadSRF(path);
    }

    ref<ResourceDatabase> loadResource(VirtualFile* file) const
    {
      return loadSRF(file);
    }

    bool writeResource(const String& path, ResourceDatabase* res_db) const
    {
      return writeSRF(path, res_db);
    }

    bool writeResource(VirtualFile* file, ResourceDatabase* res_db) const
    {
      return writeSRF(file, res_db);
    }
  };
//-----------------------------------------------------------------------------
}

#undef VL_SRF_CHECK_ERROR

#endif
