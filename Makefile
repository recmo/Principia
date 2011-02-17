NAME := proglang
CPP := g++ -std=c++0x -Wall -pedantic -O3 -ggdb
# CPP := clang

all: ${NAME}

${NAME}: $(shell find -type f -name '*.cpp')
	$(CPP) -I. $^ -o $@
	objcopy --compress-debug-sections --only-keep-debug $@ $@.debug
	objcopy --strip-unneeded $@
