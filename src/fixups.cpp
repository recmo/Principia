#include<fixups.h>
#include <execinfo.h>
#include <cxxabi.h>

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
		throw std::invalid_argument("The argument could not be decoded from UTF-8.");
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
	char* out = new char[length * 6]; // UTF-8 takes six bytes per codepoint max, there is no Byte Order Mark
	char* out_next = 0;
	
	// Call the converter
	converter::result result = facet.out(mbstate, in, in + length, in_next, out, out + (6 * length), out_next);
	
	// Parse error codes
	if(result != converter::ok)
	{
		wcerr << endl << L"input = ";
		wcerr << plaintext;
		wcerr << endl;
		wcerr << "result = " << result << endl;
		throw std::invalid_argument("The argument could not be encoded to UTF-8.");
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
		throw std::invalid_argument("The argument could not be decoded from local encoding.");
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
		wcerr << endl << L"input = ";
		wcerr << plaintext;
		wcerr << endl;
		throw std::invalid_argument("The argument could not be encoded to local encoding.");
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


void print_stacktrace(unsigned int max_frames = 63)
{
	wcerr << "stack trace:" << endl;
	
	// storage array for stack trace address data
	void* addrlist[max_frames+1];
	
	// retrieve current stack addresses
	int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
	
	if (addrlen == 0) {
		wcerr << "  <empty, possibly corrupt>" << endl;
		return;
	}
	
	// resolve addresses into strings containing "filename(function+address)",
	// this array must be free()-ed
	char** symbollist = backtrace_symbols(addrlist, addrlen);
	
	// allocate string which will be filled with the demangled function name
	size_t funcnamesize = 256;
	char* funcname = (char*)malloc(funcnamesize);
	
	// iterate over the returned symbol lines. skip the first, it is the
	// address of this function.
	for (int i = 1; i < addrlen; i++) {
		char *begin_name = 0, *begin_offset = 0, *end_offset = 0;
		
		// find parentheses and +address offset surrounding the mangled name:
		// ./module(function+0x15c) [0x8048a6d]
		for (char *p = symbollist[i]; *p; ++p) {
			if (*p == '(')
				begin_name = p;
			else if (*p == '+')
				begin_offset = p;
			else if (*p == ')' && begin_offset) {
				end_offset = p;
				break;
			}
		}
		
		if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
			*begin_name++ = '\0';
			*begin_offset++ = '\0';
			*end_offset = '\0';
			
			// mangled name is now in [begin_name, begin_offset) and caller
			// offset in [begin_offset, end_offset). now apply
			// __cxa_demangle():
			
			int status;
			char* ret = abi::__cxa_demangle(begin_name,
													  funcname, &funcnamesize, &status);
			if (status == 0) {
				funcname = ret; // use possibly realloc()-ed string
				wcerr << "  " << symbollist[i];
				wcerr << " : " << funcname;
				wcerr << "+" << begin_offset;
				wcerr << endl;
			} else {
				// demangling failed. Output function name as a C function with
				// no arguments.
				wcerr << "  " << symbollist[i];
				wcerr << " : " << begin_name;
				wcerr << "+" << begin_offset;
				wcerr << endl;
			}
		}
		else
		{
			// couldn't parse the line? print the whole line.
			wcerr << "  " << symbollist[i];
			wcerr << endl;
		}
	}
	
	free(funcname);
	free(symbollist);
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
			args.push_back(decodeLocal(argv[i]));
		
		// Call the augmented main
		return Main(args);
	}
	catch(const std::exception& e)
	{
		const std::type_info& type = typeid(e);
		std::wcerr << L"Unhandled exception of type ";
		std::wcerr << type.name() << L": \"";
		std::wcerr << e.what() << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(const std::wstring& msg)
	{
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << msg << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(const std::string& msg)
	{
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << decodeLocal(msg) << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(const wchar* msg)
	{
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << msg << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(const char* msg)
	{
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << decodeLocal(msg) << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(...)
	{
		/// TODO: Print type
		std::wcerr << L"Unhandled exception of unknown type." << std::endl;
		print_stacktrace();
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
	print_stacktrace();
	throw assertion_failed;
}

