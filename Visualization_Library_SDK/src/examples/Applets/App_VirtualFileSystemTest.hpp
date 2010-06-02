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

#include "vl/ZippedDirectory.hpp"
#include <vl/DiskFile.hpp>
#include <vl/GZipCodec.hpp>
#include <vl/FileSystem.hpp>
#include "vl/TextStream.hpp"
#include <time.h>

class App_VirtualFileSystemTest: public BaseDemo
{
public:
  virtual void shutdown() {}
  virtual void run() {}
  void initEvent()
  {
    BaseDemo::initEvent();
    srand((unsigned int)time(NULL));

    vl::ArrayByte ab1;
    vl::ArrayByte ab2;
    ab1.resize(33);
    for(size_t i=0; i<ab1.size(); ++i)
      ab1[i] = (GLbyte)(rand() % 256);
    std::sort(ab1.begin(), ab1.end());
    ab2 = ab1;
    for(size_t i=0; i<ab2.size(); ++i)
      printf("%d ", (int)ab2[i]);

    // actual test

    bool test_passed = true;

    test_passed &= testGZipStream();

    // install the zipped directory
    vl::ref<vl::ZippedDirectory> zdir = new vl::ZippedDirectory("/ztest.zip");
    vl::ref<vl::VirtualFile> vfile;

    // seek test
    vfile = zdir->file("/seek.dat");
    vfile->open(vl::OM_ReadOnly);
    long long seek_size = vfile->size();
    for(int i=0; i<100; ++i)
    {
      int pos = int(rand() % ( (seek_size-64*4)/4 ));
      vfile->seekSet( pos*4 );
      int buffer[64];
      memset(buffer,0,sizeof(int)*64);
      vfile->read(buffer, sizeof(int)*64);
      test_passed &= buffer[0] == pos;
    }
    vfile->close();

    vfile = zdir->file("/triangle_fur.gs");
    std::vector<vl::String> files;
    zdir->listFilesRecursive(files);
    vl::Log::print("Zip file content:\n");
    for(unsigned i=0; i<files.size(); ++i)
    {
      // check decompression
      vl::ref<vl::ZippedFile> zfile = zdir->zippedFile(files[i]);
      unsigned int crc32 = zfile->crc32();
      vl::Log::print( vl::Say("%s: comp.meth. = %n; crc = %s; size = %n/%n\n")
        << files[i].toStdString().c_str()
        << zfile->zippedFileInfo()->compressionMethod()
        << (zfile->zippedFileInfo()->crc32() == crc32 ? "OK":"BAD")
        << zfile->zippedFileInfo()->compressedSize()
        << zfile->size()
        );
      VL_CHECK(zfile->zippedFileInfo()->crc32() == crc32)
      test_passed &= zfile->zippedFileInfo()->crc32() == crc32;

      // check concurrent reading
      if ( zfile->path().endsWith(".gs") || zfile->path().endsWith(".txt") )
      {
        vl::ref<vl::TextStream> txtstream1 = new vl::TextStream(zdir->file(files[i]).get());
        vl::ref<vl::TextStream> txtstream2 = new vl::TextStream(zdir->file(files[i]).get());
        vl::String line1, line2, text1, text2;

        for(int i=0; i<5; ++i)
        {
          if (txtstream1->readLine(line1))
          {
            text1 += line1;
            vl::Log::print( vl::Say("1==> %s\n") << line1.toStdString().c_str() );
          }
          if (txtstream2->readLine(line2))
          {
            text2 += line2;
            vl::Log::print( vl::Say("2==> %s\n") << line2.toStdString().c_str() );
          }
        }

        VL_CHECK(text1 == text2)
        test_passed &= text1 == text2;
      }
    }

    // OBJ and material loading from zip file

    vl::VisualizationLibrary::fileSystem()->directories()->push_back(zdir.get());
    vl::ref<vl::ResourceDatabase> res_db = vl::loadResource("/obj_test_mesh.obj");

    vl::Log::print( vl::Say("obj geometries = %n\n") << res_db->count<vl::Geometry>() );
    vl::Log::print( vl::Say("obj materials  = %n\n") << res_db->count<vl::Material>() );
    vl::Log::print( vl::Say("obj effects    = %n\n") << res_db->count<vl::Effect>() );

    test_passed &= res_db->count<vl::Geometry>() != 0;
    test_passed &= res_db->count<vl::Material>() != 0;
    test_passed &= res_db->count<vl::Effect>()   != 0;

    // display test pass/failure information

    vl::Log::print(test_passed ? "\n*** test passed ***\n\n" : "\n*** TEST FAILED ***\n\n");

    vl::ref<vl::Text> text = new vl::Text;
    text->setFont( vl::VisualizationLibrary::fontManager()->acquireFont("/font/bitstream-vera/VeraMono.ttf", 12) );
    text->setText(test_passed ? "Virtual File System Test Passed" : "Virtual File System Test FAILED");
    text->setAlignment( vl::AlignHCenter | vl::AlignVCenter);
    text->setViewportAlignment( vl::AlignHCenter | vl::AlignVCenter );
    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->enable(vl::EN_BLEND);
    sceneManager()->tree()->addActor(text.get(), effect.get());
  }

  bool testGZipStream()
  {
    std::vector<vl::String> file_list;
    vl::VisualizationLibrary::fileSystem()->listFilesRecursive(file_list,"/images/*");

    for( unsigned i=0; i<file_list.size(); ++i )
    {
      printf("\nfile = '%s'\n", file_list[i].toStdString().c_str());

      vl::ref<vl::VirtualFile> file         = vl::locateFile(file_list[i]);
      vl::ref<vl::VirtualFile> gzip_outfile = new vl::DiskFile("temp_file.gz");
      vl::ref<vl::GZipCodec>   gzip_codec   = new vl::GZipCodec;

      gzip_codec->setStream(gzip_outfile.get());
      gzip_codec->setCompressionLevel(rand()%10); // 0-9

      // read original file
      std::vector<unsigned char> buffer;
      buffer.resize( (int)file->size() );
      file->open(vl::OM_ReadOnly);
      file->read(&buffer[0], buffer.size());
      file->close();

      // write compressed file
      gzip_codec->setStream(gzip_outfile.get());
      gzip_codec->open(vl::OM_WriteOnly);
      for(unsigned i=0; i<buffer.size();)
      {
        int remains = buffer.size() - i;
        int bytes = rand()%(remains+1);
        gzip_codec->write(&buffer[i], bytes);
        i+=bytes;
      }
      // gzip_codec->write(&buffer[0], buffer.size());
      gzip_codec->close();

      printf("compression ratio = %.2f\n", gzip_codec->compressionRatio());

      // compute md5 of the original file
      vl::MD5CheckSum md5_a = file->md5();
      // compute md5 from the compressed files
      vl::MD5CheckSum md5_b = gzip_codec->md5();

      vl::MD5CheckSum md5_c = gzip_codec->md5();
      printf("md5 == %s\n", md5_a.toStdString().c_str());
      printf("md5 == %s\n", md5_b.toStdString().c_str());
      VL_CHECK( md5_a == md5_b )
      if (md5_a != md5_b)
      {
        vl::Time::sleep(2500);
        return false;
      }
    }
    return true;
  }
};

// Have fun!
