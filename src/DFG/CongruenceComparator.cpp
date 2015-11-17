#include "CongruenceComparator.h"
#include <DFG/Node.h>
#include <DFG/ConstantProperty.h>
#include <Unicode/exceptions.h>
#include <Utilities/exceptions.h>
#include <iostream>
using std::wcerr;
using std::endl;

#define debug false

bool CongruenceComparator::compareNode(const Node& left, const Node& right)
{
	if(debug)
		wcerr << L"Comparing " << left << L" ≡₁ " << right << endl;
	
	// Check trivial identity
	if(&left == &right)
		return true;
	
	// Check basic shape
	if(left.type() != right.type())
		return false;
	if(left.outArity() != right.outArity())
		return false;
	if(left.inArity() != right.inArity())
		return false;
	
	// Calls are structurally equal iff all their inputs are equal
	if(left.type() == Node::Call) {
		for(uint i = 0; i < left.inArity(); ++i) {
			if(!compareIn(left.in(i), right.in(i)))
				return false;
		}
		return true;
	}
	
	// Closures are equal iff their incoming edges are equal under the
	// assumption that their outgoing edges are equal.
	if(left.type() == Node::Closure) {
		
		// Push the equality of the outgoing edges on the stack
		for(uint i = 0; i < left.outArity(); ++i) {
			if(debug) {
				wcerr << "Pushing " << left.out(i);
				wcerr << " ≡ " << right.out(i) << endl;
			}
			_recursionPairs.push_back(std::make_pair(
				left.out(i).shared_from_this(),
				right.out(i).shared_from_this()));
		}
		
		// Compare the output of the closure
		bool equal = true;
		for(uint i = 0; i < left.inArity(); ++i) {
			if(!compareIn(left.in(i), right.in(i))) {
				equal = false;
				break;
			}
		}
		
		// Pop the equality of the parameters
		for(uint i = 0; i < left.outArity(); ++i) {
			if(debug)
				wcerr << "Popping " << left.out(i) << " ≡ " << right.out(i) << endl;
			const auto pair = std::make_pair(
				left.out(i).shared_from_this(),
				right.out(i).shared_from_this());
			assert(_recursionPairs.back() == pair);
			_recursionPairs.pop_back();
		}
		return equal;
	}

	// Invalid node type
	throw invalid_enum{};
}

bool CongruenceComparator::compareIn(const InPort& left, const InPort& right)
{
	// Apply any edge mapping
	/* TODO
	auto leftMap = _connectLeft.find(left);
	if(leftMap != _connectLeft.end())
		left = leftMap->second;
	auto rightMap = _connectRight.find(right);
	if(rightMap != _connectRight.end())
		right = rightMap->second;
	*/

	// Sourceless edges (for example constants)
	std::shared_ptr<OutPort> left_source = left.source();
	std::shared_ptr<OutPort> right_source = right.source();
	if(left_source == nullptr || right_source == nullptr) {
		if(left_source != right_source)
			return false;
		if(left.has<ConstantProperty>() && right.has<ConstantProperty>()) {
			return compareValues(
				left.get<ConstantProperty>().value(),
				right.get<ConstantProperty>().value());
		}
		throw unimplemented{};
	}
	
	// Compare the sources
	assert(left_source != nullptr);
	assert(right_source != nullptr);
	const bool equal = compareOut(*left_source, *right_source);
	return equal;
}

bool CongruenceComparator::compareOut(const OutPort& left, const OutPort& right)
{
	// Recursion pairs
	const OutPair pair{left.shared_from_this(), right.shared_from_this()};
	if(contains(_recursionPairs, pair))
		return true;
	
	// Same output in the defining node
	if(left.index() != right.index())
		return false;
	
	// Compare the defining nodes
	return compareNode(left.parent(), right.parent());
}

bool CongruenceComparator::compareValues(const Value& left, const Value& right)
{
	if(debug)
		wcerr << L"Comparing " << left << L" ≡₃ " << right << endl;
	return left == right;
}
