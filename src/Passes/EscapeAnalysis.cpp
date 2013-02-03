#include <Passes/EscapeAnalysis.h>
#include <Passes/StackMachineProperty.h>
#include <Passes/EscapeProperty.h>

#define debug false

void EscapeAnalysis::analyse()
{
	foreach(Node* node, _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		analyse(node);
	}
}

void EscapeAnalysis::analyse(Node* closure)
{
	foreach(StackMachineProperty::Instruction* instruction, closure->get<StackMachineProperty>().instructions()) {
		StackMachineProperty::AllocateInstruction* alloc = dynamic_cast<StackMachineProperty::AllocateInstruction*>(instruction);
		if(!alloc)
			continue;
		const Edge* allocEdge = alloc->closure()->out(0);
		if(debug) {
			wcerr << "Does " << allocEdge << " escape " << closure << " ?" << endl;
			wcerr << "Stack " << closure->get<StackMachineProperty>().stack() << endl;
		}
		
		int closureStackPos = indexOf(closure->get<StackMachineProperty>().stack(), allocEdge);
		bool escapes = false;
		foreach(StackMachineProperty::Instruction* instruction, closure->get<StackMachineProperty>().instructions()) {
			if(StackMachineProperty::CallInstruction* call = dynamic_cast<StackMachineProperty::CallInstruction*>(instruction)) {
				/// Anywhere but the first argument (which is the function to be called)
				/// TODO: Functions returning themselves.
				for(int i = 1; i < call->arguments().size(); ++i) {
					if(call->arguments()[i] == closureStackPos) {
						if(debug)
							wcerr << "May escape through " << call << endl;
						escapes = true;
						break;
					}
				}
			}
			if(StackMachineProperty::StoreInstruction* store = dynamic_cast<StackMachineProperty::StoreInstruction*>(instruction)) {
				if(store->value() == closureStackPos) {
					if(debug)
						wcerr << "May escape through " << store << endl;
					escapes = true;
				}
			}
			if(StackMachineProperty::ReturnInstruction* ret = dynamic_cast<StackMachineProperty::ReturnInstruction*>(instruction)) {
				foreach(int stackPos, ret->returns()) {
					if(stackPos == closureStackPos) {
						if(debug)
							wcerr << "May escape through " << ret << endl;
						escapes = true;
					}
				}
			}
			if(escapes)
				break;
		}
		
		alloc->closure()->set(EscapeProperty(escapes));
	}
}
