#pragma once
#include "fixups.h"
#include "Passes/Value.h"

class Node;
class Edge;

/// TODO: Reference counting garbage collection
class Closure
{
public:
	Closure(std::shared_ptr<Node> closure);
	Closure(std::shared_ptr<Node> closure, const std::vector<Value>& context);
	
	std::shared_ptr<Node> node() const { return _closure; }
	const std::vector<Value>& context() const { return _context; }
	std::vector<Value>& context() { return _context; }
	
private:
	std::shared_ptr<Node> _closure;
	std::vector<Value> _context;
};
