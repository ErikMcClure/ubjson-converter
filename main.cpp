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

// Converts JSON into a direct, unoptimized UBJSON format
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

// Convert UBJSON to JSON
void revert_obj(std::istream& in, std::ostream& out);

struct ConvUBJSON
{
  void EvalUBJSON(const char* id, std::istream& s, char type)
  {
    revert_value(id, s, type);
  }
  void revert_value(const char* id, std::istream& in, char type)
  {
    UBJSONTuple tuple;
    WriteJSONComma(out, pretty);
    if(!id && WriteJSONIsPretty(pretty)) out << std::endl;
    WriteJSONId(id, out, pretty);

    ParseUBJSONValue(tuple, in, type);
    std::string s;
    switch(tuple.type)
    {
    case UBJSONTuple::TYPE_NULL: s = "null"; break;
    case UBJSONTuple::TYPE_TRUE: s = "true"; break;
    case UBJSONTuple::TYPE_FALSE: s = "false"; break;
    case UBJSONTuple::TYPE_CHAR: s = tuple.Int8;
    case UBJSONTuple::TYPE_INT8: s = std::to_string((int)tuple.Int8); break;
    case UBJSONTuple::TYPE_UINT8: s = std::to_string(tuple.UInt8); break;
    case UBJSONTuple::TYPE_INT16: s = std::to_string(tuple.Int16); break;
    case UBJSONTuple::TYPE_INT32: s = std::to_string(tuple.Int32); break;
    case UBJSONTuple::TYPE_INT64: s = std::to_string(tuple.Int64); break;
    case UBJSONTuple::TYPE_FLOAT: s = std::to_string(tuple.Float); break;
    case UBJSONTuple::TYPE_DOUBLE: s = std::to_string(tuple.Double); break;
    case UBJSONTuple::TYPE_BIGNUM:
    case UBJSONTuple::TYPE_STRING:
      s = tuple.String;
      break;
    case UBJSONTuple::TYPE_OBJECT:
      revert_obj(in, out, UBJSONTuple::TYPE_OBJECT, pretty);
      return;
    case UBJSONTuple::TYPE_ARRAY:
    {
      unsigned int npretty = WriteJSONPretty(pretty);
      ConvUBJSON obj = { out, npretty };
      out << '[';
      ParseUBJSONInternal<ConvUBJSON, false>::F(obj, in, UBJSONTuple::TYPE_ARRAY);
      out << ']';
    }
      return;
    }
    out << s;
  }

  std::ostream& out;
  unsigned int& pretty;
};

template<>
struct ParseUBJSONInternal<ConvUBJSON, false>
{
  static inline bool DoBulkRead(ConvUBJSON& obj, std::istream& s, __int64 count, char ty) { return false; }
  static inline void DoAddCall(ConvUBJSON& obj, std::istream& s, int& n, char ty) { obj.revert_value(s, ty); }
  static void F(ConvUBJSON& obj, std::istream& s, char ty) { ParseUBJSONArray<ConvUBJSON>(obj, s, ty); }
};

// Converts UBJSON into JSON
void revert_obj(std::istream& in, std::ostream& out, char type, unsigned int& pretty)
{
  unsigned int npretty = WriteJSONPretty(pretty);
  ConvUBJSON obj = { out, npretty };
  out << '{';
  ParseUBJSON<ConvUBJSON>(obj, in, type);
  out << '}';
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