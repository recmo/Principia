#pragma once
#include <cinttypes>
#include <Parser/Compile.h>
#include <vector>
namespace Machine {

void unload();

void load(const Compile::Program& program);

void print();
void print_graph();

void run();

void assemble();

}
