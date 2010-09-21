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

#include <vl/Primitives.hpp>

using namespace vl;

void Primitives::getTriangle( size_t tri_index, unsigned int* out_triangle ) const
{
  // initialize NULL triangle
  if ( out_triangle )
  {
    out_triangle[0] = (unsigned int)-1;
    out_triangle[1] = (unsigned int)-1;
    out_triangle[2] = (unsigned int)-1;
  }

  if( !out_triangle || tri_index < 0 || tri_index >= triangleCount() / instances() )
  {
    // error!
    VL_TRAP();
    return;
  }

  switch( primitiveType() )
  {
  case PT_TRIANGLES:
    out_triangle[0] = index( tri_index * 3 );
    out_triangle[1] = index( tri_index * 3 + 1 );
    out_triangle[2] = index( tri_index * 3 + 2 );
    break;;

  case PT_TRIANGLE_STRIP:
  case PT_QUAD_STRIP:
    if (tri_index & 1)
    {
      // odd
      out_triangle[0] = index( tri_index );
      out_triangle[1] = index( tri_index + 1 );
      out_triangle[2] = index( tri_index + 2 );
    }
    else
    {
      // even
      out_triangle[0] = index( tri_index );
      out_triangle[1] = index( tri_index + 2 );
      out_triangle[2] = index( tri_index + 1 );
    }
    break;;

  case PT_POLYGON:
  case PT_TRIANGLE_FAN:
      out_triangle[0] = index( 0 );
      out_triangle[1] = index( tri_index + 1 );
      out_triangle[2] = index( tri_index + 2 );
    break;;

  case PT_QUADS:
    // 1-----2
    // | T0 /|
    // |  /  |
    // |/ T1 |
    // 0-----3
    if ( tri_index & 1 )
    {
      // odd
      out_triangle[0] = index( tri_index * 2 );
      out_triangle[1] = index( tri_index * 2 + 1 );
      out_triangle[2] = index( tri_index * 2 - 2);
    }
    else
    {
      // even
      out_triangle[0] = index( tri_index * 2 );
      out_triangle[1] = index( tri_index * 2 + 1 );
      out_triangle[2] = index( tri_index * 2 + 2);
    }
    break;;

  default:
    return;
  }
}
