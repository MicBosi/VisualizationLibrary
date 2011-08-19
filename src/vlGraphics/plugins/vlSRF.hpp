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

#ifndef LoadWriterVRF_INCLUDE_ONCE
#define LoadWriterVRF_INCLUDE_ONCE

#include <vlCore/VRF_Tools.hpp>
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
#include <vlGraphics/DistanceLODEvaluator.hpp>
#include <vlGraphics/PixelLODEvaluator.hpp>
#include <vlGraphics/DepthSortCallback.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/DiskFile.hpp>

#define VL_VRF_CHECK_ERROR(Condition) \
  if (!(Condition))                   \
  {                                   \
  Log::error( Say("Line %n : condition failed : %s\n") << value.lineNumber() << #Condition ); \
    return NULL;                      \
  }

namespace vl
{
  static const int VL_SERIALIZER_VERSION = 100;

  class VRF_Serializer
  {
  public:
    typedef enum
    {
      NoError,
      ExportError,
      ImportError
    } EError;

    VRF_Serializer()
    {
      mError = NoError;
      mUIDCounter = 0;
    }

    EError error() const { return mError; }

    void resetError() { mError = NoError; }

    //-----------------------------------------------------------------------------
    // EXPORT
    //-----------------------------------------------------------------------------

    void registerExportedStructure(const Object* vl_obj, const VRF_Structure* srf_obj)
    {
      VL_CHECK( mVL_To_VRF.find(vl_obj) == mVL_To_VRF.end() );
      mVL_To_VRF[vl_obj] = srf_obj;
    }

    VRF_Structure* vlToVRF(const Object* srf_obj)
    {
      std::map< ref<Object>, ref<VRF_Structure> >::iterator it = mVL_To_VRF.find(srf_obj);
      if (it != mVL_To_VRF.end())
        return it->second.get();
      else
        return NULL;
    }

    const std::map< ref<Object>, ref<VRF_Structure> >& vlToVRF() const { return mVL_To_VRF; }

    //-----------------------------------------------------------------------------

    std::string generateUID(const char* prefix)
    {
      return String::printf("#%sid%d", prefix, ++mUIDCounter).toStdString();
    }

    VRF_Value toIdentifier(const std::string& str) { return VRF_Value(str.c_str(), VRF_Value::Identifier); }

    VRF_Value toUID(const std::string& str)        { return VRF_Value(str.c_str(), VRF_Value::UID); }

    VRF_Value toString(const std::string& str)     { return VRF_Value(str.c_str(), VRF_Value::String); }

    VRF_Value toRawtext(const std::string& str)    { return VRF_Value( new VRF_RawtextBlock(NULL, str.c_str()) ); }

    VRF_Value toValue(const mat4& mat)
    {
      VRF_Value val( new VRF_ArrayReal );
      VRF_ArrayReal* arr = val.getArrayReal();
      arr->value().resize(4*4);
      for(size_t i=0; i<arr->value().size(); ++i)
        arr->value()[i] = mat.ptr()[i];
      return val;
    }

    VRF_Value toValue(const vec4& vec)
    {
      VRF_Value val( new VRF_ArrayReal );
      VRF_ArrayReal* arr = val.getArrayReal();
      arr->value().resize(4);
      arr->value()[0] = vec.x();
      arr->value()[1] = vec.y();
      arr->value()[2] = vec.z();
      arr->value()[3] = vec.w();
      return val;
    }

    VRF_Value toValue(const ivec4& vec)
    {
      VRF_Value val( new VRF_ArrayReal );
      VRF_ArrayReal* arr = val.getArrayReal();
      arr->value().resize(4);
      arr->value()[0] = vec.x();
      arr->value()[1] = vec.y();
      arr->value()[2] = vec.z();
      arr->value()[3] = vec.w();
      return val;
    }

    VRF_Value toValue(const uvec4& vec)
    {
      VRF_Value val( new VRF_ArrayReal );
      VRF_ArrayReal* arr = val.getArrayReal();
      arr->value().resize(4);
      arr->value()[0] = vec.x();
      arr->value()[1] = vec.y();
      arr->value()[2] = vec.z();
      arr->value()[3] = vec.w();
      return val;
    }

    VRF_Value toValue(const vec3& vec)
    {
      VRF_Value val( new VRF_ArrayReal );
      VRF_ArrayReal* arr = val.getArrayReal();
      arr->value().resize(3);
      arr->value()[0] = vec.x();
      arr->value()[1] = vec.y();
      arr->value()[2] = vec.z();
      return val;
    }

    VRF_Value toValue(const vec2& vec)
    {
      VRF_Value val( new VRF_ArrayReal );
      VRF_ArrayReal* arr = val.getArrayReal();
      arr->value().resize(2);
      arr->value()[0] = vec.x();
      arr->value()[1] = vec.y();
      return val;
    }

    VRF_Value export_ResourceDatabase(const ResourceDatabase* res_db)
    {
      VRF_Value value;
      if (vlToVRF(res_db))
      {
        value.setUID( vlToVRF(res_db)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<ResourceDatabase>", generateUID("resourcedb_")) );
      registerExportedStructure(res_db, value.getStructure());

      value.getStructure()->value().push_back( VRF_Structure::Value("SerializerVersion", (long long)VL_SERIALIZER_VERSION) );

      VRF_Structure::Value srf_resource_list("Resources", new VRF_List);
      value.getStructure()->value().push_back( srf_resource_list );

      for(size_t i=0; i<res_db->resources().size(); ++i)
      {
        VRF_Value resource;

        if(res_db->resources()[i]->isOfType(Actor::Type()))
          resource = export_Actor(res_db->resources()[i]->as<Actor>());
        else
        if(res_db->resources()[i]->isOfType(Renderable::Type()))
          resource = export_Renderable(res_db->resources()[i]->as<Renderable>());
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
        if(res_db->resources()[i]->isOfType(Camera::Type()))
          resource = export_Camera(res_db->resources()[i]->as<Camera>());

        srf_resource_list.value().getList()->value().push_back( resource );
      }

      return value;
    }

    VRF_Value export_AABB(const AABB& aabb)
    {
      VRF_Value value ( new VRF_Structure("<AABB>", generateUID("aabb_")) );
      value.getStructure()->value().push_back( VRF_Structure::Value("MinCorner", toValue(aabb.minCorner())) );
      value.getStructure()->value().push_back( VRF_Structure::Value("MaxCorner", toValue(aabb.maxCorner())) );
      return value;
    }

    VRF_Value export_Sphere(const Sphere& sphere)
    {
      VRF_Value value ( new VRF_Structure("<Sphere>", generateUID("sphere_")) );
      value.getStructure()->value().push_back( VRF_Structure::Value("Center", toValue(sphere.center())) );
      value.getStructure()->value().push_back( VRF_Structure::Value("Radius", sphere.radius()) );
      return value;
    }

    VRF_Value export_Renderable(const Renderable* ren)
    {
      VRF_Value value;
      if (vlToVRF(ren))
      {
        value.setUID( vlToVRF(ren)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure( "<Geometry>", generateUID("geometry_") ) );
      registerExportedStructure(ren, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("VBOEnabled", ren->isVBOEnabled()) );
      values.push_back( VRF_Structure::Value("DisplayListEnabled", ren->isDisplayListEnabled()) );
      values.push_back( VRF_Structure::Value("AABB", export_AABB(ren->boundingBox())) );
      values.push_back( VRF_Structure::Value("Sphere", export_Sphere(ren->boundingSphere())) );

      if( ren->isOfType( Geometry::Type() ) )
        export_Geometry( ren->as<Geometry>(), values );
      else
      {
        VL_TRAP(); // mic fixme: support also the other renderables
      }

      return value;
    }

    void export_Geometry(const Geometry* geom, std::vector<VRF_Structure::Value>& values)
    {
      if (geom->vertexArray()) 
        values.push_back( VRF_Structure::Value("VertexArray", export_Array(geom->vertexArray())) );
    
      if (geom->normalArray()) 
        values.push_back( VRF_Structure::Value("NormalArray", export_Array(geom->normalArray())) );
    
      if (geom->colorArray()) 
        values.push_back( VRF_Structure::Value("ColorArray", export_Array(geom->colorArray())) );
    
      if (geom->secondaryColorArray()) 
        values.push_back( VRF_Structure::Value("SecondaryColorArray", export_Array(geom->secondaryColorArray())) );
    
      if (geom->fogCoordArray()) 
        values.push_back( VRF_Structure::Value("FogCoordArray", export_Array(geom->fogCoordArray())) );

      for( int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      {
        if (geom->texCoordArray(i)) 
        {
          std::string TexCoordArray = String::printf("TexCoordArray%d", i).toStdString();
          values.push_back( VRF_Structure::Value(TexCoordArray.c_str(), export_Array(geom->texCoordArray(i))) ); 
        }
      }
    
      for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      {
        if (geom->vertexAttribArray(i))
        {
          std::string VertexAttribArray = String::printf("VertexAttribArray%d", i).toStdString();
          values.push_back( VRF_Structure::Value(VertexAttribArray.c_str(), export_VertexAttribInfo(geom->vertexAttribArray(i))) );
        }
      }

      for(int i=0; i<geom->drawCalls()->size(); ++i)
        values.push_back( VRF_Structure::Value("DrawCall", export_DrawCall(geom->drawCalls()->at(i))) );
    }

    VRF_Value export_Actor(const Actor* act)
    {
      VRF_Value value;
      if (vlToVRF(act))
      {
        value.setUID( vlToVRF(act)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Actor>", generateUID("actor_")) );
      registerExportedStructure(act, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      VRF_Value renderables;
      renderables.setList( new VRF_List );
      for(size_t i=0; i<VL_MAX_ACTOR_LOD && act->lod(i); ++i)
        renderables.getList()->value().push_back( export_Renderable(act->lod(i)) );
      values.push_back( VRF_Structure::Value("Lods", renderables) );

      values.push_back( VRF_Structure::Value("AABB", export_AABB(act->boundingBox())) );
      values.push_back( VRF_Structure::Value("Sphere", export_Sphere(act->boundingSphere())) );
      if (act->effect())
        values.push_back( VRF_Structure::Value("Effect", export_Effect(act->effect())) );
      if (act->transform())
        values.push_back( VRF_Structure::Value("Transform", export_Transform(act->transform())) );

      VRF_Value uniforms;
      uniforms.setList( new VRF_List );
      for(size_t i=0; act->getUniformSet() && i<act->uniforms().size(); ++i)
        uniforms.getList()->value().push_back( export_Uniform(act->uniforms()[i].get()) );
      values.push_back( VRF_Structure::Value("Uniforms", uniforms) );

      if (act->lodEvaluator())
          values.push_back( VRF_Structure::Value("LODEvaluator", export_LODEvaluator(act->lodEvaluator())) );

      // mic fixme:
      // Scissor: scissorts might go away from the Actor

      VRF_Value callbacks;
      callbacks.setList( new VRF_List );
      for(int i=0; i<act->actorEventCallbacks()->size(); ++i)
        callbacks.getList()->value().push_back( export_ActorEventCallback(act->actorEventCallbacks()->at(i)) );
      values.push_back( VRF_Structure::Value("ActorEventCallbacks", callbacks) );

      values.push_back( VRF_Structure::Value("RenderBlock", (long long)act->renderBlock() ) );
      values.push_back( VRF_Structure::Value("RenderRank", (long long)act->renderRank() ) );
      values.push_back( VRF_Structure::Value("EnableMask", (long long)act->enableMask() ) );
      values.push_back( VRF_Structure::Value("IsOccludee", act->isOccludee() ) );

      return value;
    }

    VRF_Value export_ActorEventCallback(const ActorEventCallback* cb)
    {
      VRF_Value value;
      if (vlToVRF(cb))
      {
        value.setUID( vlToVRF(cb)->uid().c_str() );
        return value;
      }

      // mic fixme: allow other callbacks to be exported and imported
      if (cb->classType() == DepthSortCallback::Type())
      {
        const DepthSortCallback* dsc = cb->as<DepthSortCallback>();

        value.setStructure( new VRF_Structure("<DepthSortCallback>", generateUID("actorcallbk_")) );
        registerExportedStructure(cb, value.getStructure());
        std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

        if (dsc->sortMode() == SM_SortBackToFront)
          values.push_back( VRF_Structure::Value("SortMode", toIdentifier("SM_SortBackToFront")) );
        else
          values.push_back( VRF_Structure::Value("SortMode", toIdentifier("SM_SortFrontToBack")) );
      }
      else
      {
        // mic fixme: we should not output unknown objects at all
        value.setStructure( new VRF_Structure("<UnknownObjectType>", generateUID("actorcallbk_")) );
        return value;
      }

      return value;
    }

    VRF_Value export_LODEvaluator(const LODEvaluator* lod)
    {
      VRF_Value value;
      if (lod->classType() == DistanceLODEvaluator::Type())
        return export_DistanceLODEvaluator(lod->as<DistanceLODEvaluator>());
      else
      if (lod->classType() == PixelLODEvaluator::Type())
        return export_PixelLODEvaluator(lod->as<PixelLODEvaluator>());
      else
      {
        // mic fixme: we should not output unknown objects at all
        value.setStructure( new VRF_Structure("<UnknownObjectType>", generateUID("lodeval_")) );
        return value;
      }
    }

    VRF_Value export_DistanceLODEvaluator(const DistanceLODEvaluator* lod)
    {
      VL_CHECK(lod);

      VRF_Value value;
      if (vlToVRF(lod))
      {
        value.setUID( vlToVRF(lod)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<DistanceLODEvaluator>", generateUID("lodeval_")) );
      registerExportedStructure(lod, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      VRF_Value distances( new VRF_ArrayReal );
      distances.getArrayReal()->value().resize( lod->distanceRangeSet().size() );
      if (lod->distanceRangeSet().size() != 0)
        distances.getArrayReal()->copyFrom( &lod->distanceRangeSet()[0] );

      values.push_back( VRF_Structure::Value("DistanceRageSet", distances) );

      return value;
    }

    VRF_Value export_PixelLODEvaluator(const PixelLODEvaluator* lod)
    {
      VL_CHECK(lod);

      VRF_Value value;
      if (vlToVRF(lod))
      {
        value.setUID( vlToVRF(lod)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<PixelLODEvaluator>", generateUID("lodeval_")) );
      registerExportedStructure(lod, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      VRF_Value distances( new VRF_ArrayReal );
      distances.getArrayReal()->value().resize( lod->pixelRangeSet().size() );
      if (lod->pixelRangeSet().size() != 0)
        distances.getArrayReal()->copyFrom( &lod->pixelRangeSet()[0] );

      values.push_back( VRF_Structure::Value("PixelRageSet", distances) );

      return value;
    }

    VRF_Value export_Transform(const Transform* tr)
    {
      VL_CHECK(tr);

      VRF_Value value;
      if (vlToVRF(tr))
      {
        value.setUID( vlToVRF(tr)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Transform>", generateUID("transform_")) );
      registerExportedStructure(tr, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("LocalMatrix", export_Matrix(tr->localMatrix()) ) );

      if (tr->parent())
        values.push_back( VRF_Structure::Value("Parent", export_Transform(tr->parent()) ) );

      VRF_Value childs;
      childs.setList( new VRF_List );
      for(size_t i=0; i<tr->childrenCount(); ++i)
        childs.getList()->value().push_back( export_Transform(tr->children()[i].get()) );
      values.push_back( VRF_Structure::Value("Children", childs) );

      return value;
    }

    bool isTranslation(const fmat4& mat)
    {
      fmat4 tmp = mat;
      tmp.setT( fvec3(0,0,0) );
      return tmp.isIdentity();
    }

    bool isScaling(const fmat4& mat)
    {
      fmat4 tmp = mat;
      tmp.e(0,0) = 1;
      tmp.e(1,1) = 1;
      tmp.e(2,2) = 1;
      return tmp.isIdentity();
    }

    VRF_Value export_Matrix(const fmat4& mat)
    {
      VRF_Value matrix_list( new VRF_List );

      if (isTranslation(mat))
      {
        VRF_Value value( new VRF_ArrayReal("<Translation>") );
        value.getArrayReal()->value().resize(3);
        value.getArrayReal()->value()[0] = mat.getT().x();
        value.getArrayReal()->value()[1] = mat.getT().y();
        value.getArrayReal()->value()[2] = mat.getT().z();
        matrix_list.getList()->value().push_back( value );
      }
      else
      if (isScaling(mat))
      {
        VRF_Value value( new VRF_ArrayReal("<Scaling>") );
        value.getArrayReal()->value().resize(3);
        value.getArrayReal()->value()[0] = mat.e(0,0);
        value.getArrayReal()->value()[1] = mat.e(1,1);
        value.getArrayReal()->value()[2] = mat.e(2,2);
        matrix_list.getList()->value().push_back( value );
      }
      else
      {
        VRF_Value value( new VRF_ArrayReal("<Matrix>") );
        value.getArrayReal()->value().resize(4*4);
        // if we transpose this we have to transpose also the uniform matrices
        value.getArrayReal()->copyFrom(mat.ptr());
        matrix_list.getList()->value().push_back( value );
      }

      return matrix_list;
    }

    VRF_Value export_Uniform(const Uniform* uniform)
    {
      VRF_Value value;
      if (vlToVRF(uniform))
      {
        value.setUID( vlToVRF(uniform)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Uniform>", generateUID("uniform_")) );
      registerExportedStructure(uniform, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("Name", toIdentifier(uniform->name())) );
      values.push_back( VRF_Structure::Value("Type", toIdentifier(export_UniformType(uniform->type()))) );
      values.push_back( VRF_Structure::Value("Count", (long long)uniform->count()) );

      const int count = uniform->count();
      ref<VRF_ArrayInteger> arr_int = new VRF_ArrayInteger;
      ref<VRF_ArrayReal> arr_real = new VRF_ArrayReal;

      switch(uniform->type())
      {
      case UT_INT:
        {
          if (count == 1)
            { int val = 0; uniform->getUniform(&val); values.push_back( VRF_Structure::Value("Data", (long long)val) ); break; }
          else
            { arr_int->value().resize(count*1); arr_int->copyFrom( (int*)uniform->rawData() ); break; }
        }
      case UT_INT_VEC2: arr_int->value().resize(count*2); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_INT_VEC3: arr_int->value().resize(count*3); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_INT_VEC4: arr_int->value().resize(count*4); arr_int->copyFrom( (int*)uniform->rawData() ); break;

      case UT_UNSIGNED_INT:
        {
          if (count == 1)
            { unsigned int val = 0; uniform->getUniform(&val); values.push_back( VRF_Structure::Value("Data", (long long)val) ); break; }
          else
            { arr_int->value().resize(count*1); arr_int->copyFrom( (int*)uniform->rawData() ); break; }
        }
      case UT_UNSIGNED_INT_VEC2: arr_int->value().resize(count*2); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_UNSIGNED_INT_VEC3: arr_int->value().resize(count*3); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_UNSIGNED_INT_VEC4: arr_int->value().resize(count*4); arr_int->copyFrom( (int*)uniform->rawData() ); break;

      case UT_FLOAT:
        {
          if (count == 1)
            { float val = 0; uniform->getUniform(&val); values.push_back( VRF_Structure::Value("Data", (double)val) ); break; }
          else
            { arr_real->value().resize(count*1); arr_real->copyFrom( (float*)uniform->rawData() ); break; }
        }
      case UT_FLOAT_VEC2: arr_real->value().resize(count*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_VEC3: arr_real->value().resize(count*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_VEC4: arr_real->value().resize(count*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;

      case UT_FLOAT_MAT2: arr_real->value().resize(count*2*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT3: arr_real->value().resize(count*3*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT4: arr_real->value().resize(count*4*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;

      case UT_FLOAT_MAT2x3: arr_real->value().resize(count*2*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT3x2: arr_real->value().resize(count*3*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT2x4: arr_real->value().resize(count*2*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT4x2: arr_real->value().resize(count*4*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT3x4: arr_real->value().resize(count*3*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT4x3: arr_real->value().resize(count*4*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;

      case UT_DOUBLE:
        {
          if (count == 1)
            { double val = 0; uniform->getUniform(&val); values.push_back( VRF_Structure::Value("Data", (double)val) ); break; }
          else
            { arr_real->value().resize(count*1); arr_real->copyFrom( (double*)uniform->rawData() ); break; }
        }
      case UT_DOUBLE_VEC2: arr_real->value().resize(count*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_VEC3: arr_real->value().resize(count*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_VEC4: arr_real->value().resize(count*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;

      case UT_DOUBLE_MAT2: arr_real->value().resize(count*2*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT3: arr_real->value().resize(count*3*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT4: arr_real->value().resize(count*4*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;

      case UT_DOUBLE_MAT2x3: arr_real->value().resize(count*2*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT3x2: arr_real->value().resize(count*3*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT2x4: arr_real->value().resize(count*2*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT4x2: arr_real->value().resize(count*4*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT3x4: arr_real->value().resize(count*3*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT4x3: arr_real->value().resize(count*4*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;

      case UT_NONE:
        Log::warning("Error exporting uniform : uninitialized uniform.\n");
        break;

      default:
        Log::warning("Error exporting uniform : illegal uniform type.\n");
        break;
      }

      if (!arr_int->value().empty())
        values.push_back( VRF_Structure::Value("Data", arr_int.get()) );
      else
      if (!arr_real->value().empty())
        values.push_back( VRF_Structure::Value("Data", arr_real.get()) );

      return value;
    }

    std::string export_UniformType(EUniformType type)
    {
      switch(type)
      {
        case UT_INT:      return "UT_INT";
        case UT_INT_VEC2: return "UT_INT_VEC2";
        case UT_INT_VEC3: return "UT_INT_VEC3";
        case UT_INT_VEC4: return "UT_INT_VEC4";

        case UT_UNSIGNED_INT:      return "UT_UNSIGNED_INT";
        case UT_UNSIGNED_INT_VEC2: return "UT_UNSIGNED_INT_VEC2";
        case UT_UNSIGNED_INT_VEC3: return "UT_UNSIGNED_INT_VEC3";
        case UT_UNSIGNED_INT_VEC4: return "UT_UNSIGNED_INT_VEC4";

        case UT_FLOAT:      return "UT_FLOAT";
        case UT_FLOAT_VEC2: return "UT_FLOAT_VEC2";
        case UT_FLOAT_VEC3: return "UT_FLOAT_VEC3";
        case UT_FLOAT_VEC4: return "UT_FLOAT_VEC4";

        case UT_FLOAT_MAT2: return "UT_FLOAT_MAT2";
        case UT_FLOAT_MAT3: return "UT_FLOAT_MAT3";
        case UT_FLOAT_MAT4: return "UT_FLOAT_MAT4";

        case UT_FLOAT_MAT2x3: return "UT_FLOAT_MAT2x3";
        case UT_FLOAT_MAT3x2: return "UT_FLOAT_MAT3x2";
        case UT_FLOAT_MAT2x4: return "UT_FLOAT_MAT2x4";
        case UT_FLOAT_MAT4x2: return "UT_FLOAT_MAT4x2";
        case UT_FLOAT_MAT3x4: return "UT_FLOAT_MAT3x4";
        case UT_FLOAT_MAT4x3: return "UT_FLOAT_MAT4x3";

        case UT_DOUBLE:      return "UT_DOUBLE";
        case UT_DOUBLE_VEC2: return "UT_DOUBLE_VEC2";
        case UT_DOUBLE_VEC3: return "UT_DOUBLE_VEC3";
        case UT_DOUBLE_VEC4: return "UT_DOUBLE_VEC4";

        case UT_DOUBLE_MAT2: return "UT_DOUBLE_MAT2";
        case UT_DOUBLE_MAT3: return "UT_DOUBLE_MAT3";
        case UT_DOUBLE_MAT4: return "UT_DOUBLE_MAT4";

        case UT_DOUBLE_MAT2x3: return "UT_DOUBLE_MAT2x3";
        case UT_DOUBLE_MAT3x2: return "UT_DOUBLE_MAT3x2";
        case UT_DOUBLE_MAT2x4: return "UT_DOUBLE_MAT2x4";
        case UT_DOUBLE_MAT4x2: return "UT_DOUBLE_MAT4x2";
        case UT_DOUBLE_MAT3x4: return "UT_DOUBLE_MAT3x4";
        case UT_DOUBLE_MAT4x3: return "UT_DOUBLE_MAT4x3";

        default:
          return "UT_NONE";
      }
    }

    VRF_Value export_Effect(const Effect* fx)
    {
      VRF_Value value;
      if (vlToVRF(fx))
      {
        value.setUID( vlToVRF(fx)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Effect>", generateUID("effect_")) );
      registerExportedStructure(fx, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("RenderRank", (long long)fx->renderRank()) );
      values.push_back( VRF_Structure::Value("EnableMask", (long long)fx->enableMask()) );
      values.push_back( VRF_Structure::Value("ActiveLod", (long long)fx->activeLod()) );

      if (fx->lodEvaluator())
        values.push_back( VRF_Structure::Value("LODEvaluator", export_LODEvaluator(fx->lodEvaluator())) );

      // shaders
      ref<VRF_List> lod_list = new VRF_List;
      for(int i=0; fx->lod(i) && i<VL_MAX_EFFECT_LOD; ++i)
        lod_list->value().push_back( export_ShaderSequence(fx->lod(i).get()) );
      values.push_back( VRF_Structure::Value("Lods", lod_list.get()) );

      return value;
    }

    VRF_Value export_ShaderSequence(const ShaderPasses* sh_seq)
    {
      VRF_Value value;
      if (vlToVRF(sh_seq))
      {
        value.setUID( vlToVRF(sh_seq)->uid().c_str() );
        return value;
      }

      value.setList( new VRF_List("<ShaderPasses>") );
      for(int i=0; i<sh_seq->size(); ++i)
        value.getList()->value().push_back( export_Shader(sh_seq->at(i)) );

      return value;
    }

    VRF_Value export_Shader(const Shader* sh)
    {
      VRF_Value value;
      if (vlToVRF(sh))
      {
        value.setUID( vlToVRF(sh)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Shader>", generateUID("shader_")) );
      registerExportedStructure(sh, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      // uniforms
      VRF_Value uniforms;
      uniforms.setList( new VRF_List );
      if (sh->getUniformSet())
      {
        for(size_t i=0; i<sh->uniforms().size(); ++i)
          uniforms.getList()->value().push_back( export_Uniform(sh->uniforms()[i].get()) );
      }
      values.push_back( VRF_Structure::Value("Uniforms", uniforms) );

      // enables
      ref<VRF_List> enables = new VRF_List;
      if (sh->getEnableSet() )
      {
        for(size_t i=0; i<sh->getEnableSet()->enables().size(); ++i)
          enables->value().push_back( toIdentifier(export_Enable(sh->getEnableSet()->enables()[i])) );
      }
      values.push_back( VRF_Structure::Value("Enables", enables.get()) );

      // renderstates
      VRF_Value renderstates;
      renderstates.setList( new VRF_List );
      if (sh->getRenderStateSet())
      {
        for(size_t i=0; i<sh->getRenderStateSet()->renderStatesCount(); ++i)
        {
          int index = sh->getRenderStateSet()->renderStates()[i].mIndex;
          if (index != -1)
            renderstates.getList()->value().push_back( (long long)index );
          const RenderState* rs = sh->getRenderStateSet()->renderStates()[i].mRS.get();
          renderstates.getList()->value().push_back( export_RenderState(rs) );
        }
      }
      values.push_back( VRF_Structure::Value("RenderStates", renderstates) );

      return value;
    }

    VRF_Value export_RenderState(const RenderState* rs)
    {
      VRF_Value value;
      if (vlToVRF(rs))
      {
        value.setUID( vlToVRF(rs)->uid().c_str() );
        return value;
      }

      if (rs->classType() == Light::Type())
        return export_Light(rs->as<Light>());
      else
      if (rs->classType() == ClipPlane::Type())
        return export_ClipPlane(rs->as<ClipPlane>());
      else
      if (rs->classType() == Material::Type())
        return export_Material(rs->as<Material>());
      else
      if (rs->classType() == GLSLProgram::Type())
        return export_GLSLProgram(rs->as<GLSLProgram>());
      else
      if (rs->classType() == TextureSampler::Type())
        return export_TextureSampler(rs->as<TextureSampler>());
      else
      if (rs->classType() == VertexAttrib::Type())
        return export_VertexAttrib(rs->as<VertexAttrib>());
      else
      if (rs->classType() == Color::Type())
        return export_Color(rs->as<Color>());
      else
      if (rs->classType() == SecondaryColor::Type())
        return export_SecondaryColor(rs->as<SecondaryColor>());
      else
      if (rs->classType() == Normal::Type())
        return export_Normal(rs->as<Normal>());
      else
      {
        VL_TRAP();
        return VRF_Value( new VRF_Structure("<UnknownObjectType>", generateUID("renderstate_")) );
      }
    }

    VRF_Value export_VertexAttrib(const VertexAttrib* vertattrib)
    {
      VRF_Value value;
      if (vlToVRF(vertattrib))
      {
        value.setUID( vlToVRF(vertattrib)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<VertexAttrib>", generateUID("vertattrib_")) );
      registerExportedStructure(vertattrib, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("VertexAttrib", toValue(vertattrib->value()) ) );

      return value;
    }

    VRF_Value export_Color(const Color* color)
    {
      VRF_Value value;
      if (vlToVRF(color))
      {
        value.setUID( vlToVRF(color)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Color>", generateUID("color_")) );
      registerExportedStructure(color, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("Color", toValue(color->color()) ) );

      return value;
    }

    VRF_Value export_SecondaryColor(const SecondaryColor* seccolor)
    {
      VRF_Value value;
      if (vlToVRF(seccolor))
      {
        value.setUID( vlToVRF(seccolor)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<SecondaryColor>", generateUID("seccolor_")) );
      registerExportedStructure(seccolor, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("SecondaryColor", toValue(seccolor->secondaryColor()) ) );

      return value;
    }

    VRF_Value export_Normal(const Normal* normal)
    {
      VRF_Value value;
      if (vlToVRF(normal))
      {
        value.setUID( vlToVRF(normal)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Normal>", generateUID("normal_")) );
      registerExportedStructure(normal, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("Normal", toValue(normal->normal()) ) );

      return value;
    }

    VRF_Value export_TextureSampler(const TextureSampler* tex_sampler)
    {
      VRF_Value value;
      if (vlToVRF(tex_sampler))
      {
        value.setUID( vlToVRF(tex_sampler)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<TextureSampler>", generateUID("texsampler_")) );
      registerExportedStructure(tex_sampler, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      if (tex_sampler->texture())
      {
        values.push_back( VRF_Structure::Value("Texture", export_Texture(tex_sampler->texture()) ) );
      }
      if (tex_sampler->getTexParameter())
        values.push_back( VRF_Structure::Value("TexParameter", export_TexParameter(tex_sampler->getTexParameter()) ) );

      return value;
    }

    VRF_Value export_Texture(const Texture* tex)
    {
      VRF_Value value;
      if (vlToVRF(tex))
      {
        value.setUID( vlToVRF(tex)->uid().c_str() );
        return value;
      }

      std::string name;
      switch(tex->dimension())
      {
      case TD_TEXTURE_1D: name = "<Texture1D>"; break;
      default:
      case TD_TEXTURE_2D: name = "<Texture2D>"; break;
      case TD_TEXTURE_3D: name = "<Texture3D>"; break;
      case TD_TEXTURE_CUBE_MAP: name = "<TextureCubeMap>"; break;
      case TD_TEXTURE_RECTANGLE: name = "<TextureRectangle>"; break;
      case TD_TEXTURE_1D_ARRAY: name = "<Texture1DArray>"; break;
      case TD_TEXTURE_2D_ARRAY: name = "<Texture2DArray>"; break;
      case TD_TEXTURE_BUFFER: name = "<TextureBuffer>"; break;
      case TD_TEXTURE_2D_MULTISAMPLE: name = "<Texture2DMultisample>"; break;
      case TD_TEXTURE_2D_MULTISAMPLE_ARRAY: name = "<Texture2DMultisampleArray>"; break;
      }

      value.setStructure( new VRF_Structure(name.c_str(), generateUID("texture_")) );
      registerExportedStructure(tex, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      if (tex->getTexParameter())
        values.push_back( VRF_Structure::Value("TexParameter", export_TexParameter(tex->getTexParameter()) ) );

      if (tex->setupParams())
      {
        const Texture::SetupParams* par = tex->setupParams();

        if (!par->imagePath().empty())
          values.push_back( VRF_Structure::Value("ImagePath", toString(par->imagePath().toStdString()) ) );
        else
        if (par->image())
          values.push_back( VRF_Structure::Value("ImagePath", toString(par->image()->filePath().toStdString()) ) );

        values.push_back( VRF_Structure::Value("Format", toIdentifier(export_TextureFormat(par->format())) ) );

        if (par->width())
          values.push_back( VRF_Structure::Value("Width", (long long)par->width() ) );

        if (par->height())
          values.push_back( VRF_Structure::Value("Height", (long long)par->height() ) );

        if (par->depth())
          values.push_back( VRF_Structure::Value("Depth", (long long)par->depth() ) );

        values.push_back( VRF_Structure::Value("GenMipmaps", par->genMipmaps() ) );

        if(par->samples())
        {
          values.push_back( VRF_Structure::Value("Samples", (long long)par->samples() ) );
          values.push_back( VRF_Structure::Value("FixedSamplesLocation", par->fixedSamplesLocations() ) );
        }
      }


      return value;
    }

    std::string export_TextureFormat(ETextureFormat tf)
    {
      switch(tf)
      {
      default:
      case TF_UNKNOWN: return "TF_UNKNOWN";

      case TF_ALPHA  : return "ALPHA";
      case TF_ALPHA4 : return "ALPHA4";
      case TF_ALPHA8 : return "ALPHA8";
      case TF_ALPHA12: return "ALPHA12";
      case TF_ALPHA16: return "ALPHA16";

      case TF_INTENSITY  : return "INTENSITY";
      case TF_INTENSITY4 : return "INTENSITY4";
      case TF_INTENSITY8 : return "INTENSITY8";
      case TF_INTENSITY12: return "INTENSITY12";
      case TF_INTENSITY16: return "INTENSITY16";
      case TF_LUMINANCE  : return "LUMINANCE";
      case TF_LUMINANCE4 : return "LUMINANCE4";
      case TF_LUMINANCE8 : return "LUMINANCE8";
      case TF_LUMINANCE12: return "LUMINANCE12";
      case TF_LUMINANCE16: return "LUMINANCE16";
      case TF_LUMINANCE_ALPHA    : return "LUMINANCE_ALPHA";
      case TF_LUMINANCE4_ALPHA4  : return "LUMINANCE4_ALPHA4";
      case TF_LUMINANCE6_ALPHA2  : return "LUMINANCE6_ALPHA2";
      case TF_LUMINANCE8_ALPHA8  : return "LUMINANCE8_ALPHA8";
      case TF_LUMINANCE12_ALPHA4 : return "LUMINANCE12_ALPHA4";
      case TF_LUMINANCE12_ALPHA12: return "LUMINANCE12_ALPHA12";
      case TF_LUMINANCE16_ALPHA16: return "LUMINANCE16_ALPHA16";
      case TF_R3_G3_B2: return "R3_G3_B2";
      case TF_RGB     : return "RGB";
      case TF_RGB4    : return "RGB4";
      case TF_RGB5    : return "RGB5";
      case TF_RGB8    : return "RGB8";
      case TF_RGB10   : return "RGB10";
      case TF_RGB12   : return "RGB12";
      case TF_RGB16   : return "RGB16";
      case TF_RGBA    : return "RGBA";
      case TF_RGBA2   : return "RGBA2";
      case TF_RGBA4   : return "RGBA4";
      case TF_RGB5_A1 : return "RGB5_A1";
      case TF_RGBA8   : return "RGBA8";
      case TF_RGB10_A2: return "RGB10_A2";
      case TF_RGBA12  : return "RGBA12";
      case TF_RGBA16  : return "RGBA16";

      // ARB_texture_float / OpenGL 3
      case TF_RGBA32F: return "RGBA32F";
      case TF_RGB32F: return "RGB32F";
      case TF_ALPHA32F: return "ALPHA32F";
      case TF_INTENSITY32F: return "INTENSITY32F";
      case TF_LUMINANCE32F: return "LUMINANCE32F";
      case TF_LUMINANCE_ALPHA32F: return "LUMINANCE_ALPHA32F";
      case TF_RGBA16F: return "RGBA16F";
      case TF_RGB16F: return "RGB16F";
      case TF_ALPHA16F: return "ALPHA16F";
      case TF_INTENSITY16F: return "INTENSITY16F";
      case TF_LUMINANCE16F: return "LUMINANCE16F";
      case TF_LUMINANCE_ALPHA16F: return "LUMINANCE_ALPHA16F";

      // from table 3.12 opengl api specs 4.1
      case TF_R8_SNORM: return "R8_SNORM";
      case TF_R16_SNORM: return "R16_SNORM";
      case TF_RG8_SNORM: return "RG8_SNORM";
      case TF_RG16_SNORM: return "RG16_SNORM";
      case TF_RGB8_SNORM: return "RGB8_SNORM";
      case TF_RGBA8_SNORM: return "RGBA8_SNORM";
      case TF_RGB10_A2UI: return "RGB10_A2UI";
      case TF_RGBA16_SNORM: return "RGBA16_SNORM";
      case TF_R11F_G11F_B10F: return "R11F_G11F_B10F";
      case TF_RGB9_E5: return "RGB9_E5";
      case TF_RGB8I: return "RGB8I";
      case TF_RGB8UI: return "RGB8UI";
      case TF_RGB16I: return "RGB16I";
      case TF_RGB16UI: return "RGB16UI";
      case TF_RGB32I: return "RGB32I";
      case TF_RGB32UI: return "RGB32UI";
      case TF_RGBA8I: return "RGBA8I";
      case TF_RGBA8UI: return "RGBA8UI";
      case TF_RGBA16I: return "RGBA16I";
      case TF_RGBA16UI: return "RGBA16UI";
      case TF_RGBA32I: return "RGBA32I";
      case TF_RGBA32UI: return "TF_RGBA32UI";

      // ATI_texture_float (the enums are the same as ARB_texture_float)
      //case TF_RGBA_FLOAT32_ATI: return "RGBA_FLOAT32_ATI";
      //case TF_RGB_FLOAT32_ATI: return "RGB_FLOAT32_ATI";
      //case TF_ALPHA_FLOAT32_ATI: return "ALPHA_FLOAT32_ATI";
      //case TF_INTENSITY_FLOAT32_ATI: return "INTENSITY_FLOAT32_ATI";
      //case TF_LUMINANCE_FLOAT32_ATI: return "LUMINANCE_FLOAT32_ATI";
      //case TF_LUMINANCE_ALPHA_FLOAT32_ATI: return "LUMINANCE_ALPHA_FLOAT32_ATI";
      //case TF_RGBA_FLOAT16_ATI: return "RGBA_FLOAT16_ATI";
      //case TF_RGB_FLOAT16_ATI: return "RGB_FLOAT16_ATI";
      //case TF_ALPHA_FLOAT16_ATI: return "ALPHA_FLOAT16_ATI";
      //case TF_INTENSITY_FLOAT16_ATI: return "INTENSITY_FLOAT16_ATI";
      //case TF_LUMINANCE_FLOAT16_ATI: return "LUMINANCE_FLOAT16_ATI";
      //case TF_LUMINANCE_ALPHA_FLOAT16_ATI: return "LUMINANCE_ALPHA_FLOAT16_ATI";

      // EXT_texture_shared_exponent
      // case TF_RGB9_E5_EXT: return "RGB9_E5_EXT";

      // EXT_packed_float
      // case TF_11F_G11F_B10F_EXT: return "11F_G11F_B10F_EXT";

      // EXT_packed_depth_stencil / GL_ARB_framebuffer_object
      case TF_DEPTH_STENCIL   : return "DEPTH_STENCIL";
      case TF_DEPTH24_STENCIL8: return "DEPTH24_STENCIL8";

      // ARB_depth_buffer_float
      case TF_DEPTH_COMPONENT32F: return "DEPTH_COMPONENT32F";
      case TF_DEPTH32F_STENCIL8 : return "DEPTH32F_STENCIL8";

      // ARB_depth_texture
      case TF_DEPTH_COMPONENT  : return "DEPTH_COMPONENT";
      case TF_DEPTH_COMPONENT16: return "DEPTH_COMPONENT16";
      case TF_DEPTH_COMPONENT24: return "DEPTH_COMPONENT24";
      case TF_DEPTH_COMPONENT32: return "DEPTH_COMPONENT32";

      // ARB_texture_compression
      case TF_COMPRESSED_ALPHA          : return "COMPRESSED_ALPHA";
      case TF_COMPRESSED_INTENSITY      : return "COMPRESSED_INTENSITY";
      case TF_COMPRESSED_LUMINANCE      : return "COMPRESSED_LUMINANCE";
      case TF_COMPRESSED_LUMINANCE_ALPHA: return "COMPRESSED_LUMINANCE_ALPHA";
      case TF_COMPRESSED_RGB            : return "COMPRESSED_RGB";
      case TF_COMPRESSED_RGBA           : return "COMPRESSED_RGBA";

      // 3DFX_texture_compression_FXT1
      case TF_COMPRESSED_RGB_FXT1_3DFX : return "COMPRESSED_RGB_FXT1_3DFX";
      case TF_COMPRESSED_RGBA_FXT1_3DFX: return "COMPRESSED_RGBA_FXT1_3DFX";

      // EXT_texture_compression_s3tc
      case TF_COMPRESSED_RGB_S3TC_DXT1_EXT : return "COMPRESSED_RGB_S3TC_DXT1_EXT";
      case TF_COMPRESSED_RGBA_S3TC_DXT1_EXT: return "COMPRESSED_RGBA_S3TC_DXT1_EXT";
      case TF_COMPRESSED_RGBA_S3TC_DXT3_EXT: return "COMPRESSED_RGBA_S3TC_DXT3_EXT";
      case TF_COMPRESSED_RGBA_S3TC_DXT5_EXT: return "COMPRESSED_RGBA_S3TC_DXT5_EXT";

      // EXT_texture_compression_latc
      case TF_COMPRESSED_LUMINANCE_LATC1_EXT             : return "COMPRESSED_LUMINANCE_LATC1_EXT";
      case TF_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT      : return "COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT";
      case TF_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT       : return "COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT";
      case TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT: return "COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT";

      // EXT_texture_compression_rgtc
      case TF_COMPRESSED_RED_RGTC1_EXT             : return "COMPRESSED_RED_RGTC1_EXT";
      case TF_COMPRESSED_SIGNED_RED_RGTC1_EXT      : return "COMPRESSED_SIGNED_RED_RGTC1_EXT";
      case TF_COMPRESSED_RED_GREEN_RGTC2_EXT       : return "COMPRESSED_RED_GREEN_RGTC2_EXT";
      case TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT: return "COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT";

      // EXT_texture_integer
      // case TF_RGBA32UI_EXT: return "RGBA32UI_EXT";
      // case TF_RGB32UI_EXT: return "RGB32UI_EXT";
      case TF_ALPHA32UI_EXT: return "ALPHA32UI_EXT";
      case TF_INTENSITY32UI_EXT: return "INTENSITY32UI_EXT";
      case TF_LUMINANCE32UI_EXT: return "LUMINANCE32UI_EXT";
      case TF_LUMINANCE_ALPHA32UI_EXT: return "LUMINANCE_ALPHA32UI_EXT";

      // case TF_RGBA16UI_EXT: return "RGBA16UI_EXT";
      // case TF_RGB16UI_EXT: return "RGB16UI_EXT";
      case TF_ALPHA16UI_EXT: return "ALPHA16UI_EXT";
      case TF_INTENSITY16UI_EXT: return "INTENSITY16UI_EXT";
      case TF_LUMINANCE16UI_EXT: return "LUMINANCE16UI_EXT";
      case TF_LUMINANCE_ALPHA16UI_EXT: return "LUMINANCE_ALPHA16UI_EXT";

      // case TF_RGBA8UI_EXT: return "RGBA8UI_EXT";
      // case TF_RGB8UI_EXT: return "RGB8UI_EXT";
      case TF_ALPHA8UI_EXT: return "ALPHA8UI_EXT";
      case TF_INTENSITY8UI_EXT: return "INTENSITY8UI_EXT";
      case TF_LUMINANCE8UI_EXT: return "LUMINANCE8UI_EXT";
      case TF_LUMINANCE_ALPHA8UI_EXT: return "LUMINANCE_ALPHA8UI_EXT";

      // case TF_RGBA32I_EXT: return "RGBA32I_EXT";
      // case TF_RGB32I_EXT: return "RGB32I_EXT";
      case TF_ALPHA32I_EXT: return "ALPHA32I_EXT";
      case TF_INTENSITY32I_EXT: return "INTENSITY32I_EXT";
      case TF_LUMINANCE32I_EXT: return "LUMINANCE32I_EXT";
      case TF_LUMINANCE_ALPHA32I_EXT: return "LUMINANCE_ALPHA32I_EXT";

      // case TF_RGBA16I_EXT: return "RGBA16I_EXT";
      // case TF_RGB16I_EXT: return "RGB16I_EXT";
      case TF_ALPHA16I_EXT: return "ALPHA16I_EXT";
      case TF_INTENSITY16I_EXT: return "INTENSITY16I_EXT";
      case TF_LUMINANCE16I_EXT: return "LUMINANCE16I_EXT";
      case TF_LUMINANCE_ALPHA16I_EXT: return "LUMINANCE_ALPHA16I_EXT";

      // case TF_RGBA8I_EXT: return "RGBA8I_EXT";
      // case TF_RGB8I_EXT: return "RGB8I_EXT";
      case TF_ALPHA8I_EXT: return "ALPHA8I_EXT";
      case TF_INTENSITY8I_EXT: return "INTENSITY8I_EXT";
      case TF_LUMINANCE8I_EXT: return "LUMINANCE8I_EXT";
      case TF_LUMINANCE_ALPHA8I_EXT: return "LUMINANCE_ALPHA8I_EXT";

      // GL_ARB_texture_rg
      case TF_RED: return "RED";
      case TF_COMPRESSED_RED: return "COMPRESSED_RED";
      case TF_COMPRESSED_RG: return "COMPRESSED_RG";
      case TF_RG: return "RG";
      case TF_R8: return "R8";
      case TF_R16: return "R16";
      case TF_RG8: return "RG8";
      case TF_RG16: return "RG16";
      case TF_R16F: return "R16F";
      case TF_R32F: return "R32F";
      case TF_RG16F: return "RG16F";
      case TF_RG32F: return "RG32F";
      case TF_R8I: return "R8I";
      case TF_R8UI: return "R8UI";
      case TF_R16I: return "R16I";
      case TF_R16UI: return "R16UI";
      case TF_R32I: return "R32I";
      case TF_R32UI: return "R32UI";
      case TF_RG8I: return "RG8I";
      case TF_RG8UI: return "RG8UI";
      case TF_RG16I: return "RG16I";
      case TF_RG16UI: return "RG16UI";
      case TF_RG32I: return "RG32I";
      case TF_RG32UI: return "RG32UI";

      // sRGB OpenGL 2.1
      case TF_SLUMINANCE_ALPHA: return "SLUMINANCE_ALPHA";
      case TF_SLUMINANCE8_ALPHA8: return "SLUMINANCE8_ALPHA8";
      case TF_SLUMINANCE: return "SLUMINANCE";
      case TF_SLUMINANCE8: return "SLUMINANCE8";
      case TF_COMPRESSED_SLUMINANCE: return "COMPRESSED_SLUMINANCE";
      case TF_COMPRESSED_SLUMINANCE_ALPHA: return "COMPRESSED_SLUMINANCE_ALPHA";

      // sRGB OpenGL 2.1 / 3.x
      case TF_SRGB: return "SRGB";
      case TF_SRGB8: return "SRGB8";
      case TF_SRGB_ALPHA: return "SRGB_ALPHA";
      case TF_SRGB8_ALPHA8: return "SRGB8_ALPHA8";
      case TF_COMPRESSED_SRGB: return "COMPRESSED_SRGB";
      case TF_COMPRESSED_SRGB_ALPHA: return "COMPRESSED_SRGB_ALPHA";

      // GL_EXT_texture_sRGB compressed formats
      case TF_COMPRESSED_SRGB_S3TC_DXT1_EXT: return "COMPRESSED_SRGB_S3TC_DXT1_EXT";
      case TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT: return "COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
      case TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT: return "COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
      case TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT: return "COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
      }
    }

    VRF_Value export_TexParameter(const TexParameter* texparam)
    {
      VRF_Value value;
      if (vlToVRF(texparam))
      {
        value.setUID( vlToVRF(texparam)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<TexParameter>", generateUID("texparam_")) );
      registerExportedStructure(texparam, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("MinFilter", toIdentifier(export_TexParamFilter(texparam->minFilter())) ) );
      values.push_back( VRF_Structure::Value("MagFilter", toIdentifier(export_TexParamFilter(texparam->magFilter())) ) );
      values.push_back( VRF_Structure::Value("WrapS", toIdentifier(export_TexParamWrap(texparam->wrapS())) ) );
      values.push_back( VRF_Structure::Value("WrapT", toIdentifier(export_TexParamWrap(texparam->wrapT())) ) );
      values.push_back( VRF_Structure::Value("WrapR", toIdentifier(export_TexParamWrap(texparam->wrapR())) ) );
      values.push_back( VRF_Structure::Value("CompareMode", toIdentifier(export_TexCompareMode(texparam->compareMode())) ) );
      values.push_back( VRF_Structure::Value("CompareFunc", toIdentifier(export_TexCompareFunc(texparam->compareFunc())) ) );
      values.push_back( VRF_Structure::Value("DepthTextureMode", toIdentifier(export_DepthTextureMode(texparam->depthTextureMode())) ) );
      values.push_back( VRF_Structure::Value("BorderColor", toValue(texparam->borderColor()) ) );
      values.push_back( VRF_Structure::Value("Anisotropy", texparam->anisotropy() ) );
      values.push_back( VRF_Structure::Value("GenerateMipmap", texparam->generateMipmap() ) );

      return value;
    }

    std::string export_DepthTextureMode(EDepthTextureMode dtm)
    {
      switch(dtm)
      {
      case DTM_LUMINANCE: return "DTM_LUMINANCE";
      case DTM_INTENSITY: return "DTM_INTENSITY";
      case DTM_ALPHA: return "DTM_ALPHA";
      case DTM_RED:
      default:
        return "DTM_RED";
      }
    }

    std::string export_TexCompareMode(ETexCompareMode tcm)
    {
      switch(tcm)
      {
      case TCM_NONE: return "TCM_NONE";
      // case TCM_COMPARE_R_TO_TEXTURE: return "TCM_COMPARE_R_TO_TEXTURE";
      default:
      case TCM_COMPARE_REF_DEPTH_TO_TEXTURE: 
        return "TCM_COMPARE_REF_DEPTH_TO_TEXTURE";
      }
    }

    std::string export_TexCompareFunc(ETexCompareFunc tcf)
    {
      switch(tcf)
      {
      case TCF_LEQUAL: return "TCF_LEQUAL";
      case TCF_GEQUAL: return "TCF_GEQUAL";
      case TCF_LESS: return "TCF_LESS";
      case TCF_GREATER: return "TCF_GREATER";
      case TCF_EQUAL: return "TCF_EQUAL";
      case TCF_NOTEQUAL: return "TCF_NOTEQUAL";
      case TCF_ALWAYS: return "TCF_ALWAYS";
      default:
      case TCF_NEVER: 
        return "TCF_NEVER";
      }
    }

    std::string export_TexParamFilter(ETexParamFilter tpf)
    {
      switch(tpf)
      {
      case TPF_NEAREST: return "TPF_NEAREST";
      case TPF_LINEAR: return "TPF_LINEAR";
      case TPF_NEAREST_MIPMAP_NEAREST: return "TPF_NEAREST_MIPMAP_NEAREST";
      case TPF_LINEAR_MIPMAP_NEAREST: return "TPF_LINEAR_MIPMAP_NEAREST";
      case TPF_NEAREST_MIPMAP_LINEAR: return "TPF_NEAREST_MIPMAP_LINEAR";
      case TPF_LINEAR_MIPMAP_LINEAR: 
      default:
        return "TPF_LINEAR_MIPMAP_LINEAR";
      }
    }

    std::string export_TexParamWrap(ETexParamWrap tpw)
    {
      switch(tpw)
      {
      case TPW_CLAMP: return "TPW_CLAMP";
      case TPW_CLAMP_TO_BORDER: return "TPW_CLAMP_TO_BORDER";
      case TPW_CLAMP_TO_EDGE: return "TPW_CLAMP_TO_EDGE";
      case TPW_MIRRORED_REPEAT: return "TPW_MIRRORED_REPEAT";
      case TPW_REPEAT: 
      default:
        return "TPW_REPEAT";
      }
    }

    VRF_Value export_Light(const Light* light)
    {
      VRF_Value value;
      if (vlToVRF(light))
      {
        value.setUID( vlToVRF(light)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Light>", generateUID("light_")) );
      registerExportedStructure(light, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("Ambient", toValue(light->ambient()) ) );
      values.push_back( VRF_Structure::Value("Diffuse", toValue(light->diffuse()) ) );
      values.push_back( VRF_Structure::Value("Specular", toValue(light->specular()) ) );
      values.push_back( VRF_Structure::Value("Position", toValue(light->position()) ) );
      values.push_back( VRF_Structure::Value("SpotDirection", toValue(light->spotDirection()) ) );
      values.push_back( VRF_Structure::Value("SpotExponent", (double)light->spotExponent() ) );
      values.push_back( VRF_Structure::Value("SpotCutoff", (double)light->spotCutoff() ) );
      values.push_back( VRF_Structure::Value("ConstantAttenuation", (double)light->constantAttenuation() ) );
      values.push_back( VRF_Structure::Value("LinearAttenuation", (double)light->linearAttenuation() ) );
      values.push_back( VRF_Structure::Value("QuadraticAttenuation", (double)light->quadraticAttenuation() ) );
      if (light->boundTransform())
        values.push_back( VRF_Structure::Value("BoundTransform", export_Transform(light->boundTransform()) ) );

      return value;
    }

    VRF_Value export_ClipPlane(const ClipPlane* clip)
    {
      VRF_Value value;
      if (vlToVRF(clip))
      {
        value.setUID( vlToVRF(clip)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<ClipPlane>", generateUID("clip_")) );
      registerExportedStructure(clip, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("PlaneNormal", toValue(clip->plane().normal()) ) );
      values.push_back( VRF_Structure::Value("PlaneOrigin", (double)(clip->plane().origin()) ) );
      if (clip->boundTransform())
        values.push_back( VRF_Structure::Value("BoundTransform", export_Transform(clip->boundTransform()) ) );

      return value;
    }

    VRF_Value export_Material(const Material* mat)
    {
      VRF_Value value;
      if (vlToVRF(mat))
      {
        value.setUID( vlToVRF(mat)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Material>", generateUID("material_")) );
      registerExportedStructure(mat, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("FrontAmbient", toValue(mat->frontAmbient()) ) );
      values.push_back( VRF_Structure::Value("FrontDiffuse", toValue(mat->frontDiffuse()) ) );
      values.push_back( VRF_Structure::Value("FrontEmission", toValue(mat->frontEmission()) ) );
      values.push_back( VRF_Structure::Value("FrontSpecular", toValue(mat->frontSpecular()) ) );
      values.push_back( VRF_Structure::Value("FrontShininess", (double)mat->frontShininess() ) );

      values.push_back( VRF_Structure::Value("BackAmbient", toValue(mat->backAmbient()) ) );
      values.push_back( VRF_Structure::Value("BackDiffuse", toValue(mat->backDiffuse()) ) );
      values.push_back( VRF_Structure::Value("BackEmission", toValue(mat->backEmission()) ) );
      values.push_back( VRF_Structure::Value("BackSpecular", toValue(mat->backSpecular()) ) );
      values.push_back( VRF_Structure::Value("BackShininess", (double)mat->backShininess() ) );

      values.push_back( VRF_Structure::Value("ColorMaterial", toIdentifier(export_ColorMaterial(mat->colorMaterial())) ) );
      values.push_back( VRF_Structure::Value("ColorMaterialFace", toIdentifier(export_PolygonFace(mat->colorMaterialFace())) ) );

      values.push_back( VRF_Structure::Value("ColorMaterialEnabled", mat->colorMaterialEnabled() ) );

      return value;
    }

    std::string export_PolygonFace(EPolygonFace pf)
    {
      switch(pf)
      {
      case PF_FRONT: return "PF_FRONT";
      case PF_BACK:  return "PF_BACK";
      case PF_FRONT_AND_BACK: 
      default:
        return "PF_FRONT_AND_BACK";
      }
    }

    std::string export_ColorMaterial(EColorMaterial cm)
    {
      switch(cm)
      {
      case CM_EMISSION: return "CM_EMISSION";
      case CM_AMBIENT: return "CM_AMBIENT";
      case CM_DIFFUSE: return "CM_DIFFUSE";
      case CM_SPECULAR: return "CM_SPECULAR";
      case CM_AMBIENT_AND_DIFFUSE:
      default:
        return "CM_AMBIENT_AND_DIFFUSE";
      }
    }

    VRF_Value export_GLSLProgram(const GLSLProgram* glsl)
    {
      VRF_Value value;
      if (vlToVRF(glsl))
      {
        value.setUID( vlToVRF(glsl)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<GLSLProgram>", generateUID("glsl_")) );
      registerExportedStructure(glsl, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      // export glsl shaders
      for(int i=0; i<glsl->shaderCount(); ++i)
        values.push_back( VRF_Structure::Value("AttachShader", export_GLSLShader(glsl->shader(i)) ) );

      // export uniforms
      VRF_Value uniforms;
      uniforms.setList( new VRF_List );
      for(size_t i=0; glsl->getUniformSet() && i<glsl->getUniformSet()->uniforms().size(); ++i)
        uniforms.getList()->value().push_back( export_Uniform(glsl->getUniformSet()->uniforms()[i].get()) );
      values.push_back( VRF_Structure::Value("Uniforms", uniforms) );

      // frag data location
      VRF_Value frag_data_locations;
      frag_data_locations.setList( new VRF_List );
      for(std::map<std::string, int>::const_iterator it = glsl->fragDataLocations().begin(); it != glsl->fragDataLocations().end(); ++it)
      {
        VRF_Value location( new VRF_Structure("<FragDataLocation>") );
        location.getStructure()->value().push_back( VRF_Structure::Value("Name", toIdentifier(it->first) ) );
        location.getStructure()->value().push_back( VRF_Structure::Value("Location", (long long)it->second ) );
        frag_data_locations.getList()->value().push_back( location );
      }
      values.push_back( VRF_Structure::Value("FragDataLocations", frag_data_locations) );

      // auto attrib locations
      VRF_Value attrib_locations;
      attrib_locations.setList( new VRF_List );
      for(std::map<std::string, int>::const_iterator it = glsl->autoAttribLocations().begin(); it != glsl->autoAttribLocations().end(); ++it)
      {
        VRF_Value location( new VRF_Structure("<AttribLocation>") );
        location.getStructure()->value().push_back( VRF_Structure::Value("Name", toIdentifier(it->first) ) );
        location.getStructure()->value().push_back( VRF_Structure::Value("Location", (long long)it->second ) );
        attrib_locations.getList()->value().push_back( location );
      }
      values.push_back( VRF_Structure::Value("AttribLocations", attrib_locations) );



      return value;
    }

    VRF_Value export_GLSLShader(const GLSLShader* glslsh)
    {
      VRF_Value value;
      if (vlToVRF(glslsh))
      {
        value.setUID( vlToVRF(glslsh)->uid().c_str() );
        return value;
      }

      std::string name = "<GLSLShaderError>";
      if (glslsh->type() == vl::ST_VERTEX_SHADER)
        name = "<VertexShader>";
      else
      if (glslsh->type() == vl::ST_FRAGMENT_SHADER)
        name = "<FragmentShader>";
      else
      if (glslsh->type() == vl::ST_GEOMETRY_SHADER)
        name = "<GeometryShader>";
      else
      if (glslsh->type() == vl::ST_TESS_CONTROL_SHADER)
        name = "<TessControlShader>";
      else
      if (glslsh->type() == vl::ST_TESS_EVALUATION_SHADER)
        name = "<TessEvaluationShader>";

      value.setStructure( new VRF_Structure(name.c_str(), generateUID("glslsh_")) );
      registerExportedStructure(glslsh, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      if (!glslsh->path().empty())
        values.push_back( VRF_Structure::Value("Path", toString(glslsh->path()) ) );
      else
      if (!glslsh->source().empty())
        values.push_back( VRF_Structure::Value("Source", toRawtext(glslsh->source()) ) );
      else
      if (glslsh->handle())
        values.push_back( VRF_Structure::Value("Source", toRawtext(glslsh->getShaderSource()) ) );
      else
        values.push_back( VRF_Structure::Value("Source", toIdentifier("NO_SOURCE_FOUND") ) );

      return value;
    }

    std::string export_Enable(EEnable en)
    {
      switch(en)
      {
      case EN_BLEND: return "EN_BLEND";
      case EN_CULL_FACE: return "EN_CULL_FACE";
      case EN_DEPTH_TEST: return "EN_DEPTH_TEST";
      case EN_STENCIL_TEST: return "EN_STENCIL_TEST";
      case EN_DITHER: return "EN_DITHER";
      case EN_POLYGON_OFFSET_FILL: return "EN_POLYGON_OFFSET_FILL";
      case EN_POLYGON_OFFSET_LINE: return "EN_POLYGON_OFFSET_LINE";
      case EN_POLYGON_OFFSET_POINT: return "EN_POLYGON_OFFSET_POINT";
      case EN_COLOR_LOGIC_OP: return "EN_COLOR_LOGIC_OP";
      case EN_MULTISAMPLE: return "EN_MULTISAMPLE";
      case EN_POINT_SMOOTH: return "EN_POINT_SMOOTH";
      case EN_LINE_SMOOTH: return "EN_LINE_SMOOTH";
      case EN_POLYGON_SMOOTH: return "EN_POLYGON_SMOOTH";
      case EN_LINE_STIPPLE: return "EN_LINE_STIPPLE";
      case EN_POLYGON_STIPPLE: return "EN_POLYGON_STIPPLE";
      case EN_POINT_SPRITE: return "EN_POINT_SPRITE";
      case EN_PROGRAM_POINT_SIZE: return "EN_PROGRAM_POINT_SIZE";
      case EN_ALPHA_TEST: return "EN_ALPHA_TEST";
      case EN_LIGHTING: return "EN_LIGHTING";
      case EN_COLOR_SUM: return "EN_COLOR_SUM";
      case EN_FOG: return "EN_FOG";
      case EN_NORMALIZE: return "EN_NORMALIZE";
      case EN_RESCALE_NORMAL: return "EN_RESCALE_NORMAL";
      case EN_VERTEX_PROGRAM_TWO_SIDE: return "EN_VERTEX_PROGRAM_TWO_SIDE";
      case EN_TEXTURE_CUBE_MAP_SEAMLESS: return "EN_TEXTURE_CUBE_MAP_SEAMLESS";
      case EN_CLIP_DISTANCE0: return "EN_CLIP_DISTANCE0";
      case EN_CLIP_DISTANCE1: return "EN_CLIP_DISTANCE1";
      case EN_CLIP_DISTANCE2: return "EN_CLIP_DISTANCE2";
      case EN_CLIP_DISTANCE3: return "EN_CLIP_DISTANCE3";
      case EN_CLIP_DISTANCE4: return "EN_CLIP_DISTANCE4";
      case EN_CLIP_DISTANCE5: return "EN_CLIP_DISTANCE5";
      case EN_CLIP_DISTANCE6: return "EN_CLIP_DISTANCE6";
      case EN_CLIP_DISTANCE7: return "EN_CLIP_DISTANCE7";
      case EN_SAMPLE_ALPHA_TO_COVERAGE: return "EN_SAMPLE_ALPHA_TO_COVERAGE";
      case EN_SAMPLE_ALPHA_TO_ONE: return "EN_SAMPLE_ALPHA_TO_ONE";
      case EN_SAMPLE_COVERAGE: return "EN_SAMPLE_COVERAGE";
      default:
        return "EN_UnknownEnable";
      }
    }

    VRF_Value export_Camera(const Camera* cam)
    {
      VRF_Value value;
      if (vlToVRF(cam))
      {
        value.setUID( vlToVRF(cam)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Camera>", generateUID("camera_")) );
      registerExportedStructure(cam, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("ViewMatrix", toValue(cam->viewMatrix()) ) );
      values.push_back( VRF_Structure::Value("ProjectionMatrix", toValue(cam->projectionMatrix()) ) );
      values.push_back( VRF_Structure::Value("Viewport", export_Viewport(cam->viewport()) ) );
      if (cam->boundTransform())
        values.push_back( VRF_Structure::Value("BoundTransfrm", export_Transform(cam->boundTransform()) ) );
      values.push_back( VRF_Structure::Value("FOV", (double)cam->fov() ) );
      values.push_back( VRF_Structure::Value("Left", (double)cam->left() ) );
      values.push_back( VRF_Structure::Value("Right", (double)cam->right() ) );
      values.push_back( VRF_Structure::Value("Bottom", (double)cam->bottom() ) );
      values.push_back( VRF_Structure::Value("Top", (double)cam->top() ) );
      values.push_back( VRF_Structure::Value("NearPlane", (double)cam->nearPlane() ) );
      values.push_back( VRF_Structure::Value("FarPlane", (double)cam->farPlane() ) );
      values.push_back( VRF_Structure::Value("ProjectionType", toIdentifier(export_ProjectionType(cam->projectionType())) ) );

      return value;
    }

    std::string export_ProjectionType(EProjectionType pt)
    {
      switch(pt)
      {
      case PMT_UserProjection: return "PMT_UserProjection";
      case PMT_OrthographicProjection: return "PMT_OrthographicProjection";
      case PMT_PerspectiveProjection: return "PMT_PerspectiveProjection";
      case PMT_PerspectiveProjectionFrustum: 
      default:
        return "PMT_PerspectiveProjectionFrustum";
      }
    }

    VRF_Value export_Viewport(const Viewport* viewp)
    {
      VRF_Value value;
      if (vlToVRF(viewp))
      {
        value.setUID( vlToVRF(viewp)->uid().c_str() );
        return value;
      }

      value.setStructure( new VRF_Structure("<Viewport>", generateUID("viewp_")) );
      registerExportedStructure(viewp, value.getStructure());
      std::vector<VRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( VRF_Structure::Value("ClearColor", toValue(viewp->clearColor()) ) );
      values.push_back( VRF_Structure::Value("ClearColorInt", toValue(viewp->clearColorInt()) ) );
      values.push_back( VRF_Structure::Value("ClearColorUInt", toValue(viewp->clearColorUInt()) ) );
      values.push_back( VRF_Structure::Value("ClearDepth", (double)viewp->clearDepth() ) );
      values.push_back( VRF_Structure::Value("ClearStecil", (long long)viewp->clearStencil() ) );
      values.push_back( VRF_Structure::Value("ClearColorMode", toIdentifier(export_ClearColorMode(viewp->clearColorMode())) ) );
      values.push_back( VRF_Structure::Value("ClearFlags", toIdentifier(export_ClearFlags(viewp->clearFlags())) ) );
      values.push_back( VRF_Structure::Value("X", (double)viewp->x() ) );
      values.push_back( VRF_Structure::Value("Y", (double)viewp->y() ) );
      values.push_back( VRF_Structure::Value("Width", (double)viewp->width() ) );
      values.push_back( VRF_Structure::Value("Height", (double)viewp->height() ) );

      return value;
    }

    std::string export_ClearColorMode(EClearColorMode ccm)
    {
      switch(ccm)
      {
      case CCM_Float: return "CCM_Float";
      case CCM_Int: return "CCM_Int";
      default:
        return "CCM_UInt";
      }
    }

    std::string export_ClearFlags(EClearFlags cf)
    {
      switch(cf)
      {
      case CF_DO_NOT_CLEAR: return "CF_DO_NOT_CLEAR";
      case CF_CLEAR_COLOR: return "CF_CLEAR_COLOR";
      case CF_CLEAR_DEPTH: return "CF_CLEAR_DEPTH";
      case CF_CLEAR_STENCIL: return "CF_CLEAR_STENCIL";
      case CF_CLEAR_COLOR_DEPTH: return "CF_CLEAR_COLOR_DEPTH";
      case CF_CLEAR_COLOR_STENCIL: return "CF_CLEAR_COLOR_STENCIL";
      case CF_CLEAR_DEPTH_STENCIL: return "CF_CLEAR_DEPTH_STENCIL";
      case CF_CLEAR_COLOR_DEPTH_STENCIL: 
      default:
        return "CF_CLEAR_COLOR_DEPTH_STENCIL";
      }
    }

    VRF_Value export_VertexAttribInfo(const VertexAttribInfo* info)
    {
      VRF_Value value;
      if (vlToVRF(info))
      {
        value.setUID( vlToVRF(info)->uid().c_str() );
        return value;
      }

      VRF_Structure* srf_obj = value.setStructure( new VRF_Structure("<VertexAttribInfo>", generateUID("vertexattribinfo_")) );
      registerExportedStructure(info, srf_obj);
      std::vector<VRF_Structure::Value>& values = srf_obj->value();

      values.push_back( VRF_Structure::Value("Data", export_Array(info->data())) );
      values.push_back( VRF_Structure::Value("Normalize", info->normalize()) );
      std::string interpretation;
      switch(info->interpretation())
      {
      case VAI_NORMAL:  interpretation = "VAI_NORMAL";  break;
      case VAI_INTEGER: interpretation = "VAI_INTEGER"; break;
      case VAI_DOUBLE:  interpretation = "VAI_DOUBLE";  break;
      }
      values.push_back( VRF_Structure::Value("Interpretation", toIdentifier(interpretation)) );

      return value;
    }

    template<typename T_Array, typename T_VRF_Array>
    VRF_Value export_ArrayT(const ArrayAbstract* arr_abstract, const char* name)
    {
      const T_Array* arr = arr_abstract->as<T_Array>();
      VRF_Value value( new VRF_Structure(name, generateUID("array_")) );
      ref<T_VRF_Array> srf_array = new T_VRF_Array;
      if (arr->size())
      {
        srf_array->value().resize( arr->size() * arr->glSize() );
        typename T_VRF_Array::scalar_type* dst = &srf_array->value()[0];
        const typename T_Array::scalar_type* src = (const typename T_Array::scalar_type*)arr->begin();
        const typename T_Array::scalar_type* end = (const typename T_Array::scalar_type*)arr->end();
        for(; src<end; ++src, ++dst)
          *dst = (typename T_VRF_Array::scalar_type)*src;
      }
      value.getStructure()->value().push_back( VRF_Structure::Value("Value", srf_array.get() ) );
      return value;
    }

    VRF_Value export_Array(const ArrayAbstract* arr_abstract)
    {
      VRF_Value value;
      if (vlToVRF(arr_abstract))
      {
        value.setUID( vlToVRF(arr_abstract)->uid().c_str() );
        return value;
      }

      if(arr_abstract->classType() == ArrayUInt1::Type())
        value = export_ArrayT<ArrayUInt1, VRF_ArrayInteger>(arr_abstract, "<ArrayUInt1>");
      else
      if(arr_abstract->classType() == ArrayUInt2::Type())
        value = export_ArrayT<ArrayUInt2, VRF_ArrayInteger>(arr_abstract, "<ArrayUInt2>");
      else
      if(arr_abstract->classType() == ArrayUInt3::Type())
        value = export_ArrayT<ArrayUInt3, VRF_ArrayInteger>(arr_abstract, "<ArrayUInt3>");
      else
      if(arr_abstract->classType() == ArrayUInt4::Type())
        value = export_ArrayT<ArrayUInt4, VRF_ArrayInteger>(arr_abstract, "<ArrayUInt4>");
      else

      if(arr_abstract->classType() == ArrayInt1::Type())
        value = export_ArrayT<ArrayInt1, VRF_ArrayInteger>(arr_abstract, "<ArrayInt1>");
      else
      if(arr_abstract->classType() == ArrayInt2::Type())
        value = export_ArrayT<ArrayInt2, VRF_ArrayInteger>(arr_abstract, "<ArrayInt2>");
      else
      if(arr_abstract->classType() == ArrayInt3::Type())
        value = export_ArrayT<ArrayInt3, VRF_ArrayInteger>(arr_abstract, "<ArrayInt3>");
      else
      if(arr_abstract->classType() == ArrayInt4::Type())
        value = export_ArrayT<ArrayInt4, VRF_ArrayInteger>(arr_abstract, "<ArrayInt4>");
      else

      if(arr_abstract->classType() == ArrayUShort1::Type())
        value = export_ArrayT<ArrayUShort1, VRF_ArrayInteger>(arr_abstract, "<ArrayUShort1>");
      else
      if(arr_abstract->classType() == ArrayUShort2::Type())
        value = export_ArrayT<ArrayUShort2, VRF_ArrayInteger>(arr_abstract, "<ArrayUShort2>");
      else
      if(arr_abstract->classType() == ArrayUShort3::Type())
        value = export_ArrayT<ArrayUShort3, VRF_ArrayInteger>(arr_abstract, "<ArrayUShort3>");
      else
      if(arr_abstract->classType() == ArrayUShort4::Type())
        value = export_ArrayT<ArrayUShort4, VRF_ArrayInteger>(arr_abstract, "<ArrayUShort4>");
      else

      if(arr_abstract->classType() == ArrayUShort1::Type())
        value = export_ArrayT<ArrayUShort1, VRF_ArrayInteger>(arr_abstract, "<ArrayUShort1>");
      else
      if(arr_abstract->classType() == ArrayUShort2::Type())
        value = export_ArrayT<ArrayUShort2, VRF_ArrayInteger>(arr_abstract, "<ArrayUShort2>");
      else
      if(arr_abstract->classType() == ArrayUShort3::Type())
        value = export_ArrayT<ArrayUShort3, VRF_ArrayInteger>(arr_abstract, "<ArrayUShort3>");
      else
      if(arr_abstract->classType() == ArrayUShort4::Type())
        value = export_ArrayT<ArrayUShort4, VRF_ArrayInteger>(arr_abstract, "<ArrayUShort4>");
      else

      if(arr_abstract->classType() == ArrayShort1::Type())
        value = export_ArrayT<ArrayShort1, VRF_ArrayInteger>(arr_abstract, "<ArrayShort1>");
      else
      if(arr_abstract->classType() == ArrayShort2::Type())
        value = export_ArrayT<ArrayShort2, VRF_ArrayInteger>(arr_abstract, "<ArrayShort2>");
      else
      if(arr_abstract->classType() == ArrayShort3::Type())
        value = export_ArrayT<ArrayShort3, VRF_ArrayInteger>(arr_abstract, "<ArrayShort3>");
      else
      if(arr_abstract->classType() == ArrayShort4::Type())
        value = export_ArrayT<ArrayShort4, VRF_ArrayInteger>(arr_abstract, "<ArrayShort4>");
      else

      if(arr_abstract->classType() == ArrayUByte1::Type())
        value = export_ArrayT<ArrayUByte1, VRF_ArrayInteger>(arr_abstract, "<ArrayUByte1>");
      else
      if(arr_abstract->classType() == ArrayUByte2::Type())
        value = export_ArrayT<ArrayUByte2, VRF_ArrayInteger>(arr_abstract, "<ArrayUByte2>");
      else
      if(arr_abstract->classType() == ArrayUByte3::Type())
        value = export_ArrayT<ArrayUByte3, VRF_ArrayInteger>(arr_abstract, "<ArrayUByte3>");
      else
      if(arr_abstract->classType() == ArrayUByte4::Type())
        value = export_ArrayT<ArrayUByte4, VRF_ArrayInteger>(arr_abstract, "<ArrayUByte4>");
      else

      if(arr_abstract->classType() == ArrayByte1::Type())
        value = export_ArrayT<ArrayByte1, VRF_ArrayInteger>(arr_abstract, "<ArrayByte1>");
      else
      if(arr_abstract->classType() == ArrayByte2::Type())
        value = export_ArrayT<ArrayByte2, VRF_ArrayInteger>(arr_abstract, "<ArrayByte2>");
      else
      if(arr_abstract->classType() == ArrayByte3::Type())
        value = export_ArrayT<ArrayByte3, VRF_ArrayInteger>(arr_abstract, "<ArrayByte3>");
      else
      if(arr_abstract->classType() == ArrayByte4::Type())
        value = export_ArrayT<ArrayByte4, VRF_ArrayInteger>(arr_abstract, "<ArrayByte4>");
      else

      if(arr_abstract->classType() == ArrayFloat1::Type())
        value = export_ArrayT<ArrayFloat1, VRF_ArrayReal>(arr_abstract, "<ArrayFloat1>");
      else
      if(arr_abstract->classType() == ArrayFloat2::Type())
        value = export_ArrayT<ArrayFloat2, VRF_ArrayReal>(arr_abstract, "<ArrayFloat2>");
      else
      if(arr_abstract->classType() == ArrayFloat3::Type())
        value = export_ArrayT<ArrayFloat3, VRF_ArrayReal>(arr_abstract, "<ArrayFloat3>");
      else
      if(arr_abstract->classType() == ArrayFloat4::Type())
        value = export_ArrayT<ArrayFloat4, VRF_ArrayReal>(arr_abstract, "<ArrayFloat4>");
      else

      if(arr_abstract->classType() == ArrayDouble1::Type())
        value = export_ArrayT<ArrayDouble1, VRF_ArrayReal>(arr_abstract, "<ArrayDouble1>");
      else
      if(arr_abstract->classType() == ArrayDouble2::Type())
        value = export_ArrayT<ArrayDouble2, VRF_ArrayReal>(arr_abstract, "<ArrayDouble2>");
      else
      if(arr_abstract->classType() == ArrayDouble3::Type())
        value = export_ArrayT<ArrayDouble3, VRF_ArrayReal>(arr_abstract, "<ArrayDouble3>");
      else
      if(arr_abstract->classType() == ArrayDouble4::Type())
        value = export_ArrayT<ArrayDouble4, VRF_ArrayReal>(arr_abstract, "<ArrayDouble4>");
      else
      {
        Log::error("Array type not supported for export.\n");
        VL_TRAP();
      }

      registerExportedStructure(arr_abstract, value.getStructure());
      return value;
    }

    void export_DrawCallBase(const DrawCall* dcall, VRF_Structure* srf_obj)
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

      srf_obj->value().push_back( VRF_Structure::Value("PrimitiveType", toIdentifier(primitive_type) ) );
      srf_obj->value().push_back( VRF_Structure::Value("Enabled", dcall->isEnabled()) );
      if (dcall->patchParameter())
        srf_obj->value().push_back( VRF_Structure::Value("PatchParameter", export_PatchParameter(dcall->patchParameter())) );
    }

    VRF_Value export_PatchParameter(const PatchParameter* pp)
    {
      VRF_Value value;
      if (vlToVRF(pp))
      {
        value.setUID( vlToVRF(pp)->uid().c_str() );
        return value;
      }

      VRF_Structure* srf_obj = value.setStructure( new VRF_Structure("<PatchParameter>", generateUID("patchparam_")) );
      registerExportedStructure(pp, srf_obj);
      srf_obj->value().push_back( VRF_Structure::Value("PatchVertices", (long long)pp->patchVertices()) );
      srf_obj->value().push_back( VRF_Structure::Value("PatchDefaultOuterLevel", toValue(pp->patchDefaultOuterLevel())) );
      srf_obj->value().push_back( VRF_Structure::Value("PatchDefaultInnerLevel", toValue(pp->patchDefaultInnerLevel())) );

      return value;
    }

    VRF_Value export_DrawCall(const DrawCall* dcall)
    {
      VRF_Value value;
      if (vlToVRF(dcall))
      {
        value.setUID( vlToVRF(dcall)->uid().c_str() );
        return value;
      }

      if (dcall->classType() == DrawArrays::Type())
      {
        const DrawArrays* da = dcall->as<DrawArrays>();
        VRF_Structure* srf_obj = value.setStructure( new VRF_Structure("<DrawArrays>", generateUID("drawarrays_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(da, srf_obj);
        srf_obj->value().push_back( VRF_Structure::Value("Instances", (long long)da->instances()) );
        srf_obj->value().push_back( VRF_Structure::Value("Start", (long long)da->start()) );
        srf_obj->value().push_back( VRF_Structure::Value("Count", (long long)da->count()) );
      }
      else
      if (dcall->classType() == DrawElementsUInt::Type())
      {
        const DrawElementsUInt* de = dcall->as<DrawElementsUInt>();
        VRF_Structure* srf_obj = value.setStructure( new VRF_Structure("<DrawElementsUInt>", generateUID("drawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( VRF_Structure::Value("Instances", (long long)de->instances()) );
        srf_obj->value().push_back( VRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( VRF_Structure::Value("BaseVertex", (long long)de->baseVertex()) );
        srf_obj->value().push_back( VRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == DrawElementsUShort::Type())
      {
        const DrawElementsUShort* de = dcall->as<DrawElementsUShort>();
        VRF_Structure* srf_obj = value.setStructure( new VRF_Structure("<DrawElementsUShort>", generateUID("drawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( VRF_Structure::Value("Instances", (long long)de->instances()) );
        srf_obj->value().push_back( VRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( VRF_Structure::Value("BaseVertex", (long long)de->baseVertex()) );
        srf_obj->value().push_back( VRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == DrawElementsUByte::Type())
      {
        const DrawElementsUByte* de = dcall->as<DrawElementsUByte>();
        VRF_Structure* srf_obj = value.setStructure( new VRF_Structure("<DrawElementsUByte>", generateUID("drawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( VRF_Structure::Value("Instances", (long long)de->instances()) );
        srf_obj->value().push_back( VRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( VRF_Structure::Value("BaseVertex", (long long)de->baseVertex()) );
        srf_obj->value().push_back( VRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == MultiDrawElementsUInt::Type())
      {
        const MultiDrawElementsUInt* de = dcall->as<MultiDrawElementsUInt>();
        VRF_Structure* srf_obj = value.setStructure( new VRF_Structure("<MultiDrawElementsUInt>", generateUID("multidrawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( VRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( VRF_Structure::Value("BaseVertices", export_vector_int32(de->baseVertices())) );
        srf_obj->value().push_back( VRF_Structure::Value("CountVector", export_vector_int32(de->countVector())) );
        srf_obj->value().push_back( VRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == MultiDrawElementsUShort::Type())
      {
        const MultiDrawElementsUShort* de = dcall->as<MultiDrawElementsUShort>();
        VRF_Structure* srf_obj = value.setStructure( new VRF_Structure("<MultiDrawElementsUShort>", generateUID("multidrawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( VRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( VRF_Structure::Value("BaseVertices", export_vector_int32(de->baseVertices())) );
        srf_obj->value().push_back( VRF_Structure::Value("CountVector", export_vector_int32(de->countVector())) );
        srf_obj->value().push_back( VRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      if (dcall->classType() == MultiDrawElementsUByte::Type())
      {
        const MultiDrawElementsUByte* de = dcall->as<MultiDrawElementsUByte>();
        VRF_Structure* srf_obj = value.setStructure( new VRF_Structure("<MultiDrawElementsUByte>", generateUID("multidrawelems_")) );
        registerExportedStructure(dcall, srf_obj);
        export_DrawCallBase(de, srf_obj);
        srf_obj->value().push_back( VRF_Structure::Value("PrimitiveRestartEnabled", de->primitiveRestartEnabled()) );
        srf_obj->value().push_back( VRF_Structure::Value("BaseVertices", export_vector_int32(de->baseVertices())) );
        srf_obj->value().push_back( VRF_Structure::Value("CountVector", export_vector_int32(de->countVector())) );
        srf_obj->value().push_back( VRF_Structure::Value("IndexBuffer", export_Array(de->indexBuffer())) );
      }
      else
      {
        Log::error("DrawCall type not supported for export.\n");
        VL_TRAP();
      }

      return value;
    }

    VRF_Value export_vector_int32(const std::vector<int>& vec)
    {
      VRF_Value value;
      value.setArray( new VRF_ArrayInteger );
      value.getArrayInteger()->value().resize( vec.size() );
      if (vec.size())
        value.getArrayInteger()->copyFrom(&vec[0]);
      return value;
    }

    //-----------------------------------------------------------------------------
    // IMPORT FUNCTIONS
    //-----------------------------------------------------------------------------

    void registerImportedStructure(const VRF_Structure* srf_obj, const Object* vl_obj)
    {
      VL_CHECK( mVRF_To_VL.find(srf_obj) == mVRF_To_VL.end() );
      mVRF_To_VL[srf_obj] = vl_obj;
    }

    Object* srfToVL(const VRF_Structure* srf_obj)
    {
      std::map< ref<VRF_Structure>, ref<Object> >::iterator it = mVRF_To_VL.find(srf_obj);
      if (it != mVRF_To_VL.end())
        return it->second.get();
      else
        return NULL;
    }

    const std::map< ref<VRF_Structure>, ref<Object> >& srfToVL() const { return mVRF_To_VL; }

    //-----------------------------------------------------------------------------

    ResourceDatabase* import_ResourceDatabase(const VRF_Structure* srf_obj)
    {
      if (srf_obj->tag() != "<ResourceDatabase>")
        return NULL;

      // <ResourceDatabase> must have at least one "SerializerVersion" and one "Resources" keys in this order.
      if (srf_obj->value().size() < 2)
        return NULL;
      else
      {
        if (srf_obj->value()[0].key() != "SerializerVersion" || srf_obj->value()[0].value().type() != VRF_Value::Integer)
        {
          Log::error( Say("Line %n : no serializer version found.\n") << srf_obj->value()[0].value().lineNumber() );
          return NULL;
        }
        else
        if (srf_obj->value()[0].value().getInteger() != VL_SERIALIZER_VERSION )
        {
          Log::error("Unsupported serializer version.\n");
          return NULL;
        }

        if (srf_obj->value()[1].key() != "Resources" || srf_obj->value()[1].value().type() != VRF_Value::List)
        {
          Log::error( Say("Line %n : 'Resources' key/value expected.\n") << srf_obj->value()[1].value().lineNumber() );
          return NULL;
        }
      }

      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<ResourceDatabase>();

      // link the VRF to the VL object
      ref<ResourceDatabase> res_db = new ResourceDatabase;
      registerImportedStructure(srf_obj, res_db.get());

      // get the list
      const VRF_List* list = srf_obj->value()[1].value().getList();
      for(size_t i=0; i<list->value().size(); ++i)
      {
        const VRF_Value& value = list->value()[i];

        // the member of this list must be all structures, unknown ones are silently skipped

        if (value.type() != VRF_Value::Structure)
        {
          Log::error( Say("Line %n : structure expected.\n") << value.lineNumber() );
          return NULL;
        }

        const VRF_Structure* obj = value.getStructure();
      
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

    fvec2 to_fvec2(const VRF_ArrayReal* arr) { VL_CHECK(arr->value().size() == 2); return fvec2( (float)arr->value()[0], (float)arr->value()[1] ); }

    fvec3 to_fvec3(const VRF_ArrayReal* arr) { VL_CHECK(arr->value().size() == 3); return fvec3( (float)arr->value()[0], (float)arr->value()[1], (float)arr->value()[2] ); }

    fvec4 to_fvec4(const VRF_ArrayReal* arr) { VL_CHECK(arr->value().size() == 4); return fvec4( (float)arr->value()[0], (float)arr->value()[1], (float)arr->value()[2], (float)arr->value()[3] ); }

    bool import_AABB(const VRF_Structure* srf_obj, AABB& aabb)
    {
      VL_CHECK( srf_obj->tag() == "<AABB>" )

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const VRF_Value& value = srf_obj->value()[i].value();
        if (key == "MinCorner")
        {
          VL_CHECK(value.type() == VRF_Value::ArrayReal)
          aabb.setMinCorner( to_fvec3(value.getArrayReal()) );
        }
        else
        if (key == "MaxCorner")
        {
          VL_CHECK(value.type() == VRF_Value::ArrayReal)
          aabb.setMaxCorner( to_fvec3(value.getArrayReal()) );
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

    bool import_Sphere(const VRF_Structure* srf_obj, Sphere& sphere)
    {
      VL_CHECK( srf_obj->tag() == "<Sphere>" )

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const VRF_Value& value = srf_obj->value()[i].value();
        if (key == "Center")
        {
          VL_CHECK(value.type() == VRF_Value::ArrayReal)
          sphere.setCenter( to_fvec3(value.getArrayReal()) );
        }
        else
        if (key == "Radius")
        {
          VL_CHECK(value.type() == VRF_Value::Real)
          sphere.setRadius( (Real)value.getReal() );
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

    ArrayAbstract* import_Array(const VRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<ArrayAbstract>();

      if (!srf_obj->getValue("Value"))
      {
        Log::error( Say("Line %n : error. 'Value' expected in object '%s'. \n") << srf_obj->lineNumber() << srf_obj->tag() );
        return NULL;
      }

      const VRF_Value& value = *srf_obj->getValue("Value");
    
      ref<ArrayAbstract> arr_abstract;

      if (srf_obj->tag() == "<ArrayFloat1>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayReal);
        const VRF_ArrayReal* srf_arr_float = value.getArrayReal();
        ref<ArrayFloat1> arr_float1 = new ArrayFloat1; arr_abstract = arr_float1;
        arr_float1->resize( srf_arr_float->value().size() );
        srf_arr_float->copyTo((float*)arr_float1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayFloat2>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayReal);
        const VRF_ArrayReal* srf_arr_float = value.getArrayReal();
        VL_VRF_CHECK_ERROR( srf_arr_float->value().size() % 2 == 0)
        ref<ArrayFloat2> arr_float2 = new ArrayFloat2; arr_abstract = arr_float2;
        arr_float2->resize( srf_arr_float->value().size() / 2 );
        srf_arr_float->copyTo((float*)arr_float2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayFloat3>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayReal);
        const VRF_ArrayReal* srf_arr_float = value.getArrayReal();
        VL_VRF_CHECK_ERROR( srf_arr_float->value().size() % 3 == 0)
        ref<ArrayFloat3> arr_float3 = new ArrayFloat3; arr_abstract = arr_float3;
        arr_float3->resize( srf_arr_float->value().size() / 3 );
        srf_arr_float->copyTo((float*)arr_float3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayFloat4>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayReal);
        const VRF_ArrayReal* srf_arr_float = value.getArrayReal();
        VL_VRF_CHECK_ERROR( srf_arr_float->value().size() % 4 == 0)
        ref<ArrayFloat4> arr_float4 = new ArrayFloat4; arr_abstract = arr_float4;
        arr_float4->resize( srf_arr_float->value().size() / 4 );
        srf_arr_float->copyTo((float*)arr_float4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble1>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayReal);
        const VRF_ArrayReal* srf_arr_floating = value.getArrayReal();
        ref<ArrayDouble1> arr_floating1 = new ArrayDouble1; arr_abstract = arr_floating1;
        arr_floating1->resize( srf_arr_floating->value().size() );
        srf_arr_floating->copyTo((double*)arr_floating1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble2>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayReal);
        const VRF_ArrayReal* srf_arr_floating = value.getArrayReal();
        VL_VRF_CHECK_ERROR( srf_arr_floating->value().size() % 2 == 0)
        ref<ArrayDouble2> arr_floating2 = new ArrayDouble2; arr_abstract = arr_floating2;
        arr_floating2->resize( srf_arr_floating->value().size() / 2 );
        srf_arr_floating->copyTo((double*)arr_floating2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble3>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayReal);
        const VRF_ArrayReal* srf_arr_floating = value.getArrayReal();
        VL_VRF_CHECK_ERROR( srf_arr_floating->value().size() % 3 == 0)
        ref<ArrayDouble3> arr_floating3 = new ArrayDouble3; arr_abstract = arr_floating3;
        arr_floating3->resize( srf_arr_floating->value().size() / 3 );
        srf_arr_floating->copyTo((double*)arr_floating3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble4>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayReal);
        const VRF_ArrayReal* srf_arr_floating = value.getArrayReal();
        VL_VRF_CHECK_ERROR( srf_arr_floating->value().size() % 4 == 0)
        ref<ArrayDouble4> arr_floating4 = new ArrayDouble4; arr_abstract = arr_floating4;
        arr_floating4->resize( srf_arr_floating->value().size() / 4 );
        srf_arr_floating->copyTo((double*)arr_floating4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayInt1>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayInt1> arr_int1 = new ArrayInt1; arr_abstract = arr_int1;
        arr_int1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((int*)arr_int1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayInt2>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayInt2> arr_int2 = new ArrayInt2; arr_abstract = arr_int2;
        arr_int2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((int*)arr_int2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayInt3>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayInt3> arr_int3 = new ArrayInt3; arr_abstract = arr_int3;
        arr_int3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((int*)arr_int3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayInt4>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayInt4> arr_int4 = new ArrayInt4; arr_abstract = arr_int4;
        arr_int4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((int*)arr_int4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt1>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayUInt1> arr_int1 = new ArrayUInt1; arr_abstract = arr_int1;
        arr_int1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((unsigned int*)arr_int1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt2>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayUInt2> arr_int2 = new ArrayUInt2; arr_abstract = arr_int2;
        arr_int2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((unsigned int*)arr_int2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt3>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayUInt3> arr_int3 = new ArrayUInt3; arr_abstract = arr_int3;
        arr_int3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((unsigned int*)arr_int3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt4>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayUInt4> arr_int4 = new ArrayUInt4; arr_abstract = arr_int4;
        arr_int4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((unsigned int*)arr_int4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayShort1>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayShort1> arr_short1 = new ArrayShort1; arr_abstract = arr_short1;
        arr_short1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((short*)arr_short1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayShort2>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayShort2> arr_short2 = new ArrayShort2; arr_abstract = arr_short2;
        arr_short2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((short*)arr_short2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayShort3>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayShort3> arr_short3 = new ArrayShort3; arr_abstract = arr_short3;
        arr_short3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((short*)arr_short3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayShort4>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayShort4> arr_short4 = new ArrayShort4; arr_abstract = arr_short4;
        arr_short4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((short*)arr_short4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUShort1>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayUShort1> arr_short1 = new ArrayUShort1; arr_abstract = arr_short1;
        arr_short1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((unsigned short*)arr_short1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUShort2>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayUShort2> arr_short2 = new ArrayUShort2; arr_abstract = arr_short2;
        arr_short2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((unsigned short*)arr_short2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUShort3>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayUShort3> arr_short3 = new ArrayUShort3; arr_abstract = arr_short3;
        arr_short3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((unsigned short*)arr_short3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUShort4>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayUShort4> arr_short4 = new ArrayUShort4; arr_abstract = arr_short4;
        arr_short4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((unsigned short*)arr_short4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayByte1>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayByte1> arr_byte1 = new ArrayByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((char*)arr_byte1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayByte2>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayByte2> arr_byte2 = new ArrayByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((char*)arr_byte2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayByte3>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayByte3> arr_byte3 = new ArrayByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((char*)arr_byte3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayByte4>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayByte4> arr_byte4 = new ArrayByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((char*)arr_byte4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUByte1>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayUByte1> arr_byte1 = new ArrayUByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((unsigned char*)arr_byte1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUByte2>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayUByte2> arr_byte2 = new ArrayUByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((unsigned char*)arr_byte2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUByte3>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayUByte3> arr_byte3 = new ArrayUByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((unsigned char*)arr_byte3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUByte4>")
      {
        VL_VRF_CHECK_ERROR(value.type() == VRF_Value::ArrayInteger);
        const VRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_VRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayUByte4> arr_byte4 = new ArrayUByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((unsigned char*)arr_byte4->ptr());
      }
      else
      {
        Log::error( Say("Line %n : unknown array '%s'.\n") << srf_obj->lineNumber() << srf_obj->tag() );
        return NULL;
      }

      // link the VRF to the VL object
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

    DrawCall* import_DrawCall(const VRF_Structure* srf_obj)
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
          const VRF_Value& value = srf_obj->value()[i].value();
          if( key == "PrimitiveType" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Identifier )
            de->setPrimitiveType( import_EPrimitiveType( value.getIdentifier(), value.lineNumber() ) );
            VL_VRF_CHECK_ERROR( de->primitiveType() != PT_UNKNOWN );
          }
          else
          if( key == "Enabled" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Bool )
            de->setEnabled( value.getBool() );
          }
          else
          if( key == "Instances" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Integer )
            de->setInstances( (int)value.getInteger() );
          }
          else
          if( key == "PrimitiveRestartEnabled" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Bool )
            de->setPrimitiveRestartEnabled( value.getBool() );
          }
          else
          if( key == "BaseVertex" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Integer )
            de->setBaseVertex( (int)value.getInteger() );
          }
          else
          if( key == "IndexBuffer" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Structure )
            ArrayAbstract* arr_abstract = import_Array(value.getStructure());
            if(!arr_abstract)
              return NULL;

            if ( de->classType() == DrawElementsUInt::Type() )
            {
              VL_VRF_CHECK_ERROR(arr_abstract->classType() == ArrayUInt1::Type());
              de->as<DrawElementsUInt>()->setIndexBuffer( arr_abstract->as<ArrayUInt1>() );
            }
            else
            if ( de->classType() == DrawElementsUShort::Type() )
            {
              VL_VRF_CHECK_ERROR(arr_abstract->classType() == ArrayUShort1::Type());
              de->as<DrawElementsUShort>()->setIndexBuffer( arr_abstract->as<ArrayUShort1>() );
            }
            else
            if ( de->classType() == DrawElementsUByte::Type() )
            {
              VL_VRF_CHECK_ERROR(arr_abstract->classType() == ArrayUByte1::Type());
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
          const VRF_Value& value = srf_obj->value()[i].value();
          if( key == "PrimitiveType" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Identifier )
            de->setPrimitiveType( import_EPrimitiveType( value.getIdentifier(), value.lineNumber() ) );
            VL_VRF_CHECK_ERROR( de->primitiveType() != PT_UNKNOWN );
          }
          else
          if( key == "Enabled" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Bool )
            de->setEnabled( value.getBool() );
          }
          else
          if( key == "PrimitiveRestartEnabled" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Bool )
            de->setPrimitiveRestartEnabled( value.getBool() );
          }
          else
          if( key == "BaseVertices" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::ArrayInteger )
            de->baseVertices().resize( value.getArrayInteger()->value().size() );
            if (de->baseVertices().size())
              value.getArrayInteger()->copyTo( &de->baseVertices()[0] );
            // de->setBaseVertices( value.getArrayInt32()->value() );
          }
          else
          if( key == "CountVector" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::ArrayInteger )
            de->countVector().resize( value.getArrayInteger()->value().size() );
            if (de->countVector().size())
              value.getArrayInteger()->copyTo( &de->countVector()[0] );
            // de->countVector() = value.getArrayInt32()->value();
          }
          else
          if( key == "IndexBuffer" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Structure )
            ArrayAbstract* arr_abstract = import_Array(value.getStructure());
            if( !arr_abstract )
              return NULL;

            if ( de->classType() == MultiDrawElementsUInt::Type() )
            {
              VL_VRF_CHECK_ERROR(arr_abstract->classType() == ArrayUInt1::Type());
              de->as<MultiDrawElementsUInt>()->setIndexBuffer( arr_abstract->as<ArrayUInt1>() );
            }
            else
            if ( de->classType() == MultiDrawElementsUShort::Type() )
            {
              VL_VRF_CHECK_ERROR(arr_abstract->classType() == ArrayUShort1::Type());
              de->as<MultiDrawElementsUShort>()->setIndexBuffer( arr_abstract->as<ArrayUShort1>() );
            }
            else
            if ( de->classType() == MultiDrawElementsUByte::Type() )
            {
              VL_VRF_CHECK_ERROR(arr_abstract->classType() == ArrayUByte1::Type());
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
          const VRF_Value& value = srf_obj->value()[i].value();

          if( key == "PrimitiveType" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Identifier )
            da->setPrimitiveType( import_EPrimitiveType( value.getIdentifier(), value.lineNumber() ) );
            VL_VRF_CHECK_ERROR( da->primitiveType() != PT_UNKNOWN );
          }
          else
          if( key == "Enabled" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Bool )
            da->setEnabled( value.getBool() );
          }
          else
          if( key == "Instances" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Integer )
            da->setInstances( (int)value.getInteger() );
          }
          else
          if( key == "Start" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Integer )
            da->setStart( (int)value.getInteger() );
          }
          else
          if( key == "Count" )
          {
            VL_VRF_CHECK_ERROR( value.type() == VRF_Value::Integer )
            da->setCount( (int)value.getInteger() );
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

      // link the VRF to the VL object
      registerImportedStructure(srf_obj, draw_call.get());
    
      return draw_call.get();
    }

    VertexAttribInfo* import_VertexAttribInfo(const VRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<VertexAttribInfo>();

      if (srf_obj->tag() != "<VertexAttribInfo>")
      {
        Log::error( Say("Line %n : <VertexAttribInfo> expected.\n") << srf_obj->lineNumber() );
        return NULL;
      }

      // link the VRF to the VL object
      ref<VertexAttribInfo> info = new VertexAttribInfo;
      registerImportedStructure(srf_obj, info.get());

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const VRF_Value& value = srf_obj->value()[i].value();

        if (key == "Data")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
          ArrayAbstract* arr = import_Array( value.getStructure() );
          if(arr)
            info->setData(arr);
          else
            return NULL;
        }
        else
        if (key == "Normalize")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Bool) 
          info->setNormalize( value.getBool() );
        }
        else
        if (key == "Interpretation")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Identifier) 
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

    Geometry* import_Geometry(const VRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Geometry>();

      if (srf_obj->tag() != "<Geometry>")
      {
        Log::error( Say("Line %n : <Geometry> expected.\n") << srf_obj->lineNumber() );
        return NULL;
      }

      // link the VRF to the VL object
      ref<Geometry> geom = new Geometry;
      registerImportedStructure(srf_obj, geom.get());

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const VRF_Value& value = srf_obj->value()[i].value();

        if (key == "VBOEnabled")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Bool) 
          geom->setVBOEnabled( value.getBool() );
        }
        else
        if (key == "DisplayListEnabled")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Bool) 
          geom->setDisplayListEnabled( value.getBool() );
        }
        else
        if (key == "AABB")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
          AABB aabb;
          if (!import_AABB(value.getStructure(), aabb))
            return NULL;
        }
        else
        if (key == "Sphere")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
          Sphere sphere;
          if (!import_Sphere(value.getStructure(), sphere))
            return NULL;
        }
        else
        if (key == "VertexArray")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setVertexArray(arr);
          else
            return NULL;
        }
        else
        if (key == "NormalArray")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setNormalArray(arr);
          else
            return NULL;
        }
        else
        if (key == "ColorArray")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setColorArray(arr);
          else
            return NULL;
        }
        else
        if (key == "SecondaryColorArray")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
          ArrayAbstract* arr = import_Array(value.getStructure());
          if (arr)
            geom->setSecondaryColorArray(arr);
          else
            return NULL;
        }
        else
        if (key == "FogCoordArray")
        {
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
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

          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
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
        
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
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
          VL_VRF_CHECK_ERROR(value.type() == VRF_Value::Structure) 
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

    Effect* import_Effect(const VRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Effect>();

      return NULL;
    }

    Shader* import_Shader(const VRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Shader>();

      return NULL;
    }

    Transform* import_Transform(const VRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Transform>();

      return NULL;
    }

    Actor* import_Actor(const VRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Actor>();

      return NULL;
    }

    Camera* import_Camera(const VRF_Structure* srf_obj)
    {
      // return already parsed object
      if (srfToVL(srf_obj))
        return srfToVL(srf_obj)->as<Camera>();

      return NULL;
    }

    //-----------------------------------------------------------------------------

    std::string exportToText(const ResourceDatabase* res_db)
    {
      VRF_Value value = export_ResourceDatabase(res_db);

      // do not link!

      // collect the UIDs that need to be visible
      std::set< std::string > uid_set;
      VRF_UIDCollectorVisitor uid_collector;
      uid_collector.setUIDSet(&uid_set);
      value.getStructure()->acceptVisitor(&uid_collector);

      VRF_TextExportVisitor text_export_visitor;
      text_export_visitor.setUIDSet(&uid_set);
      value.getStructure()->acceptVisitor(&text_export_visitor);

      return text_export_visitor.srfText();
    }

    //-----------------------------------------------------------------------------

    ref<ResourceDatabase> importFromText(VirtualFile* file)
    {
      VRF_Parser parser;
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
    std::map< ref<VRF_Structure>, ref<Object> > mVRF_To_VL;

    // export
    std::map< ref<Object>, ref<VRF_Structure> > mVL_To_VRF;
    int mUIDCounter;

    EError mError;
  };
//-----------------------------------------------------------------------------
  VLGRAPHICS_EXPORT ref<ResourceDatabase> loadVRF(VirtualFile* file);
  VLGRAPHICS_EXPORT ref<ResourceDatabase> loadVRF(const String& path);
  VLGRAPHICS_EXPORT bool writeVRF(VirtualFile* file, const ResourceDatabase*);
  VLGRAPHICS_EXPORT bool writeVRF(const String& file, const ResourceDatabase*);
//---------------------------------------------------------------------------
// LoadWriterVRF
//---------------------------------------------------------------------------
  /**
   * The LoadWriterVRF class is a ResourceLoadWriter capable of reading Visualization Library's VRF files.
   */
  class LoadWriterVRF: public ResourceLoadWriter
  {
    VL_INSTRUMENT_CLASS(vl::LoadWriterVRF, ResourceLoadWriter)

  public:
    LoadWriterVRF(): ResourceLoadWriter("|srf|", "|srf|") {}

    ref<ResourceDatabase> loadResource(const String& path) const 
    {
      return loadVRF(path);
    }

    ref<ResourceDatabase> loadResource(VirtualFile* file) const
    {
      return loadVRF(file);
    }

    bool writeResource(const String& path, ResourceDatabase* res_db) const
    {
      return writeVRF(path, res_db);
    }

    bool writeResource(VirtualFile* file, ResourceDatabase* res_db) const
    {
      return writeVRF(file, res_db);
    }
  };
//-----------------------------------------------------------------------------
}

#undef VL_VRF_CHECK_ERROR

#endif
