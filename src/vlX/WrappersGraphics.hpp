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

#ifndef VLXWrapper_Graphics_INCLUDE_ONCE
#define VLXWrapper_Graphics_INCLUDE_ONCE

#include <vlX/WrappersCore.hpp>
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

namespace vlX
{
  /** VLX wrapper of vl::Array */
  struct VLXClassWrapper_Array: public ClassWrapper
  {
    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      if (!vlx->getValue("Value"))
      {
        vl::Log::error( vl::Say("Line %n : error. 'VLXValue' expected in object '%s'. \n") << vlx->lineNumber() << vlx->tag() );
        return NULL;
      }

      const VLXValue& value = *vlx->getValue("Value");

      vl::ref<vl::ArrayAbstract> arr_abstract;

      if (vlx->tag() == "<vl::ArrayFloat1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayReal, value);
        const VLXArrayReal* vlx_arr_float = value.getArrayReal();
        vl::ref<vl::ArrayFloat1> arr_float1 = new vl::ArrayFloat1; arr_abstract = arr_float1;
        arr_float1->resize( vlx_arr_float->value().size() );
        vlx_arr_float->copyTo((float*)arr_float1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayFloat2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayReal, value);
        const VLXArrayReal* vlx_arr_float = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_float->value().size() % 2 == 0, value)
        vl::ref<vl::ArrayFloat2> arr_float2 = new vl::ArrayFloat2; arr_abstract = arr_float2;
        arr_float2->resize( vlx_arr_float->value().size() / 2 );
        vlx_arr_float->copyTo((float*)arr_float2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayFloat3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayReal, value);
        const VLXArrayReal* vlx_arr_float = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_float->value().size() % 3 == 0, value)
        vl::ref<vl::ArrayFloat3> arr_float3 = new vl::ArrayFloat3; arr_abstract = arr_float3;
        arr_float3->resize( vlx_arr_float->value().size() / 3 );
        vlx_arr_float->copyTo((float*)arr_float3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayFloat4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayReal, value);
        const VLXArrayReal* vlx_arr_float = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_float->value().size() % 4 == 0, value)
        vl::ref<vl::ArrayFloat4> arr_float4 = new vl::ArrayFloat4; arr_abstract = arr_float4;
        arr_float4->resize( vlx_arr_float->value().size() / 4 );
        vlx_arr_float->copyTo((float*)arr_float4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayDouble1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayReal, value);
        const VLXArrayReal* vlx_arr_floating = value.getArrayReal();
        vl::ref<vl::ArrayDouble1> arr_floating1 = new vl::ArrayDouble1; arr_abstract = arr_floating1;
        arr_floating1->resize( vlx_arr_floating->value().size() );
        vlx_arr_floating->copyTo((double*)arr_floating1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayDouble2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayReal, value);
        const VLXArrayReal* vlx_arr_floating = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_floating->value().size() % 2 == 0, value)
        vl::ref<vl::ArrayDouble2> arr_floating2 = new vl::ArrayDouble2; arr_abstract = arr_floating2;
        arr_floating2->resize( vlx_arr_floating->value().size() / 2 );
        vlx_arr_floating->copyTo((double*)arr_floating2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayDouble3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayReal, value);
        const VLXArrayReal* vlx_arr_floating = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_floating->value().size() % 3 == 0, value)
        vl::ref<vl::ArrayDouble3> arr_floating3 = new vl::ArrayDouble3; arr_abstract = arr_floating3;
        arr_floating3->resize( vlx_arr_floating->value().size() / 3 );
        vlx_arr_floating->copyTo((double*)arr_floating3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayDouble4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayReal, value);
        const VLXArrayReal* vlx_arr_floating = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_floating->value().size() % 4 == 0, value)
        vl::ref<vl::ArrayDouble4> arr_floating4 = new vl::ArrayDouble4; arr_abstract = arr_floating4;
        arr_floating4->resize( vlx_arr_floating->value().size() / 4 );
        vlx_arr_floating->copyTo((double*)arr_floating4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayInt1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        vl::ref<vl::ArrayInt1> arr_int1 = new vl::ArrayInt1; arr_abstract = arr_int1;
        arr_int1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((int*)arr_int1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayInt2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        vl::ref<vl::ArrayInt2> arr_int2 = new vl::ArrayInt2; arr_abstract = arr_int2;
        arr_int2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((int*)arr_int2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayInt3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        vl::ref<vl::ArrayInt3> arr_int3 = new vl::ArrayInt3; arr_abstract = arr_int3;
        arr_int3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((int*)arr_int3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayInt4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        vl::ref<vl::ArrayInt4> arr_int4 = new vl::ArrayInt4; arr_abstract = arr_int4;
        arr_int4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((int*)arr_int4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUInt1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        vl::ref<vl::ArrayUInt1> arr_int1 = new vl::ArrayUInt1; arr_abstract = arr_int1;
        arr_int1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((unsigned int*)arr_int1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUInt2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        vl::ref<vl::ArrayUInt2> arr_int2 = new vl::ArrayUInt2; arr_abstract = arr_int2;
        arr_int2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((unsigned int*)arr_int2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUInt3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        vl::ref<vl::ArrayUInt3> arr_int3 = new vl::ArrayUInt3; arr_abstract = arr_int3;
        arr_int3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((unsigned int*)arr_int3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUInt4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        vl::ref<vl::ArrayUInt4> arr_int4 = new vl::ArrayUInt4; arr_abstract = arr_int4;
        arr_int4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((unsigned int*)arr_int4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayShort1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        vl::ref<vl::ArrayShort1> arr_short1 = new vl::ArrayShort1; arr_abstract = arr_short1;
        arr_short1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((short*)arr_short1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayShort2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        vl::ref<vl::ArrayShort2> arr_short2 = new vl::ArrayShort2; arr_abstract = arr_short2;
        arr_short2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((short*)arr_short2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayShort3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        vl::ref<vl::ArrayShort3> arr_short3 = new vl::ArrayShort3; arr_abstract = arr_short3;
        arr_short3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((short*)arr_short3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayShort4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        vl::ref<vl::ArrayShort4> arr_short4 = new vl::ArrayShort4; arr_abstract = arr_short4;
        arr_short4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((short*)arr_short4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUShort1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        vl::ref<vl::ArrayUShort1> arr_short1 = new vl::ArrayUShort1; arr_abstract = arr_short1;
        arr_short1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((unsigned short*)arr_short1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUShort2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        vl::ref<vl::ArrayUShort2> arr_short2 = new vl::ArrayUShort2; arr_abstract = arr_short2;
        arr_short2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((unsigned short*)arr_short2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUShort3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        vl::ref<vl::ArrayUShort3> arr_short3 = new vl::ArrayUShort3; arr_abstract = arr_short3;
        arr_short3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((unsigned short*)arr_short3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUShort4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        vl::ref<vl::ArrayUShort4> arr_short4 = new vl::ArrayUShort4; arr_abstract = arr_short4;
        arr_short4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((unsigned short*)arr_short4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayByte1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        vl::ref<vl::ArrayByte1> arr_byte1 = new vl::ArrayByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((char*)arr_byte1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayByte2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        vl::ref<vl::ArrayByte2> arr_byte2 = new vl::ArrayByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((char*)arr_byte2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayByte3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        vl::ref<vl::ArrayByte3> arr_byte3 = new vl::ArrayByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((char*)arr_byte3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayByte4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        vl::ref<vl::ArrayByte4> arr_byte4 = new vl::ArrayByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((char*)arr_byte4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUByte1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        vl::ref<vl::ArrayUByte1> arr_byte1 = new vl::ArrayUByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((unsigned char*)arr_byte1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUByte2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        vl::ref<vl::ArrayUByte2> arr_byte2 = new vl::ArrayUByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((unsigned char*)arr_byte2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUByte3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        vl::ref<vl::ArrayUByte3> arr_byte3 = new vl::ArrayUByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((unsigned char*)arr_byte3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUByte4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLXValue::ArrayInteger, value);
        const VLXArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        vl::ref<vl::ArrayUByte4> arr_byte4 = new vl::ArrayUByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((unsigned char*)arr_byte4->ptr());
      }
      else
      {
        s.signalImportError( vl::Say("Line %n : unknown array '%s'.\n") << vlx->lineNumber() << vlx->tag() );
      }

      // register imported structure asap
      s.registerImportedStructure(vlx, arr_abstract.get());
      return arr_abstract.get();
    }

    template<typename T_Array, typename T_VLXArray>
    vl::ref<VLXStructure> export_ArrayT(VLXSerializer& s, const vl::Object* arr_abstract)
    {
      const T_Array* arr = arr_abstract->as<T_Array>();
      vl::ref<VLXStructure> st =new VLXStructure(vlx_makeTag(arr_abstract).c_str(), s.generateID("array_"));
      vl::ref<T_VLXArray> vlx_array = new T_VLXArray;
      if (arr->size())
      {
        vlx_array->value().resize( arr->size() * arr->glSize() );
        typename T_VLXArray::scalar_type* dst = &vlx_array->value()[0];
        const typename T_Array::scalar_type* src = (const typename T_Array::scalar_type*)arr->begin();
        const typename T_Array::scalar_type* end = (const typename T_Array::scalar_type*)arr->end();
        for(; src<end; ++src, ++dst)
          *dst = (typename T_VLXArray::scalar_type)*src;
      }
      st->value().push_back( VLXStructure::KeyValue("Value", vlx_array.get() ) );
      return st;
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      vl::ref<VLXStructure> vlx;
      if(obj->classType() == vl::ArrayUInt1::Type())
        vlx = export_ArrayT<vl::ArrayUInt1, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUInt2::Type())
        vlx = export_ArrayT<vl::ArrayUInt2, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUInt3::Type())
        vlx = export_ArrayT<vl::ArrayUInt3, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUInt4::Type())
        vlx = export_ArrayT<vl::ArrayUInt4, VLXArrayInteger>(s, obj);
      else

      if(obj->classType() == vl::ArrayInt1::Type())
        vlx = export_ArrayT<vl::ArrayInt1, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayInt2::Type())
        vlx = export_ArrayT<vl::ArrayInt2, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayInt3::Type())
        vlx = export_ArrayT<vl::ArrayInt3, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayInt4::Type())
        vlx = export_ArrayT<vl::ArrayInt4, VLXArrayInteger>(s, obj);
      else

      if(obj->classType() == vl::ArrayUShort1::Type())
        vlx = export_ArrayT<vl::ArrayUShort1, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUShort2::Type())
        vlx = export_ArrayT<vl::ArrayUShort2, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUShort3::Type())
        vlx = export_ArrayT<vl::ArrayUShort3, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUShort4::Type())
        vlx = export_ArrayT<vl::ArrayUShort4, VLXArrayInteger>(s, obj);
      else

      if(obj->classType() == vl::ArrayUShort1::Type())
        vlx = export_ArrayT<vl::ArrayUShort1, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUShort2::Type())
        vlx = export_ArrayT<vl::ArrayUShort2, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUShort3::Type())
        vlx = export_ArrayT<vl::ArrayUShort3, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUShort4::Type())
        vlx = export_ArrayT<vl::ArrayUShort4, VLXArrayInteger>(s, obj);
      else

      if(obj->classType() == vl::ArrayShort1::Type())
        vlx = export_ArrayT<vl::ArrayShort1, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayShort2::Type())
        vlx = export_ArrayT<vl::ArrayShort2, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayShort3::Type())
        vlx = export_ArrayT<vl::ArrayShort3, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayShort4::Type())
        vlx = export_ArrayT<vl::ArrayShort4, VLXArrayInteger>(s, obj);
      else

      if(obj->classType() == vl::ArrayUByte1::Type())
        vlx = export_ArrayT<vl::ArrayUByte1, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUByte2::Type())
        vlx = export_ArrayT<vl::ArrayUByte2, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUByte3::Type())
        vlx = export_ArrayT<vl::ArrayUByte3, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayUByte4::Type())
        vlx = export_ArrayT<vl::ArrayUByte4, VLXArrayInteger>(s, obj);
      else

      if(obj->classType() == vl::ArrayByte1::Type())
        vlx = export_ArrayT<vl::ArrayByte1, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayByte2::Type())
        vlx = export_ArrayT<vl::ArrayByte2, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayByte3::Type())
        vlx = export_ArrayT<vl::ArrayByte3, VLXArrayInteger>(s, obj);
      else
      if(obj->classType() == vl::ArrayByte4::Type())
        vlx = export_ArrayT<vl::ArrayByte4, VLXArrayInteger>(s, obj);
      else

      if(obj->classType() == vl::ArrayFloat1::Type())
        vlx = export_ArrayT<vl::ArrayFloat1, VLXArrayReal>(s, obj);
      else
      if(obj->classType() == vl::ArrayFloat2::Type())
        vlx = export_ArrayT<vl::ArrayFloat2, VLXArrayReal>(s, obj);
      else
      if(obj->classType() == vl::ArrayFloat3::Type())
        vlx = export_ArrayT<vl::ArrayFloat3, VLXArrayReal>(s, obj);
      else
      if(obj->classType() == vl::ArrayFloat4::Type())
        vlx = export_ArrayT<vl::ArrayFloat4, VLXArrayReal>(s, obj);
      else

      if(obj->classType() == vl::ArrayDouble1::Type())
        vlx = export_ArrayT<vl::ArrayDouble1, VLXArrayReal>(s, obj);
      else
      if(obj->classType() == vl::ArrayDouble2::Type())
        vlx = export_ArrayT<vl::ArrayDouble2, VLXArrayReal>(s, obj);
      else
      if(obj->classType() == vl::ArrayDouble3::Type())
        vlx = export_ArrayT<vl::ArrayDouble3, VLXArrayReal>(s, obj);
      else
      if(obj->classType() == vl::ArrayDouble4::Type())
        vlx = export_ArrayT<vl::ArrayDouble4, VLXArrayReal>(s, obj);
      else
      {
        s.signalExportError("Array type not supported for export.\n");
      }

      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Renderable */
  struct VLXClassWrapper_Renderable: public ClassWrapper
  {
    virtual void exportRenderable(const vl::Renderable* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      *vlx << "BufferObjectEnabled" << obj->isBufferObjectEnabled();
      *vlx << "DisplayListEnabled" << obj->isDisplayListEnabled();
      if (!obj->boundsDirty())
      {
        *vlx << "AABB" << export_AABB(obj->boundingBox());
        *vlx << "Sphere" << export_Sphere(obj->boundingSphere());
      }
      else
        vl::Log::debug("VLXClassWrapper_Renderable : skipping dirty bounds.\n");
    }

    void importRenderable(const VLXStructure* vlx, vl::Renderable* ren)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        ren->setObjectName( name->getString() );

      const std::vector<VLXStructure::KeyValue>& values = vlx->value();
      for(size_t i=0; i<values.size(); ++i)
      {
        const std::string& key = values[i].key();
        if (key == "BufferObjectEnabled")
        {
          ren->setBufferObjectEnabled( values[i].value().getBool() );
        }
        else
        if (key == "DisplayListEnabled")
        {
          ren->setDisplayListEnabled( values[i].value().getBool() );
        }
        else
        if (key == "AABB")
        {
          ren->setBoundingBox( import_AABB(values[i].value().getStructure()) );
        }
        else
        if (key == "Sphere")
        {
          ren->setBoundingSphere( import_Sphere(values[i].value().getStructure()) );
        }
      }
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Geometry */
  struct VLXClassWrapper_Geometry: public VLXClassWrapper_Renderable
  {
    void importGeometry(VLXSerializer& s, const VLXStructure* vlx, vl::Geometry* geom)
    {
      VLXClassWrapper_Renderable::importRenderable(vlx, geom);

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();

        if (key == "VertexArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLXValue::Structure, value)
          vl::ArrayAbstract* arr = s.importVLX(value.getStructure())->as<vl::ArrayAbstract>();
          if (arr)
            geom->setVertexArray(arr);
          else
            s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "NormalArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLXValue::Structure, value)
          vl::ArrayAbstract* arr = s.importVLX(value.getStructure())->as<vl::ArrayAbstract>();
          if (arr)
            geom->setNormalArray(arr);
          else
            s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "ColorArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLXValue::Structure, value)
          vl::ArrayAbstract* arr = s.importVLX(value.getStructure())->as<vl::ArrayAbstract>();
          if (arr)
            geom->setColorArray(arr);
          else
            s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "SecondaryColorArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLXValue::Structure, value)
          vl::ArrayAbstract* arr = s.importVLX(value.getStructure())->as<vl::ArrayAbstract>();
          if (arr)
            geom->setSecondaryColorArray(arr);
          else
            s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "FogCoordArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLXValue::Structure, value)
          vl::ArrayAbstract* arr = s.importVLX(value.getStructure())->as<vl::ArrayAbstract>();
          if (arr)
            geom->setFogCoordArray(arr);
          else
            s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
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
              vl::Log::error( vl::Say("Line %n : error. ") << value.lineNumber() );
              vl::Log::error( "TexCoordArray must end with a number!\n" );
              s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
            }
          }

          VLX_IMPORT_CHECK_RETURN(value.type() == VLXValue::Structure, value)
          vl::ArrayAbstract* arr = s.importVLX(value.getStructure())->as<vl::ArrayAbstract>();
          if (arr)
            geom->setTexCoordArray(tex_unit, arr);
          else
            s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (strstr(key.c_str(), "VertexAttribArray") == key.c_str())
        {
          // MIC FIXME
          //const char* ch = key.c_str() + 17/*strlen("VertexAttribArray")*/;
          //int attrib_location = 0;
          //for(; *ch; ++ch)
          //{
          //  if (*ch>='0' && *ch<='9')
          //    attrib_location = attrib_location*10 + (*ch - '0');
          //  else
          //  {
          //    vl::Log::error( vl::Say("Line %n : error. ") << value.lineNumber() );
          //    vl::Log::error( "VertexAttribArray must end with a number!\n" );
          //    s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
          //  }
          //}

          //VLX_IMPORT_CHECK_RETURN(value.type() == VLXValue::Structure, value)
          //VertexAttribInfo* info_ptr = s.importVLX(value.getStructure())->as<vl::VertexAttribInfo>();
          //if (info_ptr)
          //{
          //  VertexAttribInfo info = *info_ptr;
          //  info.setAttribLocation(attrib_location);
          //  geom->setVertexAttribArray(info);
          //}
          //else
          //  s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "DrawCall")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLXValue::Structure, value)
          vl::DrawCall* draw_call = s.importVLX(value.getStructure())->as<vl::DrawCall>();
          if (draw_call)
            geom->drawCalls().push_back(draw_call);
          else
            s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Geometry> geom = new vl::Geometry;
      // registration must be done here to avoid loops
      s.registerImportedStructure(vlx, geom.get());
      importGeometry(s, vlx, geom.get());
      return geom;
    }

    void exportGeometry(VLXSerializer& s, const vl::Geometry* geom, VLXStructure* vlx)
    {
      // vl::Renderable
      VLXClassWrapper_Renderable::exportRenderable(geom, vlx);

      // vl::Geometry
      if (geom->vertexArray())
        *vlx << "VertexArray" << s.exportVLX(geom->vertexArray());

      if (geom->normalArray())
        *vlx << "NormalArray" << s.exportVLX(geom->normalArray());

      if (geom->colorArray())
        *vlx << "ColorArray" << s.exportVLX(geom->colorArray());

      if (geom->secondaryColorArray())
        *vlx << "SecondaryColorArray" << s.exportVLX(geom->secondaryColorArray());

      if (geom->fogCoordArray())
        *vlx << "FogCoordArray" << s.exportVLX(geom->fogCoordArray());

      for( int i=0; i<vl::VA_MaxTexCoordCount; ++i)
      {
        if (geom->texCoordArray(i))
        {
          std::string tex_coord_array = vl::String::printf("TexCoordArray%d", i).toStdString();
          *vlx << tex_coord_array.c_str() << s.exportVLX(geom->texCoordArray(i));
        }
      }

      // MIC FIXME: unify with above
      //for(size_t i=0; i<VA_MaxAttribCount; ++i)
      //{
      //  if (geom->vertexAttribArray(i))
      //  {
      //    std::string vertex_attrib_array = vl::String::printf("VertexAttribArray%d", i).toStdString();
      //    *vlx << vertex_attrib_array.c_str() << s.exportVLX(geom->vertexAttribArray(i));
      //  }
      //}

      for(size_t i=0; i<geom->drawCalls().size(); ++i) {
        *vlx << "DrawCall" << s.exportVLX(geom->drawCalls().at(i));
      }
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Geometry* cast_obj = obj->as<vl::Geometry>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("geometry_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportGeometry(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::DrawCall and subclasses. */
  struct VLXClassWrapper_DrawCall: public ClassWrapper
  {
    void importDrawCall(VLXSerializer& s, const VLXStructure* vlx, vl::DrawCall* draw_call)
    {
      if(draw_call->isOfType(vl::DrawElementsBase::Type()))
      {
        vl::DrawElementsBase* de= draw_call->as<vl::DrawElementsBase>();

        const VLXValue* name = vlx->getValue("ObjectName");
        if (name)
          de->setObjectName( name->getString() );

        for(size_t i=0; i<vlx->value().size(); ++i)
        {
          const std::string& key = vlx->value()[i].key();
          const VLXValue& value = vlx->value()[i].value();
          if( key == "PrimitiveType" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier , value)
            de->setPrimitiveType( vlx_EPrimitiveType( value, s ) );
            VLX_IMPORT_CHECK_RETURN( de->primitiveType() != vl::PT_UNKNOWN , value);
          }
          else
          if( key == "Enabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool , value)
            de->setEnabled( value.getBool() );
          }
          else
          if( key == "Instances" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer , value)
            de->setInstances( (int)value.getInteger() );
          }
          else
          if( key == "PrimitiveRestartEnabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool , value)
            de->setPrimitiveRestartEnabled( value.getBool() );
          }
          else
          if( key == "BaseVertex" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer , value)
            de->setBaseVertex( (int)value.getInteger() );
          }
          else
          if( key == "IndexBuffer" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure , value)
            vl::ArrayAbstract* arr_abstract = s.importVLX(value.getStructure())->as<vl::ArrayAbstract>();
            if(!arr_abstract)
              s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );

            if ( de->isOfType(vl::DrawElementsUInt::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == vl::ArrayUInt1::Type(), value);
              de->as<vl::DrawElementsUInt>()->setIndexBuffer( arr_abstract->as<vl::ArrayUInt1>() );
            }
            else
            if ( de->isOfType(vl::DrawElementsUShort::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == vl::ArrayUShort1::Type(), value);
              de->as<vl::DrawElementsUShort>()->setIndexBuffer( arr_abstract->as<vl::ArrayUShort1>() );
            }
            else
            if ( de->isOfType(vl::DrawElementsUByte::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == vl::ArrayUByte1::Type(), value);
              de->as<vl::DrawElementsUByte>()->setIndexBuffer( arr_abstract->as<vl::ArrayUByte1>() );
            }
          }
        }
      }
      else
      if(draw_call->isOfType(vl::MultiDrawElementsBase::Type()))
      {
        vl::MultiDrawElementsBase* de = draw_call->as<vl::MultiDrawElementsBase>();

        VL_CHECK(de)
        VL_CHECK(draw_call)

        const VLXValue* name = vlx->getValue("ObjectName");
        if (name)
          de->setObjectName( name->getString() );

        for(size_t i=0; i<vlx->value().size(); ++i)
        {
          const std::string& key = vlx->value()[i].key();
          const VLXValue& value = vlx->value()[i].value();
          if( key == "PrimitiveType" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier , value)
            de->setPrimitiveType( vlx_EPrimitiveType( value, s ) );
            VLX_IMPORT_CHECK_RETURN( de->primitiveType() != vl::PT_UNKNOWN , value);
          }
          else
          if( key == "Enabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool , value)
            de->setEnabled( value.getBool() );
          }
          else
          if( key == "PrimitiveRestartEnabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool , value)
            de->setPrimitiveRestartEnabled( value.getBool() );
          }
          else
          if( key == "BaseVertices" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayInteger , value)
            de->baseVertices().resize( value.getArrayInteger()->value().size() );
            if (de->baseVertices().size())
              value.getArrayInteger()->copyTo( &de->baseVertices()[0] );
            // de->setBaseVertices( value.getArrayInt32()->value() );
          }
          else
          if( key == "CountVector" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayInteger , value)
            de->countVector().resize( value.getArrayInteger()->value().size() );
            if (de->countVector().size())
              value.getArrayInteger()->copyTo( &de->countVector()[0] );
            // de->countVector() = value.getArrayInt32()->value();
          }
          else
          if( key == "IndexBuffer" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure , value)
            vl::ArrayAbstract* arr_abstract = s.importVLX(value.getStructure())->as<vl::ArrayAbstract>();
            if( !arr_abstract )
              s.signalImportError( vl::Say("Line %n : import error.\n") << value.lineNumber() );

            if ( de->isOfType(vl::MultiDrawElementsUInt::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == vl::ArrayUInt1::Type(), value);
              de->as<vl::MultiDrawElementsUInt>()->setIndexBuffer( arr_abstract->as<vl::ArrayUInt1>() );
            }
            else
            if ( de->isOfType(vl::MultiDrawElementsUShort::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == vl::ArrayUShort1::Type(), value);
              de->as<vl::MultiDrawElementsUShort>()->setIndexBuffer( arr_abstract->as<vl::ArrayUShort1>() );
            }
            else
            if ( de->isOfType(vl::MultiDrawElementsUByte::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == vl::ArrayUByte1::Type(), value);
              de->as<vl::MultiDrawElementsUByte>()->setIndexBuffer( arr_abstract->as<vl::ArrayUByte1>() );
            }
          }
        }

        // finalize setup
        de->computePointerVector();
        de->computeBufferObjectPointerVector();
        if ( de->baseVertices().size() != de->countVector().size() )
          de->baseVertices().resize( de->countVector().size() );
      }
      else
      if( draw_call->isOfType(vl::DrawArrays::Type()) )
      {
        vl::ref<vl::DrawArrays> da = draw_call->as<vl::DrawArrays>();

        const VLXValue* name = vlx->getValue("ObjectName");
        if (name)
          da->setObjectName( name->getString() );

        for(size_t i=0; i<vlx->value().size(); ++i)
        {
          const std::string& key = vlx->value()[i].key();
          const VLXValue& value = vlx->value()[i].value();

          if( key == "PrimitiveType" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier , value)
            da->setPrimitiveType( vlx_EPrimitiveType( value, s ) );
            VLX_IMPORT_CHECK_RETURN( da->primitiveType() != vl::PT_UNKNOWN , value);
          }
          else
          if( key == "Enabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool , value)
            da->setEnabled( value.getBool() );
          }
          else
          if( key == "Instances" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer , value)
            da->setInstances( (int)value.getInteger() );
          }
          else
          if( key == "Start" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer , value)
            da->setStart( (int)value.getInteger() );
          }
          else
          if( key == "Count" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer , value)
            da->setCount( (int)value.getInteger() );
          }
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::DrawCall> dc;
      if (vlx->tag() == "<vl::DrawElementsUInt>")
        dc = new vl::DrawElementsUInt;
      else
      if (vlx->tag() == "<vl::DrawElementsUShort>")
        dc = new vl::DrawElementsUShort;
      else
      if (vlx->tag() == "<vl::DrawElementsUByte>")
        dc = new vl::DrawElementsUByte;
      else
      if (vlx->tag() == "<vl::MultiDrawElementsUInt>")
        dc = new vl::MultiDrawElementsUInt;
      else
      if (vlx->tag() == "<vl::MultiDrawElementsUShort>")
        dc = new vl::MultiDrawElementsUShort;
      else
      if (vlx->tag() == "<vl::MultiDrawElementsUByte>")
        dc = new vl::MultiDrawElementsUByte;
      else
      if (vlx->tag() == "<vl::DrawArrays>")
        dc = new vl::DrawArrays;
      else
        s.signalImportError( vl::Say("Line %n : error. Unknown draw call.\n") << vlx->lineNumber() );
      // register imported structure asap
      s.registerImportedStructure(vlx, dc.get());
      importDrawCall(s, vlx, dc.get());
      return dc;
    }

    void exportDrawCallBase(VLXSerializer& s, const vl::DrawCall* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      *vlx << "PrimitiveType" << vlx_Identifier(vlx_EPrimitiveType(obj->primitiveType()));
      *vlx << "Enabled" << obj->isEnabled();
      if (obj->patchParameter())
        *vlx << "PatchParameter" << s.exportVLX(obj->patchParameter());
    }

    void exportDrawCall(VLXSerializer& s, const vl::DrawCall* dcall, VLXStructure* vlx)
    {
      exportDrawCallBase(s, dcall, vlx);

      if (dcall->isOfType(vl::DrawArrays::Type()))
      {
        const vl::DrawArrays* da = dcall->as<vl::DrawArrays>();
        *vlx << "Instances" << (long long)da->instances();
        *vlx << "Start" << (long long)da->start();
        *vlx << "Count" << (long long)da->count();
      }
      else
      if (dcall->isOfType(vl::DrawElementsUInt::Type()))
      {
        const vl::DrawElementsUInt* de = dcall->as<vl::DrawElementsUInt>();
        *vlx << "Instances" << (long long)de->instances();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertex" << (long long)de->baseVertex();
        *vlx << "IndexBuffer" << s.exportVLX(de->indexBuffer());
      }
      else
      if (dcall->isOfType(vl::DrawElementsUShort::Type()))
      {
        const vl::DrawElementsUShort* de = dcall->as<vl::DrawElementsUShort>();
        *vlx << "Instances" << (long long)de->instances();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertex" << (long long)de->baseVertex();
        *vlx << "IndexBuffer" << s.exportVLX(de->indexBuffer());
      }
      else
      if (dcall->isOfType(vl::DrawElementsUByte::Type()))
      {
        const vl::DrawElementsUByte* de = dcall->as<vl::DrawElementsUByte>();
        *vlx << "Instances" << (long long)de->instances();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertex" << (long long)de->baseVertex();
        *vlx << "IndexBuffer" << s.exportVLX(de->indexBuffer());
      }
      else
      if (dcall->isOfType(vl::MultiDrawElementsUInt::Type()))
      {
        const vl::MultiDrawElementsUInt* de = dcall->as<vl::MultiDrawElementsUInt>();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertices" << vlx_toValue(de->baseVertices());
        *vlx << "CountVector" << vlx_toValue(de->countVector());
        *vlx << "IndexBuffer" << s.exportVLX(de->indexBuffer());
      }
      else
      if (dcall->isOfType(vl::MultiDrawElementsUShort::Type()))
      {
        const vl::MultiDrawElementsUShort* de = dcall->as<vl::MultiDrawElementsUShort>();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertices" << vlx_toValue(de->baseVertices());
        *vlx << "CountVector" << vlx_toValue(de->countVector());
        *vlx << "IndexBuffer" << s.exportVLX(de->indexBuffer());
      }
      else
      if (dcall->isOfType(vl::MultiDrawElementsUByte::Type()))
      {
        const vl::MultiDrawElementsUByte* de = dcall->as<vl::MultiDrawElementsUByte>();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertices" << vlx_toValue(de->baseVertices());
        *vlx << "CountVector" << vlx_toValue(de->countVector());
        *vlx << "IndexBuffer" << s.exportVLX(de->indexBuffer());
      }
      else
      {
        vl::Log::error("vl::DrawCall type not supported for export.\n");
      }
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::DrawCall* cast_obj = obj->as<vl::DrawCall>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("drawcall_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportDrawCall(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::PatchParameter */
  struct VLXClassWrapper_PatchParameter: public ClassWrapper
  {
    void importPatchParameter(const VLXStructure* vlx, vl::PatchParameter* pp)
    {
      std::vector<VLXStructure::KeyValue> values = vlx->value();
      for(size_t i=0; i<values.size(); ++i)
      {
        const std::string& key = values[i].key();
        if (key == "PatchVertices")
        {
          pp->setPatchVertices( (int)values[i].value().getInteger() );
        }
        else
        if (key == "PatchDefaultOuterLevel")
        {
          pp->setPatchDefaultOuterLevel( (vl::fvec4)vlx_vec4(values[i].value().getArrayReal()) );
        }
        else
        if (key == "PatchDefaultInnerLevel")
        {
          pp->setPatchDefaultInnerLevel( (vl::fvec2)vlx_vec2(values[i].value().getArrayReal()) );
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::PatchParameter> pp = new vl::PatchParameter;
      // register imported structure asap
      s.registerImportedStructure(vlx, pp.get());
      importPatchParameter(vlx, pp.get());
      return pp;
    }

    void exportPatchParameter(const vl::PatchParameter* pp, VLXStructure* vlx)
    {
      *vlx << "PatchVertices" << (long long)pp->patchVertices();
      *vlx << "PatchDefaultOuterLevel" << vlx_toValue((vl::vec4)pp->patchDefaultOuterLevel());
      *vlx << "PatchDefaultInnerLevel" << vlx_toValue((vl::vec2)pp->patchDefaultInnerLevel());
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::PatchParameter* cast_obj = obj->as<vl::PatchParameter>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("patchparam_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportPatchParameter(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::ResourceDatabase */
  struct VLXClassWrapper_ResourceDatabase: public ClassWrapper
  {
    void importResourceDatabase(VLXSerializer& s, const VLXStructure* vlx, vl::ResourceDatabase* resdb)
    {
      const VLXValue* vlx_obj_name = vlx->getValue("ObjectName");
      if (vlx_obj_name)
        resdb->setObjectName( vlx_obj_name->getString() );

      const VLXValue* vlx_res = vlx->getValue("Resources");
      if (vlx_res)
      {
        VLX_IMPORT_CHECK_RETURN( vlx_res->type() == VLXValue::List, *vlx_res );
        // get the list
        const VLXList* list = vlx_res->getList();
        for(size_t i=0; i<list->value().size(); ++i)
        {
          const VLXValue& value = list->value()[i];

          // the member of this list must be all structures.

          if (value.type() != VLXValue::Structure)
          {
            s.signalImportError( vl::Say("Line %n : structure expected.\n") << value.lineNumber() );
            return;
          }

          resdb->resources().push_back( s.importVLX(value.getStructure()) );
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::ResourceDatabase> resdb = new vl::ResourceDatabase;
      // register imported structure asap
      s.registerImportedStructure(vlx, resdb.get());
      importResourceDatabase(s, vlx, resdb.get());
      return resdb;
    }

    void exportResourceDatabase(VLXSerializer& s, const vl::ResourceDatabase* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      vl::ref<VLXList> list = new VLXList;
      *vlx << "Resources" << list.get();

      for(size_t i=0; i<obj->resources().size(); ++i)
      {
        if ( !s.canExport(obj->resources().at(i).get()) )
        {
          vl::Log::debug( vl::Say("VLXClassWrapper_ResourceDatabase : skipping '%s'.\n") << obj->resources().at(i).get()->className() );
          continue;
        }
        else
        {
          *list << s.exportVLX(obj->resources().at(i).get());
        }
      }
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::ResourceDatabase* cast_obj = obj->as<vl::ResourceDatabase>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("resdb_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportResourceDatabase(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Uniform */
  struct VLXClassWrapper_Uniform: public ClassWrapper
  {
    void importUniform(VLXSerializer& s, const VLXStructure* vlx, vl::Uniform* uniform)
    {
      const VLXValue* val = vlx->getValue("Name");
      if (val)
      {
        VL_CHECK( val->type() == VLXValue::Identifier );
        uniform->setName( val->getIdentifier() );
      }
      else
      {
        s.signalImportError( vl::Say("Line %d : uniform without 'Name'.\n") << vlx->lineNumber() );
        return;
      }

      // 'Count' is optional
      int count = 1;
      val = vlx->getValue("Count");
      if (val)
      {
        VL_CHECK( val->type() == VLXValue::Integer );
        count = (int)val->getInteger();
      }

      vl::EUniformType type = vl::UT_NONE;
      val = vlx->getValue("Type");
      if (val)
      {
        VL_CHECK( val->type() == VLXValue::Identifier );
        type = vlx_EUniformType( *val, s );
      }
      else
      {
        s.signalImportError( vl::Say("Line %d : uniform without 'Type'.\n") << vlx->lineNumber() );
        return;
      }

      val = vlx->getValue("Data");
      const VLXArrayReal* arr_real = NULL;
      const VLXArrayInteger* arr_int = NULL;
      if (!val)
      {
        s.signalImportError( vl::Say("Line %d : uniform without 'Data'.\n") << vlx->lineNumber() );
        return;
      }
      else
      {
        if (val->type() == VLXValue::ArrayReal)
          arr_real = val->getArrayReal();
        else
        if (val->type() == VLXValue::ArrayInteger)
          arr_int = val->getArrayInteger();
      }

      std::vector<int> int_vec;
      std::vector<unsigned int> uint_vec;
      std::vector<float> float_vec;
      std::vector<double> double_vec;

      switch(type)
      {
      case vl::UT_INT:
        int_vec.resize(count*1); if (arr_int) arr_int->copyTo(&int_vec[0]); else int_vec[0] = (int)val->getInteger();
        uniform->setUniform1i(count, &int_vec[0]);
        break;
      case vl::UT_INT_VEC2:
        int_vec.resize(count*2); arr_int->copyTo(&int_vec[0]); VLX_IMPORT_CHECK_RETURN(int_vec.size() == arr_int->value().size(), *val);
        uniform->setUniform2i(count, &int_vec[0]);
        break;
      case vl::UT_INT_VEC3:
        int_vec.resize(count*3); arr_int->copyTo(&int_vec[0]); VLX_IMPORT_CHECK_RETURN(int_vec.size() == arr_int->value().size(), *val);
        uniform->setUniform3i(count, &int_vec[0]);
        break;
      case vl::UT_INT_VEC4:
        int_vec.resize(count*4); arr_int->copyTo(&int_vec[0]); VLX_IMPORT_CHECK_RETURN(int_vec.size() == arr_int->value().size(), *val);
        uniform->setUniform4i(count, &int_vec[0]);
        break;

      case vl::UT_UNSIGNED_INT:
        uint_vec.resize(count*1); if (arr_int) arr_int->copyTo(&uint_vec[0]); else uint_vec[0] = (unsigned int)val->getInteger();
        uniform->setUniform1ui(count, &uint_vec[0]);
        break;
      case vl::UT_UNSIGNED_INT_VEC2:
        uint_vec.resize(count*2); arr_int->copyTo(&uint_vec[0]); VLX_IMPORT_CHECK_RETURN(uint_vec.size() == arr_int->value().size(), *val);
        uniform->setUniform2ui(count, &uint_vec[0]);
        break;
      case vl::UT_UNSIGNED_INT_VEC3:
        uint_vec.resize(count*3); arr_int->copyTo(&uint_vec[0]); VLX_IMPORT_CHECK_RETURN(uint_vec.size() == arr_int->value().size(), *val);
        uniform->setUniform3ui(count, &uint_vec[0]);
        break;
      case vl::UT_UNSIGNED_INT_VEC4:
        uint_vec.resize(count*4); arr_int->copyTo(&uint_vec[0]); VLX_IMPORT_CHECK_RETURN(uint_vec.size() == arr_int->value().size(), *val);
        uniform->setUniform4ui(count, &uint_vec[0]);
        break;

      case vl::UT_FLOAT:
        float_vec.resize(count*1); if (arr_real) arr_real->copyTo(&float_vec[0]); else float_vec[0] = (float)val->getReal();
        uniform->setUniform1f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_VEC2:
        float_vec.resize(count*2); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniform2f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_VEC3:
        float_vec.resize(count*3); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniform3f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_VEC4:
        float_vec.resize(count*4); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniform4f(count, &float_vec[0]);
        break;

      case vl::UT_FLOAT_MAT2:
        float_vec.resize(count*2*2); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix2f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_MAT3:
        float_vec.resize(count*3*3); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix3f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_MAT4:
        float_vec.resize(count*4*4); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix4f(count, &float_vec[0]);
        break;

      case vl::UT_FLOAT_MAT2x3:
        float_vec.resize(count*2*3); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix2x3f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_MAT3x2:
        float_vec.resize(count*3*2); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix3x2f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_MAT2x4:
        float_vec.resize(count*2*4); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix2x4f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_MAT4x2:
        float_vec.resize(count*4*2); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix4x2f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_MAT3x4:
        float_vec.resize(count*3*4); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix3x4f(count, &float_vec[0]);
        break;
      case vl::UT_FLOAT_MAT4x3:
        float_vec.resize(count*4*3); arr_real->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix4x3f(count, &float_vec[0]);
        break;

      case vl::UT_DOUBLE:
        double_vec.resize(count*1); if (arr_real) arr_real->copyTo(&double_vec[0]); else double_vec[0] = (double)val->getReal();
        uniform->setUniform1d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_VEC2:
        double_vec.resize(count*2); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniform2d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_VEC3:
        double_vec.resize(count*3); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniform3d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_VEC4:
        double_vec.resize(count*4); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniform4d(count, &double_vec[0]);
        break;

      case vl::UT_DOUBLE_MAT2:
        double_vec.resize(count*2*2); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix2d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_MAT3:
        double_vec.resize(count*3*3); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix3d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_MAT4:
        double_vec.resize(count*4*4); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix4d(count, &double_vec[0]);
        break;

      case vl::UT_DOUBLE_MAT2x3:
        double_vec.resize(count*2*3); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix2x3d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_MAT3x2:
        double_vec.resize(count*3*2); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix3x2d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_MAT2x4:
        double_vec.resize(count*2*4); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix2x4d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_MAT4x2:
        double_vec.resize(count*4*2); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix4x2d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_MAT3x4:
        double_vec.resize(count*3*4); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix3x4d(count, &double_vec[0]);
        break;
      case vl::UT_DOUBLE_MAT4x3:
        double_vec.resize(count*4*3); arr_real->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix4x3d(count, &double_vec[0]);
        break;

      case vl::UT_NONE:
        vl::Log::error( vl::Say("Error importing uniform : uninitialized uniform (%s).\n") << uniform->name() );
        break;

      default:
        vl::Log::error( vl::Say("Error importing uniform : illegal uniform type (%s).\n") << uniform->name() );
        break;
      }

    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Uniform> obj = new vl::Uniform;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importUniform(s, vlx, obj.get());
      return obj;
    }

    void exportUniform(const vl::Uniform* uniform, VLXStructure* vlx)
    {
      *vlx << "Name" << vlx_Identifier(uniform->name());
      *vlx << "Type" << vlx_Identifier(vlx_EUniformType(uniform->type()));
      *vlx << "Count" << (long long)uniform->count();

      const int count = uniform->count();
      vl::ref<VLXArrayInteger> arr_int = new VLXArrayInteger;
      vl::ref<VLXArrayReal> arr_real = new VLXArrayReal;

      switch(uniform->type())
      {
      case vl::UT_INT:
        {
          if (count == 1)
            { int val = 0; uniform->getUniform(&val); *vlx << "Data" << (long long)val; break; }
          else
            { arr_int->value().resize(count*1); arr_int->copyFrom( (int*)uniform->rawData() ); break; }
        }
      case vl::UT_INT_VEC2: arr_int->value().resize(count*2); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case vl::UT_INT_VEC3: arr_int->value().resize(count*3); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case vl::UT_INT_VEC4: arr_int->value().resize(count*4); arr_int->copyFrom( (int*)uniform->rawData() ); break;

      case vl::UT_UNSIGNED_INT:
        {
          if (count == 1)
            { unsigned int val = 0; uniform->getUniform(&val); *vlx << "Data" << (long long)val; break; }
          else
            { arr_int->value().resize(count*1); arr_int->copyFrom( (int*)uniform->rawData() ); break; }
        }
      case vl::UT_UNSIGNED_INT_VEC2: arr_int->value().resize(count*2); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case vl::UT_UNSIGNED_INT_VEC3: arr_int->value().resize(count*3); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case vl::UT_UNSIGNED_INT_VEC4: arr_int->value().resize(count*4); arr_int->copyFrom( (int*)uniform->rawData() ); break;

      case vl::UT_FLOAT:
        {
          if (count == 1)
            { float val = 0; uniform->getUniform(&val); *vlx << "Data" << (double)val; break; }
          else
            { arr_real->value().resize(count*1); arr_real->copyFrom( (float*)uniform->rawData() ); break; }
        }
      case vl::UT_FLOAT_VEC2: arr_real->value().resize(count*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case vl::UT_FLOAT_VEC3: arr_real->value().resize(count*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case vl::UT_FLOAT_VEC4: arr_real->value().resize(count*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;

      case vl::UT_FLOAT_MAT2: arr_real->value().resize(count*2*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case vl::UT_FLOAT_MAT3: arr_real->value().resize(count*3*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case vl::UT_FLOAT_MAT4: arr_real->value().resize(count*4*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;

      case vl::UT_FLOAT_MAT2x3: arr_real->value().resize(count*2*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case vl::UT_FLOAT_MAT3x2: arr_real->value().resize(count*3*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case vl::UT_FLOAT_MAT2x4: arr_real->value().resize(count*2*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case vl::UT_FLOAT_MAT4x2: arr_real->value().resize(count*4*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case vl::UT_FLOAT_MAT3x4: arr_real->value().resize(count*3*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case vl::UT_FLOAT_MAT4x3: arr_real->value().resize(count*4*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;

      case vl::UT_DOUBLE:
        {
          if (count == 1)
            { double val = 0; uniform->getUniform(&val); *vlx << "Data" << (double)val; break; }
          else
            { arr_real->value().resize(count*1); arr_real->copyFrom( (double*)uniform->rawData() ); break; }
        }
      case vl::UT_DOUBLE_VEC2: arr_real->value().resize(count*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case vl::UT_DOUBLE_VEC3: arr_real->value().resize(count*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case vl::UT_DOUBLE_VEC4: arr_real->value().resize(count*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;

      case vl::UT_DOUBLE_MAT2: arr_real->value().resize(count*2*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case vl::UT_DOUBLE_MAT3: arr_real->value().resize(count*3*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case vl::UT_DOUBLE_MAT4: arr_real->value().resize(count*4*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;

      case vl::UT_DOUBLE_MAT2x3: arr_real->value().resize(count*2*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case vl::UT_DOUBLE_MAT3x2: arr_real->value().resize(count*3*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case vl::UT_DOUBLE_MAT2x4: arr_real->value().resize(count*2*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case vl::UT_DOUBLE_MAT4x2: arr_real->value().resize(count*4*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case vl::UT_DOUBLE_MAT3x4: arr_real->value().resize(count*3*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case vl::UT_DOUBLE_MAT4x3: arr_real->value().resize(count*4*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;

      case vl::UT_NONE:
        vl::Log::error( vl::Say("Error exporting uniform : uninitialized uniform (%s).\n") << uniform->name() );
        break;

      default:
        vl::Log::error( vl::Say("Error exporting uniform : illegal uniform type (%s).\n") << uniform->name() );
        break;
      }

      if (!arr_int->value().empty())
        *vlx << "Data" << arr_int.get();
      else
      if (!arr_real->value().empty())
        *vlx << "Data" << arr_real.get();
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Uniform* cast_obj = obj->as<vl::Uniform>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("uniform_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportUniform(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Shader */
  struct VLXClassWrapper_Shader: public ClassWrapper
  {
    void importShader(VLXSerializer& s, const VLXStructure* vlx, vl::Shader* sh)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        sh->setObjectName( name->getString() );

      // enables
      const VLXValue* enables = vlx->getValue("Enables");
      if (enables)
      {
        VLX_IMPORT_CHECK_RETURN( enables->type() == VLXValue::List, *enables )
        const VLXList* list = enables->getList();
        for(size_t i=0; i<list->value().size(); ++i)
        {
          VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLXValue::Identifier, list->value()[i] );
          vl::EEnable en = vlx_EEnable( list->value()[i], s );
          VLX_IMPORT_CHECK_RETURN( en != vl::EN_UnknownEnable, list->value()[i] );
          sh->enable(en);
        }
      }

      // render states
      const VLXValue* renderstates = vlx->getValue("RenderStates");
      if (renderstates)
      {
        VLX_IMPORT_CHECK_RETURN( renderstates->type() == VLXValue::List, *renderstates )
        const VLXList* list = renderstates->getList();
        int index = -1;
        for(size_t i=0; i<list->value().size(); ++i)
        {
          VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLXValue::Structure || list->value()[i].type() == VLXValue::Integer, list->value()[i] );
          if (list->value()[i].type() == VLXValue::Integer)
          {
            VLX_IMPORT_CHECK_RETURN( index == -1, list->value()[i] );
            index = (int)list->value()[i].getInteger();
          }
          else
          {
            vl::RenderState* renderstate = s.importVLX( list->value()[i].getStructure() )->as<vl::RenderState>();
            VLX_IMPORT_CHECK_RETURN( renderstate != NULL, list->value()[i] )
            VLX_IMPORT_CHECK_RETURN( (index == -1 && !renderstate->isOfType(vl::RenderStateIndexed::Type())) || (index >= 0 && renderstate->isOfType(vl::RenderStateIndexed::Type())), list->value()[i] )
            sh->setRenderState(renderstate, index);
            // consume index in any case
            index = -1;
          }
        }
      }

      // uniforms
      const VLXValue* uniforms = vlx->getValue("Uniforms");
      if (uniforms)
      {
        VLX_IMPORT_CHECK_RETURN( uniforms->type() == VLXValue::List, *uniforms )
        const VLXList* list = uniforms->getList();
        for(size_t i=0; i<list->value().size(); ++i)
        {
          VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLXValue::Structure, list->value()[i] );
          vl::Uniform* uniform = s.importVLX( list->value()[i].getStructure() )->as<vl::Uniform>();
          VLX_IMPORT_CHECK_RETURN( uniform != NULL, list->value()[i] )
          sh->setUniform(uniform);
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Shader> obj = new vl::Shader;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importShader(s, vlx, obj.get());
      return obj;
    }

    void exportShader(VLXSerializer& s, const vl::Shader* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());

      // uniforms
      VLXValue uniforms;
      uniforms.setList( new VLXList );
      if (obj->getUniformSet())
      {
        for(size_t i=0; i<obj->uniforms().size(); ++i)
          *uniforms.getList() << s.exportVLX(obj->uniforms()[i].get());
      }
      *vlx << "Uniforms" << uniforms;

      // enables
      vl::ref<VLXList> enables = new VLXList;
      if (obj->getEnableSet() )
      {
        for(size_t i=0; i<obj->getEnableSet()->enables().size(); ++i)
          *enables << vlx_Identifier(vlx_EEnable(obj->getEnableSet()->enables()[i]));
      }
      *vlx << "Enables" << enables.get();

      // renderstates
      VLXValue renderstates;
      renderstates.setList( new VLXList );
      if (obj->getRenderStateSet())
      {
        for(size_t i=0; i<obj->getRenderStateSet()->renderStatesCount(); ++i)
        {
          const vl::RenderState* rs = obj->getRenderStateSet()->renderStates()[i].mRS.get();
          if ( !s.canExport(rs) )
          {
            vl::Log::debug( vl::Say("VLXClassWrapper_Shader : skipping '%s'.\n") << rs->className() );
            continue;
          }
          int index = obj->getRenderStateSet()->renderStates()[i].mIndex;
          if (index != -1)
            *renderstates.getList() << (long long)index;
          *renderstates.getList() << s.exportVLX(rs);
        }
      }
      *vlx << "RenderStates" << renderstates;
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Shader* cast_obj = obj->as<vl::Shader>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("shader_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportShader(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::DistanceLODEvaluator and vl::PixelLODEvaluator */
  struct VLXClassWrapper_LODEvaluator: public ClassWrapper
  {
    void importLODEvaluator(VLXSerializer& s, const VLXStructure* vlx, vl::LODEvaluator* obj)
    {
      if (obj->isOfType(vl::DistanceLODEvaluator::Type()))
      {
        vl::DistanceLODEvaluator* lod = obj->as<vl::DistanceLODEvaluator>();
        const VLXValue* vlx_distances = vlx->getValue("DistanceRageSet");
        VLX_IMPORT_CHECK_RETURN( vlx_distances != NULL, *vlx );
        VLX_IMPORT_CHECK_RETURN( vlx_distances->type() == VLXValue::ArrayReal, *vlx_distances );
        const VLXArrayReal* arr = vlx_distances->getArrayReal();
        if (arr->value().size())
        {
          lod->distanceRangeSet().resize( arr->value().size() );
          arr->copyTo( &lod->distanceRangeSet()[0] );
        }
      }
      else
      if (obj->isOfType(vl::PixelLODEvaluator::Type()))
      {
        vl::PixelLODEvaluator* lod = obj->as<vl::PixelLODEvaluator>();
        const VLXValue* vlx_pixels = vlx->getValue("PixelRageSet");
        VLX_IMPORT_CHECK_RETURN( vlx_pixels != NULL, *vlx );
        VLX_IMPORT_CHECK_RETURN( vlx_pixels->type() == VLXValue::ArrayReal, *vlx_pixels );
        const VLXArrayReal* arr = vlx_pixels->getArrayReal();
        if (arr->value().size())
        {
          lod->pixelRangeSet().resize( arr->value().size() );
          arr->copyTo( &lod->pixelRangeSet()[0] );
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      if (vlx->tag() == "<vl::DistanceLODEvaluator>")
      {
        vl::ref<vl::LODEvaluator> obj = new vl::DistanceLODEvaluator;
        // register imported structure asap
        s.registerImportedStructure(vlx, obj.get());
        importLODEvaluator(s, vlx, obj.get());
        return obj;
      }
      else
      if (vlx->tag() == "<vl::PixelLODEvaluator>")
      {
        vl::ref<vl::LODEvaluator> obj = new vl::PixelLODEvaluator;
        // register imported structure asap
        s.registerImportedStructure(vlx, obj.get());
        importLODEvaluator(s, vlx, obj.get());
        return obj;
      }
      else
      {
        return NULL;
      }
    }

    void exportLODEvaluator(VLXSerializer& s, const vl::LODEvaluator* obj, VLXStructure* vlx)
    {
      if (obj->classType() == vl::DistanceLODEvaluator::Type())
      {
        const vl::DistanceLODEvaluator* lod = obj->as<vl::DistanceLODEvaluator>();
        VLXValue distances( new VLXArrayReal );
        distances.getArrayReal()->value().resize( lod->distanceRangeSet().size() );
        if (lod->distanceRangeSet().size() != 0)
          distances.getArrayReal()->copyFrom( &lod->distanceRangeSet()[0] );
        *vlx << "DistanceRageSet" << distances;
      }
      else
      if (obj->classType() == vl::PixelLODEvaluator::Type())
      {
        const vl::PixelLODEvaluator* lod = obj->as<vl::PixelLODEvaluator>();
        VLXValue pixels( new VLXArrayReal );
        pixels.getArrayReal()->value().resize( lod->pixelRangeSet().size() );
        if (lod->pixelRangeSet().size() != 0)
          pixels.getArrayReal()->copyFrom( &lod->pixelRangeSet()[0] );
        *vlx << "PixelRageSet" << pixels;
      }
      else
      {
        s.signalExportError("vl::LODEvaluator type not supported for export.\n");
      }
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::LODEvaluator* cast_obj = obj->as<vl::LODEvaluator>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("lodeval_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportLODEvaluator(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Effect */
  struct VLXClassWrapper_Effect: public ClassWrapper
  {
    void importEffect(VLXSerializer& s, const VLXStructure* vlx, vl::Effect* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      const std::vector<VLXStructure::KeyValue>& values = vlx->value();
      for(size_t i=0; i<values.size(); ++i)
      {
        const std::string& key = values[i].key();
        const VLXValue& value = values[i].value();
        if (key == "RenderRank")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value )
          obj->setRenderRank( (int)value.getInteger() );
        }
        else
        if (key == "EnableMask")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value )
          obj->setEnableMask( (int)value.getInteger() );
        }
        else
        if (key == "ActiveLod")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value )
          obj->setActiveLod( (int)value.getInteger() );
        }
        else
        if (key == "LODEvaluator")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value )
          vl::LODEvaluator* lod_eval = s.importVLX( value.getStructure() )->as<vl::LODEvaluator>();
          VLX_IMPORT_CHECK_RETURN( lod_eval, value )
          obj->setLODEvaluator(lod_eval);
        }
        else
        if (key == "Lods")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value )
          const VLXList* lod_list = value.getList();
          for(size_t ilod=0; ilod< lod_list->value().size(); ++ilod)
          {
            const VLXValue& lod_shaders = lod_list->value()[ilod];
            VLX_IMPORT_CHECK_RETURN( lod_shaders.type() == VLXValue::List, lod_shaders )
            obj->lod((int)ilod) = new vl::ShaderPasses;
            for( size_t ish=0; ish<lod_shaders.getList()->value().size(); ++ish)
            {
              const VLXValue& vlx_sh = lod_shaders.getList()->value()[ish];
              VLX_IMPORT_CHECK_RETURN( vlx_sh.type() == VLXValue::Structure, vlx_sh )
              vl::Shader* shader = s.importVLX( vlx_sh.getStructure() )->as<vl::Shader>();
              VLX_IMPORT_CHECK_RETURN( shader, vlx_sh )
              obj->lod((int)ilod)->push_back( shader );
            }
          }
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Effect> obj = new vl::Effect;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importEffect(s, vlx, obj.get());
      return obj;
    }

    VLXValue export_ShaderPasses(VLXSerializer& s, const vl::ShaderPasses* sh_seq)
    {
      VLXValue value( new VLXList(vlx_makeTag(sh_seq).c_str()) );
      for(size_t i=0; i<sh_seq->size(); ++i)
        *value.getList() << s.exportVLX(sh_seq->at(i));
      return value;
    }

    void exportEffect(VLXSerializer& s, const vl::Effect* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      *vlx << "RenderRank" << (long long)obj->renderRank();
      *vlx << "EnableMask" << (long long)obj->enableMask();
      *vlx << "ActiveLod" << (long long)obj->activeLod();

      if (obj->lodEvaluator())
        *vlx << "LODEvaluator" << s.exportVLX(obj->lodEvaluator());

      // shaders
      vl::ref<VLXList> lod_list = new VLXList;
      for(int i=0; obj->lod(i) && i<VL_MAX_EFFECT_LOD; ++i)
        *lod_list << export_ShaderPasses(s, obj->lod(i).get());
      *vlx << "Lods" << lod_list.get();
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Effect* cast_obj = obj->as<vl::Effect>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("effect_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportEffect(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Actor */
  struct VLXClassWrapper_Actor: public ClassWrapper
  {
    void importActor(VLXSerializer& s, const VLXStructure* vlx, vl::Actor* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      const std::vector<VLXStructure::KeyValue>& values = vlx->value();
      for(size_t i=0; i<values.size(); ++i)
      {
        const std::string& key = values[i].key();
        const VLXValue& value = values[i].value();
        if (key == "RenderRank")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value )
          obj->setRenderRank( (int)value.getInteger() );
        }
        else
        if (key == "RenderBlock")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value )
          obj->setRenderBlock( (int)value.getInteger() );
        }
        else
        if (key == "EnableMask")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value )
          obj->setEnableMask( (int)value.getInteger() );
        }
        else
        if (key == "IsOccludee")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool, value )
          obj->setOccludee( value.getBool() );
        }
        else
        if (key == "Enabled")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool, value )
          obj->setEnabled( value.getBool() );
        }
        else
        if (key == "Lods")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value )
          const VLXList* list = value.getList();
          for(size_t i=0; i<list->value().size(); ++i)
          {
            const VLXValue& lod = list->value()[i];
            VLX_IMPORT_CHECK_RETURN( lod.type() == VLXValue::Structure, lod )
            vl::Renderable* rend = s.importVLX( lod.getStructure() )->as<vl::Renderable>();
            VLX_IMPORT_CHECK_RETURN( rend != NULL, lod )
            obj->setLod(i, rend);
          }
        }
        else
        if (key == "Effect")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value )
          vl::Effect* fx = s.importVLX(value.getStructure())->as<vl::Effect>();
          VLX_IMPORT_CHECK_RETURN( fx != NULL, value )
          obj->setEffect(fx);
        }
        else
        if (key == "Transform")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value )
          vl::Transform* tr = s.importVLX(value.getStructure())->as<vl::Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          obj->setTransform(tr);
        }
        else
        if (key == "Uniforms")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value )
          const VLXList* list = value.getList();
          for(size_t i=0; i<list->value().size(); ++i)
          {
            VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLXValue::Structure, list->value()[i] )
            vl::Uniform* uniform = s.importVLX( list->value()[i].getStructure() )->as<vl::Uniform>();
            VLX_IMPORT_CHECK_RETURN( uniform != NULL, list->value()[i] )
            obj->setUniform(uniform);
          }
        }
        // bounding volumes are not serialized, they are computed based on the geometry's bounds
        // else if (key == "AABB") {}
        // else if (key == "Sphere") {}
        else
        if (key == "LODEvaluator")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value )
          if (s.canImport( value.getStructure() ) )
          {
            vl::LODEvaluator* lod = s.importVLX( value.getStructure() )->as<vl::LODEvaluator>();
            VLX_IMPORT_CHECK_RETURN( lod != NULL, value )
            obj->setLODEvaluator(lod);
          }
        }
        else
        if (key == "ActorEventCallbacks")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value )
          const VLXList* list = value.getList();
          for(size_t i=0; i<list->value().size(); ++i)
          {
            const VLXValue& elem = list->value()[i];
            VLX_IMPORT_CHECK_RETURN( elem.type() == VLXValue::Structure, elem )
            if (s.canImport(elem.getStructure()))
            {
              vl::ActorEventCallback* cb = s.importVLX( elem.getStructure() )->as<vl::ActorEventCallback>();
              VLX_IMPORT_CHECK_RETURN( cb != NULL, elem )
              obj->actorEventCallbacks()->push_back(cb);
            }
          }
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Actor> obj = new vl::Actor;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importActor(s, vlx, obj.get());
      return obj;
    }

    void exportActor(VLXSerializer& s, const vl::Actor* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      *vlx << "EnableMask" << (long long)obj->enableMask();
      *vlx << "RenderBlock" << (long long)obj->renderBlock();
      *vlx << "RenderRank" << (long long)obj->renderRank();
      *vlx << "IsOccludee" << obj->isOccludee();
      *vlx << "Enabled" << obj->isEnabled();

      VLXValue renderables;
      renderables.setList( new VLXList );
      for(size_t i=0; i<VL_MAX_ACTOR_LOD && obj->lod(i); ++i)
        *renderables.getList() << s.exportVLX(obj->lod(i));
      *vlx << "Lods" << renderables;

      // bounding volumes are not serialized, they are computed based on the geometry's bounds
      // *vlx << "AABB" << export_AABB(obj->boundingBox());
      // *vlx << "Sphere" << export_Sphere(obj->boundingSphere());

      if (obj->effect())
        *vlx << "Effect" << s.exportVLX(obj->effect());
      if (obj->transform())
        *vlx << "Transform" << s.exportVLX(obj->transform());

      VLXValue uniforms;
      uniforms.setList( new VLXList );
      for(size_t i=0; obj->getUniformSet() && i<obj->uniforms().size(); ++i)
        *uniforms.getList() << s.exportVLX(obj->uniforms()[i].get());
      *vlx << "Uniforms" << uniforms;

      if (obj->lodEvaluator())
        *vlx << "LODEvaluator" << s.exportVLX(obj->lodEvaluator());

      // mic fixme:
      // Scissor: scissors might go away from the vl::Actor

      VLXValue callbacks;
      callbacks.setList( new VLXList );
      for(size_t i=0; i<obj->actorEventCallbacks()->size(); ++i)
        *callbacks.getList() << s.exportVLX(obj->actorEventCallbacks()->at(i));
      *vlx << "ActorEventCallbacks" << callbacks;
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Actor* cast_obj = obj->as<vl::Actor>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("actor_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportActor(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Camera */
  struct VLXClassWrapper_Camera: public ClassWrapper
  {
    void importCamera(VLXSerializer& s, const VLXStructure* vlx, vl::Camera* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();
        if (key == "ViewMatrix")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value);
          obj->setViewMatrix( vlx_mat4(value.getList()) );
          // VLX_IMPORT_CHECK_RETURN( !obj->viewMatrix().isNull(), value )
        }
        else
        if (key == "ProjectionMatrix")
        {
          vl::EProjectionMatrixType ptype = vl::PMT_UserProjection;
          const VLXValue* pmtype = vlx->getValue("ProjectionMatrixType");
          if ( pmtype )
          {
            VLX_IMPORT_CHECK_RETURN( pmtype->type() == VLXValue::Identifier, *pmtype );
            ptype = vlx_EProjectionMatrixType( *pmtype, s );
          }

          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value);
          obj->setProjectionMatrix( vlx_mat4(value.getList()), ptype );
          // VLX_IMPORT_CHECK_RETURN( !obj->projectionMatrix().isNull(), value )
        }
        else
        if (key == "Viewport")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value)
          vl::Viewport* viewp = s.importVLX( value.getStructure() )->as<vl::Viewport>();
          VLX_IMPORT_CHECK_RETURN( viewp != NULL, value )
          obj->setViewport(viewp);
        }
        else
        if (key == "FOV")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setFOV( (float)value.getReal() );
        }
        else
        if (key == "NearPlane")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setNearPlane( (float)value.getReal() );
        }
        else
        if (key == "FarPlane")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setFarPlane( (float)value.getReal() );
        }
        else
        if (key == "Left")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setLeft( (float)value.getReal() );
        }
        else
        if (key == "Right")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setRight( (float)value.getReal() );
        }
        else
        if (key == "Bottom")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setBottom( (float)value.getReal() );
        }
        else
        if (key == "Top")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setTop( (float)value.getReal() );
        }
        else
        if (key == "BoundTransform")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value)
          vl::Transform* tr= s.importVLX( value.getStructure() )->as<vl::Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          obj->bindTransform(tr);
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Camera> obj = new vl::Camera;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importCamera(s, vlx, obj.get());
      return obj;
    }

    void exportCamera(VLXSerializer& s, const vl::Camera* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      *vlx << "ViewMatrix" << vlx_toValue(obj->viewMatrix());
      *vlx << "ProjectionMatrix" << vlx_toValue(obj->projectionMatrix());
      *vlx << "ProjectionMatrixType" << vlx_Identifier(vlx_EProjectionMatrixType(obj->projectionMatrixType()));
      *vlx << "Viewport" << s.exportVLX(obj->viewport());
      *vlx << "NearPlane" << (double)obj->nearPlane();
      *vlx << "FarPlane" << (double)obj->farPlane();
      *vlx << "FOV" << (double)obj->fov();
      *vlx << "Left" << (double)obj->left();
      *vlx << "Right" << (double)obj->right();
      *vlx << "Bottom" << (double)obj->bottom();
      *vlx << "Top" << (double)obj->top();
      if (obj->boundTransform())
        *vlx << "BoundTransfrm" << s.exportVLX(obj->boundTransform());
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Camera* cast_obj = obj->as<vl::Camera>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("camera_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportCamera(s, cast_obj, vlx.get());
      return vlx;
    }
  };


  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Viewport */
  struct VLXClassWrapper_Viewport: public ClassWrapper
  {
    void importViewport(VLXSerializer& s, const VLXStructure* vlx, vl::Viewport* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();
        if (key == "ClearColor")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setClearColor( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "ClearColorInt")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayInteger, value );
          obj->setClearColorInt( vlx_ivec4( value.getArrayInteger() ) );
        }
        else
        if (key == "ClearColorUInt")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayInteger, value );
          obj->setClearColorUInt( vlx_uivec4( value.getArrayInteger() ) );
        }
        else
        if (key == "ClearDepth")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value );
          obj->setClearDepth( (float)value.getReal() );
        }
        else
        if (key == "ClearStecil")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value );
          obj->setClearStencil( (int)value.getInteger() );
        }
        else
        if (key == "ClearColorMode")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setClearColorMode( vlx_EClearColorMode( value, s) );
        }
        else
        if (key == "ClearFlags")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setClearFlags( vlx_EClearFlags( value, s ) );
        }
        else
        if (key == "X")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value );
          obj->setX( (int)value.getInteger()  );
        }
        else
        if (key == "Y")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value );
          obj->setY( (int)value.getInteger()  );
        }
        else
        if (key == "Width")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value );
          obj->setWidth( (int)value.getInteger()  );
        }
        else
        if (key == "Height")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value );
          obj->setHeight( (int)value.getInteger()  );
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Viewport> obj = new vl::Viewport;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importViewport(s, vlx, obj.get());
      return obj;
    }

    void exportViewport(const vl::Viewport* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      *vlx << "ClearColor" << vlx_toValue((vl::vec4)obj->clearColor());
      *vlx << "ClearColorInt" << vlx_toValue(obj->clearColorInt());
      *vlx << "ClearColorUInt" << vlx_toValue(obj->clearColorUInt());
      *vlx << "ClearDepth" << (double)obj->clearDepth();
      *vlx << "ClearStecil" << (long long)obj->clearStencil();
      *vlx << "ClearColorMode" << vlx_Identifier(vlx_EClearColorMode(obj->clearColorMode()));
      *vlx << "ClearFlags" << vlx_Identifier(vlx_EClearFlags(obj->clearFlags()));
      *vlx << "X" << (long long)obj->x();
      *vlx << "Y" << (long long)obj->y();
      *vlx << "Width" << (long long)obj->width();
      *vlx << "Height" << (long long)obj->height();
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Viewport* cast_obj = obj->as<vl::Viewport>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("viewport_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportViewport(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Transform */
  struct VLXClassWrapper_Transform: public ClassWrapper
  {
    void importTransform(VLXSerializer& s, const VLXStructure* vlx, vl::Transform* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();
        if (key == "LocalMatrix")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value )
          obj->setLocalAndWorldMatrix( vlx_mat4( value.getList() ) );
        }
        else
        // let the "Children" property take care of children binding
        /*
        if (key == "Parent")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value )
          vl::Transform* tr = s.importVLX( value.getStructure() )->as<vl::Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          tr->addChild(obj);
        }
        else
        */
        if (key == "Children")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value )
          const VLXList* list = value.getList();
          for(size_t ich=0; ich<list->value().size(); ++ich)
          {
            VLX_IMPORT_CHECK_RETURN( list->value()[ich].type() == VLXValue::Structure, list->value()[ich] )
            const VLXStructure* vlx_tr = list->value()[ich].getStructure();
            vl::Transform* child = s.importVLX( vlx_tr )->as<vl::Transform>();
            VLX_IMPORT_CHECK_RETURN( child != NULL, *vlx_tr )
            obj->addChild(child);
          }
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Transform> obj = new vl::Transform;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importTransform(s, vlx, obj.get());
      return obj;
    }

    void exportTransform(VLXSerializer& s, const vl::Transform* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      *vlx << "LocalMatrix" << vlx_toValue(obj->localMatrix());

      // not needed
      /*if (obj->parent())
        *vlx << "Parent" << s.exportVLX(obj->parent());*/

      VLXValue childs;
      childs.setList( new VLXList );
      for(size_t i=0; i<obj->childrenCount(); ++i)
        childs.getList()->value().push_back( s.exportVLX(obj->children()[i].get()) );
      *vlx << "Children" << childs;
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Transform* cast_obj = obj->as<vl::Transform>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("transform_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportTransform(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Light */
  struct VLXClassWrapper_Light: public ClassWrapper
  {
    void importLight(VLXSerializer& s, const VLXStructure* vlx, vl::Light* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();
        if (key == "Ambient")
        {
          // note: what if the user specifies ( 1 0 0 0 ) -> becomes a ArrayInteger and an error is issued.
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value )
          obj->setAmbient( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "Diffuse")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value )
          obj->setDiffuse( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "Specular")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value )
          obj->setSpecular( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "Position")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value )
          obj->setPosition( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "SpotDirection")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value )
          obj->setSpotDirection( (vl::fvec3)vlx_vec3( value.getArrayReal() ) );
        }
        else
        if (key == "SpotExponent")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setSpotExponent( (float)value.getReal() );
        }
        else
        if (key == "SpotCutoff")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setSpotCutoff( (float)value.getReal() );
        }
        else
        if (key == "ConstantAttenuation")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setConstantAttenuation( (float)value.getReal() );
        }
        else
        if (key == "LinearAttenuation")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setLinearAttenuation( (float)value.getReal() );
        }
        else
        if (key == "QuadraticAttenuation")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->setQuadraticAttenuation( (float)value.getReal() );
        }
        else
        if (key == "BoundTransform")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value)
          vl::Transform* tr= s.importVLX( value.getStructure() )->as<vl::Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          obj->bindTransform(tr);
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Light> obj = new vl::Light;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importLight(s, vlx, obj.get());
      return obj;
    }

    void exportLight(VLXSerializer& s, const vl::Light* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      *vlx << "Ambient" << vlx_toValue((vl::vec4)obj->ambient());
      *vlx << "Diffuse" << vlx_toValue((vl::vec4)obj->diffuse());
      *vlx << "Specular" << vlx_toValue((vl::vec4)obj->specular());
      *vlx << "Position" << vlx_toValue((vl::vec4)obj->position());
      *vlx << "SpotDirection" << vlx_toValue((vl::vec3)obj->spotDirection());
      *vlx << "SpotExponent" << obj->spotExponent();
      *vlx << "SpotCutoff" << obj->spotCutoff();
      *vlx << "ConstantAttenuation" << obj->constantAttenuation();
      *vlx << "LinearAttenuation" << obj->linearAttenuation();
      *vlx << "QuadraticAttenuation" << obj->quadraticAttenuation();
      if (obj->boundTransform())
        *vlx << "BoundTransform" << s.exportVLX(obj->boundTransform());
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Light* cast_obj = obj->as<vl::Light>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("light_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportLight(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::ClipPlane*/
  struct VLXClassWrapper_ClipPlane: public ClassWrapper
  {
    void importClipPlane(VLXSerializer& s, const VLXStructure* vlx, vl::ClipPlane* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();
        if (key == "PlaneNormal")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value )
          obj->plane().setNormal( vlx_vec3( value.getArrayReal() ) );
        }
        else
        if (key == "PlaneOrigin")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value )
          obj->plane().setOrigin( (float)value.getReal() );
        }
        else
        if (key == "BoundTransform")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value)
          vl::Transform* tr= s.importVLX( value.getStructure() )->as<vl::Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          obj->bindTransform(tr);
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::ClipPlane> obj = new vl::ClipPlane;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importClipPlane(s, vlx, obj.get());
      return obj;
    }

    void exportClipPlane(VLXSerializer& s, const vl::ClipPlane* clip, VLXStructure* vlx)
    {
      *vlx << "PlaneNormal" << vlx_toValue(clip->plane().normal());
      *vlx << "PlaneOrigin" << clip->plane().origin();
      if (clip->boundTransform())
        *vlx << "BoundTransform" << s.exportVLX(clip->boundTransform());
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::ClipPlane* cast_obj = obj->as<vl::ClipPlane>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("clipplane_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportClipPlane(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::GLSLProgram */
  struct VLXClassWrapper_GLSLProgram: public ClassWrapper
  {
    void importGLSLProgram(VLXSerializer& s, const VLXStructure* vlx, vl::GLSLProgram* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();
        if (key == "AttachShader")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value )
          const VLXStructure* st = value.getStructure();
          vl::GLSLShader* glsl_sh = s.importVLX(st)->as<vl::GLSLShader>();
          VLX_IMPORT_CHECK_RETURN( glsl_sh != NULL, *st )
          obj->attachShader(glsl_sh);
        }
        else
        if (key == "FragDataLocation")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value )
          const VLXList* list = value.getList();
          VLX_IMPORT_CHECK_RETURN( list->value().size() == 2, *list )
          VLX_IMPORT_CHECK_RETURN( list->value()[0].type() == VLXValue::Identifier, *list )
          VLX_IMPORT_CHECK_RETURN( list->value()[1].type() == VLXValue::Integer, *list )
          const char* name = list->value()[0].getIdentifier().c_str();
          int index = (int)list->value()[1].getInteger();
          obj->bindFragDataLocation(index, name);
        }
        // MIC FIXME
        //else
        //if (key == "AttribLocation")
        //{
        //  VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value )
        //  const VLXList* list = value.getList();
        //  VLX_IMPORT_CHECK_RETURN( list->value().size() == 2, *list )
        //  VLX_IMPORT_CHECK_RETURN( list->value()[0].type() == VLXValue::Identifier, *list )
        //  VLX_IMPORT_CHECK_RETURN( list->value()[1].type() == VLXValue::Integer, *list )
        //  const char* name = list->value()[0].getIdentifier().c_str();
        //  int index = (int)list->value()[1].getInteger();
        //  obj->addAutoAttribLocation(index, name);
        //}
        else
        if (key == "Uniforms")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::List, value )
          const VLXList* list = value.getList();
          for(size_t i=0; i<list->value().size(); ++i)
          {
            VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLXValue::Structure, list->value()[i] )
            vl::Uniform* uniform = s.importVLX( list->value()[i].getStructure() )->as<vl::Uniform>();
            VLX_IMPORT_CHECK_RETURN( uniform != NULL, list->value()[i] )
            obj->setUniform(uniform);
          }
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::GLSLProgram> obj = new vl::GLSLProgram;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importGLSLProgram(s, vlx, obj.get());
      return obj;
    }

    void exportGLSLProgram(VLXSerializer& s, const vl::GLSLProgram* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());

      // export obj shaders
      for(int i=0; i<obj->shaderCount(); ++i)
        *vlx << "AttachShader" << s.exportVLX(obj->shader(i));

      // export uniforms
      VLXValue uniforms;
      uniforms.setList( new VLXList );
      for(size_t i=0; obj->getUniformSet() && i<obj->getUniformSet()->uniforms().size(); ++i)
        *uniforms.getList() << s.exportVLX(obj->getUniformSet()->uniforms()[i].get());
      *vlx << "Uniforms" << uniforms;

      // frag data location
      for(std::map<std::string, int>::const_iterator it = obj->fragDataLocations().begin(); it != obj->fragDataLocations().end(); ++it)
      {
        VLXList* location = new VLXList;
        *location << vlx_Identifier(it->first); // Name
        *location << (long long)it->second;   // Location
        *vlx << "FragDataLocation" << location;
      }

      //// auto attrib locations
      // MIC FIXME
      //for(std::map<std::string, int>::const_iterator it = obj->autoAttribLocations().begin(); it != obj->autoAttribLocations().end(); ++it)
      //{
      //  VLXList* location = new VLXList;
      //  *location << vlx_Identifier(it->first); // Name
      //  *location << (long long)it->second;   // Location
      //  *vlx << "AttribLocation" << location;
      //}
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::GLSLProgram* cast_obj = obj->as<vl::GLSLProgram>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("glslprog_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportGLSLProgram(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::GLSLVertexShader, vl::GLSLFragmentShader, vl::GLSLGeometryShader, vl::GLSLTessControlShader, vl::GLSLTessEvaluationShader. */
  struct VLXClassWrapper_GLSLShader: public ClassWrapper
  {
    void importGLSLShader(VLXSerializer& s, const VLXStructure* vlx, vl::GLSLShader* obj)
    {
      const VLXValue* path   = vlx->getValue("Path");
      const VLXValue* source = vlx->getValue("Source");
      VLX_IMPORT_CHECK_RETURN( path != NULL || source != NULL, *vlx )
      if (path)
      {
        VLX_IMPORT_CHECK_RETURN( path->type() == VLXValue::String, *path )
        std::string resolved_path = path->getString();
        s.resolvePath(resolved_path);
        obj->setSource(resolved_path.c_str()); // this automatically loads the source and sets the path
      }
      else
      if (source)
      {
        VLX_IMPORT_CHECK_RETURN( source->type() == VLXValue::RawtextBlock, *source )
        obj->setSource(source->getRawtextBlock()->value().c_str());
      }
      else
      {
        vl::Log::warning( vl::Say("Line %n : no source or path specified for glsl shader.\n") << vlx->lineNumber() );
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::GLSLShader> obj = NULL;
      if (vlx->tag() == "<vl::GLSLVertexShader>")
        obj = new vl::GLSLVertexShader;
      else
      if (vlx->tag() == "<vl::GLSLFragmentShader>")
        obj = new vl::GLSLFragmentShader;
      else
      if (vlx->tag() == "<vl::GLSLGeometryShader>")
        obj = new vl::GLSLGeometryShader;
      else
      if (vlx->tag() == "<vl::GLSLTessControlShader>")
        obj = new vl::GLSLTessControlShader;
      else
      if (vlx->tag() == "<vl::GLSLTessEvaluationShader>")
        obj = new vl::GLSLTessEvaluationShader;
      else
      {
        s.signalImportError( vl::Say("Line %n : shader type '%s' not supported.\n") << vlx->tag() );
        return NULL;
      }

      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importGLSLShader(s, vlx, obj.get());
      return obj;
    }

    void exportGLSLShader(const vl::GLSLShader* glslsh, VLXStructure* vlx)
    {
      if (!glslsh->path().empty())
        *vlx << "Path" << vlx_String(glslsh->path().toStdString());
      else
      if (!glslsh->source().empty())
        *vlx << "Source" << vlx_Rawtext(glslsh->source());
      else
      if (glslsh->handle())
        *vlx << "Source" << vlx_Rawtext(glslsh->getShaderSource());
      else
        *vlx << "Source" << vlx_Identifier("NO_SOURCE_FOUND");
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::GLSLShader* cast_obj = obj->as<vl::GLSLShader>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("glslsh_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportGLSLShader(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::VertexAttrib */
  struct VLXClassWrapper_VertexAttrib: public ClassWrapper
  {
    void importVertexAttrib(VLXSerializer& s, const VLXStructure* vlx, vl::VertexAttrib* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      const VLXValue* value = vlx->getValue("Value");
      VLX_IMPORT_CHECK_RETURN( value->type() == VLXValue::ArrayReal, *value )
      obj->setValue( (vl::fvec4)vlx_vec4( value->getArrayReal() ) );
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::VertexAttrib> obj = new vl::VertexAttrib;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importVertexAttrib(s, vlx, obj.get());
      return obj;
    }

    void exportVertexAttrib(const vl::VertexAttrib* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());
      *vlx << "Value" << vlx_toValue((vl::vec4)obj->value());
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::VertexAttrib* cast_obj = obj->as<vl::VertexAttrib>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("vertexattrib_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportVertexAttrib(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Color */
  struct VLXClassWrapper_Color: public ClassWrapper
  {
    void importColor(VLXSerializer& s, const VLXStructure* vlx, vl::Color* obj)
    {
      const VLXValue* value = vlx->getValue("Value");
      VLX_IMPORT_CHECK_RETURN( value->type() == VLXValue::ArrayReal, *value )
      obj->setValue( (vl::fvec4)vlx_vec4( value->getArrayReal() ) );
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Color> obj = new vl::Color;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importColor(s, vlx, obj.get());
      return obj;
    }

    void exportColor(const vl::Color* obj, VLXStructure* vlx)
    {
      *vlx << "Value" << vlx_toValue((vl::vec4)obj->value());
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Color* cast_obj = obj->as<vl::Color>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("color_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportColor(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::SecondaryColor */
  struct VLXClassWrapper_SecondaryColor: public ClassWrapper
  {
    void importSecondaryColor(VLXSerializer& s, const VLXStructure* vlx, vl::SecondaryColor* obj)
    {
      const VLXValue* value = vlx->getValue("Value");
      VLX_IMPORT_CHECK_RETURN( value->type() == VLXValue::ArrayReal, *value )
      obj->setValue( (vl::fvec3)vlx_vec3( value->getArrayReal() ) );
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::SecondaryColor> obj = new vl::SecondaryColor;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importSecondaryColor(s, vlx, obj.get());
      return obj;
    }

    void exportSecondaryColor(const vl::SecondaryColor* obj, VLXStructure* vlx)
    {
      *vlx << "Value" << vlx_toValue((vl::vec3)obj->value());
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::SecondaryColor* cast_obj = obj->as<vl::SecondaryColor>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("seccolor_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportSecondaryColor(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Normal */
  struct VLXClassWrapper_Normal: public ClassWrapper
  {
    void importNormal(VLXSerializer& s, const VLXStructure* vlx, vl::Normal* obj)
    {
      const VLXValue* value = vlx->getValue("Value");
      VLX_IMPORT_CHECK_RETURN( value->type() == VLXValue::ArrayReal, *value )
      obj->setValue( (vl::fvec3)vlx_vec3( value->getArrayReal() ) );
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Normal> obj = new vl::Normal;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importNormal(s, vlx, obj.get());
      return obj;
    }

    void exportNormal(const vl::Normal* obj, VLXStructure* vlx)
    {
      *vlx << "Value" << vlx_toValue((vl::vec3)obj->value());
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Normal* cast_obj = obj->as<vl::Normal>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("normal_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportNormal(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Material */
  struct VLXClassWrapper_Material: public ClassWrapper
  {
    void importMaterial(VLXSerializer& s, const VLXStructure* vlx, vl::Material* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();
        if (key == "FrontAmbient")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setFrontAmbient( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "FrontDiffuse")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setFrontDiffuse( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "FrontEmission")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setFrontEmission( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "FrontSpecular")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setFrontSpecular( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "FrontShininess")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value );
          obj->setFrontShininess( (float)value.getReal() );
        }
        else
        if (key == "BackAmbient")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setBackAmbient( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "BackDiffuse")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setBackDiffuse( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "BackEmission")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setBackEmission( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "BackSpecular")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setBackSpecular( (vl::fvec4)vlx_vec4( value.getArrayReal() ) );
        }
        else
        if (key == "BackShininess")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value );
          obj->setBackShininess( (float)value.getReal() );
        }
        else
        if (key == "ColorMaterialEnabled")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool, value );
          obj->setColorMaterialEnabled( value.getBool() );
        }
      }

      vl::EColorMaterial col_mat = vl::CM_AMBIENT_AND_DIFFUSE;
      const VLXValue* vlx_col_mat = vlx->getValue("ColorMaterial");
      if (vlx_col_mat)
      {
        VLX_IMPORT_CHECK_RETURN( vlx_col_mat->type() == VLXValue::Identifier, *vlx_col_mat );
        col_mat = vlx_EColorMaterial( *vlx_col_mat, s );
      }

      vl::EPolygonFace poly_face = vl::PF_FRONT_AND_BACK;
      const VLXValue* vlx_poly_mat = vlx->getValue("ColorMaterialFace");
      if (vlx_poly_mat)
      {
        VLX_IMPORT_CHECK_RETURN( vlx_poly_mat->type() == VLXValue::Identifier, *vlx_poly_mat );
        poly_face = vlx_EPolygonFace( *vlx_poly_mat, s );
      }

      obj->setColorMaterial( poly_face, col_mat );
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Material> obj = new vl::Material;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importMaterial(s, vlx, obj.get());
      return obj;
    }

    void exportMaterial(const vl::Material* obj, VLXStructure* vlx)
    {
      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());

      *vlx << "FrontAmbient" << vlx_toValue((vl::vec4)obj->frontAmbient());
      *vlx << "FrontDiffuse" << vlx_toValue((vl::vec4)obj->frontDiffuse());
      *vlx << "FrontEmission" << vlx_toValue((vl::vec4)obj->frontEmission());
      *vlx << "FrontSpecular" << vlx_toValue((vl::vec4)obj->frontSpecular());
      *vlx << "FrontShininess" << (double)obj->frontShininess();

      *vlx << "BackAmbient" << vlx_toValue((vl::vec4)obj->backAmbient());
      *vlx << "BackDiffuse" << vlx_toValue((vl::vec4)obj->backDiffuse());
      *vlx << "BackEmission" << vlx_toValue((vl::vec4)obj->backEmission());
      *vlx << "BackSpecular" << vlx_toValue((vl::vec4)obj->backSpecular());
      *vlx << "BackShininess" << (double)obj->backShininess();

      *vlx << "ColorMaterial" << vlx_Identifier(vlx_EColorMaterial(obj->colorMaterial()));
      *vlx << "ColorMaterialFace" << vlx_Identifier(vlx_EPolygonFace(obj->colorMaterialFace()));

      *vlx << "ColorMaterialEnabled" << obj->colorMaterialEnabled();
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Material* cast_obj = obj->as<vl::Material>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("material_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportMaterial(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::DepthSortCallback */
  struct VLXClassWrapper_ActorEventCallback: public ClassWrapper
  {
    void importActorEventCallback(VLXSerializer& s, const VLXStructure* vlx, vl::ActorEventCallback* obj)
    {
      if (obj->isOfType(vl::DepthSortCallback::Type()))
      {
        const VLXValue* vlx_sm = vlx->getValue("SortMode");
        VLX_IMPORT_CHECK_RETURN( vlx_sm->type() == VLXValue::Identifier, *vlx_sm )
        if (vlx_sm)
        {
          vl::ESortMode sm = vl::SM_SortBackToFront;
          if ( vlx_sm->getIdentifier() == "SM_SortBackToFront" )
            sm = vl::SM_SortBackToFront;
          else
          if ( vlx_sm->getIdentifier() == "SM_SortFrontToBack" )
            sm = vl::SM_SortFrontToBack;
          else
            s.signalImportError( vl::Say("Line %n : unknown sort mode '%s'.\n") << vlx_sm->lineNumber() << vlx_sm->getIdentifier() );
          obj->as<vl::DepthSortCallback>()->setSortMode(sm);
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::ActorEventCallback> obj = NULL;

      if (vlx->tag() == "<vl::DepthSortCallback>")
        obj = new vl::DepthSortCallback;
      else
      {
        s.signalImportError( vl::Say("Line %n : ActorEventCallback type not supported for import.\n") << vlx->lineNumber() );
        return NULL;
      }

      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importActorEventCallback(s, vlx, obj.get());
      return obj;
    }

    void exportActorEventCallback(VLXSerializer& s, const vl::ActorEventCallback* cb, VLXStructure* vlx)
    {
      if (cb->classType() == vl::DepthSortCallback::Type())
      {
        const vl::DepthSortCallback* dsc = cb->as<vl::DepthSortCallback>();

        if (dsc->sortMode() == vl::SM_SortBackToFront)
          *vlx << "SortMode" << vlx_Identifier("SM_SortBackToFront");
        else
          *vlx << "SortMode" << vlx_Identifier("SM_SortFrontToBack");
      }
      else
      {
        s.signalExportError("ActorEventCallback type not supported for export.\n");
      }
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::ActorEventCallback* cast_obj = obj->as<vl::ActorEventCallback>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("actorcallback_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportActorEventCallback(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::Texture */
  struct VLXClassWrapper_Texture: public ClassWrapper
  {
    void importTexture(VLXSerializer& s, const VLXStructure* vlx, vl::Texture* obj)
    {
      const VLXValue* name = vlx->getValue("ObjectName");
      if (name)
        obj->setObjectName( name->getString() );

      obj->setSetupParams( new vl::Texture::SetupParams );

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();

        if (key == "Dimension")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setupParams()->setDimension( vlx_ETextureDimension( value, s ) );
        }
        else
        if (key == "TexParameter")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value );
          vl::TexParameter* tex_param = s.importVLX( value.getStructure() )->as<vl::TexParameter>();
          VLX_IMPORT_CHECK_RETURN( tex_param != NULL, value );
          // copy the content over
          *obj->getTexParameter() = *tex_param;
        }
        else
        if (key == "ImagePath")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::String, value );
          std::string resolved_path = value.getString();
          s.resolvePath( resolved_path );
          obj->setupParams()->setImagePath( resolved_path.c_str() );
        }
        else
        if (key == "Format")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setupParams()->setFormat( vlx_ETextureFormat( value, s ) );
        }
        else
        if (key == "Width")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value );
          obj->setupParams()->setWidth( (int)value.getInteger() );
        }
        else
        if (key == "Height")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value );
          obj->setupParams()->setHeight( (int)value.getInteger() );
        }
        else
        if (key == "Depth")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value );
          obj->setupParams()->setDepth( (int)value.getInteger() );
        }
        else
        if (key == "GenMipmaps")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool, value );
          obj->setupParams()->setGenMipmaps( (int)value.getBool() );
        }
        else
        if (key == "BufferObject")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Structure, value );
          vl::BufferObject* buf_obj = s.importVLX( value.getStructure() )->as<vl::BufferObject>();
          VLX_IMPORT_CHECK_RETURN( buf_obj, value );
          obj->setupParams()->setBufferObject( buf_obj );
        }
        else
        if (key == "Samples")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Integer, value );
          obj->setupParams()->setSamples( (int)value.getInteger() );
        }
        else
        if (key == "FixedSamplesLocations")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool, value );
          obj->setupParams()->setFixedSamplesLocations( (int)value.getBool() );
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::Texture> obj = new vl::Texture;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importTexture(s, vlx, obj.get());
      return obj;
    }

    void exportTexture(VLXSerializer& s, const vl::Texture* obj, VLXStructure* vlx)
    {
      // mic fixme:
      // - we should allow patterns such as initializing a texture from an image filled by a shader or procedure.
      // - we should allow avoid loading twice the same image or shader source or any externa resource etc. time for a resource manager?

      if (!obj->objectName().empty() && obj->objectName() != obj->className())
        *vlx << "ObjectName" << vlx_String(obj->objectName());

      if (obj->getTexParameter())
        *vlx << "TexParameter" << s.exportVLX(obj->getTexParameter());

      if (obj->setupParams())
      {
        const vl::Texture::SetupParams* par = obj->setupParams();

        if (par)
        {
          *vlx << "Dimension" << vlx_Identifier(vlx_ETextureDimension(par->dimension()));

          *vlx << "Format" << vlx_Identifier(vlx_ETextureFormat(par->format()));

          if (!par->imagePath().empty())
            *vlx << "ImagePath" << vlx_String(par->imagePath().toStdString());
          else
          if (par->image())
            *vlx << "ImagePath" << vlx_String(par->image()->filePath().toStdString());

          if (par->width())
            *vlx << "Width" << (long long)par->width();

          if (par->height())
            *vlx << "Height" << (long long)par->height();

          if (par->depth())
            *vlx << "Depth" << (long long)par->depth();

          *vlx << "GenMipmaps" << par->genMipmaps();

          // mic fixme: implement BufferObject importer/exporter
#if 0
          if (par->bufferObject())
          {
            *vlx << "BufferObject" << s.exportVLX(par->bufferObject());
          }
#endif

          if(par->samples())
          {
            *vlx << "Samples" << (long long)par->samples();
            *vlx << "FixedSamplesLocations" << par->fixedSamplesLocations();
          }
        }
      }
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::Texture* cast_obj = obj->as<vl::Texture>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("texture_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportTexture(s, cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::TexParameter */
  struct VLXClassWrapper_TexParameter: public ClassWrapper
  {
    void importTexParameter(VLXSerializer& s, const VLXStructure* vlx, vl::TexParameter* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLXValue& value = vlx->value()[i].value();

        if (key == "MinFilter")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setMinFilter( vlx_ETexParamFilter( value, s ) );
        }
        else
        if (key == "MagFilter")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setMagFilter( vlx_ETexParamFilter( value, s ) );
        }
        else
        if (key == "WrapS")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setWrapS( vlx_ETexParamWrap( value, s ) );
        }
        else
        if (key == "WrapT")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setWrapT( vlx_ETexParamWrap( value, s ) );
        }
        else
        if (key == "WrapR")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setWrapR( vlx_ETexParamWrap( value, s ) );
        }
        else
        if (key == "CompareMode")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setCompareMode( vlx_ETexCompareMode( value, s ) );
        }
        else
        if (key == "CompareFunc")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setCompareFunc( vlx_ETexCompareFunc( value, s ) );
        }
        else
        if (key == "DepthTextureMode")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Identifier, value );
          obj->setDepthTextureMode( vlx_EDepthTextureMode( value, s ) );
        }
        else
        if (key == "BorderColor")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::ArrayReal, value );
          obj->setBorderColor( (vl::fvec4)vlx_vec4(value.getArrayReal()) );
        }
        else
        if (key == "Anisotropy")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Real, value );
          obj->setAnisotropy( (float)value.getReal() );
        }
        else
        if (key == "GenerateMipmap")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLXValue::Bool, value );
          obj->setGenerateMipmap( value.getBool() );
        }
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::TexParameter> obj = new vl::TexParameter;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importTexParameter(s, vlx, obj.get());
      return obj;
    }

    void exportTexParameter(const vl::TexParameter* texparam, VLXStructure* vlx)
    {
      *vlx << "MinFilter" << vlx_Identifier(vlx_ETexParamFilter(texparam->minFilter()));
      *vlx << "MagFilter" << vlx_Identifier(vlx_ETexParamFilter(texparam->magFilter()));
      *vlx << "WrapS" << vlx_Identifier(vlx_ETexParamWrap(texparam->wrapS()));
      *vlx << "WrapT" << vlx_Identifier(vlx_ETexParamWrap(texparam->wrapT()));
      *vlx << "WrapR" << vlx_Identifier(vlx_ETexParamWrap(texparam->wrapR()));
      *vlx << "CompareMode" << vlx_Identifier(vlx_ETexCompareMode(texparam->compareMode()));
      *vlx << "CompareFunc" << vlx_Identifier(vlx_ETexCompareFunc(texparam->compareFunc()));
      *vlx << "DepthTextureMode" << vlx_Identifier(vlx_EDepthTextureMode(texparam->depthTextureMode()));
      *vlx << "BorderColor" << vlx_toValue((vl::vec4)texparam->borderColor());
      *vlx << "Anisotropy" << texparam->anisotropy();
      *vlx << "GenerateMipmap" << texparam->generateMipmap();
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::TexParameter* cast_obj = obj->as<vl::TexParameter>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("texparam_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportTexParameter(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  /** VLX wrapper of vl::TextureImageUnit */
  struct VLXClassWrapper_TextureImageUnit: public ClassWrapper
  {
    void importTextureSampler(VLXSerializer& s, const VLXStructure* vlx, vl::TextureImageUnit* obj)
    {
      const VLXValue* vlx_texture = vlx->getValue("Texture");
      if (vlx_texture)
      {
        VLX_IMPORT_CHECK_RETURN(vlx_texture->type() == VLXValue::Structure, *vlx_texture);
        vl::Texture* texture = s.importVLX(vlx_texture->getStructure())->as<vl::Texture>();
        VLX_IMPORT_CHECK_RETURN( texture != NULL , *vlx_texture);
        obj->setTexture(texture);
      }
    }

    virtual vl::ref<vl::Object> importVLX(VLXSerializer& s, const VLXStructure* vlx)
    {
      vl::ref<vl::TextureImageUnit> obj = new vl::TextureImageUnit;
      // register imported structure asap
      s.registerImportedStructure(vlx, obj.get());
      importTextureSampler(s, vlx, obj.get());
      return obj;
    }

    void exportTextureSampler(VLXSerializer& s, const vl::TextureImageUnit* tex_sampler, VLXStructure* vlx)
    {
      if (tex_sampler->texture())
        *vlx << "Texture" << s.exportVLX(tex_sampler->texture());
    }

    virtual vl::ref<VLXStructure> exportVLX(VLXSerializer& s, const vl::Object* obj)
    {
      const vl::TextureImageUnit* cast_obj = obj->as<vl::TextureImageUnit>(); VL_CHECK(cast_obj)
      vl::ref<VLXStructure> vlx = new VLXStructure(vlx_makeTag(obj).c_str(), s.generateID("texsampler_"));
      // register exported object asap
      s.registerExportedObject(obj, vlx.get());
      exportTextureSampler(s, cast_obj, vlx.get());
      return vlx;
    }
  };
}

#endif
