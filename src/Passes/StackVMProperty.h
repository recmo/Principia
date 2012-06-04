#pragma once
#include <DFG/Property.h>
#include <vector>
class Edge;
class Node;

class StackVMProperty: public Property
{
public:
	class Instruction;
	class CallInstruction;
	class AllocateInstruction;
	class StoreInstruction;
	class ReturnInstruction;
	
	StackVMProperty(): Property(), _stack(), _instructions() { }
	StackVMProperty(const std::vector<const Edge*>& stack, const std::vector<Instruction*>& order): Property(), _stack(stack), _instructions(order) { }
	StackVMProperty(const StackVMProperty& copy);
	virtual ~StackVMProperty();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual StackVMProperty* clone() const { return new StackVMProperty(*this); }
	
	virtual void print(std::wostream& out) const;
	
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

class StackVMProperty::Instruction
{
public:
	Instruction() { }
	virtual ~Instruction() { }
	virtual Instruction* clone() const = 0;
	virtual void print(std::wostream& out) const = 0;
};

class StackVMProperty::CallInstruction: public StackVMProperty::Instruction
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

class StackVMProperty::AllocateInstruction: public StackVMProperty::Instruction
{
public:
	AllocateInstruction(const Node* closure): _closure(closure) { }
	AllocateInstruction(const AllocateInstruction& other): _closure(other._closure) { }
	virtual ~AllocateInstruction() { }
	virtual AllocateInstruction* clone() const { return new AllocateInstruction(*this); }
	virtual void print(std::wostream& out) const;
	
	const Node* closure() const { return _closure; }
	
private:
	const Node* _closure;
};

class StackVMProperty::StoreInstruction: public StackVMProperty::Instruction
{
public:
	StoreInstruction(int closure, int index, int value): _closure(closure), _index(index), _value(value) { }
	StoreInstruction(const StoreInstruction& other) : _closure(other._closure), _index(other._index), _value(other._value) { }
	virtual ~StoreInstruction() { }
	virtual StoreInstruction* clone() const { return new StoreInstruction(*this); }
	virtual void print(std::wostream& out) const;
	
	int closure() const { return _closure; }
	int index() const { return _index; }
	int value() const { return _value; }
	
private:
	int _closure;
	int _index;
	int _value;
};

class StackVMProperty::ReturnInstruction: public StackVMProperty::Instruction
{
public:
	ReturnInstruction(): _returns() { }
	ReturnInstruction(const ReturnInstruction& other): _returns(other._returns) { }
	virtual ~ReturnInstruction() { }
	virtual ReturnInstruction* clone() const { return new ReturnInstruction(*this); }
	virtual void print(std::wostream& out) const;
	
	void addReturnValue(int value) { _returns.push_back(value); }
	const std::vector<int>& returns() const { return _returns; }
	
private:
	std::vector<int> _returns;
};

