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
#include <vlGraphics/DistanceLODEvaluator.hpp>
#include <vlGraphics/PixelLODEvaluator.hpp>
#include <vlGraphics/DepthSortCallback.hpp>
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

    SRF_Value toIdentifier(const std::string& str) { return SRF_Value(str.c_str(), SRF_Value::Identifier); }

    SRF_Value toUID(const std::string& str)        { return SRF_Value(str.c_str(), SRF_Value::UID); }

    SRF_Value toString(const std::string& str)     { return SRF_Value(str.c_str(), SRF_Value::String); }
    
    SRF_Value toValue(const fvec4& vec)
    {
      SRF_Value val( new SRF_ArrayReal );
      SRF_ArrayReal* arr = val.getArrayReal();
      arr->value().resize(4);
      arr->value()[0] = vec.x();
      arr->value()[1] = vec.y();
      arr->value()[2] = vec.z();
      arr->value()[3] = vec.w();
      return val;
    }

    SRF_Value toValue(const fvec3& vec)
    {
      SRF_Value val( new SRF_ArrayReal );
      SRF_ArrayReal* arr = val.getArrayReal();
      arr->value().resize(3);
      arr->value()[0] = vec.x();
      arr->value()[1] = vec.y();
      arr->value()[2] = vec.z();
      return val;
    }

    SRF_Value toValue(const fvec2& vec)
    {
      SRF_Value val( new SRF_ArrayReal );
      SRF_ArrayReal* arr = val.getArrayReal();
      arr->value().resize(2);
      arr->value()[0] = vec.x();
      arr->value()[1] = vec.y();
      return val;
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
        if(res_db->resources()[i]->isOfType(Actor::Type()))
          resource = export_Actor(res_db->resources()[i]->as<Actor>());
        else
        if(res_db->resources()[i]->isOfType(Camera::Type()))
          resource = export_Camera(res_db->resources()[i]->as<Camera>());

        srf_resource_list.value().getList()->value().push_back( resource );
      }

      return value;
    }

    SRF_Value export_AABB(const AABB& aabb)
    {
      SRF_Value value ( new SRF_Structure("<AABB>", generateUID("aabb_")) );
      value.getStructure()->value().push_back( SRF_Structure::Value("MinCorner", toValue(aabb.minCorner())) );
      value.getStructure()->value().push_back( SRF_Structure::Value("MaxCorner", toValue(aabb.maxCorner())) );
      return value;
    }

    SRF_Value export_Sphere(const Sphere& sphere)
    {
      SRF_Value value ( new SRF_Structure("<Sphere>", generateUID("sphere_")) );
      value.getStructure()->value().push_back( SRF_Structure::Value("Center", toValue(sphere.center())) );
      value.getStructure()->value().push_back( SRF_Structure::Value("Radius", sphere.radius()) );
      return value;
    }

    SRF_Value export_Renderable(const Renderable* ren)
    {
      SRF_Value value;
      if (vlToSRF(ren))
      {
        value.setUID( vlToSRF(ren)->uid().c_str() );
        return value;
      }

      value.setStructure( new SRF_Structure( "<Geometry>", generateUID("geometry_") ) );
      registerExportedStructure(ren, value.getStructure());
      std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( SRF_Structure::Value("VBOEnabled", ren->isVBOEnabled()) );
      values.push_back( SRF_Structure::Value("DisplayListEnabled", ren->isDisplayListEnabled()) );
      values.push_back( SRF_Structure::Value("AABB", export_AABB(ren->boundingBox())) );
      values.push_back( SRF_Structure::Value("Sphere", export_Sphere(ren->boundingSphere())) );

      if( ren->isOfType( Geometry::Type() ) )
        export_Geometry( ren->as<Geometry>(), values );
      else
      {
        VL_TRAP(); // mic fixme: support also the other renderables
      }

      return value;
    }

    void export_Geometry(const Geometry* geom, std::vector<SRF_Structure::Value>& values)
    {
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
    }

    SRF_Value export_Actor(const Actor* act)
    {
      SRF_Value value;
      if (vlToSRF(act))
      {
        value.setUID( vlToSRF(act)->uid().c_str() );
        return value;
      }

      value.setStructure( new SRF_Structure("<Actor>", generateUID("actor_")) );
      registerExportedStructure(act, value.getStructure());
      std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

      SRF_Value renderables;
      renderables.setList( new SRF_List );
      for(size_t i=0; i<VL_MAX_ACTOR_LOD && act->lod(i); ++i)
        renderables.getList()->value().push_back( export_Renderable(act->lod(i)) );
      values.push_back( SRF_Structure::Value("Lods", renderables) );

      values.push_back( SRF_Structure::Value("AABB", export_AABB(act->boundingBox())) );
      values.push_back( SRF_Structure::Value("Sphere", export_Sphere(act->boundingSphere())) );
      if (act->effect())
        values.push_back( SRF_Structure::Value("Effect", export_Effect(act->effect())) );
      if (act->transform())
        values.push_back( SRF_Structure::Value("Transform", export_Transform(act->transform())) );

      SRF_Value uniforms;
      uniforms.setList( new SRF_List );
      for(size_t i=0; act->getUniformSet() && i<act->uniforms().size(); ++i)
        uniforms.getList()->value().push_back( export_Uniform(act->uniforms()[i].get()) );
      values.push_back( SRF_Structure::Value("Uniforms", uniforms) );

      if (act->lodEvaluator())
          values.push_back( SRF_Structure::Value("LODEvaluator", export_LODEvaluator(act->lodEvaluator())) );

      // mic fixme:
      // Scissor: scissorts might go away from the Actor

      SRF_Value callbacks;
      callbacks.setList( new SRF_List );
      for(int i=0; i<act->actorEventCallbacks()->size(); ++i)
        callbacks.getList()->value().push_back( export_ActorEventCallback(act->actorEventCallbacks()->at(i)) );
      values.push_back( SRF_Structure::Value("ActorEventCallbacks", callbacks) );

      values.push_back( SRF_Structure::Value("RenderBlock", (long long)act->renderBlock() ) );
      values.push_back( SRF_Structure::Value("RenderRank", (long long)act->renderRank() ) );
      values.push_back( SRF_Structure::Value("EnableMask", (long long)act->enableMask() ) );
      values.push_back( SRF_Structure::Value("IsOccludee", act->isOccludee() ) );

      return value;
    }

    SRF_Value export_ActorEventCallback(const ActorEventCallback* cb)
    {
      SRF_Value value;
      if (vlToSRF(cb))
      {
        value.setUID( vlToSRF(cb)->uid().c_str() );
        return value;
      }

      // mic fixme: allow other callbacks to be exported and imported
      if (cb->classType() == DepthSortCallback::Type())
      {
        const DepthSortCallback* dsc = cb->as<DepthSortCallback>();

        value.setStructure( new SRF_Structure("<DepthSortCallback>", generateUID("actorcallbk_")) );
        registerExportedStructure(cb, value.getStructure());
        std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

        if (dsc->sortMode() == SM_SortBackToFront)
          values.push_back( SRF_Structure::Value("SortMode", toIdentifier("SM_SortBackToFront")) );
        else
          values.push_back( SRF_Structure::Value("SortMode", toIdentifier("SM_SortFrontToBack")) );
      }
      else
      {
        // mic fixme: we should not output unknown objects at all
        value.setStructure( new SRF_Structure("<UnknownObjectType>", generateUID("actorcallbk_")) );
        return value;
      }

      return value;
    }

    SRF_Value export_LODEvaluator(const LODEvaluator* lod)
    {
      SRF_Value value;
      if (lod->classType() == DistanceLODEvaluator::Type())
        return export_DistanceLODEvaluator(lod->as<DistanceLODEvaluator>());
      else
      if (lod->classType() == PixelLODEvaluator::Type())
        return export_PixelLODEvaluator(lod->as<PixelLODEvaluator>());
      else
      {
        // mic fixme: we should not output unknown objects at all
        value.setStructure( new SRF_Structure("<UnknownObjectType>", generateUID("lodeval_")) );
        return value;
      }
    }

    SRF_Value export_DistanceLODEvaluator(const DistanceLODEvaluator* lod)
    {
      SRF_Value value;
      if (vlToSRF(lod))
      {
        value.setUID( vlToSRF(lod)->uid().c_str() );
        return value;
      }

      value.setStructure( new SRF_Structure("<DistanceLODEvaluator>", generateUID("lodeval_")) );
      registerExportedStructure(lod, value.getStructure());
      std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

      SRF_Value distances( new SRF_ArrayReal );
      distances.getArrayReal()->value().resize( lod->distanceRangeSet().size() );
      if (lod->distanceRangeSet().size() != 0)
        distances.getArrayReal()->copyFrom( &lod->distanceRangeSet()[0] );

      values.push_back( SRF_Structure::Value("DistanceRageSet", distances) );

      return value;
    }

    SRF_Value export_PixelLODEvaluator(const PixelLODEvaluator* lod)
    {
      SRF_Value value;
      if (vlToSRF(lod))
      {
        value.setUID( vlToSRF(lod)->uid().c_str() );
        return value;
      }

      value.setStructure( new SRF_Structure("<PixelLODEvaluator>", generateUID("lodeval_")) );
      registerExportedStructure(lod, value.getStructure());
      std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

      SRF_Value distances( new SRF_ArrayReal );
      distances.getArrayReal()->value().resize( lod->pixelRangeSet().size() );
      if (lod->pixelRangeSet().size() != 0)
        distances.getArrayReal()->copyFrom( &lod->pixelRangeSet()[0] );

      values.push_back( SRF_Structure::Value("PixelRageSet", distances) );

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

      value.setStructure( new SRF_Structure("<Transform>", generateUID("transform_")) );
      registerExportedStructure(tr, value.getStructure());
      std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( SRF_Structure::Value("LocalMatrix", export_Matrix(tr->localMatrix()) ) );

      if (tr->parent())
        values.push_back( SRF_Structure::Value("Parent", export_Transform(tr->parent()) ) );

      SRF_Value childs;
      childs.setList( new SRF_List );
      for(size_t i=0; i<tr->childrenCount(); ++i)
        childs.getList()->value().push_back( export_Transform(tr->children()[i].get()) );
      values.push_back( SRF_Structure::Value("Children", childs) );

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

    SRF_Value export_Matrix(const fmat4& mat)
    {
      SRF_Value matrix_list( new SRF_List );

      if (isTranslation(mat))
      {
        SRF_Value value( new SRF_ArrayReal("<Translation>") );
        value.getArrayReal()->value().resize(3);
        value.getArrayReal()->value()[0] = mat.getT().x();
        value.getArrayReal()->value()[1] = mat.getT().y();
        value.getArrayReal()->value()[2] = mat.getT().z();
        matrix_list.getList()->value().push_back( value );
      }
      else
      if (isScaling(mat))
      {
        SRF_Value value( new SRF_ArrayReal("<Scaling>") );
        value.getArrayReal()->value().resize(3);
        value.getArrayReal()->value()[0] = mat.e(0,0);
        value.getArrayReal()->value()[1] = mat.e(1,1);
        value.getArrayReal()->value()[2] = mat.e(2,2);
        matrix_list.getList()->value().push_back( value );
      }
      else
      {
        SRF_Value value( new SRF_ArrayReal("<Matrix>") );
        value.getArrayReal()->value().resize(4*4);
        // if we transpose this we have to transpose also the uniform matrices
        value.getArrayReal()->copyFrom(mat.ptr());
        matrix_list.getList()->value().push_back( value );
      }

      return matrix_list;
    }

    SRF_Value export_Uniform(const Uniform* uniform)
    {
      SRF_Value value;
      if (vlToSRF(uniform))
      {
        value.setUID( vlToSRF(uniform)->uid().c_str() );
        return value;
      }

      value.setStructure( new SRF_Structure("<Uniform>", generateUID("uniform_")) );
      registerExportedStructure(uniform, value.getStructure());
      std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( SRF_Structure::Value("Name", toIdentifier(uniform->name())) );
      values.push_back( SRF_Structure::Value("Type", toIdentifier(export_UniformType(uniform->type()))) );
      values.push_back( SRF_Structure::Value("Count", (long long)uniform->count()) );

      const int count = uniform->count();
      ref<SRF_ArrayInteger> arr_int = new SRF_ArrayInteger;
      ref<SRF_ArrayReal> arr_real = new SRF_ArrayReal;

      switch(uniform->type())
      {
      case UT_INT:
        {
          if (count == 1)
            { int val = 0; uniform->getUniform(&val); values.push_back( SRF_Structure::Value("Data", (long long)val) ); break; }
          else
            { arr_int->value().resize(count*1); arr_int->copyFrom( (int*)uniform->rawData() ); break; }
        }
      case UT_INT_VEC2: arr_int->value().resize(count*2); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_INT_VEC3: arr_int->value().resize(count*3); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_INT_VEC4: arr_int->value().resize(count*4); arr_int->copyFrom( (int*)uniform->rawData() ); break;

      case UT_UNSIGNED_INT:
        {
          if (count == 1)
            { unsigned int val = 0; uniform->getUniform(&val); values.push_back( SRF_Structure::Value("Data", (long long)val) ); break; }
          else
            { arr_int->value().resize(count*1); arr_int->copyFrom( (int*)uniform->rawData() ); break; }
        }
      case UT_UNSIGNED_INT_VEC2: arr_int->value().resize(count*2); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_UNSIGNED_INT_VEC3: arr_int->value().resize(count*3); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_UNSIGNED_INT_VEC4: arr_int->value().resize(count*4); arr_int->copyFrom( (int*)uniform->rawData() ); break;

      case UT_FLOAT:
        {
          if (count == 1)
            { float val = 0; uniform->getUniform(&val); values.push_back( SRF_Structure::Value("Data", (double)val) ); break; }
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
            { double val = 0; uniform->getUniform(&val); values.push_back( SRF_Structure::Value("Data", (double)val) ); break; }
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
        values.push_back( SRF_Structure::Value("Data", arr_int.get()) );
      else
      if (!arr_real->value().empty())
        values.push_back( SRF_Structure::Value("Data", arr_real.get()) );

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

    SRF_Value export_Effect(const Effect* fx)
    {
      SRF_Value value;
      if (vlToSRF(fx))
      {
        value.setUID( vlToSRF(fx)->uid().c_str() );
        return value;
      }

      value.setStructure( new SRF_Structure("<Effect>", generateUID("effect_")) );
      registerExportedStructure(fx, value.getStructure());
      std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

      values.push_back( SRF_Structure::Value("RenderRank", (long long)fx->renderRank()) );
      values.push_back( SRF_Structure::Value("EnableMask", (long long)fx->enableMask()) );
      values.push_back( SRF_Structure::Value("ActiveLod", (long long)fx->activeLod()) );

      if (fx->lodEvaluator())
        values.push_back( SRF_Structure::Value("LODEvaluator", export_LODEvaluator(fx->lodEvaluator())) );

      ref<SRF_List> lod_list = new SRF_List;
      for(int i=0; fx->lod(i) && i<VL_MAX_EFFECT_LOD; ++i)
        lod_list->value().push_back( export_ShaderSequence(fx->lod(i).get()) );
      values.push_back( SRF_Structure::Value("Lods", lod_list.get()) );

      return value;
    }

    SRF_Value export_ShaderSequence(const ShaderPasses* sh_seq)
    {
      SRF_Value value;
      if (vlToSRF(sh_seq))
      {
        value.setUID( vlToSRF(sh_seq)->uid().c_str() );
        return value;
      }

      value.setList( new SRF_List("<ShaderPasses>") );
      for(int i=0; i<sh_seq->size(); ++i)
        value.getList()->value().push_back( export_Shader(sh_seq->at(i)) );

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
      // std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

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
      // std::vector<SRF_Structure::Value>& values = value.getStructure()->value();

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
      values.push_back( SRF_Structure::Value("Interpretation", toIdentifier(interpretation)) );

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
        const typename T_Array::scalar_type* src = (const typename T_Array::scalar_type*)arr->begin();
        const typename T_Array::scalar_type* end = (const typename T_Array::scalar_type*)arr->end();
        for(; src<end; ++src, ++dst)
          *dst = (typename T_SRF_Array::scalar_type)*src;
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
        value = export_ArrayT<ArrayUInt1, SRF_ArrayInteger>(arr_abstract, "<ArrayUInt1>");
      else
      if(arr_abstract->classType() == ArrayUInt2::Type())
        value = export_ArrayT<ArrayUInt2, SRF_ArrayInteger>(arr_abstract, "<ArrayUInt2>");
      else
      if(arr_abstract->classType() == ArrayUInt3::Type())
        value = export_ArrayT<ArrayUInt3, SRF_ArrayInteger>(arr_abstract, "<ArrayUInt3>");
      else
      if(arr_abstract->classType() == ArrayUInt4::Type())
        value = export_ArrayT<ArrayUInt4, SRF_ArrayInteger>(arr_abstract, "<ArrayUInt4>");
      else

      if(arr_abstract->classType() == ArrayInt1::Type())
        value = export_ArrayT<ArrayInt1, SRF_ArrayInteger>(arr_abstract, "<ArrayInt1>");
      else
      if(arr_abstract->classType() == ArrayInt2::Type())
        value = export_ArrayT<ArrayInt2, SRF_ArrayInteger>(arr_abstract, "<ArrayInt2>");
      else
      if(arr_abstract->classType() == ArrayInt3::Type())
        value = export_ArrayT<ArrayInt3, SRF_ArrayInteger>(arr_abstract, "<ArrayInt3>");
      else
      if(arr_abstract->classType() == ArrayInt4::Type())
        value = export_ArrayT<ArrayInt4, SRF_ArrayInteger>(arr_abstract, "<ArrayInt4>");
      else

      if(arr_abstract->classType() == ArrayUShort1::Type())
        value = export_ArrayT<ArrayUShort1, SRF_ArrayInteger>(arr_abstract, "<ArrayUShort1>");
      else
      if(arr_abstract->classType() == ArrayUShort2::Type())
        value = export_ArrayT<ArrayUShort2, SRF_ArrayInteger>(arr_abstract, "<ArrayUShort2>");
      else
      if(arr_abstract->classType() == ArrayUShort3::Type())
        value = export_ArrayT<ArrayUShort3, SRF_ArrayInteger>(arr_abstract, "<ArrayUShort3>");
      else
      if(arr_abstract->classType() == ArrayUShort4::Type())
        value = export_ArrayT<ArrayUShort4, SRF_ArrayInteger>(arr_abstract, "<ArrayUShort4>");
      else

      if(arr_abstract->classType() == ArrayUShort1::Type())
        value = export_ArrayT<ArrayUShort1, SRF_ArrayInteger>(arr_abstract, "<ArrayUShort1>");
      else
      if(arr_abstract->classType() == ArrayUShort2::Type())
        value = export_ArrayT<ArrayUShort2, SRF_ArrayInteger>(arr_abstract, "<ArrayUShort2>");
      else
      if(arr_abstract->classType() == ArrayUShort3::Type())
        value = export_ArrayT<ArrayUShort3, SRF_ArrayInteger>(arr_abstract, "<ArrayUShort3>");
      else
      if(arr_abstract->classType() == ArrayUShort4::Type())
        value = export_ArrayT<ArrayUShort4, SRF_ArrayInteger>(arr_abstract, "<ArrayUShort4>");
      else

      if(arr_abstract->classType() == ArrayShort1::Type())
        value = export_ArrayT<ArrayShort1, SRF_ArrayInteger>(arr_abstract, "<ArrayShort1>");
      else
      if(arr_abstract->classType() == ArrayShort2::Type())
        value = export_ArrayT<ArrayShort2, SRF_ArrayInteger>(arr_abstract, "<ArrayShort2>");
      else
      if(arr_abstract->classType() == ArrayShort3::Type())
        value = export_ArrayT<ArrayShort3, SRF_ArrayInteger>(arr_abstract, "<ArrayShort3>");
      else
      if(arr_abstract->classType() == ArrayShort4::Type())
        value = export_ArrayT<ArrayShort4, SRF_ArrayInteger>(arr_abstract, "<ArrayShort4>");
      else

      if(arr_abstract->classType() == ArrayUByte1::Type())
        value = export_ArrayT<ArrayUByte1, SRF_ArrayInteger>(arr_abstract, "<ArrayUByte1>");
      else
      if(arr_abstract->classType() == ArrayUByte2::Type())
        value = export_ArrayT<ArrayUByte2, SRF_ArrayInteger>(arr_abstract, "<ArrayUByte2>");
      else
      if(arr_abstract->classType() == ArrayUByte3::Type())
        value = export_ArrayT<ArrayUByte3, SRF_ArrayInteger>(arr_abstract, "<ArrayUByte3>");
      else
      if(arr_abstract->classType() == ArrayUByte4::Type())
        value = export_ArrayT<ArrayUByte4, SRF_ArrayInteger>(arr_abstract, "<ArrayUByte4>");
      else

      if(arr_abstract->classType() == ArrayByte1::Type())
        value = export_ArrayT<ArrayByte1, SRF_ArrayInteger>(arr_abstract, "<ArrayByte1>");
      else
      if(arr_abstract->classType() == ArrayByte2::Type())
        value = export_ArrayT<ArrayByte2, SRF_ArrayInteger>(arr_abstract, "<ArrayByte2>");
      else
      if(arr_abstract->classType() == ArrayByte3::Type())
        value = export_ArrayT<ArrayByte3, SRF_ArrayInteger>(arr_abstract, "<ArrayByte3>");
      else
      if(arr_abstract->classType() == ArrayByte4::Type())
        value = export_ArrayT<ArrayByte4, SRF_ArrayInteger>(arr_abstract, "<ArrayByte4>");
      else

      if(arr_abstract->classType() == ArrayFloat1::Type())
        value = export_ArrayT<ArrayFloat1, SRF_ArrayReal>(arr_abstract, "<ArrayFloat1>");
      else
      if(arr_abstract->classType() == ArrayFloat2::Type())
        value = export_ArrayT<ArrayFloat2, SRF_ArrayReal>(arr_abstract, "<ArrayFloat2>");
      else
      if(arr_abstract->classType() == ArrayFloat3::Type())
        value = export_ArrayT<ArrayFloat3, SRF_ArrayReal>(arr_abstract, "<ArrayFloat3>");
      else
      if(arr_abstract->classType() == ArrayFloat4::Type())
        value = export_ArrayT<ArrayFloat4, SRF_ArrayReal>(arr_abstract, "<ArrayFloat4>");
      else

      if(arr_abstract->classType() == ArrayDouble1::Type())
        value = export_ArrayT<ArrayDouble1, SRF_ArrayReal>(arr_abstract, "<ArrayDouble1>");
      else
      if(arr_abstract->classType() == ArrayDouble2::Type())
        value = export_ArrayT<ArrayDouble2, SRF_ArrayReal>(arr_abstract, "<ArrayDouble2>");
      else
      if(arr_abstract->classType() == ArrayDouble3::Type())
        value = export_ArrayT<ArrayDouble3, SRF_ArrayReal>(arr_abstract, "<ArrayDouble3>");
      else
      if(arr_abstract->classType() == ArrayDouble4::Type())
        value = export_ArrayT<ArrayDouble4, SRF_ArrayReal>(arr_abstract, "<ArrayDouble4>");
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

      srf_obj->value().push_back( SRF_Structure::Value("PrimitiveType", toIdentifier(primitive_type) ) );
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
      srf_obj->value().push_back( SRF_Structure::Value("PatchDefaultOuterLevel", toValue(pp->patchDefaultOuterLevel())) );
      srf_obj->value().push_back( SRF_Structure::Value("PatchDefaultInnerLevel", toValue(pp->patchDefaultInnerLevel())) );

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
      value.setArray( new SRF_ArrayInteger );
      value.getArrayInteger()->value().resize( vec.size() );
      if (vec.size())
        value.getArrayInteger()->copyFrom(&vec[0]);
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
        if (srf_obj->value()[0].key() != "SerializerVersion" || srf_obj->value()[0].value().type() != SRF_Value::Integer)
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

    fvec2 to_fvec2(const SRF_ArrayReal* arr) { VL_CHECK(arr->value().size() == 2); return fvec2( (float)arr->value()[0], (float)arr->value()[1] ); }

    fvec3 to_fvec3(const SRF_ArrayReal* arr) { VL_CHECK(arr->value().size() == 3); return fvec3( (float)arr->value()[0], (float)arr->value()[1], (float)arr->value()[2] ); }

    fvec4 to_fvec4(const SRF_ArrayReal* arr) { VL_CHECK(arr->value().size() == 4); return fvec4( (float)arr->value()[0], (float)arr->value()[1], (float)arr->value()[2], (float)arr->value()[3] ); }

    bool import_AABB(const SRF_Structure* srf_obj, AABB& aabb)
    {
      VL_CHECK( srf_obj->tag() == "<AABB>" )

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const SRF_Value& value = srf_obj->value()[i].value();
        if (key == "MinCorner")
        {
          VL_CHECK(value.type() == SRF_Value::ArrayReal)
          aabb.setMinCorner( to_fvec3(value.getArrayReal()) );
        }
        else
        if (key == "MaxCorner")
        {
          VL_CHECK(value.type() == SRF_Value::ArrayReal)
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

    bool import_Sphere(const SRF_Structure* srf_obj, Sphere& sphere)
    {
      VL_CHECK( srf_obj->tag() == "<Sphere>" )

      for(size_t i=0; i<srf_obj->value().size(); ++i)
      {
        const std::string& key = srf_obj->value()[i].key();
        const SRF_Value& value = srf_obj->value()[i].value();
        if (key == "Center")
        {
          VL_CHECK(value.type() == SRF_Value::ArrayReal)
          sphere.setCenter( to_fvec3(value.getArrayReal()) );
        }
        else
        if (key == "Radius")
        {
          VL_CHECK(value.type() == SRF_Value::Real)
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
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayReal);
        const SRF_ArrayReal* srf_arr_float = value.getArrayReal();
        ref<ArrayFloat1> arr_float1 = new ArrayFloat1; arr_abstract = arr_float1;
        arr_float1->resize( srf_arr_float->value().size() );
        srf_arr_float->copyTo((float*)arr_float1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayFloat2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayReal);
        const SRF_ArrayReal* srf_arr_float = value.getArrayReal();
        VL_SRF_CHECK_ERROR( srf_arr_float->value().size() % 2 == 0)
        ref<ArrayFloat2> arr_float2 = new ArrayFloat2; arr_abstract = arr_float2;
        arr_float2->resize( srf_arr_float->value().size() / 2 );
        srf_arr_float->copyTo((float*)arr_float2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayFloat3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayReal);
        const SRF_ArrayReal* srf_arr_float = value.getArrayReal();
        VL_SRF_CHECK_ERROR( srf_arr_float->value().size() % 3 == 0)
        ref<ArrayFloat3> arr_float3 = new ArrayFloat3; arr_abstract = arr_float3;
        arr_float3->resize( srf_arr_float->value().size() / 3 );
        srf_arr_float->copyTo((float*)arr_float3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayFloat4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayReal);
        const SRF_ArrayReal* srf_arr_float = value.getArrayReal();
        VL_SRF_CHECK_ERROR( srf_arr_float->value().size() % 4 == 0)
        ref<ArrayFloat4> arr_float4 = new ArrayFloat4; arr_abstract = arr_float4;
        arr_float4->resize( srf_arr_float->value().size() / 4 );
        srf_arr_float->copyTo((float*)arr_float4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayReal);
        const SRF_ArrayReal* srf_arr_floating = value.getArrayReal();
        ref<ArrayDouble1> arr_floating1 = new ArrayDouble1; arr_abstract = arr_floating1;
        arr_floating1->resize( srf_arr_floating->value().size() );
        srf_arr_floating->copyTo((double*)arr_floating1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayReal);
        const SRF_ArrayReal* srf_arr_floating = value.getArrayReal();
        VL_SRF_CHECK_ERROR( srf_arr_floating->value().size() % 2 == 0)
        ref<ArrayDouble2> arr_floating2 = new ArrayDouble2; arr_abstract = arr_floating2;
        arr_floating2->resize( srf_arr_floating->value().size() / 2 );
        srf_arr_floating->copyTo((double*)arr_floating2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayReal);
        const SRF_ArrayReal* srf_arr_floating = value.getArrayReal();
        VL_SRF_CHECK_ERROR( srf_arr_floating->value().size() % 3 == 0)
        ref<ArrayDouble3> arr_floating3 = new ArrayDouble3; arr_abstract = arr_floating3;
        arr_floating3->resize( srf_arr_floating->value().size() / 3 );
        srf_arr_floating->copyTo((double*)arr_floating3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayDouble4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayReal);
        const SRF_ArrayReal* srf_arr_floating = value.getArrayReal();
        VL_SRF_CHECK_ERROR( srf_arr_floating->value().size() % 4 == 0)
        ref<ArrayDouble4> arr_floating4 = new ArrayDouble4; arr_abstract = arr_floating4;
        arr_floating4->resize( srf_arr_floating->value().size() / 4 );
        srf_arr_floating->copyTo((double*)arr_floating4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayInt1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayInt1> arr_int1 = new ArrayInt1; arr_abstract = arr_int1;
        arr_int1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((int*)arr_int1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayInt2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayInt2> arr_int2 = new ArrayInt2; arr_abstract = arr_int2;
        arr_int2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((int*)arr_int2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayInt3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayInt3> arr_int3 = new ArrayInt3; arr_abstract = arr_int3;
        arr_int3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((int*)arr_int3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayInt4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayInt4> arr_int4 = new ArrayInt4; arr_abstract = arr_int4;
        arr_int4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((int*)arr_int4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayUInt1> arr_int1 = new ArrayUInt1; arr_abstract = arr_int1;
        arr_int1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((unsigned int*)arr_int1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayUInt2> arr_int2 = new ArrayUInt2; arr_abstract = arr_int2;
        arr_int2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((unsigned int*)arr_int2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayUInt3> arr_int3 = new ArrayUInt3; arr_abstract = arr_int3;
        arr_int3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((unsigned int*)arr_int3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUInt4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayUInt4> arr_int4 = new ArrayUInt4; arr_abstract = arr_int4;
        arr_int4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((unsigned int*)arr_int4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayShort1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayShort1> arr_short1 = new ArrayShort1; arr_abstract = arr_short1;
        arr_short1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((short*)arr_short1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayShort2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayShort2> arr_short2 = new ArrayShort2; arr_abstract = arr_short2;
        arr_short2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((short*)arr_short2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayShort3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayShort3> arr_short3 = new ArrayShort3; arr_abstract = arr_short3;
        arr_short3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((short*)arr_short3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayShort4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayShort4> arr_short4 = new ArrayShort4; arr_abstract = arr_short4;
        arr_short4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((short*)arr_short4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUShort1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayUShort1> arr_short1 = new ArrayUShort1; arr_abstract = arr_short1;
        arr_short1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((unsigned short*)arr_short1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUShort2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayUShort2> arr_short2 = new ArrayUShort2; arr_abstract = arr_short2;
        arr_short2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((unsigned short*)arr_short2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUShort3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayUShort3> arr_short3 = new ArrayUShort3; arr_abstract = arr_short3;
        arr_short3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((unsigned short*)arr_short3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUShort4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayUShort4> arr_short4 = new ArrayUShort4; arr_abstract = arr_short4;
        arr_short4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((unsigned short*)arr_short4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayByte1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayByte1> arr_byte1 = new ArrayByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((char*)arr_byte1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayByte2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayByte2> arr_byte2 = new ArrayByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((char*)arr_byte2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayByte3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayByte3> arr_byte3 = new ArrayByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((char*)arr_byte3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayByte4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayByte4> arr_byte4 = new ArrayByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((char*)arr_byte4->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUByte1>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        ref<ArrayUByte1> arr_byte1 = new ArrayUByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( srf_arr_int->value().size() );
        srf_arr_int->copyTo((unsigned char*)arr_byte1->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUByte2>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 2 == 0)
        ref<ArrayUByte2> arr_byte2 = new ArrayUByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( srf_arr_int->value().size() / 2 );
        srf_arr_int->copyTo((unsigned char*)arr_byte2->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUByte3>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 3 == 0)
        ref<ArrayUByte3> arr_byte3 = new ArrayUByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( srf_arr_int->value().size() / 3 );
        srf_arr_int->copyTo((unsigned char*)arr_byte3->ptr());
      }
      else
      if (srf_obj->tag() == "<ArrayUByte4>")
      {
        VL_SRF_CHECK_ERROR(value.type() == SRF_Value::ArrayInteger);
        const SRF_ArrayInteger* srf_arr_int = value.getArrayInteger();
        VL_SRF_CHECK_ERROR( srf_arr_int->value().size() % 4 == 0)
        ref<ArrayUByte4> arr_byte4 = new ArrayUByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( srf_arr_int->value().size() / 4 );
        srf_arr_int->copyTo((unsigned char*)arr_byte4->ptr());
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
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Integer )
            de->setInstances( (int)value.getInteger() );
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
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Integer )
            de->setBaseVertex( (int)value.getInteger() );
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
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::ArrayInteger )
            de->baseVertices().resize( value.getArrayInteger()->value().size() );
            if (de->baseVertices().size())
              value.getArrayInteger()->copyTo( &de->baseVertices()[0] );
            // de->setBaseVertices( value.getArrayInt32()->value() );
          }
          else
          if( key == "CountVector" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::ArrayInteger )
            de->countVector().resize( value.getArrayInteger()->value().size() );
            if (de->countVector().size())
              value.getArrayInteger()->copyTo( &de->countVector()[0] );
            // de->countVector() = value.getArrayInt32()->value();
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
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Integer )
            da->setInstances( (int)value.getInteger() );
          }
          else
          if( key == "Start" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Integer )
            da->setStart( (int)value.getInteger() );
          }
          else
          if( key == "Count" )
          {
            VL_SRF_CHECK_ERROR( value.type() == SRF_Value::Integer )
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
