#include "CongruenceComparator.h"
#include <DFG/Node.h>
#include <DFG/ConstantProperty.h>
#include <Unicode/exceptions.h>
#include <iostream>
using std::wcerr;
using std::endl;

#define debug false
/*

bool CongruenceComparator::compareEdges(
	std::shared_ptr<Edge> left, std::shared_ptr<Edge> right)
{
	if(debug)
		wcerr << L"Comparing " << left << L" ≡₁ " << right << endl;
	
	if(left == nullptr || right == nullptr)
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
	if(left->source() == nullptr || right->source() == nullptr ) {
		if(left->source() != right->source())
			return false;
		if(left->has<ConstantProperty>() && right->has<ConstantProperty>()) {
			return compareValues(
				left->get<ConstantProperty>().value(),
				right->get<ConstantProperty>().value());
		}
		throw runtime_error(L"Unimplemented");
	}
	
	// Same output in the defining node
	if(left->index() != right->index())
		return false;
	
	// Recursion pairs
	if(contains(_recursionPairs, std::make_pair(left, right)))
		return true;
	
	// Compare the defining nodes
	return compareNodes(left->source(), right->source());
}

bool CongruenceComparator::compareNodes(
	std::shared_ptr<Node> left, std::shared_ptr<Node> right)
{
	if(debug)
		wcerr << L"Comparing " << left << L" ≡₂ " << right << endl;
	
	if(left == nullptr|| right == nullptr)
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
	if(left->type() == Node::Call) {
		for(uint i = 0; i < left->inArity(); ++i) {
			if(!compareEdges(left->in(i), right->in(i)))
				return false;
		}
		return true;
	}
	
	// Closures are equal iff their incoming edges are equal under the
	// assumption that their outgoing edges are equal.
	if(left->type() == Node::Closure) {
		
		// Push the equality of the outgoing edges on the stack
		for(uint i = 0; i < left->outArity(); ++i) {
			if(debug) {
				wcerr << "Pushing " << left->out(i);
				wcerr << " ≡ " << right->out(i) << endl;
			}
			_recursionPairs.push_back(std::make_pair(left->out(i), right->out(i)));
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
			const auto pair = std::make_pair(left->out(i), right->out(i));
			assert(_recursionPairs.back() == pair);
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
	return left == right;
}
*/
