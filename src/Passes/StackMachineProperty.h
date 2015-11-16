#pragma once
#include <DFG/PropertyMap.h>
#include <vector>
class Edge;
class Node;

class StackMachineProperty: public PropertyMap::Property
{
public:
	class Instruction;
	class CallInstruction;
	class AllocateInstruction;
	class StoreInstruction;
	class ReturnInstruction;
	
	StackMachineProperty(): Property(), _stack(), _instructions() { }
	StackMachineProperty(const std::vector<const Edge*>& stack, const std::vector<Instruction*>& order): Property(), _stack(stack), _instructions(order) { }
	StackMachineProperty(const StackMachineProperty& copy);
	virtual ~StackMachineProperty();
	virtual void print(std::wostream& out) const override;
	
	std::vector<const Edge*>& stack() { return _stack; }
	void stack(const std::vector<const Edge*>& value) { _stack = value; }
	const std::vector<const Edge*>& stack() const { return _stack; }
	
	std::vector<Instruction*>& instructions() { return _instructions; }
	void instructions(const std::vector<Instruction*>& value) { _instructions = value; }
	const std::vector<Instruction*>& instructions() const { return _instructions; }

private:
	std::vector<const Edge*> _stack;
	std::vector<Instruction*> _instructions;
};

class StackMachineProperty::Instruction
{
public:
	Instruction() { }
	virtual ~Instruction() { }
	virtual Instruction* clone() const = 0;
	virtual void print(std::wostream& out) const = 0;
};

inline std::wostream& operator<<(std::wostream& out, const StackMachineProperty::Instruction& inst)
{
	inst.print(out);
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const StackMachineProperty::Instruction* inst)
{
	if(inst)
		inst->print(out);
	else
		out << L"null";
	return out;
}

class StackMachineProperty::CallInstruction: public StackMachineProperty::Instruction
{
public:
	CallInstruction(const Node* node): _node(node), _arguments() { }
	CallInstruction(const CallInstruction& other): _node(other._node), _arguments(other._arguments), _numReturns(other._numReturns) { }
	virtual ~CallInstruction() { }
	virtual CallInstruction* clone() const { return new CallInstruction(*this); }
	virtual void print(std::wostream& out) const;
	
	const Node* node() const { return _node; }
	
	void addArgument(int value) { _arguments.push_back(value); }
	const std::vector<int>& arguments() const { return _arguments; }
	
	int numReturns() const { return _numReturns; }
	void numReturns(int value) { _numReturns = value; }
	
private:
	const Node* _node;
	std::vector<int> _arguments;
	int _numReturns;
};

class StackMachineProperty::AllocateInstruction: public StackMachineProperty::Instruction
{
public:
	AllocateInstruction(Node* closure): _closure(closure) { }
	AllocateInstruction(const AllocateInstruction& other): _closure(other._closure) { }
	virtual ~AllocateInstruction() { }
	virtual AllocateInstruction* clone() const { return new AllocateInstruction(*this); }
	virtual void print(std::wostream& out) const;
	
	Node* closure() const { return _closure; }
	
private:
	Node* _closure;
};

class StackMachineProperty::StoreInstruction: public StackMachineProperty::Instruction
{
public:
	StoreInstruction(int closure, int index, int value): _closure(closure), _slot(index), _value(value) { }
	StoreInstruction(const StoreInstruction& other) : _closure(other._closure), _slot(other._slot), _value(other._value) { }
	virtual ~StoreInstruction() { }
	virtual StoreInstruction* clone() const { return new StoreInstruction(*this); }
	virtual void print(std::wostream& out) const;
	
	int closure() const { return _closure; }
	int slot() const { return _slot; }
	int value() const { return _value; }
	
private:
	int _closure;
	int _slot;
	int _value;
};

class StackMachineProperty::ReturnInstruction: public StackMachineProperty::Instruction
{
public:
	ReturnInstruction(const Node* closure): _closure(closure), _returns() { }
	ReturnInstruction(const ReturnInstruction& other): _closure(other._closure), _returns(other._returns) { }
	virtual ~ReturnInstruction() { }
	virtual ReturnInstruction* clone() const { return new ReturnInstruction(*this); }
	virtual void print(std::wostream& out) const;
	
	const Node* closure() const { return _closure; }
	
	void addReturnValue(int value) { _returns.push_back(value); }
	const std::vector<int>& returns() const { return _returns; }
	
private:
	const Node* _closure;
	std::vector<int> _returns;
};

