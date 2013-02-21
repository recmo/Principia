#include <Verifier/Verifier.h>
#include "PropositionProperty.h"
#include <Passes/StackMachineProperty.h>
#include <DFG/CongruenceComparator.h>
#include <Parser/ConstantProperty.h>

#define debug true

void Verifier::verify()
{
	for(const Node* node: _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		verify(node);
	}
}

/// @todo Verify arity of calls
/// @todo Verify totality of closures
void Verifier::verify(const Node* closure)
{
	_assumptions.clear();
	assert(closure->type() == NodeType::Closure);
	if(!closure->has<PropositionProperty>()) {
		wcerr << "No logic for" << closure << endl;
		return;
	}
	assert(closure->has<StackMachineProperty>());
	const PropositionProperty& pp = closure->get<PropositionProperty>();
	const StackMachineProperty& smp = closure->get<StackMachineProperty>();
	
	if(debug)
		wcerr << endl << "Verifying " << closure << endl;
	
	// Assume all prepositions and axioms are true
	for(const Edge* edge: pp.preconditions())
		assume(edge);
	for(const Edge* edge: pp.axioms())
		assume(edge);
	
	// Check all calls
	for(const StackMachineProperty::Instruction* instruction: smp.instructions()) {
		const StackMachineProperty::CallInstruction* call = dynamic_cast<const StackMachineProperty::CallInstruction*>(instruction);
		if(!call)
			continue;
		const Node* callNode = call->node();
		
		// Find the call's closure
		const Node* callClosure = callNode->constIn().front()->source();
		if(!callClosure) {
			wcerr << "No closure for " << callNode << endl;
			continue;
		}
		if(!callClosure->has<PropositionProperty>()) {
			wcerr << "No logic for" << callNode << endl;
			continue;
		}
		
		if(debug)
			wcerr << "Checking call " << callNode << " to " << callClosure << endl;
		const PropositionProperty& ppc = callClosure->get<PropositionProperty>();
		
		// Connect the inputs and outputs
		Connections connections;
		for(uint i = 0; i < callNode->inArity(); ++i) {
			if(callNode->in(i) == callClosure->out(i))
				continue;
			connections[callClosure->out(i)] = callNode->in(i);
		}
		for(uint i = 0; i < callNode->outArity(); ++i) {
			if(callNode->out(i) == callClosure->in(i))
				continue;
			connections[callClosure->in(i)] = callNode->out(i);
		}
		
		// Verify the preconditions
		for(const Edge* precondition: ppc.preconditions())
			verify(precondition, connections);
		
		// Assume the postconditions
		for(const Edge* postcondition: ppc.postconditions())
			assume(postcondition, connections);
		
		// Check assertions on the call site
		if(callNode->has<PropositionProperty>())
			for(const Edge* assertion: callNode->get<PropositionProperty>().assertions())
				verify(assertion);
	}
	
	// Check the postconditions
	for(const Edge* postcondition: pp.postconditions())
		verify(postcondition);
}

void Verifier::assume(const Edge* statement, const Connections& connections)
{
	if(debug)
		wcerr << L"∴ " << statement << " " << connections << endl;
	_assumptions.push_back(std::pair<const Edge*, Connections>(statement, connections));
}

void Verifier::verify(const Edge* statement, const Connections& connections)
{
	if(debug)
		wcerr << L"∵ " << statement << " " << connections << endl;
	for(auto assumption: _assumptions) {
		if(debug)
			wcerr << "? " << assumption.first << endl;
		CongruenceComparator cc;
		cc.connectLeft(assumption.second);
		cc.connectRight(connections);
		if(cc.compareEdges(assumption.first, statement)) {
			if(debug)
				wcerr << "OK" << endl;
			return;
		}
	}
	wcerr << endl << "ERROR: Could not verify statement: " << statement << endl << endl;
}
