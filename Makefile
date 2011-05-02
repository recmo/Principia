program = principia
sources := $(shell find -type f -name '*.cpp')
lexers := $(shell find -type f -name '*.qx')
parsers := $(shell find -type f -name '*.y')

compiler = g++ -std=c++0x -Wall -Wextra -I. -pipe -O3 -g3 -fgraphite -flto ${flags}
linker = ${compiler} -fwhole-program

quex := ${QUEX_PATH}/quex-exe.py

# Set the buffer to UTF-8, the symbols 0xFF and 0xFE are therefore disallowed in the buffer and can be used otherwise
quex := ${quex} --codec utf8 --buffer-limit 0xFF --path-termination 0xFE
quex := ${quex} --no-mode-transition-check
quex := ${quex} --template-compression 1.0
# quex := ${quex} --path-compression
# quex := ${quex} --path-compression-uniform
quex := ${quex} --file-extension-scheme xx
quex := ${quex} --token-prefix Token

# Compiler flags for Queχ:
flags := ${flags} -I${QUEX_PATH}
flags := ${flags} -DQUEX_OPTION_COMPUTED_GOTOS
flags := ${flags} -DQUEX_OPTION_ASSERTS_DISABLED 
flags := ${flags} -DQUEX_OPTION_TOKEN_STAMPING_WITH_LINE_AND_COLUMN
flags := ${flags} -DQUEX_SETTING_BUFFER_SIZE=32768

# Compiler flags for Lemon:
flags := ${flags} -DNDEBUG

# Own Compiler flags
# flags := ${flags} -fno-exceptions
# flags := ${flags} -DDEBUG
# flags := ${flags} -DRELEASE
flags := ${flags} -fprofile-use
flags := ${flags} -fprofile-use -fprofile-generate
flags := ${flags} -funsafe-loop-optimizations -Wunsafe-loop-optimizations
flags := ${flags} -ffast-math -freciprocal-math

# TODO:
# Subfolders:
# - Source
# - Tests
# - Benchmarks
# - Build/Resources (generated source files, .d files, stuff common to all builds)
# - Build/Object.generate-profile (object files and executable for the profile generating build)
# - Build/Profile (profiling data, to be used for all future builds)
# - Build/Object (object files for the optimized build)
# - Install/ ()



all: $(program)

-include $(sources:.cpp=.d)

.SECONDARY:

%.d: %.cpp
	@echo "Deps  " $<
	@$(compiler) -w -MM -MG -MP -MF $@ -MT "$*.o $@" $< 

%.o: %.cxx
	@echo "C++   " $<
	@$(compiler) -c $< -o $@

%.o: %.cpp
	@echo "C++   " $<
	@$(compiler) -c $< -o $@

$(program): $(sources:.cpp=.o) $(lexers:.qx=_Lexer.o) $(parsers:.y=_Parser.o)
	@echo "Link  " $@
	@$(compiler) -fwhole-program $^ -o $@
	@echo "Split " $@
	@objcopy --only-keep-debug --compress-debug-sections $@ $@.dbg
	@objcopy --strip-unneeded $@
	@objcopy --add-gnu-debuglink=$@.dbg $@
	@#echo "Pack  " $@
	@#upx -q --ultra-brute --best $@ > /dev/null

clean:
	@rm -f $(sources:.cpp=.d) $(sources:.cpp=.gcda) $(sources:.cpp=.o)
	@rm -f $(lexers:.qx=_Lexer.cxx) $(lexers:.qx=_Lexer.o) $(lexers:.qx=_Lexer.hxx) $(lexers:.qx=_Lexer-token.hxx) $(lexers:.qx=_Lexer-token_ids.hxx)
	@rm -f $(parsers:.y=_Parser.cxx) $(parsers:.y=_Parser.o) $(parsers:.y=_Parser.hxx)
	@rm -f $(shell find -type f -name '*~')
	@rm -f callgrind.out.*
	@rm -f $(program) $(program).debug

%_Parser.hxx %_Parser.cxx: %.y
	@echo "Lemon " $<
	@lemon $<
	@mv $*.h $*_Parser.hxx
	@mv $*.c $*_Parser.cxx

%_Lexer.cxx %_Lexer.hxx %_Lexer-token.hxx %_Lexer-token_ids.hxx %_Lexer-configuration.hxx: %.qx %_Parser.hxx
	@echo "Queχ  " $<
	@cd $(dir $<); $(quex) -i $(notdir $<) --analyzer-class $(basename $(notdir $<))::Lexer --foreign-token-id-file $(notdir $*)_Parser.hxx

