#include "Passes/ClosureTree.h"

ClosureTree::ClosureTree(DataFlowGraph* program)
: _program(program)
, _parents(program->nodes().size())
{
	for(int i =0; i < _program->nodes().size(); ++i)
		_parents[i] = -1;
}

ClosureTree::~ClosureTree()
{
}

void ClosureTree::calculate()
{
	for(int i = 0; i < _program->nodes().size(); ++i) {
		if(_parents[i] != -1)
			continue;
		Node* node = _program->nodes()[i];
		if(node->type() != NodeType::Closure)
			continue;
		_parent = i;
		
		
		
		recurse(node);
	}
}

void ClosureTree::recurse(Node* node)
{
	for(int o = 0; o < node->outArrity();
}

