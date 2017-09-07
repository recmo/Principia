// clang++ --std=c++14 ./unicode.cpp -licutu -licuio -licui18n -licule -licuuc -licudata -o unicode; and ./unicode
#include<iostream>
#include<fstream>
#include<string>
#include<exception>
#include<stdexcept>
#include<cassert>

#include<unicode/uversion.h>
#include<unicode/utypes.h>
#include<unicode/unistr.h>
#include<unicode/errorcode.h>
#include<unicode/bytestream.h>
#include<unicode/normalizer2.h>

/*
// See: https://ssl.icu-project.org/apiref/icu4c/index.html

std::string read_file(const std::string& filename)
{
  std::ifstream stream{filename, std::ios::binary};
  return std::string{
    std::istreambuf_iterator<char>{stream},
    std::istreambuf_iterator<char>{}};
}

icu::UnicodeString from_utf8(const std::string& bytes)
{
  return icu::UnicodeString::fromUTF8(icu::StringPiece(bytes));
}

std::string to_utf8(const icu::UnicodeString& str)
{
  std::string result;
  icu::StringByteSink<std::string> sink{&result};
  str.toUTF8(sink);
  return result;
}

bool is_identifier(uint32_t codepoint)
{
  if(u_hasBinaryProperty(codepoint, UCHAR_PATTERN_SYNTAX)) {
    return false;
  }
  if(u_hasBinaryProperty(codepoint, UCHAR_PATTERN_WHITE_SPACE)) {
    return false;
  }
  if(u_hasBinaryProperty(codepoint, UCHAR_NONCHARACTER_CODE_POINT)) {
    return false;
  }
  int8_t general_category = u_charType(codepoint);
  if(general_category == U_PRIVATE_USE_CHAR) {
    return false;
  }
  if(general_category == U_SURROGATE) {
    return false;
  }
  if(general_category == U_CONTROL_CHAR) {
    return false;
  }
  return true;
}

bool is_operator(uint32_t codepoint)
{
  return u_hasBinaryProperty(codepoint, UCHAR_PATTERN_SYNTAX);
}

const uint32_t quote_open = 0x201C;
const uint32_t quote_close = 0x201D;

std::string normalize(const std::string& bytes)
{
  icu::ErrorCode status;
  const icu::Normalizer2* nfc = icu::Normalizer2::getNFCInstance(status);
  if(status.isFailure()) {
    throw std::runtime_error{status.errorName()};
  }
  assert(nfc != nullptr);
  status.reset();

  icu::UnicodeString str = from_utf8(bytes);
  str = nfc->normalize(str, status);
  if(status.isFailure()) {
    throw std::runtime_error{status.errorName()};
  }
  status.reset();

  for(uint i = 0; i < str.length(); ++i) {
    std::cout << str[i] << " ";
    std::cout << (is_identifier(str[i]) ? "yes" : "no") << " ";
    std::cout << (is_operator(str[i]) ? "yes" : "no") << "\n";
  }
  std::cout << "\n";

  return to_utf8(str);
}

std::string case_fold(const std::string& bytes)
{
  icu::ErrorCode status;
  const icu::Normalizer2* nkfcc = icu::Normalizer2::getNFKCCasefoldInstance(status);
  if(status.isFailure()) {
    throw std::runtime_error{status.errorName()};
  }
  assert(nkfcc != nullptr);
  status.reset();

  icu::UnicodeString str = from_utf8(bytes);
  str = nkfcc->normalize(str, status);
  if(status.isFailure()) {
    throw std::runtime_error{status.errorName()};
  }
  status.reset();
  return to_utf8(str);
}

int main(int argc, char* argv[])
{
  std::string bytes{read_file("./test")};

  uint8_t version[U_MAX_VERSION_LENGTH];
  char version_str[U_MAX_VERSION_STRING_LENGTH];
  u_getUnicodeVersion(version);
  u_versionToString(version, version_str);
  std::cout << "Unicode " << version_str << "\n";


  std::cout << bytes << "\n";
  std::cout << to_utf8(from_utf8(bytes)) << "\n";
  std::cout << normalize(bytes) << "\n";
  std::cout << case_fold(bytes) << "\n";

  return EXIT_SUCCESS;
}
*/
