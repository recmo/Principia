sources = $(shell find -type f -name '*.cpp')
program = proglang
compiler = g++ -std=c++0x -Wall -Wextra -I. -ggdb -pipe -O3 -fgraphite -flto -DDEBUG
# compiler = g++ -std=c++0x -Wall -Wextra -I. -ggdb -pipe -O3 -fgraphite -flto -DRELEASE -fprofile-use -fprofile-generate
# compiler = g++ -std=c++0x -Wall -Wextra -I. -ggdb -pipe -O3 -fgraphite -flto -DRELEASE -fprofile-use

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
	@echo "Split " $@
	@objcopy --only-keep-debug --compress-debug-sections $@ $@.dbg
	@objcopy --strip-unneeded $@
	@objcopy --add-gnu-debuglink=$@.dbg $@
#	@echo "Pack  " $@
#	@upx -q --ultra-brute --best $@ > /dev/null

clean:
	@rm -f $(sources:.cpp=.d)
	@rm -f $(sources:.cpp=.gcda)
	@rm -f $(sources:.cpp=.o)
	@rm -f $(shell find -type f -name '*~')
	@rm -f callgrind.out.*
	@rm -f $(program) $(program).debug
