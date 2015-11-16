#include "string.h"
#include <sstream>
#include <iterator>

std::vector<std::wstring> tokenize(const std::wstring& string)
{
	using namespace std;
	typedef istream_iterator<wstring, wchar_t, char_traits<wchar_t>> iterator;
	vector<wstring> tokens;
	wistringstream iss(string);
	copy(iterator(iss), iterator(), back_inserter<vector<wstring>>(tokens));
	return tokens;
}

bool startsWith(const std::wstring& string, const std::wstring& prefix)
{
	if(string.size() < prefix.size())
		return false;
	for(std::wstring::const_iterator i = prefix.begin(), j = string.begin();
		i != prefix.end(); i++, j++)
		if(*i != *j)
			return false;
	return true;
}