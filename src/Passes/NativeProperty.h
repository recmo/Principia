#include <DFG/Property.h>
#include <fixups.h>
#include <Passes/Value.h>

class NativeProperty: public Property
{
public:
	typedef void (*Function)(const int64_t*, const int64_t*, int64_t*);
	
	NativeProperty(const NativeProperty& copy);
	NativeProperty(Function function, int numClosure, int numInputs, int numOutputs);
	virtual ~NativeProperty(); 
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	NativeProperty* clone() const { return new NativeProperty(*this); }
	
	virtual void print(std::wostream& out) const;
	
	Function function() const { return _function; }
	vector<Value> call(const vector<Value>& closure, const vector<Value>& inputs) const;
	vector<Value> call(const vector<Value>& inputs) const;
	
private:
	int _numClosure;
	int _numInputs;
	int _numOutputs;
	Function _function;
};
