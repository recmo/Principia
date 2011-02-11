NAME := proglang

all: ${NAME}

${NAME}: $(shell find -type f -name '*.cpp')
	g++ -std=c++0x -Wall -pedantic -O3 -ggdb -I. $^ -o $@
	# objcopy --compress-debug-sections --only-keep-debug $@ $@.debug
	# objcopy --strip-unneeded $@
