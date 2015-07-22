// Copyright ©2015 Black Sphere Studios

#include "cJSON.h"
#include <iostream>
#include <fstream>

using namespace bss_util;

struct ConvJSON
{
  void SerializeJSON(std::ostream& s, unsigned int& pretty) const {}
  void EvalJSON(const char* name, std::istream& s) {}
};

void convert(std::istream& s)
{
  ConvJSON obj;

  ParseJSON<ConvJSON>(obj, s);
}

int main(int argc, char** argv)
{
  std::string s;
  while(std::cin) std::cin >> s;

  if(s.empty())
  {
    if(argc < 2 || !argv[1])
    {
      std::cout << "No argument supplied." << std::endl;
      return -1;
    }
    s = argv[1];
  }

  std::fstream fs(s, std::ios::in);
  if(!fs)
  {
    std::cout << "File does not exist: " << s << std::endl;
    return -1;
  }
  convert(fs);

  return 0;
}