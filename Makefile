program := principia

# Find all source files
sources := $(shell find -wholename './src/*.cpp')
lexers  := $(shell find -wholename './src/*.y')
parsers := $(shell find -wholename './src/*.qx')

# Tools
finddeps := g++ -Isrc -MM -MP

compiler := g++ -pipe
compiler := ${compiler} -std=c++0x -Wall -Wextra
compiler := ${compiler} -fgraphite -flto
compiler := ${compiler} -I. -Isrc -Ibuild/resources
compiler := ${compiler} -march=native -O3 -g3
compiler := ${compiler} -funsafe-loop-optimizations
# compiler := ${compiler} -Wunsafe-loop-optimizations
compiler := ${compiler} -ffast-math -freciprocal-math

linker := ${compiler} -fwhole-program

profiling_flags := -fprofile-dir=build/profile -fprofile-generate --coverage
profiled_flags := -fprofile-dir=build/profile -fprofile-use

quex := ${QUEX_PATH}/quex-exe.py 
quex := ${quex} --file-extension-scheme pp
quex := ${quex} --codec utf8 --buffer-limit 0xFF --path-termination 0xFE
quex := ${quex} --token-prefix Token
quex := ${quex} --no-mode-transition-check
quex := ${quex} --template-compression 1.0
compiler := ${compiler} -I${QUEX_PATH}
compiler := ${compiler} -DQUEX_OPTION_COMPUTED_GOTOS
compiler := ${compiler} -DQUEX_OPTION_ASSERTS_DISABLED
compiler := ${compiler} -DQUEX_OPTION_TOKEN_STAMPING_WITH_LINE_AND_COLUMN
compiler := ${compiler} -DQUEX_SETTING_BUFFER_SIZE=32768

lemon := lemon
compiler := ${compiler} -DNDEBUG


profiling_objects := $(patsubst ./src/%.cpp, ./build/profiling/src/%.o, $(sources)) \
	$(patsubst ./src/%.y, ./build/profiling/build/resources/%.y.o, $(lexers)) \
	$(patsubst ./src/%.qx, ./build/profiling/build/resources/%.qx.o, $(parsers))

profiled_objects := $(patsubst ./src/%.cpp, ./build/profiled/src/%.o, $(sources)) \
	$(patsubst ./src/%.y, ./build/profiled/build/resources/%.y.o, $(lexers)) \
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
	@sed "s_[^ ]*\.y\.h_build/resources/\0_" -i $@

build/resources/%.y.h build/resources/%.y.cpp: src/%.y
	@echo "Lemon " $*.y
	@mkdir -p $(dir $@)
	@$(lemon) $<
	@mv src/$*.h build/resources/$*.y.h
	@mv src/$*.c build/resources/$*.y.cpp

build/resources/%.qx.h build/resources/%.qx.cpp: src/%.qx build/resources/%.y.h
	@echo "Queχ  " $*.qx
	@mkdir -p $(dir $@)
	@$(quex) -i $< --analyzer-class $(basename $(notdir $<)) --foreign-token-id-file build/resources/$*.y.h
	@cp $(notdir $*).hpp build/resources/$*.qx.h
	@mv $(notdir $*).hpp build/resources/$*.hpp
	@mv $(notdir $*)-token.hpp build/resources/$*-token.hpp
	@mv $(notdir $*)-token_ids.hpp build/resources/$*-token_ids.hpp
	@mv $(notdir $*)-configuration.hpp build/resources/$*-configuration.hpp
	@mv $(notdir $*).cpp build/resources/$*.qx.cpp

build/profiling/%.o: %.cpp
	@echo "C++   " $*.cpp
	@mkdir -p $(dir $@)
	@$(compiler) $(profiling_flags) -c $< -o $@

build/profile/build/profiling/%.gcda:
	@echo "Profile missing: " $@

build/profiled/%.o: %.cpp build/profile/build/profiling/%.gcda
	@echo "C+++  " $*.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir build/profile/build/profiled/$*)
	-@cp build/profile/build/profiling/$*.gcda build/profile/build/profiled/$*.gcda
	@$(compiler) $(profiled_flags) -c $< -o $@

build/profiling/$(program): $(profiling_objects)
	@echo "Link  " $@
	@$(linker) $(profiling_flags) $^ -o $@

build/profiled/$(program): $(profiled_objects)
	@echo "Link  " $@
	@$(linker) $(profiled_flags) $^ -o $@
	@echo "Split " $@
	@objcopy --only-keep-debug --compress-debug-sections $@ $@.dbg
	@objcopy --strip-unneeded $@
	@objcopy --add-gnu-debuglink=$@.dbg $@
	@#echo "Pack  " $@
	@#upx -q --ultra-brute --best $@ > /dev/null

profiling: build/profiling/$(program)
	cp $< $@

profile: profiling
	@echo Profiling...
	./$< Ackermann.txt PRA 3 6
	./$< Factorial.txt fact 23
	./$< Factorial2.txt fact 23
	./$< EvenOdd.txt odd? 4321

benchmark: profiled
	@echo Computing...
	./$< Ackermann.txt PRA 3 6
	./$< Factorial.txt fact 23
	./$< Factorial2.txt fact 23
	./$< EvenOdd.txt odd? 4321

profiled: build/profiled/$(program)
	cp $< $@

all: profile profiled

clean:
	@rm -R build/*
