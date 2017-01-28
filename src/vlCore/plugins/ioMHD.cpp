/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2017, Michele Bosi                                             */
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

#include "ioMHD.hpp"
#include <vlCore/LoadWriterManager.hpp>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/FileSystem.hpp>
#include <vlCore/VirtualFile.hpp>
#include <vlCore/Image.hpp>
#include <vlCore/TextStream.hpp>
#include <stdio.h>

using namespace vl;

#include <vlCore/ImageTools.hpp>

//-----------------------------------------------------------------------------
ref<Image> vl::loadMHD( const String& path )
{
  ref<VirtualFile> file = defFileSystem()->locateFile(path);
  if ( !file )
  {
    Log::error( Say("File '%s' not found.\n") << path );
    return NULL;
  }

  return loadMHD(file.get());
}
//-----------------------------------------------------------------------------
ref<Image> vl::loadMHD(VirtualFile* file)
{
  if ( ! file->open( OM_ReadOnly ) )
  {
    Log::error( Say( "%s: could not find MHD file '%s'.\n" ) << __FUNCTION__ << file->path() );
    return NULL;
  }

  ref<TextStream> stream = new TextStream(file);
  std::string line;
  std::map<String, String> keyvals;
  std::vector<String> keyval;
  String entry;

  while( stream->readLine( line ) )
  {
    entry = line;
    entry.split( '=', keyval, true );
    if ( keyval.size() == 2 ) {
      keyval[0].trim();
      keyval[1].trim();
      keyvals[ keyval[0] ] = keyval[1];
    }
  }

  int ndims = 0;
  ivec3 offset;
  fvec3 center_of_rotation;
  fvec3 element_spacing;
  int width=0, height=0, depth=0, bytealign=1;
  EImageFormat format = vl::IF_LUMINANCE;
  EImageType type = vl::IT_UNSIGNED_SHORT;
  String raw_file;

  for( std::map<String, String>::const_iterator it = keyvals.begin(); it != keyvals.end(); ++it )
  {
    const String& key = it->first;
    const String& val = it->second;

    if ( key == "ObjectType" ) {
      if ( val != "Image" ) {
        Log::error( Say("%s: ObjectType must be Image ('%s').\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
    } else
    if ( key == "NDims" ) {
      if ( val != "3" ) {
        Log::error( Say("%s: NDims must be 3 ('%s').\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
      ndims = 3;
    } else
    if ( key == "BinaryData" ) {
      if ( val != "True" ) {
        Log::error( Say("%s: BinaryData must be True ('%s').\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
    } else
    if ( key == "BinaryDataByteOrderMSB" ) {
      if ( val != "False" ) {
        Log::error( Say("%s: BinaryDataByteOrderMSB must be False ('%s').\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
    } else
    if ( key == "CompressedData" ) {
      if ( val != "False" ) {
        Log::error( Say("%s: CompressedData must be False ('%s').\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
    } else
    if ( key == "TransformMatrix" ) {
      if ( val != "1 0 0 0 1 0 0 0 1" ) {
        Log::error( Say("%s: TransformMatrix must be set to identity matrix ('%s').\n") << __FUNCTION__ << file->path() );
        // return NULL;
      }
    } else
    if ( key == "Offset" ) {
      if ( sscanf( val.toStdString().c_str(), "%d %d %d", &offset.x(), &offset.y(), &offset.z() ) != 3 ) {
        Log::error( Say("%s: invalid Offset value, must be three ints ('%s').\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
    } else
    if ( key == "CenterOfRotation" ) {
      if ( sscanf( val.toStdString().c_str(), "%f %f %f", &center_of_rotation.x(), &center_of_rotation.y(), &center_of_rotation.z() ) != 3 ) {
        Log::error( Say("%s: invalid CenterOfRotation value, must be three floats ('%s') (%n).\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
    } else
    if ( key == "ElementSpacing" ) {
      if ( sscanf( val.toStdString().c_str(), "%f %f %f", &element_spacing.x(), &element_spacing.y(), &element_spacing.z() ) != 3 ) {
        Log::error( Say("%s: invalid ElementSpacing value, must be three floats ('%s').\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
    } else
    if ( key == "DimSize" ) {
      if ( sscanf( val.toStdString().c_str(), "%d %d %d", &width, &height, &depth ) != 3 ) {
        Log::error( Say("%s: invalid DimSize value, must be three ints ('%s').\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
    } else
    if ( key == "AnatomicalOrientation" ) {
      // TODO: ???
    } else
    if ( key == "ElementType" ) {
      if ( val != "MET_SHORT" ) {
        Log::error( Say("%s: invalid ElementType value, only MET_SHORT is supported ('%s').\n") << __FUNCTION__ << file->path() );
        return NULL;
      }
    } else
    if ( key == "ElementDataFile" ) {
      raw_file = file->path().extractPath() + val;
    }
  }

  ref<VirtualFile> rawf = defFileSystem()->locateFile( raw_file );
  if (rawf)
  {
    return loadRAW( rawf.get(), -1, width, height, depth, bytealign, format, type );
  }
  else
  {
    Log::error( Say("loadMHD('%s'): could not find RAW file '%s'.\n") << file->path() << raw_file );
    return NULL;
  }
}
//-----------------------------------------------------------------------------
bool vl::isMHD(VirtualFile* file)
{
  // Simplistic way to determine if it's an MHD file
  return file->path().toLowerCase().endsWith('.mhd');
}
//-----------------------------------------------------------------------------
