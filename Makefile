program := Principia

# Find all source files
sources := $(shell find -wholename './src/*.cpp')
parsers := $(shell find -wholename './src/*.qx')

# Header dependency finder
finddeps := clang++ -I. -Isrc -MM -MP

# C++11 Compiler
compiler := clang++ -std=c++11 -g -fsanitize=address
compiler := ${compiler} -Wall -Wextra -Wno-unused-parameter -Werror=return-type -Werror=switch
compiler := ${compiler} -Ibuild/quex -Isrc -Ibuild/resources

# Linker
linker := clang++ -fsanitize=address
libs :=

# LLVM library support
compiler := ${compiler} $(filter-out -fno-exceptions,$(shell llvm-config --cxxflags))
compiler := ${compiler} -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
finddeps := ${finddeps} -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
linker := ${linker} $(shell llvm-config --ldflags)  -L$(shell llvm-config --libdir)
libs :=  ${libs} $(shell llvm-config --libs all core analysis mcjit native)
libs :=  ${libs} $(shell llvm-config --system-libs)

# Queχ
quex := quex
quex := ${quex} --file-extension-scheme pp
quex := ${quex} --token-id-prefix Token
# quex := ${quex} --codec utf8 --buffer-element-size 1
quex := ${quex} --iconv --buffer-element-size 4
quex := ${quex} --buffer-limit 0xFF --path-termination 0xFE
quex := ${quex} --comment-state-machine --comment-transitions --comment-mode-patterns
quex := ${quex} --no-DOS
quex := ${quex} --template-compression-uniform
# quex := ${quex} --path-compression
quex := ${quex} --template-compression
quex := ${quex} --no-mode-transition-check
compiler := ${compiler} -DQUEX_OPTION_COMPUTED_GOTOS
compiler := ${compiler} -DQUEX_OPTION_ASSERTS_DISABLED
compiler := ${compiler} -DQUEX_OPTION_TOKEN_STAMPING_WITH_LINE_AND_COLUMN
compiler := ${compiler} -DQUEX_SETTING_BUFFER_SIZE=32768

# All object files
objects := $(patsubst ./src/%.cpp, ./build/%.o, $(sources)) \
	build/quex/QuexParser.o

# Make the whole program by default (obviously)
all: $(program)

# Keep all intermediates
.SECONDARY:

# Include header dependency info
-include $(patsubst ./src/%.cpp, ./build/resources/%.d, $(sources))

# Build header dependency info
build/%.d: src/%.cpp
	@echo "Deps  " $*.cpp
	@mkdir -p $(dir $@)
	@$(finddeps) -MG -MF $@ -MT "build/src/$*.o build/src/$*.o  $@" $<

build/quex/QuexParser.hpp build/quex/QuexParser.cpp: src/Parser/QuexParser.qx
	@echo "Queχ  " $*.qx
	mkdir -p build/quex
	$(quex) -i $< \
		--analyzer-class QuexParser \
		--source-package build/quex
	sed -i 's|std::string(Tok)|std::string(Tok.get_string())|' build/quex/QuexParser-token.hpp

# Special rule for the two llvm passes that need to be compiled without rtti
build/src/Passes/Llvm%Combine.o: src/Passes/Llvm%Combine.cpp
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	@$(compiler) -fno-rtti -c $< -o $@

build/%.o: src/%.cpp build/quex/QuexParser.hpp
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	$(compiler) -c $< -o $@

build/quex/%.o: build/quex/%.cpp
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	$(compiler) -c $< -o $@

$(program): $(objects)
	@echo "Link  " $@
	$(linker) $^ $(libs) -o $@

benchmark: $(program)
	@echo Computing...
	./$< Ackermann.txt PRA 3 6
	./$< Factorial.txt fact 23
	./$< EvenOdd.txt odd 4321

# TODO: make tests
# TODO: Code coverage report for test
# TODO: make documentation

clean:
	@rm -Rf build/* $(program)
