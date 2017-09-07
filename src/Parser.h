#pragma once
#include <cstdint>
#include <string>

class Parser {
public:
  static std::string unicode_version();
  static std::string icu_version();

  static bool token_match(const std::string& a, const std::string& b);
  static int token_compare(const std::string& a, const std::string& b);

  Parser();
  virtual ~Parser();

  void parse_file(const std::string& filename);

  virtual void on_file_begin(const std::string& name);
  virtual void on_file_end();
  virtual void on_indentation_begin(uint depth) {}
  virtual void on_indentation_end(){}
  virtual void on_line_begin();
  virtual void on_line_end();
  virtual void on_token(const std::string& token, const std::string& case_folded);
  virtual void on_literal(const std::string& literal);

private:
  uint indent_depth;
  uint quote_depth;
  uint line_depth;

  static std::string normalize_nfc(const std::string&);
  static std::string normalize_nfkc_case_folded(const std::string&);
};
