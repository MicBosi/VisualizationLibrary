

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <string>
#include <vector>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/Time.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlGraphics/plugins/ioVLX.hpp>
#include <vlGraphics/expandResourceDatabase.hpp>

using namespace vl;

void printHelp()
{
  printf("\nusage:\n");
  printf("\tvlxtool -in file1 file2 file3 -out file_out\n");
}

int main(int argc, const char* argv[])
{
  VisualizationLibrary::init(true);

  printf("vlxtool 1.0\n");

  std::vector<std::string> in_files;
  String out_file;

  bool input = false;
  bool output = false;

  for(int i=1; i<argc; ++i)
  {
    if ( strcmp(argv[i], "-in") == 0)
    {
      input = true;
      output = false;
    }
    else
    if ( strcmp(argv[i], "-out") == 0)
    {
      input = false;
      output = true;
    }
    else
    if (input)
    {
      in_files.push_back(argv[i]);
    }
    else
    if (output)
    {
      if (out_file.empty())
        out_file = argv[i];
      else
      {
        printf("too many output files.\n");
        return 1;
      }
    }
    else
    {
      printHelp();
      return 1;
    }
  }

  if (in_files.empty() || out_file.empty())
  {
    printHelp();
    return 1;
  }

  printf("Loading...\n");
  ref<ResourceDatabase> db = new ResourceDatabase;
  for(size_t i=0; i<in_files.size(); ++i)
  {
    Time timer; timer.start();
    printf("\t%s ", in_files[i].c_str());
    ref<ResourceDatabase> res = vl::loadResource(in_files[i].c_str(), true);
    if (res)
    {
      printf("\t... %.2fs\n", timer.elapsed());
      db->resources().insert(db->resources().end(), res->resources().begin(), res->resources().end());
    }
  }

  expandResourceDatabase(db.get());

  Time timer; timer.start();
  if (out_file.endsWith(".vlt"))
  {
    printf("Saving...\n");
    printf("\t%s ", out_file.toStdString().c_str());
    vl::saveVLT(out_file, db.get());
    printf("\t... %.2fs\n", timer.elapsed());
  }
  else
  if (out_file.endsWith(".vlb"))
  {
    printf("Saving...\n");
    printf("\t%s ", out_file.toStdString().c_str());
    vl::saveVLB(out_file, db.get());
    printf("\t... %.2fs\n", timer.elapsed());
  }
  else
  {
    printf("output file must be either a .vlt or .vlb\n");
    return 1;
  }

  return 0;
}
