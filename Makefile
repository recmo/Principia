proglang: $(shell find -type f -name '*.cpp' -o -name '*.h')
	g++ -std=c++0x -Wall -pedantic -O3 -ggdb -I. \
	$(shell find -type f -name '*.cpp') -o $@
	# objcopy --compress-debug-sections --only-keep-debug $@ $@.debug
	# objcopy --strip-unneeded $@
