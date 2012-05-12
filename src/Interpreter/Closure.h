#pragma once
#include "fixups.h"
#include "Interpreter/Value.h"

class Node;
class Edge;

/// TODO: Reference counting garbage collection
class Closure
{
public:
	Closure(const Node* closure);
	Closure(const Node* closure, const std::vector<Value>& context);
	
	const Node* node() const { return _closure; }
	const std::vector<Value>& context() const { return _context; }
	std::vector<Value>& context() { return _context; }
	
private:
	const Node* _closure;
	std::vector<Value> _context;
};
