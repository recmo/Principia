#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
namespace Parser { struct Program; };

namespace Compile {

std::vector<std::set<uint>> calculate_dependencies(const Parser::Program& p);

std::vector<uint> calculate_closure_calls(const Parser::Program& p);

std::vector<std::vector<std::pair<uint,uint>>> calculate_closures(
	const Parser::Program& p);

void compile(const Parser::Program& p);

};
