/**************************************************************************************/
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi.                                            */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  This file is part of Visualization Library                                        */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Released under the OSI approved Simplified BSD License                            */
/*  http://www.opensource.org/licenses/bsd-license.php                                */
/*                                                                                    */
/**************************************************************************************/

// Standard VL vertex attributes

in vec4 vl_VertexPosition;
in vec3 vl_VertexNormal;
in vec4 vl_VertexColor;
in vec4 vl_VertexSecondaryColor;
in vec4 vl_VertexFogCoord;
in vec4 vl_VertexTexCoord0;
in vec4 vl_VertexTexCoord1;
in vec4 vl_VertexTexCoord2;
in vec4 vl_VertexTexCoord3;
in vec4 vl_VertexTexCoord4;
in vec4 vl_VertexTexCoord5;
in vec4 vl_VertexTexCoord6;
in vec4 vl_VertexTexCoord7;
in vec4 vl_VertexTexCoord8;
in vec4 vl_VertexTexCoord9;
in vec4 vl_VertexTexCoord10;

// Aliases

in vec4 vl_VertexTexCoordDiffuse;   // diffuse texture coords
in vec4 vl_VertexTexCoordDetail;    // detail texture coords
in vec4 vl_VertexTexCoordShadow;    // precomputed shadow/light map texture coords
in vec4 vl_VertexTexCoordSpecular;  // specular texture coords
in vec4 vl_VertexTexCoordNormalMap; // normal mapping texture coords
in vec4 vl_VertexTexCoordDisplace;  // dispace texture coords for tesselation, heightmap etc.
in vec4 vl_VertexTangent;           // tangent vectors (normal mapping)
in vec4 vl_VertexBitangent;         // bitangent vectors (normal mapping)
in vec4 vl_VertexNextPosition;      // next frame vertex position for vertex blending
in vec3 vl_VertexNextNormal;        // next frame vertex position for vertex blending

