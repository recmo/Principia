#include "CongruenceComparator.h"
#include "Edge.h"
#include "Node.h"
#include <Parser/ConstantProperty.h>

#define debug false

CongruenceComparator::CongruenceComparator()
: _connectLeft()
, _connectRight()
, _recursionPairs()
{
}

CongruenceComparator::~CongruenceComparator()
{
}

bool CongruenceComparator::compareEdges(const Edge* left, const Edge* right)
{
	if(debug)
		wcerr << L"Comparing " << left << L" ≡₁ " << right << endl;
	
	if(!left || !right)
		return false;
	if(left == right)
		return true;
	
	// Apply any edge mapping
	auto leftMap = _connectLeft.find(left);
	if(leftMap != _connectLeft.end())
		left = leftMap->second;
	auto rightMap = _connectRight.find(right);
	if(rightMap != _connectRight.end())
		right = rightMap->second;
	
	// Sourceless edges (for example constants)
	if(!left->source() || !right->source()) {
		if(left->source() != right->source())
			return false;
		if(left->has<ConstantProperty>() && right->has<ConstantProperty>())
			return compareValues(left->get<ConstantProperty>().value(), right->get<ConstantProperty>().value());
		if(debug)
			wcerr << L"Incomparable edges detected, assuming different." << endl;
		return false;
	}
	
	// Same output in the defining node
	if(left->sourceIndex() != right->sourceIndex())
		return false;
	
	// Recursion pairs
	for(const std::pair<const Edge*, const Edge*> pair: _recursionPairs)
		if(left == pair.first && right == pair.second)
			return true;
	
	// Compare the defining nodes
	return compareNodes(left->source(), right->source());
}

bool CongruenceComparator::compareNodes(const Node* left, const Node* right)
{
	if(debug)
		wcerr << L"Comparing " << left << L" ≡₂ " << right << endl;
	
	if(!left || !right)
		return false;
	if(left == right)
		return true;
	
	// Equivalence of type and arity
	if(left->type() != right->type())
		return false;
	if(left->outArity() != right->outArity())
		return false;
	if(left->inArity() != right->inArity())
		return false;
	
	// Calls are structurally equal iff all their inputs are equal
	if(left->type() == NodeType::Call) {
		for(uint i = 0; i < left->inArity(); ++i) {
			if(!compareEdges(left->in(i), right->in(i)))
				return false;
		}
		return true;
	}
	
	// Closures are equal iff their incoming edges are equal assuming their outgoing edges are equal
	if(left->type() == NodeType::Closure) {
		
		// Push the equality of the parameters on the stack
		for(uint i = 0; i < left->outArity(); ++i) {
			if(debug)
				wcerr << "Pushing " << left->out(i) << " ≡ " << right->out(i) << endl;
			std::pair<const Edge*, const Edge*> recursionPair;
			recursionPair.first = left->out(i);
			recursionPair.second = right->out(i);
			_recursionPairs.push_back(recursionPair);
		}
		
		// Compare the output of the closure
		bool equal = true;
		for(uint i = 0; i < left->inArity(); ++i) {
			if(!compareEdges(left->in(i), right->in(i))) {
				equal = false;
				break;
			}
		}
		
		// Pop the equality of the parameters
		for(uint i = 0; i < left->outArity(); ++i) {
			if(debug)
				wcerr << "Poping " << left->out(i) << " ≡ " << right->out(i) << endl;
			_recursionPairs.pop_back();
		}
		return equal;
	}
	
	throw L"Unimplemented node type";
}

bool CongruenceComparator::compareValues(const Value& left, const Value& right)
{
	if(debug)
		wcerr << L"Comparing " << left << L" ≡₃ " << right << endl;
	
	if(left.kind != right.kind)
		return false;
	switch(left.kind) {
	case Value::Integer: return left.integer() == right.integer();
	case Value::Real: return left.real() == right.real();
	case Value::String: return left.stringValue() == right.stringValue();
	case Value::Builtin: return left.builtin() == right.builtin();
	default:
		wcerr << L"Incomparable values detected, assuming different." << endl;
		return false;
	}
}
