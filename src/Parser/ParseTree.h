#pragma once
#include <DFG/NodeType.h>
#include <DFG/Node.h>
#include "SourceProperty.h"
#include <Passes/Value.h>

/// @todo refactor

class ParseTree
{
public:
	ParseTree();
	~ParseTree();
	
	class Node;
	class Statement;
	class Scope;
	class Identifier;
	class Constant;
	class Proposition;
	
	const Scope* top() const { return _top; }
	Scope* top() { return _top; }
	
	void print(std::wostream& out) const;
	bool validate() const;
	
private:
	Scope* _top;
};

class ParseTree::Node
{
public:
	virtual ~Node();
	
	virtual void print(std::wostream& out, uint indentation = 0) const = 0;
	virtual bool validate() const;
	
	Node* parent() const { return _parent; }
	uint indexInParent() const { return _indexInParent; }
	Node* prevSibbling() const;
	Node* nextSibbling() const;
	Node* firstChild() const;
	Node* lastChild() const;
	uint numChildren() const { return _children.size(); }
	Node* child(uint index) const;
	const std::vector<Node*>& children() const { return _children; }
	
	void appendChild(Node* child);
	void insertChild(Node* child, uint position);
	
	template<class T>
	bool isA() const { return dynamic_cast<const T*>(this) != nullptr; }
	
	template<class T>
	T* to() { return dynamic_cast<T*>(this); }
	
	template<class T>
	const T* to() const { return dynamic_cast<T*>(this); }
	
protected:
	Node();
	Node* _parent;
	uint _indexInParent;
	std::vector<Node*> _children;
};

class ParseTree::Scope: public Node
{
public:
	Scope() { }
	virtual ~Scope() { }
	virtual void print(std::wostream& out, uint indentation = 0) const;
};

class ParseTree::Identifier: public Node
{
public:
	Identifier(): _name(), _outbound(false), _source(), _bindingSite(nullptr) { }
	virtual ~Identifier() { }
	
	const std::wstring& name() const { return _name; }
	Identifier& name(const std::wstring& value) { _name = value; return *this; }
	
	bool outbound() const { return _outbound; }
	bool inbound() const { return !_outbound; }
	Identifier& setOutbound() { _outbound = true; return *this; }
	Identifier& setInbound() { _outbound = true; return *this; }
	
	const SourceProperty& soureProperty() const { return _source; };
	Identifier& soureProperty(const SourceProperty& value) { _source = value; return *this; }
	
	Identifier* bindingSite() const { return _bindingSite; }
	Identifier& bindingSite(Identifier* value) { _bindingSite = value; return *this; }
	
	virtual void print(std::wostream& out, uint indentation = 0) const;
	
private:
	std::wstring _name;
	bool _outbound;
	SourceProperty _source;
	Identifier* _bindingSite;
};

class ParseTree::Constant: public Node
{
public:
	Constant(Value value): _value(value) { }
	virtual ~Constant() { }
	
	Value value() const { return _value; }
	Constant& value(Value val) { _value = val; return *this; }
	
	virtual void print(std::wostream& out, uint indentation = 0) const;
	
private:
	Value _value;
};

class ParseTree::Proposition: public Node
{
public:
	enum Kind {
		Precondition,
		Postcondition,
		Axiom,
		Assertion
	};
	
	Proposition(Kind kind): _kind(kind) { }
	virtual ~Proposition() { }
	virtual void print(std::wostream& out, uint indentation = 0) const;
	
protected:
	Kind _kind;
};

class ParseTree::Statement: public Node
{
public:
	Statement(): _type(NodeType::Call), _outArrity(0) { }
	virtual ~Statement() { }
	
	
	NodeType type() const { return _type; }
	Statement& type(NodeType value) { _type = value; return *this; }
	
	const std::vector<Identifier*>& out() const;
	   std::vector< Node* > in() const;
	
	Statement& addOut(Identifier* value);
	Statement& addIn(Node* value);
	
	bool isInline() const;
	
	virtual void print(std::wostream& out, uint indentation = 0) const;
	
private:
	NodeType _type;
	uint _outArrity;
};

inline std::wostream& operator<<(std::wostream& out, const ParseTree::Node& node)
{
	node.print(out);
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const ParseTree::Node* node)
{
	if(node)
		node->print(out);
	else
		out << L"<null>";
	return out;
}
