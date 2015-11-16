#pragma once
#include <string>
#include <iostream>

std::wstring decodeUtf8(const std::string& encoded);
std::string encodeUtf8(const std::wstring& plaintext);
std::wstring decodeLocal(const std::string& encoded);
std::string encodeLocal(const std::wstring& plaintext);
