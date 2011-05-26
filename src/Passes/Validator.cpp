#include "Passes/Validator.h"
#include <Parser/Identifier.h>

Validator::Validator(DataFlowGraph* program)
: _program(program)
, _preorderCounter(0)
, _componentCounter(0)
, _preorder(program->nodes().size())
, _component(program->nodes().size())
{
	for(int i = 0; i < _program->nodes().size(); ++i) {
		_preorder[i] = -1;
		_component[i] = -1;
	}
}

Validator::~Validator()
{
}

void Validator::validate()
{
	// http://en.wikipedia.org/wiki/Cheriyan–Mehlhorn/Gabow_algorithm
	depthFirstSearch();
}

int Validator::indexOf(Node* node)
{
	for(int i = 0; i < _program->nodes().size(); ++i)
		if(_program->nodes()[i] == node)
			return i;
	assert(!"Found");
}

void Validator::depthFirstSearch()
{
	for(int i = 0; i < _program->nodes().size(); ++i)
		if(_preorder[i] == -1)
			visit(i);
		
	wcerr << "Left: " << _unassigned << endl;
	wcerr << "Left: " << _undetermined << endl;
}

void Validator::visit(int i)
{
	wcerr << "Visit " << i << endl;
	wcerr << _preorder << endl;
	wcerr << _component << endl;
	wcerr << _unassigned << endl;
	wcerr << _undetermined << endl;
	
	// Set the preorder number of v to C, and increment C.
	_preorder[i] = _preorderCounter++;
	
	// Push v onto S and also onto P.
	_unassigned.push_back(i);
	_undetermined.push_back(i);
	
	// For each edge from v to a neighboring vertex w:
	Node* node = _program->nodes()[i];
	for(int j = 0; j < node->outArrity(); ++j) {
		Edge* out = node->out(j);
		
		//if(node->type() == NodeType::Closure && j == 0)
		//	continue;
		
		for(int k = 0; k < out->sinks().size(); ++k) {
			Node* child = out->sinks()[k];
			int child_index = indexOf(child);
			
			// If the preorder number of w has not yet been assigned, recursively search w;
			if(_preorder[child_index] == -1)
				visit(child_index);
			
			// Otherwise, if w has not yet been assigned to a strongly connected component:
			else if (_component[child_index] == -1) 
				
				// Repeatedly pop vertices from P until the top element of P has a preorder number less than or equal to the preorder number of w.
				while(_preorder[_undetermined.back()] > _preorder[child_index])
					_undetermined.pop_back();
		}
	}
	
	// If v is the top element of P:
	if(_undetermined.back() == i) {
		
		// Pop vertices from S until v has been popped, and assign the popped vertices to a new component.
		wcerr << _unassigned << endl;
		_component[i] = ++_componentCounter;
		while(_unassigned.back() != i) {
			assert(_unassigned.size() > 0);
			_component[_unassigned.back()] = _componentCounter;
			_unassigned.pop_back();
		}
		_unassigned.pop_back();
		
		// Pop v from P.
		_undetermined.pop_back();
	}
	
	assert(_preorder[i] != -1);
}

void Validator::visit(Node* node)
{
	if(node->type() == NodeType::Call)
		if(node->in(0)->has(PropertyType::Identifier))
			wcerr << node->in(0)->get<Identifier>().value();
		else
			wcerr << node;
	else if (node->type() == NodeType::Closure)
		if(node->out(0)->has(PropertyType::Identifier))
			wcerr << node->out(0)->get<Identifier>().value();
		else
			wcerr << node;
}

void Validator::print()
{
	for(int i = 0; i < _program->nodes().size(); ++i) {
		visit(_program->nodes()[i]);
		wcerr << "\t" << _preorder[i] << "\t" << _component[i] << endl;
	}
	for(int c = 1; c <= _componentCounter; ++c)
	{
		vector<Node> group;
		for(int i = 0; i < _program->nodes().size(); ++i) {
			if(_component[i] == c) {
				visit(_program->nodes()[i]);
				wcerr << " ";
			}
		}
		wcerr << endl;
	}
}

