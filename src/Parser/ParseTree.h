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
	
	class StatementOrScope;
	class Statement;
	class Scope;
	class Identifier;
	class Expression;
	class InlineStatement;
	class Constant;
	class Proposition;
	
	const Scope* topLevel() const { return _topLevel; }
	Scope* topLevel() { return _topLevel; }
	
	void print(std::wostream& out);
	
private:
	Scope* _topLevel;
};


class ParseTree::StatementOrScope
{
public:
	virtual ~StatementOrScope() {}
	
	Scope* parent() { return _parent; }
	StatementOrScope& parent(Scope* value) { _parent = value; return *this; }
	
	int index() { return _index; }
	StatementOrScope& index(int value) { _index = value; return *this; }
	
	StatementOrScope* prevSibbling();
	StatementOrScope* nextSibbling();
	
	virtual void print(std::wostream& out, int indentation = 0) = 0;
	
protected:
	StatementOrScope(): _parent(0), _index(0) { }
	Scope* _parent;
	int _index;
};

class ParseTree::Scope: public StatementOrScope
{
public:
	Scope();
	virtual ~Scope();
	
	const std::vector<StatementOrScope*>& children() const { return _children; }
	Scope& add(StatementOrScope* value) { value->parent(this).index(_children.size()); _children.push_back(value); return *this; }
	Scope& add(Proposition* value) { _propositions.push_back(value); return *this; }
	
	StatementOrScope* first() { return (_children.empty()) ? 0 : _children.front(); }
	StatementOrScope* last() { return (_children.empty()) ? 0 : _children.back(); }
	
	virtual void print(std::wostream& out, int indentation = 0);
	
private:
	std::vector<StatementOrScope*> _children;
	std::vector<Proposition*> _propositions;
};

class ParseTree::Proposition
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
	void setCondition(Expression* value) { _condition = value; }
	
	virtual void print(std::wostream& out);
	
protected:
	Kind _kind;
	Expression* _condition;
};

class ParseTree::Expression
{
public:
	virtual ~Expression() { }
	
	Statement* parent() { return _parent; }
	Expression& parent(Statement* value) { _parent = value; return *this; }
	
	bool isOutbound() { return _parent && _index < 0; }
	bool isInbound() { return _parent && _index >= 0; }
	
	Expression* prevSibbling();
	Expression* nextSibbling();
	
	int index() { return _index; }
	Expression& index(int value) { _index = value; return *this; }
	
	virtual void print(std::wostream& out) = 0;
	
protected:
	Expression(): _parent(0), _index(0) { }
	Statement* _parent;
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
	
	virtual void print(std::wostream& out);
	
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
	
	virtual void print(std::wostream& out);
	
private:
	Value _value;
};

class ParseTree::Statement: public StatementOrScope
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
	
	virtual void print(std::wostream& out, int indentation = 0);
	
private:
	NodeType _type;
	vector<Identifier*> _out;
	vector<Expression*> _in;
};

class ParseTree::InlineStatement: public Expression, public Statement
{
public:
	virtual ~InlineStatement() { }
	
	virtual void print(std::wostream& out);
};
