#include <unistd.h>
#include <cstdio>
#include <fstream>

#include <tinyrpc/util.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <tinyrpc/stubgenerator/ServiceStubGenerator.h>
#include <tinyrpc/stubgenerator/ClientStubGenerator.h>

#define GENSERVER  1
#define GENSCLIENT 2



using namespace rapidjson;
using namespace tinyrpc;

static void writeToFile(StubGenerator& generator) {
  FILE* output;
  std::string outputFileName = generator.genStubClassName() + ".h";
  output = fopen(outputFileName.c_str(), "w");
  if (output == nullptr) {
    perror("error");
    exit(1);
  }
  auto stubString = generator.genStub();
  fputs(stubString.c_str(), output);
}

static void genStub(FILE* fp, int mode) {
  char readBuffer[65536];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  Document doc;
  doc.ParseStream(is);
  if (doc.HasParseError()) {
    fprintf(stderr, "parse the json error\n");
    exit(1);
  }
  if (mode & GENSERVER) {
    auto stubgenerator = std::make_unique<ServiceStubGenerator>(doc);
    writeToFile(*stubgenerator);
  }
  if (mode & GENSCLIENT) {
    auto stubgenerator = std::make_unique<ClientStubGenerator>(doc);
    writeToFile(*stubgenerator);    
  }
}

int main(int argc, char* argv[]) {
  int  opt, mode =  GENSERVER;
  const char* inputFileName = "spec.json";
  while ((opt = getopt(argc, argv, "csi:")) != -1) {
    switch (opt) {
      case 'c':
        mode |= GENSCLIENT;
        break;
      case 's':
        mode |= GENSERVER;
        break;
      case 'i':
        inputFileName = optarg;
        break;
      default:
        fprintf(stderr, "usage: stub_generator <-c/s> [-o] [-i input]\n");
        exit(1);
    }
  }
  if (inputFileName == nullptr) {
    printf("files are required\n");
    exit(1);
  }
  FILE* input = fopen(inputFileName, "r");
  if (input == nullptr) {
    printf("fail to open file\n");
    exit(1);    
  }
  genStub(input, mode);
}