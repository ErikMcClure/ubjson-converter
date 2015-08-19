// Copyright ©2015 Black Sphere Studios

#include "bss-util/cJSON.h"
#include "bss-util/cUBJSON.h"
#include <iostream>
#include <fstream>
#include <io.h>

using namespace bss_util;

struct ConvJSON
{
  void EvalJSON(const char* name, std::istream& s)
  {
  
  }
};

void convert(std::istream& s)
{
  ConvJSON obj;

  ParseJSON<ConvJSON>(obj, s);
}

void convdir_value(std::istream& in, std::ostream& out);

void convdir_id(std::istream& in, std::ostream& out)
{
  ParseJSONEatWhitespace(in);
  if(in.peek() != '"') return;
  in.get();
  cStr buf;
  while(!!in && in.peek() != '"' && in.peek() != -1) ParseJSONEatCharacter(buf, in);
  if(in) in.get(); // eat " character
  ParseJSONEatWhitespace(in);
  if(in.get() != ':') return;

  WriteUBJSONString(buf, buf.length(), out, UBJSONTuple::TYPE_STRING);
  convdir_value(in, out);
}

void convdir_obj(std::istream& in, std::ostream& out)
{
  if(in.peek() != '{') return;
  in.get();
  out.put(UBJSONTuple::TYPE_OBJECT);
  while(in && in.peek() != -1 && in.peek() != '}')
  {
    convdir_id(in, out);
    ParseJSONEatWhitespace(in);
    if(in.peek() == ',') in.get();
  }
  if(in) in.get();
  out.put(UBJSONTuple::TYPE_OBJECT_END);
}

void convdir_array(std::istream& in, std::ostream& out)
{
  if(in.peek() != '[') return;
  in.get();
  out.put(UBJSONTuple::TYPE_ARRAY);
  while(in && in.peek() != -1 && in.peek() != ']')
  {
    convdir_value(in, out);
    ParseJSONEatWhitespace(in);
    if(in.peek() == ',') in.get();
  }
  if(in) in.get();
  out.put(UBJSONTuple::TYPE_ARRAY_END);
}

void convdir_value(std::istream& in, std::ostream& out)
{
  cStr buf;
  const char* dot;

  ParseJSONEatWhitespace(in);
  switch(in.peek())
  {
  case '{':
    convdir_obj(in, out);
    break;
  case '[':
    convdir_array(in, out);
    break;
  case '"':
    ParseJSON<std::string>(buf, in);
    WriteUBJSONString(buf, buf.length(), out, 0);
    break;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '.':
  case '-':
    ParseJSON<std::string>(buf, in);
    dot = strchr(buf, '.');
    if(dot)
    {
      ptrdiff_t pos = dot - buf.c_str();
      double d = strtod(buf, 0);
      if(pos > 8 || (buf.length() - pos) > 5)
        WriteUBJSON<double>(d, out);
      else
        WriteUBJSON<float>((float)d, out);
    }
    else
      WriteUBJSON<__int64>(strtoll(buf, 0, 10), out);
    break;
  default:
    ParseJSON<std::string>(buf, in);
    if(!STRICMP(buf, "true")) out.put(UBJSONTuple::TYPE_TRUE);
    else if(!STRICMP(buf, "false")) out.put(UBJSONTuple::TYPE_FALSE);
    else if(!STRICMP(buf, "null")) out.put(UBJSONTuple::TYPE_NULL);
    else WriteUBJSONString(buf, buf.length(), out, 0);
    break;
  }
}

int main(int argc, char** argv)
{
  std::string s;
  std::string sout;
  
  if(!_isatty(_fileno(stdin)))
    while(std::cin >> s);

  if(s.empty())
  {
    if(argc < 2 || !argv[1])
    {
      std::cout << "No argument supplied." << std::endl;
      return -1;
    }
    s = argv[1];
  }

  if(argc > 2)
    sout = argv[2];

  if(sout.empty())
  {
    sout = s;
    const char* dot = strrchr(sout.c_str(), '.');
    if(dot)
      sout.resize(dot - sout.c_str());
    sout += ".ubj";
  }

  std::fstream fs(s, std::ios::in);
  if(!fs)
  {
    std::cout << "File does not exist: " << s << std::endl;
    return -2;
  }
  std::fstream fout(sout, std::ios::out|std::ios::trunc|std::ios::binary);
  if(!fs)
  {
    std::cout << "Could not open output file: " << sout << std::endl;
    return -3;
  }

  ParseJSONEatWhitespace(fs);
  convdir_obj(fs, fout);

  return 0;
}