program := principia

# Find all source files
sources := $(shell find -wholename './src/*.cpp')
parsers := $(shell find -wholename './src/*.qx')

# Tools
finddeps := g++ -Isrc -MM -MP

compiler := clang++ -fsanitize=address
compiler := ${compiler} -std=c++11 -Wall -Wextra -Wno-unused-parameter -Werror=return-type -Werror=switch
compiler := ${compiler} -I. -Isrc -Ibuild/resources
# compiler := ${compiler} -fgraphite -flto
compiler := ${compiler} -march=corei7 -O3
# compiler := ${compiler} -march=native -O3
# compiler := ${compiler} -funsafe-loop-optimizations
# compiler := ${compiler} -Wunsafe-loop-optimizations
# compiler := ${compiler} -ffast-math -freciprocal-math

linker := ${compiler}
# linker := -fwhole-program

libs :=

# LLVM Support
# compiler := ${compiler} $(shell llvm-config --cxxflags)
compiler := ${compiler} -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
finddeps := ${finddeps} -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
linker := ${linker} $(shell llvm-config --ldflags)  -L$(shell llvm-config --libdir)
libs :=  ${libs} $(shell llvm-config --libs all core analysis mcjit native)

debug_flags := -ggdb
profiling_flags := -fprofile-generate --coverage
profiled_flags := -fprofile-use

# QUEX_PATH := /home/remco/src/quex/quex-0.58.3/
# quex := QUEX_PATH=${QUEX_PATH} ${QUEX_PATH}/quex-exe.py 

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
compiler := ${compiler} -Iquex
compiler := ${compiler} -DQUEX_OPTION_COMPUTED_GOTOS
compiler := ${compiler} -DQUEX_OPTION_ASSERTS_DISABLED
compiler := ${compiler} -DQUEX_OPTION_TOKEN_STAMPING_WITH_LINE_AND_COLUMN
compiler := ${compiler} -DQUEX_SETTING_BUFFER_SIZE=32768

debug_objects := $(patsubst ./src/%.cpp, ./build/debug/src/%.o, $(sources)) \
	$(patsubst ./src/%.qx, ./build/debug/build/resources/%.qx.o, $(parsers))

profiling_objects := $(patsubst ./src/%.cpp, ./build/profiling/src/%.o, $(sources)) \
	$(patsubst ./src/%.qx, ./build/profiling/build/resources/%.qx.o, $(parsers))

profiled_objects := $(patsubst ./src/%.cpp, ./build/profiled/src/%.o, $(sources)) \
	$(patsubst ./src/%.qx, ./build/profiled/build/resources/%.qx.o, $(parsers))


# Keep all intermediates
.SECONDARY:

all:

-include $(patsubst ./src/%.cpp, ./build/resources/%.d, $(sources))

build/resources/%.d: src/%.cpp
	@echo "Deps  " $*.cpp
	@mkdir -p $(dir $@)
	@$(finddeps) -MG -MF $@ -MT "build/profiling/src/$*.o build/profiled/src/$*.o  $@" $<
	@sed "s_[^ ]*\.qx\.h_build/resources/\0_" -i $@

build/resources/%.qx.h build/resources/%.qx.cpp: src/%.qx
	@echo "Queχ  " $*.qx
	@mkdir -p $(dir $@)
	$(quex) -i $< --analyzer-class $(basename $(notdir $<))
	@cp $(notdir $*).hpp build/resources/$*.qx.h
	@mv $(notdir $*).hpp build/resources/$*.hpp
	@mv $(notdir $*)-token.hpp build/resources/$*-token.hpp
	@mv $(notdir $*)-token_ids.hpp build/resources/$*-token_ids.hpp
	@mv $(notdir $*)-configuration.hpp build/resources/$*-configuration.hpp
	@mv $(notdir $*).cpp build/resources/$*.qx.cpp

# Special rule for the two llvm passes that need to be compiled without rtti
build/debug/src/Passes/Llvm%Combine.o: src/Passes/Llvm%Combine.cpp
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	@$(compiler) $(debug_flags) -fno-rtti -c $< -o $@

build/debug/%.o: %.cpp
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	@$(compiler) $(debug_flags) -c $< -o $@

build/profiling/%.o: %.cpp
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	@$(compiler) $(profiling_flags) -c $< -o $@

build/profiling/%.gcda:
	@echo "Profile missing: " $@

build/profiled/%.o: %.cpp build/profiling/%.gcda
	@echo "C+++  " $*.cpp
	@mkdir -p $(dir $@)
	-@cp build/profiling/$*.gcda build/profiled/$*.gcda &> /dev/null
	@$(compiler) $(profiled_flags) -c $< -o $@

build/debug/$(program): $(debug_objects)
	@echo "Link  " $@
	$(linker) $(debug_flags) $^ $(libs) -o $@

build/profiling/$(program): $(profiling_objects)
	@echo "Link  " $@
	@$(linker) $(profiling_flags) $^ $(libs) -o $@

build/profiled/$(program): $(profiled_objects)
	@echo "Link  " $@
	@$(linker) $(profiled_flags) $^ $(libs) -o $@
	@echo "Split " $@
	@objcopy --only-keep-debug --compress-debug-sections $@ $@.dbg
	@objcopy --strip-unneeded $@
	@objcopy --add-gnu-debuglink=$@.dbg $@
	@echo "Pack  " $@
	@upx -q --ultra-brute --best $@ > /dev/null

debug: build/debug/$(program)
	@cp $< $@

profiling: build/profiling/$(program)
	@cp $< $@

profile: profiling
	@echo Profiling...
	./$< Ackermann.txt PRA 3 6
	./$< Factorial.txt fact 23
	./$< EvenOdd.txt odd 4321

benchmark: profiled
	@echo Computing...
	./$< Ackermann.txt PRA 3 6
	/$< Factorial.txt fact 23
	/$< EvenOdd.txt odd 4321

# TODO: make tests
# TODO: Code coverage report for test
# TODO: Profiling (oprofile ?) report for benchmark
# TODO: make documentation

profiled: build/profiled/$(program)
	@cp $< $@

all: debug

clean:
	@rm -R build/*
