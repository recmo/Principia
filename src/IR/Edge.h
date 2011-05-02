#pragma once
#include "fixups.h"
#include "BindingType.h"
#include "CallNode.h"
#include "ClosureNode.h"

class Edge
{
public:
	BindingType bindingType() const { return _bindingType; }
	CallNode* callNode() const;
	ClosureNode* closureNode() const;
	
	void setExternal();
	void setReturnedBy(CallNode* call, uint position);
	void setFunctionOf(ClosureNode* closure);
	void setArgumentOf(ClosureNode* closure, uint position);
	
private:
	BindingType _bindingType;
	uint _position;
	union
	{
		CallNode* _callNode;
		ClosureNode* _closureNode;
	};
};

inline CallNode* Edge::callNode() const
{
	assert(bindingType() == BindingType::Return);
	return _callNode;
}

inline ClosureNode* Edge::closureNode() const
{
	assert(bindingType() == BindingType::Closure || bindingType() == BindingType::Argument);
	return _closureNode;
}
