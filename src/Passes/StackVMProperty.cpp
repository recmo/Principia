#include "Passes/StackVMProperty.h"
#include <DFG/Node.h>
#include <DFG/Edge.h>

PropertyType StackVMProperty::classType = PropertyType::StackVM;

StackVMProperty::StackVMProperty(const StackVMProperty& copy)
: Property()
, _instructions()
{
	foreach(Instruction* instruction, copy._instructions)
		_instructions.push_back(instruction->clone());
}

StackVMProperty::~StackVMProperty()
{
	foreach(Instruction* instruction, _instructions)
		delete instruction;
}

void StackVMProperty::print(std::wostream& out) const
{
	out << L"Stack: " << _stack << endl;
	foreach(Instruction* instruction, _instructions)
		instruction->print(out);
}

void StackVMProperty::CallInstruction::print(std::wostream& out) const
{
	out << L"\t\tCall " << _node << L" " << _arguments << L" → " << _numReturns << endl;
}

void StackVMProperty::AllocateInstruction::print(std::wostream& out) const
{
	out << L"\t\tAlloc " << _closure << endl;
}

void StackVMProperty::StoreInstruction::print(std::wostream& out) const
{
	out << L"\t\tStore " << _closure << " " << _index << " " << _value << endl;
}

void StackVMProperty::ReturnInstruction::print(std::wostream& out) const
{
	out << L"\t\tReturn " << _returns << endl;
}
