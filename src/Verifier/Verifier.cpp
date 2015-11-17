#include <Verifier/Verifier.h>
#include "PropositionProperty.h"
#include <Passes/StackMachineProperty.h>
#include <DFG/CongruenceComparator.h>
#include <Parser/ConstantProperty.h>
#include <Parser/SourceProperty.h>

#define debug true

void Verifier::verify()
{
	for(auto node: _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		verify(node);
	}
}

/// @todo Verify arity of calls
/// @todo Verify totality of closures
void Verifier::verify(std::shared_ptr<Node> closure)
{
	_propositions.clear();
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
	for(auto edge: pp.preconditions())
		assume(edge);
	for(auto edge: pp.axioms())
		assume(edge);
	
	// Check all calls
	for(const StackMachineProperty::Instruction* instruction: smp.instructions()) {
		const StackMachineProperty::CallInstruction* call = dynamic_cast<const StackMachineProperty::CallInstruction*>(instruction);
		if(!call)
			continue;
		std::shared_ptr<Node> callNode = call->node();
		
		// Find the call's closure
		std::shared_ptr<Node> callClosure = callNode->in().front()->source();
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
		for(auto precondition: ppc.preconditions())
			verify(precondition, connections);
		
		// Assume the postconditions
		for(auto postcondition: ppc.postconditions())
			assume(postcondition, connections);
		
		// Check assertions on the call site
		if(callNode->has<PropositionProperty>())
			for(auto assertion: callNode->get<PropositionProperty>().assertions())
				verify(assertion);
	}
	
	// Check the postconditions
	for(auto postcondition: pp.postconditions())
		verify(postcondition);
}

void Verifier::assume(std::shared_ptr<Edge> statement, const Connections& connections)
{
	if(debug)
		wcerr << L"∴ " << statement << " " << connections << endl;
	_propositions.push_back(std::pair<std::shared_ptr<Edge>, Connections>(statement, connections));
}

void Verifier::verify(std::shared_ptr<Edge> statement, const Connections& connections)
{
	if(debug)
		wcerr << L"∵ " << statement << " " << connections << endl;
	for(auto assumption: _propositions) {
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
	wcerr << endl << "ERROR: Could not verify statement:" << endl;
	if(statement->has<SourceProperty>())
		statement->get<SourceProperty>().printCaret(wcerr);
	else
		wcerr << statement << endl;
	wcerr << endl;
}
