sources = $(shell find -type f -name '*.cpp')
program = proglang
compiler = g++ -std=c++0x -Wall -I. -ggdb -O3 -fgraphite -flto -DDEBUG
#compiler = g++ -std=c++0x -Wall -I. -ggdb -O3 -fgraphite -flto -DRELEASE

all: $(program)

include $(sources:.cpp=.d)

%.d: %.cpp
	@$(compiler) -MM -MF $@ -MT "$*.o $@" $< 

%.o: %.cpp
	@echo "C++   " $<
	@$(compiler) -c $< -o $@

$(program): $(sources:.cpp=.o)
	@echo "Link  " $@
	@$(compiler) -fwhole-program $^ -o $@
	@echo "Strip " $@
	@objcopy --only-keep-debug --compress-debug-sections $@ $@.debug
	@objcopy --strip-unneeded $@
	@objcopy --add-gnu-debuglink=$@.debug $@
#	@upx --ultra-brute --best $@

clean:
	@rm -f $(sources:.cpp=.d)
	@rm -f $(sources:.cpp=.o)
	@rm -f $(shell find -type f -name '*~')
	@rm -f callgrind.out.*
	@rm -f $(program) $(program).debug

