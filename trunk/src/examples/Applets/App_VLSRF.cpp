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

// mic fixme
#include <vlCore/SRF.hpp>

#include "BaseDemo.hpp"
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/DrawPixels.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlCore/BufferedStream.hpp>
#include <vlCore/DiskFile.hpp>
#include <vlGraphics/MultiDrawElements.hpp>
#include <vlGraphics/TriangleStripGenerator.hpp>
#include <vlCore/ResourceDatabase.hpp>

using namespace vl;

class App_VLSRF: public BaseDemo
{
public:
  const char* getBoolCR(bool ok)
  {
    return ok ? "true\n" : "false\n";
  }

  void srfExport_Renderable(Renderable* ren)
  {
    if (mObject_Ref_Count[ren] > 1)
      mSRFString += indent() + "ID = " + getUID(ren) + "\n";
    mSRFString += indent() + "VBOEnabled = " + getBoolCR(ren->isVBOEnabled());
    mSRFString += indent() + "DisplayListEnabled = " + getBoolCR(ren->isDisplayListEnabled());
    mSRFString += indent() + "AABB = "; mAssign = true; srfExport_AABB(ren->boundingBox());
    mSRFString += indent() + "Sphere = "; mAssign = true; srfExport_Sphere(ren->boundingSphere());
  }

  void srfExport_AABB(const AABB& aabb)
  {
    // AABBs are always inlined
    mSRFString += indent() + "<AABB>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    mSRFString += indent() + String::printf("MinCorner = ( %f %f %f )\n", aabb.minCorner().x(), aabb.minCorner().y(), aabb.minCorner().z() );
    mSRFString += indent() + String::printf("MaxCorner = ( %f %f %f )\n", aabb.maxCorner().x(), aabb.maxCorner().y(), aabb.maxCorner().z() );
    --mIndent;
    mSRFString += indent() + "}\n"; 
  }

  void srfExport_Sphere(const Sphere& sphere)
  {
    // AABBs are always inlined
    mSRFString += indent() + "<Sphere>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    mSRFString += indent() + String::printf("Center = ( %f %f %f )\n", sphere.center().x(), sphere.center().y(), sphere.center().z() );
    mSRFString += indent() + String::printf("Radius = %f\n", sphere.radius() );
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfPrelink_Geometry(const Geometry* geom)
  {
    // --- generate UIDs ---
    // geometry itself
    generateUID(geom, "geometry_");
    // vertex arrays
    generateUID(geom->vertexArray(), "vertex_array_");
    generateUID(geom->normalArray(), "normal_array");
    generateUID(geom->colorArray(),  "color_array");
    generateUID(geom->secondaryColorArray(), "secondary_color_array");
    generateUID(geom->fogCoordArray(), "fogcoord_array");
    for(size_t i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      generateUID(geom->texCoordArray(i), "texcoord_array_");
    for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      if (geom->vertexAttribArray(i))
        generateUID(geom->vertexAttribArray(i)->data(), "vertexattrib_array_");
    // draw elements
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      generateUID(geom->drawCalls()->at(i), "drawcall_");
      generateUID(geom->drawCalls()->at(i)->patchParameter(), "patchparam_");
    }
  }

  void srfExport_Geometry(Geometry* geom)
  {
    if (isDefined(geom))
    {
      mSRFString += indent() + getUID(geom) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(geom);

    mSRFString += indent() + "<Geometry>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    mSRFString += indent() + "// renderable\n";
    srfExport_Renderable(geom);
    // vertex arrays
    mSRFString += "\n" + indent() + "// vertex arrays\n";
    if (geom->vertexArray()) { mSRFString += indent() + "VertexArray = ";  mAssign = true; srfExport_Array(geom->vertexArray()); }
    if (geom->normalArray()) { mSRFString += indent() + "NormalArray = ";  mAssign = true; srfExport_Array(geom->normalArray()); }
    if (geom->colorArray()) { mSRFString += indent() + "ColorArray = ";  mAssign = true; srfExport_Array(geom->colorArray()); }
    if (geom->secondaryColorArray()) { mSRFString += indent() + "SecondaryColorArray = ";  mAssign = true; srfExport_Array(geom->secondaryColorArray()); }
    if (geom->fogCoordArray()) { mSRFString += indent() + "FogCoordArray = ";  mAssign = true; srfExport_Array(geom->fogCoordArray()); }
    for( int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      if (geom->texCoordArray(i)) { mSRFString += indent() + String::printf("TexCoordArray%d = ", i);  mAssign = true; srfExport_Array(geom->texCoordArray(i)); }
    for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      if (geom->vertexAttribArray(i))
      {
        if (geom->vertexAttribArray(i)) { mSRFString += indent() + String::printf("VertexAttribArray%d = ",i); mAssign = true; srfExport_VertexAttribInfo(geom->vertexAttribArray(i)); }
      }
    // draw calls
    mSRFString += "\n" + indent() + "// draw calls\n";
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      mSRFString += indent() + "DrawCall = ";  mAssign = true; srfExport_DrawCall(geom->drawCalls()->at(i));
    }
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfPrelink_Effect(const Effect*)
  {
    // mic fixme: implement
  }

  void srfExport_Effect(const Effect*)
  {
    // mic fixme: implement
  }

  void srfPrelink_Shader(const Shader* )
  {
    // mic fixme: implement
  }

  void srfExport_Shader(const Shader* )
  {
    // mic fixme: implement
  }

  void srfPrelink_Transform(const Transform*)
  {
    // mic fixme: implement
  }

  void srfExport_Transform(const Transform*)
  {
    // mic fixme: implement
  }

  void srfPrelink_Actor(const Actor*)
  {
    // mic fixme: implement
  }

  void srfExport_Actor(const Actor*)
  {
    // mic fixme: implement
  }

  void srfPrelink_Camera(const Camera*)
  {
    // mic fixme: implement
  }

  void srfExport_Camera(const Camera*)
  {
    // mic fixme: implement
  }

  void srfExport_VertexAttribInfo(VertexAttribInfo* info)
  {
    if (isDefined(info))
    {
      mSRFString += indent() + getUID(info) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(info);

      mSRFString += indent() + "<VertexAttribInfo>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      if (mObject_Ref_Count[info] > 1)
        mSRFString += indent() + "ID = " + getUID(info) + "\n";
      // mSRFString += indent() + String::printf("AttribLocation = %d\n", info->attribLocation());
      mSRFString += indent() + "Data = "; mAssign = true; srfExport_Array(info->data());
      mSRFString += indent() + "Normalize = " + (info->normalize() ? "true\n" : "false\n");
      mSRFString += indent() + "Interpretation = ";
      switch(info->interpretation())
      {
      case VAI_NORMAL: mSRFString += "VAI_NORMAL\n"; break;
      case VAI_INTEGER: mSRFString += "VAI_INTEGER\n"; break;
      case VAI_DOUBLE: mSRFString += "VAI_DOUBLE\n"; break;
      }
      --mIndent;
      mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array1(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 40)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = ( ";
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i) );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n" + indent();
      }
      mSRFString += " )\n";
    }
    mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array2(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 30)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = ( ";
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i).x(), arr->at(i).y() );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n" + indent();
      }
      mSRFString += " )\n";
    }
    mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array3(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 20)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = ( ";
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i).x(), arr->at(i).y(), arr->at(i).z() );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n" + indent();
      }
      mSRFString += " )\n";
    }
    mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array4(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 10)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = ( ";
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i).x(), arr->at(i).y(), arr->at(i).z(), arr->at(i).w() );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n" + indent();
      }
      mSRFString += " )\n";
    }
    mSRFString += indent() + "}\n";
  }

  void srfExport_Array(ArrayAbstract* arr_abstract)
  {
    if (isDefined(arr_abstract))
    {
      mSRFString += indent() + getUID(arr_abstract) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(arr_abstract);

    if(arr_abstract->classType() == ArrayUInt1::Type())
      srfExport_Array1<ArrayUInt1>(arr_abstract, "<ArrayUInt1>", "%u "); // mic fixme: these can actually be user specified
    else
    if(arr_abstract->classType() == ArrayUInt2::Type())
      srfExport_Array2<ArrayUInt2>(arr_abstract, "<ArrayUInt2>", "%u %u ");
    else
    if(arr_abstract->classType() == ArrayUInt3::Type())
      srfExport_Array3<ArrayUInt3>(arr_abstract, "<ArrayUInt3>", "%u %u %u ");
    else
    if(arr_abstract->classType() == ArrayUInt4::Type())
      srfExport_Array4<ArrayUInt4>(arr_abstract, "<ArrayUInt4>", "%u %u %u %u ");
    else

    if(arr_abstract->classType() == ArrayInt1::Type())
      srfExport_Array1<ArrayInt1>(arr_abstract, "<ArrayInt1>", "%d ");
    else
    if(arr_abstract->classType() == ArrayInt2::Type())
      srfExport_Array2<ArrayInt2>(arr_abstract, "<ArrayInt2>", "%d %d ");
    else
    if(arr_abstract->classType() == ArrayInt3::Type())
      srfExport_Array3<ArrayInt3>(arr_abstract, "<ArrayInt3>", "%d %d %d ");
    else
    if(arr_abstract->classType() == ArrayInt4::Type())
      srfExport_Array4<ArrayInt4>(arr_abstract, "<ArrayInt4>", "%d %d %d %d ");
    else

    if(arr_abstract->classType() == ArrayUShort1::Type())
      srfExport_Array1<ArrayUShort1>(arr_abstract, "<ArrayUShort1>", "%u ");
    else
    if(arr_abstract->classType() == ArrayUShort2::Type())
      srfExport_Array2<ArrayUShort2>(arr_abstract, "<ArrayUShort2>", "%u %u ");
    else
    if(arr_abstract->classType() == ArrayUShort3::Type())
      srfExport_Array3<ArrayUShort3>(arr_abstract, "<ArrayUShort3>", "%u %u %u ");
    else
    if(arr_abstract->classType() == ArrayUShort4::Type())
      srfExport_Array4<ArrayUShort4>(arr_abstract, "<ArrayUShort4>", "%u %u %u %u ");
    else

    if(arr_abstract->classType() == ArrayUShort1::Type())
      srfExport_Array1<ArrayUShort1>(arr_abstract, "<ArrayUShort1>", "%u ");
    else
    if(arr_abstract->classType() == ArrayUShort2::Type())
      srfExport_Array2<ArrayUShort2>(arr_abstract, "<ArrayUShort2>", "%u %u ");
    else
    if(arr_abstract->classType() == ArrayUShort3::Type())
      srfExport_Array3<ArrayUShort3>(arr_abstract, "<ArrayUShort3>", "%u %u %u ");
    else
    if(arr_abstract->classType() == ArrayUShort4::Type())
      srfExport_Array4<ArrayUShort4>(arr_abstract, "<ArrayUShort4>", "%u %u %u %u ");
    else

    if(arr_abstract->classType() == ArrayShort1::Type())
      srfExport_Array1<ArrayShort1>(arr_abstract, "<ArrayShort1>", "%d ");
    else
    if(arr_abstract->classType() == ArrayShort2::Type())
      srfExport_Array2<ArrayShort2>(arr_abstract, "<ArrayShort2>", "%d %d ");
    else
    if(arr_abstract->classType() == ArrayShort3::Type())
      srfExport_Array3<ArrayShort3>(arr_abstract, "<ArrayShort3>", "%d %d %d ");
    else
    if(arr_abstract->classType() == ArrayShort4::Type())
      srfExport_Array4<ArrayShort4>(arr_abstract, "<ArrayShort4>", "%d %d %d %d ");
    else

    if(arr_abstract->classType() == ArrayUByte1::Type())
      srfExport_Array1<ArrayUByte1>(arr_abstract, "<ArrayUByte1>", "%u ");
    else
    if(arr_abstract->classType() == ArrayUByte2::Type())
      srfExport_Array2<ArrayUByte2>(arr_abstract, "<ArrayUByte2>", "%u %u ");
    else
    if(arr_abstract->classType() == ArrayUByte3::Type())
      srfExport_Array3<ArrayUByte3>(arr_abstract, "<ArrayUByte3>", "%u %u %u ");
    else
    if(arr_abstract->classType() == ArrayUByte4::Type())
      srfExport_Array4<ArrayUByte4>(arr_abstract, "<ArrayUByte4>", "%u %u %u %u ");
    else

    if(arr_abstract->classType() == ArrayByte1::Type())
      srfExport_Array1<ArrayByte1>(arr_abstract, "<ArrayByte1>", "%d ");
    else
    if(arr_abstract->classType() == ArrayByte2::Type())
      srfExport_Array2<ArrayByte2>(arr_abstract, "<ArrayByte2>", "%d %d ");
    else
    if(arr_abstract->classType() == ArrayByte3::Type())
      srfExport_Array3<ArrayByte3>(arr_abstract, "<ArrayByte3>", "%d %d %d ");
    else
    if(arr_abstract->classType() == ArrayByte4::Type())
      srfExport_Array4<ArrayByte4>(arr_abstract, "<ArrayByte4>", "%d %d %d %d ");
    else

    if(arr_abstract->classType() == ArrayFloat1::Type())
      srfExport_Array1<ArrayFloat1>(arr_abstract, "<ArrayFloat1>", "%f ");
    else
    if(arr_abstract->classType() == ArrayFloat2::Type())
      srfExport_Array2<ArrayFloat2>(arr_abstract, "<ArrayFloat2>", "%f %f ");
    else
    if(arr_abstract->classType() == ArrayFloat3::Type())
      srfExport_Array3<ArrayFloat3>(arr_abstract, "<ArrayFloat3>", "%f %f %f ");
    else
    if(arr_abstract->classType() == ArrayFloat4::Type())
      srfExport_Array4<ArrayFloat4>(arr_abstract, "<ArrayFloat4>", "%f %f %f %f ");
    else

    if(arr_abstract->classType() == ArrayDouble1::Type())
      srfExport_Array1<ArrayDouble1>(arr_abstract, "<ArrayDouble1>", "%Lf ");
    else
    if(arr_abstract->classType() == ArrayDouble2::Type())
      srfExport_Array2<ArrayDouble2>(arr_abstract, "<ArrayDouble2>", "%Lf %Lf ");
    else
    if(arr_abstract->classType() == ArrayDouble3::Type())
      srfExport_Array3<ArrayDouble3>(arr_abstract, "<ArrayDouble3>", "%Lf %Lf %Lf ");
    else
    if(arr_abstract->classType() == ArrayDouble4::Type())
      srfExport_Array4<ArrayDouble4>(arr_abstract, "<ArrayDouble4>", "%Lf %Lf %Lf %Lf ");
    else
    {
      Log::error("Array type not supported for export.\n");
      VL_TRAP();
    }
  }

  void srfExport_DrawCallBase(DrawCall* dcall)
  {
      if (mObject_Ref_Count[dcall] > 1)
        mSRFString += indent() + "ID = " + getUID(dcall) + "\n";
      switch(dcall->primitiveType())
      {
        case PT_POINTS:                   mSRFString += indent() + "PrimitiveType = PT_POINTS\n"; break;
        case PT_LINES:                    mSRFString += indent() + "PrimitiveType = PT_LINES\n"; break;
        case PT_LINE_LOOP:                mSRFString += indent() + "PrimitiveType = PT_LINE_LOOP\n"; break;
        case PT_LINE_STRIP:               mSRFString += indent() + "PrimitiveType = PT_LINE_STRIP\n"; break;
        case PT_TRIANGLES:                mSRFString += indent() + "PrimitiveType = PT_TRIANGLES\n"; break;
        case PT_TRIANGLE_STRIP:           mSRFString += indent() + "PrimitiveType = PT_TRIANGLE_STRIP\n"; break;
        case PT_TRIANGLE_FAN:             mSRFString += indent() + "PrimitiveType = PT_TRIANGLE_FAN\n"; break;
        case PT_QUADS:                    mSRFString += indent() + "PrimitiveType = PT_QUADS\n"; break;
        case PT_QUAD_STRIP:               mSRFString += indent() + "PrimitiveType = PT_QUAD_STRIP\n"; break;
        case PT_POLYGON:                  mSRFString += indent() + "PrimitiveType = PT_POLYGON\n"; break;

        case PT_LINES_ADJACENCY:          mSRFString += indent() + "PrimitiveType = PT_LINES_ADJACENCY\n"; break;
        case PT_LINE_STRIP_ADJACENCY:     mSRFString += indent() + "PrimitiveType = PT_LINE_STRIP_ADJACENCY\n"; break;
        case PT_TRIANGLES_ADJACENCY:      mSRFString += indent() + "PrimitiveType = PT_TRIANGLES_ADJACENCY\n"; break;
        case PT_TRIANGLE_STRIP_ADJACENCY: mSRFString += indent() + "PrimitiveType = PT_TRIANGLE_STRIP_ADJACENCY\n"; break;

        case PT_PATCHES:                  mSRFString += indent() + "PrimitiveType = PT_PATCHES\n"; break;

        case PT_UNKNOWN:                  mSRFString += indent() + "PrimitiveType = PT_UNKNOWN\n"; break;
      }
      mSRFString += indent() + "Enabled = " + (dcall->isEnabled() ? "true" : "false") + "\n";

      if (dcall->patchParameter())
      {
        mSRFString += indent() + "PatchParameter = "; mAssign = true; srfExport_PatchParameter(dcall->patchParameter());
      }
  }

  void srfExport_PatchParameter(PatchParameter* pp)
  {
    if (isDefined(pp))
    {
      mSRFString += indent() + getUID(pp) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(pp);

    mSRFString += indent() + "<PatchParameter>\n";
    mSRFString += indent() + "{\n";
    ++mIndent;
    if (mObject_Ref_Count[pp] > 1)
      mSRFString += indent() + "ID = " + getUID(pp) + "\n";
    mSRFString += indent() + String::printf("PatchVertices = %d\n", pp->patchVertices());
    mSRFString += indent() + String::printf("PatchDefaultOuterLevel = ( %f %f %f %f )\n", pp->patchDefaultOuterLevel().x(), pp->patchDefaultOuterLevel().y(), pp->patchDefaultOuterLevel().z(), pp->patchDefaultOuterLevel().w() );
    mSRFString += indent() + String::printf("PatchDefaultInnerLevel = ( %f %f )\n",       pp->patchDefaultInnerLevel().x(), pp->patchDefaultInnerLevel().y() );
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfExport_DrawCall(DrawCall* dcall)
  {
    if (isDefined(dcall))
    {
      mSRFString += indent() + getUID(dcall) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(dcall);

    if (dcall->classType() == DrawArrays::Type())
    {
      DrawArrays* da = dcall->as<DrawArrays>();
      mSRFString += indent() + "<DrawArrays>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(da);
      mSRFString += indent() + String::printf("Instances = %d\n", da->instances());
      mSRFString += indent() + String::printf("Start = %d\n", da->start());
      mSRFString += indent() + String::printf("Count= %d\n", da->count());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == DrawElementsUInt::Type())
    {
      DrawElementsUInt* de = dcall->as<DrawElementsUInt>();
      mSRFString += indent() + "<DrawElementsUInt>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + String::printf("Instances = %d\n", de->instances());
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + String::printf("BaseVertex = %d\n", de->baseVertex());
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == DrawElementsUShort::Type())
    {
      DrawElementsUShort* de = dcall->as<DrawElementsUShort>();
      mSRFString += indent() + "<DrawElementsUShort>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + String::printf("Instances = %d\n", de->instances());
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + String::printf("BaseVertex = %d\n", de->baseVertex());
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == DrawElementsUByte::Type())
    {
      DrawElementsUByte* de = dcall->as<DrawElementsUByte>();
      mSRFString += indent() + "<DrawElementsUByte>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + String::printf("Instances = %d\n", de->instances());
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + String::printf("BaseVertex = %d\n", de->baseVertex());
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == MultiDrawElementsUInt::Type())
    {
      MultiDrawElementsUInt* de = dcall->as<MultiDrawElementsUInt>();
      mSRFString += indent() + "<MultiDrawElementsUInt>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      mSRFString += indent() + "CountVector = "; srfExport_vector1(de->countVector(), "%u ");
      mSRFString += indent() + "BaseVertices = "; srfExport_vector1(de->baseVertices(), "%u ");
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == MultiDrawElementsUShort::Type())
    {
      MultiDrawElementsUShort* de = dcall->as<MultiDrawElementsUShort>();
      mSRFString += indent() + "<MultiDrawElementsUShort>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      mSRFString += indent() + "CountVector = "; srfExport_vector1(de->countVector(), "%u ");
      mSRFString += indent() + "BaseVertices = "; srfExport_vector1(de->baseVertices(), "%u ");
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == MultiDrawElementsUByte::Type())
    {
      MultiDrawElementsUByte* de = dcall->as<MultiDrawElementsUByte>();
      mSRFString += indent() + "<MultiDrawElementsUByte>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      mSRFString += indent() + "CountVector = "; srfExport_vector1(de->countVector(), "%u ");
      mSRFString += indent() + "BaseVertices = "; srfExport_vector1(de->baseVertices(), "%u ");
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    {
      Log::error("DrawCall type not supported for export.\n");
      VL_TRAP();
    }
  }

  template<typename T>
  void srfExport_vector1(const std::vector<T>& vec, const char* format)
  {
    mSRFString += "( ";
    for(size_t i=0; i<vec.size(); ++i)
    {
      mSRFString += String::printf(format, vec[i]);
      /*if (i && (i%10==0) && i!=vec.size()-1)
        mSRFString += indent() + "\n"*/
    }
    mSRFString += " )\n";
  }

  String indent()
  {
    String str;
    if (mAssign)  
      mAssign = false;
    else
      for(int i=0; i<mIndent; ++i)
        str += '\t';
    return str;
  }

  String getUID(Object* object)
  {
    std::map< ref<Object>, String >::iterator it = mObject_To_UID.find(object);
    if( it != mObject_To_UID.end() )
      return it->second;
    else
    {
      VL_TRAP();
      return "";
    }
  }

  void generateUID(const Object* object, const char* prefix)
  {
    if (object)
    {
      // add reference count
      ++mObject_Ref_Count[object];
      if (mObject_To_UID.find(object) == mObject_To_UID.end())
      {
        String uid = String::printf("#%sid%d", prefix, ++mUIDCounter);
        mObject_To_UID[object] = uid;
      }
    }
  }

  bool isDefined(Object* obj)
  {
    return mAlreadyDefined.find(obj) != mAlreadyDefined.end();
  }

  void srfExport_ResourceDatabase(ResourceDatabase* res_db)
  {
    mSRFString += indent() + "<ResourceDatabase>\n";
    mSRFString += indent() + "{\n";
    ++mIndent;
    {
      mSRFString += indent() + "Resources = [\n";
      ++mIndent;
      {
        for(size_t i=0; i<res_db->resources().size(); ++i)
        {
          if(res_db->resources()[i]->classType() == Geometry::Type())
            srfExport_Geometry(res_db->resources()[i]->as<Geometry>());
          else
          if(res_db->resources()[i]->classType() == Effect::Type())
            srfExport_Effect(res_db->resources()[i]->as<Effect>());
          else
          if(res_db->resources()[i]->classType() == Shader::Type())
            srfExport_Shader(res_db->resources()[i]->as<Shader>());
          else
          if(res_db->resources()[i]->classType() == Transform::Type())
            srfExport_Transform(res_db->resources()[i]->as<Transform>());
          else
          if(res_db->resources()[i]->classType() == Actor::Type())
            srfExport_Actor(res_db->resources()[i]->as<Actor>());
          else
          if(res_db->resources()[i]->classType() == Camera::Type())
            srfExport_Camera(res_db->resources()[i]->as<Camera>());
        }
      }
      --mIndent;
      mSRFString += indent() + "]\n";
    }
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfPrelink_ResourceDatabase(ResourceDatabase* res_db)
  {
    for(size_t i=0; i<res_db->resources().size(); ++i)
    {
      if(res_db->resources()[i]->classType() == Geometry::Type())
        srfPrelink_Geometry(res_db->resources()[i]->as<Geometry>());
      else
      if(res_db->resources()[i]->classType() == Effect::Type())
        srfPrelink_Effect(res_db->resources()[i]->as<Effect>());
      else
      if(res_db->resources()[i]->classType() == Shader::Type())
        srfPrelink_Shader(res_db->resources()[i]->as<Shader>());
      else
      if(res_db->resources()[i]->classType() == Transform::Type())
        srfPrelink_Transform(res_db->resources()[i]->as<Transform>());
      else
      if(res_db->resources()[i]->classType() == Actor::Type())
        srfPrelink_Actor(res_db->resources()[i]->as<Actor>());
      else
      if(res_db->resources()[i]->classType() == Camera::Type())
        srfPrelink_Camera(res_db->resources()[i]->as<Camera>());
    }
  }

  //-----------------------------------------------------------------------------
  // IMPORT FUNCTIONS
  //-----------------------------------------------------------------------------

  ResourceDatabase* srfImport_ResourceDatabase(const SRF_Object* srf_res_db)
  {
    if (srf_res_db->tag() != "<ResourceDatabase>")
      return NULL;

    // mic fixme: output errors with line
    // <ResourceDatabase> must have at least one "Resources" key and must be the first one and must be a list!
    if (srf_res_db->value().size() < 1 || srf_res_db->value()[0].key() != "Resources" || srf_res_db->value()[0].value().type() != SRF_Value::List)
      return NULL;

    // link the SRF to the VL object
    ref<ResourceDatabase> res_db = new ResourceDatabase;
    mSRF_To_VL[srf_res_db] = res_db;

    // get the list
    const SRF_List* list = srf_res_db->value()[0].value().getList();
    for(size_t i=0; i<list->value().size(); ++i)
    {
      const SRF_Value& value = list->value()[i];

      // the member of this list must be all objects!
      // mic fixme: issue error
      if (value.type() != SRF_Value::Object)
        return false;

      const SRF_Object* obj = value.getObject();
      
      if (obj->tag() == "<Geometry>")
      {
        Geometry* geom = srfImport_Geometry(obj);
        if (geom)
          res_db->resources().push_back(geom);
      }
      else
      if (obj->tag() == "<Effect>")
      {
        Effect* fx = srfImport_Effect(obj);
        if (fx)
          res_db->resources().push_back(fx);
      }
      else
      if (obj->tag() == "<Shader>")
      {
        Shader* sh = srfImport_Shader(obj);
        if (sh)
          res_db->resources().push_back(sh);
      }
      else
      if (obj->tag() == "<Transform>")
      {
        Transform* tr = srfImport_Transform(obj);
        if (tr)
          res_db->resources().push_back(tr);
      }
      else
      if (obj->tag() == "<Actor>")
      {
        Actor* act = srfImport_Actor(obj);
        if (act)
          res_db->resources().push_back(act);
      }
      else
      if (obj->tag() == "<Camera>")
      {
        Camera* cam = srfImport_Camera(obj);
        if (cam)
          res_db->resources().push_back(cam);
      }
      else
      {
        // debug: this should not be an error, maybe a Log::debug()
        Log::error( Say("Skipping SRF_Object = %s\n") << obj->tag() );
      }
    }

    return res_db.get();
  }

  bool srfImport_AABB(const SRF_Object* srf_obj, AABB& aabb)
  {
    // mic fixme: we should check that the members are exactly two and are the two required
    // would be nice to have a validator...

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
        return false; // mic fixme
    }

    return true;
  }

  bool srfImport_Sphere(const SRF_Object* srf_obj, Sphere& sphere)
  {
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
        return false; // mic fixme
    }

    return true;
  }

  ArrayAbstract* srfImport_Array(const SRF_Object* srf_obj)
  {
    const SRF_Value* value = srf_obj->getValue("Value");
    
    // mic fixme
    if (!value)
      return NULL;

    ref<ArrayAbstract> arr_abstract;

    if (srf_obj->tag() == "<ArrayFloat1>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayFloat); // mic fixme
      const SRF_ArrayFloat* srf_arr_float = value->getArrayFloat();
      ref<ArrayFloat1> arr_float1 = new ArrayFloat1; arr_abstract = arr_float1;
      arr_float1->resize( srf_arr_float->value().size() );
      memcpy(arr_float1->ptr(), srf_arr_float->ptr(), arr_float1->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayFloat2>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayFloat); // mic fixme
      const SRF_ArrayFloat* srf_arr_float = value->getArrayFloat();
      VL_CHECK( srf_arr_float->value().size() % 2 == 0) // mic fixme
      ref<ArrayFloat2> arr_float2 = new ArrayFloat2; arr_abstract = arr_float2;
      arr_float2->resize( srf_arr_float->value().size() / 2 );
      memcpy(arr_float2->ptr(), srf_arr_float->ptr(), arr_float2->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayFloat3>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayFloat); // mic fixme
      const SRF_ArrayFloat* srf_arr_float = value->getArrayFloat();
      VL_CHECK( srf_arr_float->value().size() % 3 == 0) // mic fixme
      ref<ArrayFloat3> arr_float3 = new ArrayFloat3; arr_abstract = arr_float3;
      arr_float3->resize( srf_arr_float->value().size() / 3 );
      memcpy(arr_float3->ptr(), srf_arr_float->ptr(), arr_float3->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayFloat4>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayFloat); // mic fixme
      const SRF_ArrayFloat* srf_arr_float = value->getArrayFloat();
      VL_CHECK( srf_arr_float->value().size() % 4 == 0) // mic fixme
      ref<ArrayFloat4> arr_float4 = new ArrayFloat4; arr_abstract = arr_float4;
      arr_float4->resize( srf_arr_float->value().size() / 4 );
      memcpy(arr_float4->ptr(), srf_arr_float->ptr(), arr_float4->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayDouble1>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayDouble); // mic fixme
      const SRF_ArrayDouble* srf_arr_double = value->getArrayDouble();
      ref<ArrayDouble1> arr_double1 = new ArrayDouble1; arr_abstract = arr_double1;
      arr_double1->resize( srf_arr_double->value().size() );
      memcpy(arr_double1->ptr(), srf_arr_double->ptr(), arr_double1->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayDouble2>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayDouble); // mic fixme
      const SRF_ArrayDouble* srf_arr_double = value->getArrayDouble();
      VL_CHECK( srf_arr_double->value().size() % 2 == 0) // mic fixme
      ref<ArrayDouble2> arr_double2 = new ArrayDouble2; arr_abstract = arr_double2;
      arr_double2->resize( srf_arr_double->value().size() / 2 );
      memcpy(arr_double2->ptr(), srf_arr_double->ptr(), arr_double2->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayDouble3>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayDouble); // mic fixme
      const SRF_ArrayDouble* srf_arr_double = value->getArrayDouble();
      VL_CHECK( srf_arr_double->value().size() % 3 == 0) // mic fixme
      ref<ArrayDouble3> arr_double3 = new ArrayDouble3; arr_abstract = arr_double3;
      arr_double3->resize( srf_arr_double->value().size() / 3 );
      memcpy(arr_double3->ptr(), srf_arr_double->ptr(), arr_double3->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayDouble4>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayDouble); // mic fixme
      const SRF_ArrayDouble* srf_arr_double = value->getArrayDouble();
      VL_CHECK( srf_arr_double->value().size() % 4 == 0) // mic fixme
      ref<ArrayDouble4> arr_double4 = new ArrayDouble4; arr_abstract = arr_double4;
      arr_double4->resize( srf_arr_double->value().size() / 4 );
      memcpy(arr_double4->ptr(), srf_arr_double->ptr(), arr_double4->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayInt1>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      ref<ArrayInt1> arr_int1 = new ArrayInt1; arr_abstract = arr_int1;
      arr_int1->resize( srf_arr_int->value().size() );
      memcpy(arr_int1->ptr(), srf_arr_int->ptr(), arr_int1->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayInt2>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 2 == 0) // mic fixme
      ref<ArrayInt2> arr_int2 = new ArrayInt2; arr_abstract = arr_int2;
      arr_int2->resize( srf_arr_int->value().size() / 2 );
      memcpy(arr_int2->ptr(), srf_arr_int->ptr(), arr_int2->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayInt3>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 3 == 0) // mic fixme
      ref<ArrayInt3> arr_int3 = new ArrayInt3; arr_abstract = arr_int3;
      arr_int3->resize( srf_arr_int->value().size() / 3 );
      memcpy(arr_int3->ptr(), srf_arr_int->ptr(), arr_int3->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayInt4>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 4 == 0) // mic fixme
      ref<ArrayInt4> arr_int4 = new ArrayInt4; arr_abstract = arr_int4;
      arr_int4->resize( srf_arr_int->value().size() / 4 );
      memcpy(arr_int4->ptr(), srf_arr_int->ptr(), arr_int4->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayUInt1>") // mic fixme: make this read the full unsigned int field
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      ref<ArrayUInt1> arr_int1 = new ArrayUInt1; arr_abstract = arr_int1;
      arr_int1->resize( srf_arr_int->value().size() );
      memcpy(arr_int1->ptr(), srf_arr_int->ptr(), arr_int1->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayUInt2>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 2 == 0) // mic fixme
      ref<ArrayUInt2> arr_int2 = new ArrayUInt2; arr_abstract = arr_int2;
      arr_int2->resize( srf_arr_int->value().size() / 2 );
      memcpy(arr_int2->ptr(), srf_arr_int->ptr(), arr_int2->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayUInt3>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 3 == 0) // mic fixme
      ref<ArrayUInt3> arr_int3 = new ArrayUInt3; arr_abstract = arr_int3;
      arr_int3->resize( srf_arr_int->value().size() / 3 );
      memcpy(arr_int3->ptr(), srf_arr_int->ptr(), arr_int3->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayUInt4>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 4 == 0) // mic fixme
      ref<ArrayUInt4> arr_int4 = new ArrayUInt4; arr_abstract = arr_int4;
      arr_int4->resize( srf_arr_int->value().size() / 4 );
      memcpy(arr_int4->ptr(), srf_arr_int->ptr(), arr_int4->bytesUsed());
    }
    else
    if (srf_obj->tag() == "<ArrayShort1>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      ref<ArrayShort1> arr_short1 = new ArrayShort1; arr_abstract = arr_short1;
      arr_short1->resize( srf_arr_int->value().size() );
      typedef ArrayShort1::scalar_type type;
      for(size_t i=0; i<arr_short1->size(); ++i)
        arr_short1->at(i) = (type)srf_arr_int->value()[i];
    }
    else
    if (srf_obj->tag() == "<ArrayShort2>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 2 == 0) // mic fixme
      ref<ArrayShort2> arr_short2 = new ArrayShort2; arr_abstract = arr_short2;
      arr_short2->resize( srf_arr_int->value().size() / 2 );
      typedef ArrayShort2::scalar_type type;
      for(size_t i=0; i<arr_short2->size(); ++i)
        arr_short2->at(i) = svec2((type)srf_arr_int->value()[i*2+0], (type)srf_arr_int->value()[i*2+1]);
    }
    else
    if (srf_obj->tag() == "<ArrayShort3>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 3 == 0) // mic fixme
      ref<ArrayShort3> arr_short3 = new ArrayShort3; arr_abstract = arr_short3;
      arr_short3->resize( srf_arr_int->value().size() / 3 );
      typedef ArrayShort3::scalar_type type;
      for(size_t i=0; i<arr_short3->size(); ++i)
        arr_short3->at(i) = svec3((type)srf_arr_int->value()[i*3+0], (type)srf_arr_int->value()[i*3+1], (type)srf_arr_int->value()[i*3+2]);
    }
    else
    if (srf_obj->tag() == "<ArrayShort4>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 4 == 0) // mic fixme
      ref<ArrayShort4> arr_short4 = new ArrayShort4; arr_abstract = arr_short4;
      arr_short4->resize( srf_arr_int->value().size() / 4 );
      typedef ArrayShort4::scalar_type type;
      for(size_t i=0; i<arr_short4->size(); ++i)
        arr_short4->at(i) = svec4((type)srf_arr_int->value()[i*4+0], (type)srf_arr_int->value()[i*4+1], (type)srf_arr_int->value()[i*4+2], (type)srf_arr_int->value()[i*4+3]);
    }
    else
    if (srf_obj->tag() == "<ArrayUShort1>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      ref<ArrayUShort1> arr_short1 = new ArrayUShort1; arr_abstract = arr_short1;
      arr_short1->resize( srf_arr_int->value().size() );
      typedef ArrayUShort1::scalar_type type;
      for(size_t i=0; i<arr_short1->size(); ++i)
        arr_short1->at(i) = (type)srf_arr_int->value()[i];
    }
    else
    if (srf_obj->tag() == "<ArrayUShort2>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 2 == 0) // mic fixme
      ref<ArrayUShort2> arr_short2 = new ArrayUShort2; arr_abstract = arr_short2;
      arr_short2->resize( srf_arr_int->value().size() / 2 );
      typedef ArrayUShort2::scalar_type type;
      for(size_t i=0; i<arr_short2->size(); ++i)
        arr_short2->at(i) = usvec2((type)srf_arr_int->value()[i*2+0], (type)srf_arr_int->value()[i*2+1]);
    }
    else
    if (srf_obj->tag() == "<ArrayUShort3>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 3 == 0) // mic fixme
      ref<ArrayUShort3> arr_short3 = new ArrayUShort3; arr_abstract = arr_short3;
      arr_short3->resize( srf_arr_int->value().size() / 3 );
      typedef ArrayUShort3::scalar_type type;
      for(size_t i=0; i<arr_short3->size(); ++i)
        arr_short3->at(i) = usvec3((type)srf_arr_int->value()[i*3+0], (type)srf_arr_int->value()[i*3+1], (type)srf_arr_int->value()[i*3+2]);
    }
    else
    if (srf_obj->tag() == "<ArrayUShort4>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 4 == 0) // mic fixme
      ref<ArrayUShort4> arr_short4 = new ArrayUShort4; arr_abstract = arr_short4;
      arr_short4->resize( srf_arr_int->value().size() / 4 );
      typedef ArrayUShort4::scalar_type type;
      for(size_t i=0; i<arr_short4->size(); ++i)
        arr_short4->at(i) = usvec4((type)srf_arr_int->value()[i*4+0], (type)srf_arr_int->value()[i*4+1], (type)srf_arr_int->value()[i*4+2], (type)srf_arr_int->value()[i*4+3]);
    }
    else
    if (srf_obj->tag() == "<ArrayByte1>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      ref<ArrayByte1> arr_byte1 = new ArrayByte1; arr_abstract = arr_byte1;
      arr_byte1->resize( srf_arr_int->value().size() );
      typedef ArrayByte1::scalar_type type;
      for(size_t i=0; i<arr_byte1->size(); ++i)
        arr_byte1->at(i) = (type)srf_arr_int->value()[i];
    }
    else
    if (srf_obj->tag() == "<ArrayByte2>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 2 == 0) // mic fixme
      ref<ArrayByte2> arr_byte2 = new ArrayByte2; arr_abstract = arr_byte2;
      arr_byte2->resize( srf_arr_int->value().size() / 2 );
      typedef ArrayByte2::scalar_type type;
      for(size_t i=0; i<arr_byte2->size(); ++i)
        arr_byte2->at(i) = bvec2((type)srf_arr_int->value()[i*2+0], (type)srf_arr_int->value()[i*2+1]);
    }
    else
    if (srf_obj->tag() == "<ArrayByte3>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 3 == 0) // mic fixme
      ref<ArrayByte3> arr_byte3 = new ArrayByte3; arr_abstract = arr_byte3;
      arr_byte3->resize( srf_arr_int->value().size() / 3 );
      typedef ArrayByte3::scalar_type type;
      for(size_t i=0; i<arr_byte3->size(); ++i)
        arr_byte3->at(i) = bvec3((type)srf_arr_int->value()[i*3+0], (type)srf_arr_int->value()[i*3+1], (type)srf_arr_int->value()[i*3+2]);
    }
    else
    if (srf_obj->tag() == "<ArrayByte4>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 4 == 0) // mic fixme
      ref<ArrayByte4> arr_byte4 = new ArrayByte4; arr_abstract = arr_byte4;
      arr_byte4->resize( srf_arr_int->value().size() / 4 );
      typedef ArrayByte4::scalar_type type;
      for(size_t i=0; i<arr_byte4->size(); ++i)
        arr_byte4->at(i) = bvec4((type)srf_arr_int->value()[i*4+0], (type)srf_arr_int->value()[i*4+1], (type)srf_arr_int->value()[i*4+2], (type)srf_arr_int->value()[i*4+3]);
    }
    else
    if (srf_obj->tag() == "<ArrayUByte1>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      ref<ArrayUByte1> arr_byte1 = new ArrayUByte1; arr_abstract = arr_byte1;
      arr_byte1->resize( srf_arr_int->value().size() );
      typedef ArrayUByte1::scalar_type type;
      for(size_t i=0; i<arr_byte1->size(); ++i)
        arr_byte1->at(i) = (type)srf_arr_int->value()[i];
    }
    else
    if (srf_obj->tag() == "<ArrayUByte2>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 2 == 0) // mic fixme
      ref<ArrayUByte2> arr_byte2 = new ArrayUByte2; arr_abstract = arr_byte2;
      arr_byte2->resize( srf_arr_int->value().size() / 2 );
      typedef ArrayUByte2::scalar_type type;
      for(size_t i=0; i<arr_byte2->size(); ++i)
        arr_byte2->at(i) = ubvec2((type)srf_arr_int->value()[i*2+0], (type)srf_arr_int->value()[i*2+1]);
    }
    else
    if (srf_obj->tag() == "<ArrayUByte3>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 3 == 0) // mic fixme
      ref<ArrayUByte3> arr_byte3 = new ArrayUByte3; arr_abstract = arr_byte3;
      arr_byte3->resize( srf_arr_int->value().size() / 3 );
      typedef ArrayUByte3::scalar_type type;
      for(size_t i=0; i<arr_byte3->size(); ++i)
        arr_byte3->at(i) = ubvec3((type)srf_arr_int->value()[i*3+0], (type)srf_arr_int->value()[i*3+1], (type)srf_arr_int->value()[i*3+2]);
    }
    else
    if (srf_obj->tag() == "<ArrayUByte4>")
    {
      VL_CHECK(value->type() == SRF_Value::ArrayInt32); // mic fixme
      const SRF_ArrayInt32* srf_arr_int = value->getArrayInt32();
      VL_CHECK( srf_arr_int->value().size() % 4 == 0) // mic fixme
      ref<ArrayUByte4> arr_byte4 = new ArrayUByte4; arr_abstract = arr_byte4;
      arr_byte4->resize( srf_arr_int->value().size() / 4 );
      typedef ArrayUByte4::scalar_type type;
      for(size_t i=0; i<arr_byte4->size(); ++i)
        arr_byte4->at(i) = ubvec4((type)srf_arr_int->value()[i*4+0], (type)srf_arr_int->value()[i*4+1], (type)srf_arr_int->value()[i*4+2], (type)srf_arr_int->value()[i*4+3]);
    }
    else
    {
      // mic fixme
      VL_TRAP();
      return NULL;
    }

    // link the SRF to the VL object
    mSRF_To_VL[srf_obj] = arr_abstract;

    return arr_abstract.get();
  }

  EPrimitiveType srfImport_EPrimitiveType(const std::string& str)
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
    if ("PT_UNKNOWN" == str) return PT_UNKNOWN;
    
    VL_TRAP();
    return PT_UNKNOWN;
  }

  DrawCall* srfImport_DrawCall(const SRF_Object* srf_obj)
  {
    ref<DrawCall> draw_call;

    // mic fixme: support MultiDrawElements

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
          VL_CHECK( value.type() == SRF_Value::Identifier ) // mic fixme
          de->setPrimitiveType( srfImport_EPrimitiveType( value.getIdentifier() ) );
        }
        else
        if( key == "Enabled" )
        {
          VL_CHECK( value.type() == SRF_Value::Bool ) // mic fixme
          de->setEnabled( value.getBool() );
        }
        else
        if( key == "Instances" )
        {
          VL_CHECK( value.type() == SRF_Value::Int64 ) // mic fixme
          de->setInstances( (int)value.getInt64() );
        }
        else
        if( key == "PrimitiveRestartEnabled" )
        {
          VL_CHECK( value.type() == SRF_Value::Bool ) // mic fixme
          de->setPrimitiveRestartEnabled( value.getBool() );
        }
        else
        if( key == "BaseVertex" )
        {
          VL_CHECK( value.type() == SRF_Value::Int64 ) // mic fixme
          de->setBaseVertex( (int)value.getInt64() );
        }
        else
        if( key == "IndexBuffer" )
        {
          VL_CHECK( value.type() == SRF_Value::Object ) // mic fixme
          ArrayAbstract* arr_abstract = srfImport_Array(value.getObject());

          if ( de->classType() == DrawElementsUInt::Type() )
          {
            VL_CHECK(arr_abstract->classType() == ArrayUInt1::Type());
            de->as<DrawElementsUInt>()->setIndexBuffer( arr_abstract->as<ArrayUInt1>() );
          }
          else
          if ( de->classType() == DrawElementsUShort::Type() )
          {
            VL_CHECK(arr_abstract->classType() == ArrayUShort1::Type());
            de->as<DrawElementsUShort>()->setIndexBuffer( arr_abstract->as<ArrayUShort1>() );
          }
          else
          if ( de->classType() == DrawElementsUByte::Type() )
          {
            VL_CHECK(arr_abstract->classType() == ArrayUByte1::Type());
            de->as<DrawElementsUByte>()->setIndexBuffer( arr_abstract->as<ArrayUByte1>() );
          }
        }
      }
    }
    else
    {
      // mic fixme
      VL_TRAP();
      return NULL;
    }

    // link the SRF to the VL object
    mSRF_To_VL[srf_obj] = draw_call;
    
    return draw_call.get();
  }

  Geometry* srfImport_Geometry(const SRF_Object* srf_obj)
  {
    if (srf_obj->tag() != "<Geometry>")
      return false;

    // link the SRF to the VL object
    ref<Geometry> geom = new Geometry;
    mSRF_To_VL[srf_obj] = geom;

    for(size_t i=0; i<srf_obj->value().size(); ++i)
    {
      const std::string& key = srf_obj->value()[i].key();
      const SRF_Value& value = srf_obj->value()[i].value();

      if (key == "VBOEnabled")
      {
        VL_CHECK(value.type() == SRF_Value::Bool) // mic fixme: issue error
        geom->setVBOEnabled( value.getBool() );
      }
      else
      if (key == "DisplayListEnabled")
      {
        VL_CHECK(value.type() == SRF_Value::Bool) // mic fixme: issue error
        geom->setDisplayListEnabled( value.getBool() );
      }
      else
      if (key == "AABB")
      {
        VL_CHECK(value.type() == SRF_Value::Object) // mic fixme: issue error
        AABB aabb;
        if (!srfImport_AABB(value.getObject(), aabb))
          return NULL;
      }
      else
      if (key == "Sphere")
      {
        VL_CHECK(value.type() == SRF_Value::Object) // mic fixme: issue error
        Sphere sphere;
        if (!srfImport_Sphere(value.getObject(), sphere))
          return NULL;
      }
      else
      if (key == "VertexArray")
      {
        VL_CHECK(value.type() == SRF_Value::Object) // mic fixme: issue error
        ArrayAbstract* arr = srfImport_Array(value.getObject());
        if (arr)
          geom->setVertexArray(arr);
        else
          return NULL;
      }
      else
      if (key == "NormalArray")
      {
        VL_CHECK(value.type() == SRF_Value::Object) // mic fixme: issue error
        ArrayAbstract* arr = srfImport_Array(value.getObject());
        if (arr)
          geom->setNormalArray(arr);
        else
          return NULL;
      }
      else
      if (key == "ColorArray")
      {
        VL_CHECK(value.type() == SRF_Value::Object) // mic fixme: issue error
        ArrayAbstract* arr = srfImport_Array(value.getObject());
        if (arr)
          geom->setColorArray(arr);
        else
          return NULL;
      }
      else
      if (key == "SecondaryColorArray")
      {
        VL_CHECK(value.type() == SRF_Value::Object) // mic fixme: issue error
        ArrayAbstract* arr = srfImport_Array(value.getObject());
        if (arr)
          geom->setSecondaryColorArray(arr);
        else
          return NULL;
      }
      else
      if (key == "FogCoordArray")
      {
        VL_CHECK(value.type() == SRF_Value::Object) // mic fixme: issue error
        ArrayAbstract* arr = srfImport_Array(value.getObject());
        if (arr)
          geom->setFogCoordArray(arr);
        else
          return NULL;
      }
      // ... textures and vertex attribs ...
      else
      if (key == "DrawCall")
      {
        VL_CHECK(value.type() == SRF_Value::Object) // mic fixme: issue error
        DrawCall* draw_call = srfImport_DrawCall(value.getObject());
        if (draw_call)
          geom->drawCalls()->push_back(draw_call);
        else
          return NULL;
      }
      else
      {
        VL_TRAP()
        // mic fixme
      }

    }

    return geom.get();
  }

  Effect* srfImport_Effect(const SRF_Object* srf_obj)
  {
    return NULL;
  }

  Shader* srfImport_Shader(const SRF_Object* srf_obj)
  {
    return NULL;
  }

  Transform* srfImport_Transform(const SRF_Object* srf_obj)
  {
    return NULL;
  }

  Actor* srfImport_Actor(const SRF_Object* srf_obj)
  {
    return NULL;
  }

  Camera* srfImport_Camera(const SRF_Object* srf_obj)
  {
    return NULL;
  }

  Object* srfToVL(const Object* srf_obj)
  {
    std::map< ref<Object>, ref<Object> >::iterator it = mSRF_To_VL.find(srf_obj);
    if (it != mSRF_To_VL.end())
      return it->second.get();
    else
      return NULL;
  }

  virtual void initEvent()
  {
    Log::notify(appletInfo());
#if 1
    ref<Geometry> geom = makeIcosphere( vec3(0,0,0), 10, 0 );
    // ref<Geometry> geom = makeTeapot( vec3(0,0,0), 10, 4 );
    geom->computeNormals();
    geom->setColorArray( vl::crimson );
    // geom->setSecondaryColorArray( geom->normalArray() );
    // TriangleStripGenerator::stripfy(geom.get(), 22, false, false, true);
    // mic fixme: this does no realizes that we are using primitive restart
    // mic fixme: make this manage also MultiDrawElements
    // geom->makeGLESFriendly();

    ref<Effect> fx = new Effect;
    fx->shader()->enable(EN_LIGHTING);
    fx->shader()->enable(EN_DEPTH_TEST);
    fx->shader()->gocMaterial()->setColorMaterialEnabled(true);
    fx->shader()->setRenderState( new Light, 0 );

    // sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL);

    // @@@ @@@ @@@ EXPORT @@@ @@@ @@@
    mUIDCounter = 0;
    mAssign = 0;
    mIndent = 0;
    ref<ResourceDatabase> res_db = new ResourceDatabase;
    res_db->resources().push_back(geom);

    srfPrelink_ResourceDatabase(res_db.get());
    srfExport_ResourceDatabase(res_db.get());
    std::fstream fout;
    fout.open("D:/VL/srf_export.vl", std::ios::out);
    fout.write( mSRFString.toStdString().c_str(), mSRFString.length() );
    fout.close();
#endif

    SRF_Parser parser;
    parser.tokenizer()->setInputFile( new DiskFile("D:/VL/srf_export.vl") );
    parser.parse();
    String dump = parser.dump();
    parser.link();
    // dump the dump
    {
      std::fstream fout;
      fout.open( "D:/VL/srf_export_dump.vl", std::ios::out );
      fout.write( dump.toStdString().c_str(), dump.length() );
      fout.close();
    }

    // @@@@@@@@@@@@@@@@@@@@@@@@
    {
      // first import
      ResourceDatabase* db = srfImport_ResourceDatabase( parser.root() );

      // the retrieve the imported objects
      Object* red_db_obj = srfToVL( parser.root() );
      VL_CHECK(red_db_obj && red_db_obj->classType() == ResourceDatabase::Type())
      ref<ResourceDatabase> res_db = red_db_obj->as<ResourceDatabase>();
      // checking just for debug
      VL_CHECK(db == res_db.get())

      ref<Geometry> geom = res_db->get<Geometry>(0);
      sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL);
    }

  }

protected:
  // export
  std::map< ref<Object>, String > mObject_To_UID;
  std::map< ref<Object>, int > mObject_Ref_Count;
  std::set< ref<Object> > mAlreadyDefined;
  // import
  std::map< ref<Object>, ref<Object> > mSRF_To_VL;

  int mUIDCounter;
  String mSRFString;
  int mIndent;
  bool mAssign;
};

// Have fun!

BaseDemo* Create_App_VLSRF() { return new App_VLSRF; }
