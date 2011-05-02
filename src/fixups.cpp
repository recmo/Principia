#include<fixups.h>

std::wstring decodeUtf8(const std::string& encoded)
{
	// De-uglify names
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
	mbstate_t mbstate = mbstate_t();
	std::memset(&mbstate, 0, sizeof(mbstate_t));
	
	// Construct input and output buffers
	uint32 length = encoded.length();
	const char* in = encoded.c_str();
	const char* in_next = 0;
	wchar* out = new wchar[length];
	wchar* out_next = 0;
	
	// Call the converter
	converter::result result = utf8Encoding.in(mbstate, in, in + length, in_next, out, out + length, out_next);
	
	// Parse error codes
	if(result != converter::ok)
	{
		throw std::invalid_argument("The argument could not be decoded.");
	}
	
	// Copy to a wstring object
	wstring wstr(out, out_next - out);
	delete[] out;
	return wstr;
}

std::string encodeUtf8(const std::wstring& plaintext)
{
	// De-uglify names
	using namespace std; 
	typedef codecvt<wchar_t, char, mbstate_t> converter;
	typedef codecvt_byname<wchar_t, char, mbstate_t> converterByName;
	
	// Construct the locale and converter
	// Note that converterByName _requires_ a language/culture
	locale utf8locale(locale("C"), new converterByName("en_GB.UTF-8"));
	
	// Retrieve the converter facet
	const converter& facet = std::use_facet<converter>(utf8locale);
	
	// Construct the mbstate
	// Bug workaround (see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=28059):
	mbstate_t mbstate = mbstate_t();
	std::memset(&mbstate, 0, sizeof(mbstate_t));
	
	// Construct input and output buffers
	uint32 length = plaintext.length();
	const wchar_t* in = plaintext.c_str();
	const wchar_t* in_next = 0;
	char* out = new char[length];
	char* out_next = 0;
	
	// Call the converter
	converter::result result = facet.out(mbstate, in, in + length, in_next, out, out + length, out_next);
	
	// Parse error codes
	if(result != converter::ok)
	{
		throw std::invalid_argument("The argument could not be decoded.");
	}
	
	std::string str(out, out_next - out);
	delete[] out;
	return str;
}

std::wstring decodeLocal(const std::string& encoded)
{
	// De-uglify names
	using namespace std; 
	typedef codecvt<wchar_t, char, mbstate_t> converter;
	
	// Construct the locale and converter
	std::locale localLocale("");
	
	// Retrieve the converter facet
	const converter& facet = std::use_facet<converter>(localLocale);
	
	// Construct the mbstate
	// Bug workaround (see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=28059):
	mbstate_t mbstate = mbstate_t();
	std::memset(&mbstate, 0, sizeof(mbstate_t));
	
	// Construct input and output buffers
	uint32 length = encoded.length();
	const char* in = encoded.c_str();
	const char* in_next = 0;
	wchar_t* out = new wchar_t[length];
	wchar_t* out_next = 0;
	
	// Call the converter
	converter::result result = facet.in(mbstate, in, in + length, in_next, out, out + length, out_next);
	
	// Parse error codes
	if(result != converter::ok)
	{
		throw std::invalid_argument("The argument could not be decoded.");
	}
	
	std::wstring wstr(out, out_next - out);
	delete[] out;
	return wstr;
}

std::string encodeLocal(const std::wstring& plaintext)
{
	// De-uglify names
	using namespace std; 
	typedef codecvt<wchar_t, char, mbstate_t> converter;
	
	// Construct the locale and converter
	std::locale localLocale("");
	
	// Retrieve the converter facet
	const converter& facet = std::use_facet<converter>(localLocale);
	
	// Construct the mbstate
	// Bug workaround (see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=28059):
	mbstate_t mbstate = mbstate_t();
	std::memset(&mbstate, 0, sizeof(mbstate_t));
	
	// Construct input and output buffers
	uint32 length = plaintext.length();
	const wchar_t* in = plaintext.c_str();
	const wchar_t* in_next = 0;
	char* out = new char[length];
	char* out_next = 0;
	
	// Call the converter
	converter::result result = facet.out(mbstate, in, in + length, in_next, out, out + length, out_next);
	
	// Parse error codes
	if(result != converter::ok)
	{
		throw std::invalid_argument("The argument could not be decoded.");
	}
	
	std::string str(out, out_next - out);
	delete[] out;
	return str;
}

std::vector<std::wstring> tokenize(const std::wstring& string)
{
	// Split into ids
	std::vector<std::wstring> tokens;
	std::wistringstream iss(string);
	copy(std::istream_iterator<std::wstring, wchar, std::char_traits<wchar> >(iss),
		  std::istream_iterator<std::wstring, wchar, std::char_traits<wchar> >(),
		  std::back_inserter<std::vector<std::wstring> >(tokens));
	return tokens;
}

sint32 main(sint32 argc, char* argv[])
{
	try
	{
		// Retrieve the locale of the environment
		std::locale locale("");
		std::locale::global(locale);
		
		// Decode the arguments
		std::vector<std::wstring> args;
		args.reserve(argc);
		for(sint32 i = 0; i < argc; i++)
		{
			args.push_back(decodeLocal(argv[i]));
		}
		
		// Call the augmented main
		return Main(args);
	}
	catch(const std::exception& e)
	{
		const std::type_info& type = typeid(e);
		std::wcerr << L"Unhandled exception of type ";
		std::wcerr << type.name() << L": \"";
		std::wcerr << e.what() << L"\"" << std::endl;
		return -1;
	}
	catch(const std::wstring& msg)
	{
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << msg << L"\"" << std::endl;
		return -1;
	}
	catch(const std::string& msg)
	{
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << decodeLocal(msg) << L"\"" << std::endl;
		return -1;
	}
	catch(const wchar* msg)
	{
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << msg << L"\"" << std::endl;
		return -1;
	}
	catch(const char* msg)
	{
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << decodeLocal(msg) << L"\"" << std::endl;
		return -1;
	}
	catch(...)
	{
		/// TODO: Print type
		std::wcerr << L"Unhandled exception of unknown type." << std::endl;
		return -1;
	}
}

bool assert_fail(const char* expression, const char* file, const int line, const char* function)
{
	/// TODO: What is the propper encoding of expression, file and function?
	/// TODO: Make special assertion_failure exception class
	std::wstring message;
	message += L"In function " + decodeUtf8(function) + L":\n";
	message += L"Assertion failed: " + decodeUtf8(expression) + L"\n";
	message += L"In " + decodeUtf8(file) + L" at " + toString<int>(line) + L"\n";
	std::runtime_error assertion_failed(encodeLocal(message));
	throw assertion_failed;
}

