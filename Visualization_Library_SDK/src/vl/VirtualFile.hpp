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

#ifndef File_INCLUDE_ONCE
#define File_INCLUDE_ONCE

#include <vl/Object.hpp>
#include <vl/vlnamespace.hpp>
#include <vl/String.hpp>
#include <vl/MD5CheckSum.hpp>
#include <map>

namespace vl
{

//---------------------------------------------------------------------------
// VirtualFile
//---------------------------------------------------------------------------
  /**
   * An abstract class representing a file.
   *
   * \sa
   * - VirtualDirectory
   * - DiskDirectory
   * - MemoryDirectory
   * - ZippedDirectory
   * - FileSystem
   * - DiskFile
   * - MemoryFile
   * - ZippedFile
  */
  class VirtualFile: public Object
  {
  protected:
    VirtualFile(const VirtualFile& other): Object(other) {}

  public:
    virtual const char* className() { return "VirtualFile"; }

    VirtualFile() {}

    ~VirtualFile() {}

    unsigned int crc32();

    MD5CheckSum md5();

    virtual bool open(EOpenMode mode) = 0;

    virtual bool isOpen() const = 0;

    virtual bool exists() const = 0;

    virtual void close() = 0;

    virtual long long size() const = 0;

    virtual ref<VirtualFile> clone() const = 0;

    VirtualFile& operator=(const VirtualFile& other) { Object::operator=(other); mPath = other.mPath; return *this; }

    const String& path() const { return mPath; }

    //! Renames the file. Use carefully this function, you shouldn't rename a VirtualFile managed by a VirtualDirectory.
    void setPath(const String& name) { mPath = name; mPath.normalizeSlashes(); }

    //! Reads byte_count bytes and returns to the original position.
    //! Returns the number of bytes read.
    long long peek(void* buffer, long long byte_count);

    long long read(void* buffer, long long byte_count);

    long long write(const void* buffer, long long byte_count);

    long long position() const;

    bool seekSet(long long offset);

    bool seekCur(long long offset);

    bool seekEnd(long long offset);

    bool endOfFile() const { return position() >= size(); }

    //! Loads the entire file in the specified vector.
    //! Returns the number of bytes read.
    //! The file must be closed before calling this function.
    long long load(std::vector<unsigned char>& data);

    //! Loads the entire file in the specified buffer.
    //! Returns the number of bytes read.
    //! The file must be closed before calling this function.
    long long load(void* buffer, long long max=-1);

    float readFloat(bool little_endian_data=true);
    long long readFloat(void* buffer, long long count);

    double readDouble(bool little_endian_data=true);
    long long readDouble(void* buffer, long long count);

    unsigned char readUByte();
    long long readUByte(void* buffer, long long count);

    char readSByte();
    long long readSByte(void* buffer, long long count);

    unsigned long long readUInt64(bool little_endian_data=true);
    long long readUInt64(void* buffer, long long count, bool little_endian_data=true);

    long long readSInt64(bool little_endian_data=true);
    long long readSInt64(void* buffer, long long count, bool little_endian_data=true);

    unsigned int readUInt32(bool little_endian_data=true);
    long long readUInt32(void* buffer, long long count, bool little_endian_data=true);

    int readSInt32(bool little_endian_data=true);
    long long readSInt32(void* buffer, long long count, bool little_endian_data=true);

    unsigned short readUInt16(bool little_endian_data=true);
    long long readUInt16(void* buffer, long long count, bool little_endian_data=true);

    short readSInt16(bool little_endian_data=true);
    long long readSInt16(void* buffer, long long count, bool little_endian_data=true);

    long long writeUInt32(unsigned long data, bool little_endian=true);
    long long writeUInt16(unsigned short data, bool little_endian=true);
    long long writeUInt8(unsigned char data);
    long long writeSInt32(long data, bool little_endian=true);
    long long writeSInt16(short data, bool little_endian=true);
    long long writeSInt8(char data);

  protected:
    virtual long long read_Implementation(void* buffer, long long byte_count) = 0;

    virtual long long write_Implementation(const void* buffer, long long byte_count) = 0;

    virtual long long position_Implementation() const = 0;

    virtual bool seekSet_Implementation(long long offset) = 0;

  protected:
    String mPath;
  };
//-----------------------------------------------------------------------------
  //! Utility function, equivalent to \p vl::VisualizationLibrary::fileSystem()->locateFile(path)
  ref<VirtualFile> locateFile(const String& path);
//-----------------------------------------------------------------------------
}

#endif
