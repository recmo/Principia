NAME=proglang
CPP=g++ -std=c++0x -Wall -pedantic -O3 -ggdb -I.
OBJECTS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: ${NAME}

${NAME}: ${OBJECTS}
	${CPP} $^ -o $@

%.o: %.cpp *.h Makefile
	${CPP} -c $<
