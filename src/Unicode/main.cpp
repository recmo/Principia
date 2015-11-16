#include <Unicode/convert.h>
#include <Unicode/string.h>
#include <cinttypes>
#include <vector>
#include <execinfo.h>
#include <cxxabi.h>

int32_t Main(const std::vector<string>& arguments);

bool locale_supports_unicode()
{
	try {
		encodeLocal(L"Test ∃ ∀ ∫ x×y⁷·π dx");
	}
	catch(std::invalid_argument) {
		return false;
	}
	return true;
}

void print_stacktrace(unsigned int max_frames = 63)
{
	using namespace std;
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
	char* funcname = new char[funcnamesize];
	
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
		
		if (begin_name && begin_offset && end_offset
			&& begin_name < begin_offset) {
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
		else {
			// couldn't parse the line? print the whole line.
			wcerr << "  " << symbollist[i];
			wcerr << endl;
		}
	}
	
	delete[] funcname;
	free(symbollist);
}

int main(int argc, char* argv[])
{
	try {
		// Retrieve the locale of the environment
		std::locale locale("");
		std::locale::global(locale);
		
		// Test the locale for Unicode support
		if(!locale_supports_unicode()) {
			std::wcerr << L"FATAL: Environment locale does"
				"not support unicode." << std::endl;
			return -1;
		}
		
		// Decode the arguments
		std::vector<std::wstring> args;
		args.reserve(argc);
		for(int i = 0; i < argc; i++)
			args.push_back(decodeLocal(argv[i]));
		
		// Call the augmented main
		return Main(args);
	}
	catch(const std::exception& e) {
		const std::type_info& type = typeid(e);
		std::wcerr << L"Unhandled exception of type ";
		std::wcerr << type.name() << L": \"";
		std::wcerr << e.what() << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(const std::wstring& msg) {
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << msg << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(const std::string& msg) {
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << decodeLocal(msg) << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(const wchar_t* msg) {
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << msg << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(const char* msg) {
		std::wcerr << L"Unhandled exception: \"";
		std::wcerr << decodeLocal(msg) << L"\"" << std::endl;
		print_stacktrace();
		return -1;
	}
	catch(...) {
		/// TODO: Print type
		std::wcerr << L"Unhandled exception of unknown type." << std::endl;
		print_stacktrace();
		return -1;
	}
}
