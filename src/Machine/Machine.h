#pragma once
#include <cinttypes>
#include <Parser/Compile.h>
#include <vector>
namespace Machine {

void load(const Compile::Program& program);

void print();

void run();

}
