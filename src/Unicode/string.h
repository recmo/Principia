#pragma once
#include <string>
#include <iostream>

typedef std::wstring string;

std::ostream& operator<<(std::ostream& out, const std::wstring& value);
