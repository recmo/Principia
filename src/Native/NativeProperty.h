#pragma once
#include <DFG/PropertyMap.h>
#include <fixups.h>
#include <Passes/Value.h>

class NativeProperty: public PropertyMap::Property
{
public:
	typedef void (*Function)(const int64_t*, const int64_t*, int64_t*);
	
	NativeProperty(const NativeProperty& copy);
	NativeProperty(Function function, int numClosure, int numInputs, int numOutputs);
	virtual ~NativeProperty(); 
	virtual void print(std::wostream& out) const override;
	
	Function function() const { return _function; }
	vector<Value> call(const vector<Value>& closure, const vector<Value>& inputs) const;
	vector<Value> call(const vector<Value>& inputs) const;
	
private:
	uint _numClosure;
	uint _numInputs;
	uint _numOutputs;
	Function _function;
};
