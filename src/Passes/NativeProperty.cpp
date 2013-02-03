#include "Passes/NativeProperty.h"

PropertyType NativeProperty::classType = PropertyType::Native;

NativeProperty::NativeProperty(NativeProperty::Function function, int numClosure, int numInputs, int numOutputs)
: _numClosure(numClosure)
, _numInputs(numInputs)
, _numOutputs(numOutputs)
, _function(function)
{
}

NativeProperty::NativeProperty(const NativeProperty& copy)
: _numClosure(copy._numClosure)
, _numInputs(copy._numInputs)
, _numOutputs(copy._numOutputs)
, _function(copy._function)
{
}

NativeProperty::~NativeProperty()
{
}

vector<Value> NativeProperty::call(const vector<Value>& inputs) const
{
	return call(vector<Value>(), inputs);
}

vector<Value> NativeProperty::call(const vector<Value>& closure, const vector<Value>& inputs) const
{
	if(closure.size() != _numClosure || inputs.size() != _numInputs)
		wcerr << endl << *this << L" = " << closure.size() << " " << inputs.size() << " " << _numOutputs << endl;
	assert(closure.size() == _numClosure);
	assert(inputs.size() == _numInputs);
	vector<int64_t> closureVec;
	foreach(const Value& value, closure)
		closureVec.push_back(value.integer());
	vector<int64_t> inputVec;
	foreach(const Value& value, inputs)
		inputVec.push_back(value.integer());
	vector<int64_t> outputVec;
	outputVec.resize(_numOutputs);
	_function(0, inputVec.data(), outputVec.data());
	vector<Value> outputs;
	foreach(int64_t value, outputVec)
		outputs.push_back(value);
	return outputs;
}

void NativeProperty::print(std::wostream& out) const
{
	out << L"has native " << _numClosure << " " << _numInputs << " " << _numOutputs;
}
