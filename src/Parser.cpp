#include "Parser.h"
#include <stdexcept>
#include <cassert>
#include <fstream>
#include <sstream>
#include <unicode/uversion.h>
#include <unicode/utypes.h>
#include <unicode/unistr.h>
#include <unicode/errorcode.h>
#include <unicode/bytestream.h>
#include <unicode/normalizer2.h>

std::string read_file(const std::string& filename)
{
  std::ifstream stream{filename, std::ios::binary};
  return std::string{
    std::istreambuf_iterator<char>{stream},
    std::istreambuf_iterator<char>{}};
}

icu::UnicodeString icu_from_utf8(const std::string& bytes)
{
  return icu::UnicodeString::fromUTF8(icu::StringPiece(bytes));
}

std::string icu_to_utf8(const icu::UnicodeString& str)
{
  std::string result;
  icu::StringByteSink<std::string> sink{&result};
  str.toUTF8(sink);
  return result;
}

const uint32_t newline = 0x000A;
const uint32_t tab = 0x0009;
const uint32_t quote_open = 0x201C;
const uint32_t quote_close = 0x201D;

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
  if(codepoint == quote_open || codepoint == quote_close) {
    return false;
  }
  return u_hasBinaryProperty(codepoint, UCHAR_PATTERN_SYNTAX);
}

bool is_whitespace(uint32_t codepoint)
{
  return u_hasBinaryProperty(codepoint, UCHAR_PATTERN_WHITE_SPACE;
}

std::string normalize(const std::string& bytes)
{
  icu::ErrorCode status;
  const icu::Normalizer2* nfc = icu::Normalizer2::getNFCInstance(status);
  if(status.isFailure()) {
    throw std::runtime_error{status.errorName()};
  }
  assert(nfc != nullptr);
  status.reset();

  icu::UnicodeString str = icu_from_utf8(bytes);
  str = nfc->normalize(str, status);
  if(status.isFailure()) {
    throw std::runtime_error{status.errorName()};
  }
  status.reset();
  return icu_to_utf8(str);
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

  icu::UnicodeString str = icu_from_utf8(bytes);
  str = nkfcc->normalize(str, status);
  if(status.isFailure()) {
    throw std::runtime_error{status.errorName()};
  }
  status.reset();
  return icu_to_utf8(str);
}

std::string Parser::unicode_version()
{
  uint8_t version[U_MAX_VERSION_LENGTH];
  char version_str[U_MAX_VERSION_STRING_LENGTH];
  u_getUnicodeVersion(version);
  u_versionToString(version, version_str);
  return std::string{version_str};
}

std::string Parser::icu_version()
{
  uint8_t version[U_MAX_VERSION_LENGTH];
  char version_str[U_MAX_VERSION_STRING_LENGTH];
  u_getVersion(version);
  u_versionToString(version, version_str);
  return std::string{version_str};
}

Parser::Parser():
indent_depth{0},
quote_depth{0},
line_depth{0}
{
}

Parser::~Parser()
{
}

void Parser::parse_file(const std::string& filename)
{
  // TODO: Overflow
  const icu::UnicodeString contents = icu_from_utf8(read_file(filename));
  on_file_begin(filename);
  uint indentation = 0;
  for(int i = 0; i < contents.length(); ++i) {
    // <sp>* <nl> #<tab>* </sp>
    if(i == 0 || contents[i] == newline) {
      while(is_whitespace(contents[i])) {

      }
      on_indentation_begin(0);
    }
    if(is_whitespace(contents[i])) {
      continue;
    }
    if(is_identifier(contents[i])) {
      const uint start = i;
      while(is_identifier(contents[i])) {
        ++i;
      }
      const uint end = i;
      --i;
      const auto ss = contents.tempSubStringBetween(start, end);
      const std::string id = icu_to_utf8(ss);
      on_token(normalize(id), case_fold(id));
    }
    if(contents[i] == quote_open) {
      const uint start = i + 1;
      uint depth = 1;
      while(depth > 0) {
        ++i;
        if(contents[i] == quote_open) {
          ++depth;
        }
        if(contents[i] == quote_close) {
          --depth;
        }
      }
      const uint end = i;
      const auto ss = contents.tempSubStringBetween(start, end);
      on_literal(normalize(icu_to_utf8(ss)));
    }
    if(is_operator(contents[i])) {
      std::string id = icu_to_utf8(contents.tempSubString(i, 1));
      on_token(normalize(id), case_fold(id));
    }
  }
  on_file_end();
}

void Parser::on_file_begin(const std::string& name)
{
  std::cerr << "on_file_begin: " << name << "\n";
}

void Parser::on_file_end()
{
  std::cerr << "on_file_end\n";
}

void Parser::on_token(const std::string& token, const std::string& case_folded)
{
  std::cerr << "on_token: " << token << " " << case_folded << "\n";
}

void Parser::on_literal(const std::string& literal)
{
  std::cerr << "on_literal: " << literal << "\n";
}


void Parser::on_line_begin()
{
  std::cerr << "on_line_begin\n";
}

void Parser::on_line_end()
{
  std::cerr << "on_line_end\n";
}
