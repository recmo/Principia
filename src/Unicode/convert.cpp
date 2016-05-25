#include "locale.h"
#include <locale>
#include <iostream>
#include <cstring> // for std::memset

std::wstring decodeUtf8(const std::string& encoded)
{
	using namespace std; 
	typedef codecvt<wchar_t, char, mbstate_t> converter;
	typedef codecvt_byname<wchar_t, char, mbstate_t> converterByName;
	
	// Construct the locale and converter
	// Note that converterByName _requires_ a language/culture
	locale utf8locale(locale("C"), new converterByName("en_GB.UTF-8"));
	
	// Retrieve the converter facet
	const converter& utf8Encoding = use_facet<converter>(utf8locale);
	
	// Construct the mbstate
	// Bug workaround (see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=28059):
	mbstate_t mbstate;
	memset(&mbstate, 0, sizeof(mbstate_t));
	
	// Construct input and output buffers
	uint32_t length = encoded.length();
	const char* in = encoded.c_str();
	const char* in_next = 0;
	wchar_t* out = new wchar_t[length];
	wchar_t* out_next = 0;
	
	// Call the converter
	converter::result result = utf8Encoding.in(mbstate,
		in, in + length, in_next,
		out, out + length, out_next);
	
	// Parse error codes
	if(result != converter::ok) {
		throw std::invalid_argument("The argument could not be decoded from UTF-8.");
	}
	
	// Copy to a wstring object
	std::wstring wstr(out, out_next - out);
	delete[] out;
	return wstr;
}

std::string encodeUtf8(const std::wstring& plaintext)
{
	using namespace std; 
	typedef codecvt<wchar_t, char, mbstate_t> converter;
	typedef codecvt_byname<wchar_t, char, mbstate_t> converterByName;
	
	// Construct the locale and converter
	// Note that converterByName _requires_ a language/culture
	locale utf8locale(locale("C"), new converterByName("en_GB.UTF-8"));
	
	// Retrieve the converter facet
	const converter& facet = use_facet<converter>(utf8locale);
	
	// Construct the mbstate
	// Bug workaround (see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=28059):
	mbstate_t mbstate;
	memset(&mbstate, 0, sizeof(mbstate_t));
	
	// Construct input and output buffers
	uint32_t length = plaintext.length();
	uint32_t max_size = length * 6; // Max 6 bytes per character
	const wchar_t* in = plaintext.c_str();
	const wchar_t* in_next = 0;
	char* out = new char[max_size];
	char* out_next = 0;
	
	// Call the converter
	converter::result result = facet.out(mbstate,
		in, in + length, in_next,
		out, out + max_size, out_next);
	
	// Parse error codes
	if(result != converter::ok) {
		throw std::invalid_argument("The argument could not be encoded to UTF-8.");
	}
	
	std::string str(out, out_next - out);
	delete[] out;
	return str;
}

std::wstring decodeLocal(const std::string& encoded)
{
	using namespace std; 
	typedef codecvt<wchar_t, char, mbstate_t> converter;
	
	// Construct the locale and converter
	locale localLocale("");
	
	// Retrieve the converter facet
	const converter& facet = use_facet<converter>(localLocale);
	
	// Construct the mbstate
	// Bug workaround (see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=28059):
	mbstate_t mbstate;
	memset(&mbstate, 0, sizeof(mbstate_t));
	
	// Construct input and output buffers
	uint32_t length = encoded.length();
	const char* in = encoded.c_str();
	const char* in_next = 0;
	wchar_t* out = new wchar_t[length];
	wchar_t* out_next = 0;
	
	// Call the converter
	converter::result result = facet.in(mbstate,
		in, in + length, in_next,
		out, out + length, out_next);
	
	// Parse error codes
	if(result != converter::ok) {
		throw std::invalid_argument("The argument could not be decoded from local encoding.");
	}
	
	std::wstring wstr(out, out_next - out);
	delete[] out;
	return wstr;
}

std::string encodeLocal(const std::wstring& plaintext)
{
	using namespace std; 
	typedef codecvt<wchar_t, char, mbstate_t> converter;
	
	// Construct the locale and converter
	locale localLocale("");
	
	// Retrieve the converter facet
	const converter& facet = use_facet<converter>(localLocale);
	
	// Construct the mbstate
	// Bug workaround (see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=28059):
	mbstate_t mbstate;
	memset(&mbstate, 0, sizeof(mbstate_t));
	
	// Construct input and output buffers
	uint32_t length = plaintext.length();
	uint32_t max_size = length * 6; // Max 6 bytes per character
	const wchar_t* in = plaintext.c_str();
	const wchar_t* in_next = 0;
	char* out = new char[max_size];
	char* out_next = 0;
	
	// Call the converter
	converter::result result = facet.out(mbstate,
		in, in + length, in_next,
		out, out + max_size, out_next);
	
	// Parse error codes
	if(result != converter::ok) {
		throw std::invalid_argument("The argument could not be encoded to local encoding.");
	}
	
	std::string str(out, out_next - out);
	delete[] out;
	return str;
}
