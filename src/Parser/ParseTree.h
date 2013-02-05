#pragma once
#include <DFG/NodeType.h>
#include <DFG/Node.h>
#include "SourceProperty.h"
#include <Passes/Value.h>

class ParseTree
{
public:
	ParseTree();
	~ParseTree();
	
	class ScopedElement;
	class Statement;
	class Scope;
	class Identifier;
	class Expression;
	class InlineStatement;
	class Constant;
	class Proposition;
	
	const Scope* topLevel() const { return _topLevel; }
	Scope* topLevel() { return _topLevel; }
	
	void print(std::wostream& out) const;
	
private:
	Scope* _topLevel;
};


class ParseTree::ScopedElement
{
public:
	virtual ~ScopedElement() {}
	
	Scope* parent() { return _parent; }
	ScopedElement& parent(Scope* value) { _parent = value; return *this; }
	
	int index() { return _index; }
	ScopedElement& index(int value) { _index = value; return *this; }
	
	ScopedElement* prevSibbling();
	ScopedElement* nextSibbling();
	
	virtual void print(std::wostream& out, int indentation = 0) const = 0;
	
protected:
	ScopedElement(): _parent(0), _index(0) { }
	Scope* _parent;
	int _index;
};

class ParseTree::Scope: public ScopedElement
{
public:
	Scope();
	virtual ~Scope();
	
	const std::vector<ScopedElement*>& children() const { return _children; }
	Scope& add(ScopedElement* value) { value->parent(this).index(_children.size()); _children.push_back(value); return *this; }
	
	ScopedElement* first() { return (_children.empty()) ? 0 : _children.front(); }
	ScopedElement* last() { return (_children.empty()) ? 0 : _children.back(); }
	
	virtual void print(std::wostream& out, int indentation = 0) const;
	
private:
	std::vector<ScopedElement*> _children;
};

class ParseTree::Expression
{
public:
	virtual ~Expression() { }
	
	ScopedElement* parent() { return _parent; }
	Expression& parent(ScopedElement* value) { _parent = value; return *this; }
	
	bool isOutbound() { return _parent && _index < 0; }
	bool isInbound() { return _parent && _index >= 0; }
	
	Expression* prevSibbling();
	Expression* nextSibbling();
	
	int index() { return _index; }
	Expression& index(int value) { _index = value; return *this; }
	
	virtual void print(std::wostream& out) const = 0;
	
protected:
	Expression(): _parent(0), _index(0) { }
	ScopedElement* _parent;
	int _index;
};

class ParseTree::Identifier: public Expression
{
public:
	Identifier(): Expression(), _name(), _source(), _bindingSite(0) { }
	virtual ~Identifier() { }
	
	const std::wstring& name() const { return _name; }
	Identifier& name(const std::wstring& value) { _name = value; return *this; }
	
	const SourceProperty& soureProperty() const { return _source; };
	Identifier& soureProperty(const SourceProperty& value) { _source = value; return *this; }
	
	Identifier* bindingSite() const { return _bindingSite; }
	Identifier& bindingSite(Identifier* value) { _bindingSite = value; return *this; }
	
	virtual void print(std::wostream& out) const;
	
private:
	std::wstring _name;
	SourceProperty _source;
	Identifier* _bindingSite;
};

class ParseTree::Constant: public Expression
{
public:
	Constant(Value value): _value(value) { }
	virtual ~Constant() { }
	
	Value value() const { return _value; }
	Constant& value(Value val) { _value = val; return *this; }
	
	virtual void print(std::wostream& out) const;
	
private:
	Value _value;
};

class ParseTree::Proposition: public ScopedElement
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
	
	Expression* condition() const { return _condition; }
	void setCondition(Expression* value) { value->parent(this).index(0); _condition = value; }
	
	virtual void print(std::wostream& out, int indentation = 0) const;
	
protected:
	Kind _kind;
	Expression* _condition;
};

class ParseTree::Statement: public ScopedElement
{
public:
	Statement();
	virtual ~Statement();
	
	NodeType type() const { return _type; }
	Statement& type(NodeType value) { _type = value; return *this; }
	
	const std::vector<Identifier*>& out() const { return _out; }
	const std::vector<Expression*>& in() const { return _in; }
	
	Expression* first();
	Expression* last();
	
	Statement& addOut(Identifier* value) { value->parent(this).index(-_out.size() - 1); _out.push_back(value); return *this; }
	Statement& addIn(Expression* value) { value->parent(this).index(_in.size()); _in.push_back(value); return *this; }
	
	virtual void print(std::wostream& out, int indentation = 0) const;
	
private:
	NodeType _type;
	vector<Identifier*> _out;
	vector<Expression*> _in;
};

class ParseTree::InlineStatement: public Expression, public Statement
{
public:
	virtual ~InlineStatement() { }
	
	virtual void print(std::wostream& out) const;
};
