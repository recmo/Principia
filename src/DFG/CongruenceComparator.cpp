#include "CongruenceComparator.h"
#include "Edge.h"
#include "Node.h"
#include <Parser/ConstantProperty.h>

#define debug true

bool CongruenceComparator::compare(const Edge* left, const Edge* right)
{
	return CongruenceComparator().compareEdges(left, right);
}

bool CongruenceComparator::compare(const Node* left, const Node* right)
{
	return CongruenceComparator().compareNodes(left, right);
}

bool CongruenceComparator::compare(const Value& left, const Value& right)
{
	return CongruenceComparator().compareValues(left, right);
}


CongruenceComparator::CongruenceComparator()
: recursionPairs()
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
	
	// Sourceless edges (for example constants)
	if(!left->source() || !right->source()) {
		if(left->source() != right->source())
			return false;
		if(left->has<ConstantProperty>() && right->has<ConstantProperty>())
			return compareValues(left->get<ConstantProperty>().value(), right->get<ConstantProperty>().value());
		if(debug)
			wcerr << L"Uncomparable edges detected, assuming different." << endl;
		return false;
	}
	
	// Same output in the defining node
	if(left->sourceIndex() != right->sourceIndex())
		return false;
	
	// Recursion pairs
	for(const std::pair<const Edge*, const Edge*> pair: recursionPairs)
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
	
	// Equivalence of type and arrity
	if(left->type() != right->type())
		return false;
	if(left->outArrity() != right->outArrity())
		return false;
	if(left->inArrity() != right->inArrity())
		return false;
	
	// Calls are structurally equal iff all their inputs are equal
	if(left->type() == NodeType::Call) {
		for(uint i = 0; i < left->inArrity(); ++i) {
			if(!compareEdges(left->in(i), right->in(i)))
				return false;
		}
		return true;
	}
	
	// Closures are equal iff their incoming edges are equal assuming their first outgoing edges are equal
	// The assumption amounts to stating that on recursion
	if(left->type() == NodeType::Closure) {
		
		// Push the equality of the parameters on the stack
		for(uint i = 0; i < left->outArrity(); ++i) {
			if(debug)
				wcerr << "Pushing " << left->out(i) << " ≡ " << right->out(i) << endl;
			std::pair<const Edge*, const Edge*> recursionPair;
			recursionPair.first = left->out(i);
			recursionPair.second = right->out(i);
			recursionPairs.push_back(recursionPair);
		}
		
		// Compare the output of the closure
		bool equal = true;
		for(uint i = 0; i < left->inArrity(); ++i) {
			if(!compareEdges(left->in(i), right->in(i))) {
				equal = false;
				break;
			}
		}
		
		// Pop the equality of the parameters
		for(uint i = 0; i < left->outArrity(); ++i) {
			if(debug)
				wcerr << "Poping " << left->out(i) << " ≡ " << right->out(i) << endl;
			recursionPairs.pop_back();
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
		wcerr << L"Uncomparable values detected, assuming different." << endl;
		return false;
	}
}
