#pragma once
#include <DFG/NodeType.h>
#include <DFG/Node.h>
#include "SourceProperty.h"
#include <Interpreter/Value.h>

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
	
	virtual void print(std::wostream& out, int indentation = 0) = 0;
	
protected:
	StatementOrScope() { }
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
	
	Statement& addOut(Identifier* value) { _out.push_back(value); return *this; }
	Statement& addIn(Expression* value) { _in.push_back(value); return *this; }
	
	virtual void print(std::wostream& out, int indentation = 0);
	
private:
	NodeType _type;
	vector<Identifier*> _out;
	vector<Expression*> _in;
};

class ParseTree::Scope: public StatementOrScope
{
public:
	Scope();
	virtual ~Scope();
	
	const std::vector<StatementOrScope*>& children() const { return _children; }
	Scope& add(StatementOrScope* value) { _children.push_back(value); return *this; }
	
	virtual void print(std::wostream& out, int indentation = 0);
	
private:
	std::vector<StatementOrScope*> _children;
};

class ParseTree::Expression
{
public:
	virtual ~Expression() { }
	
	virtual void print(std::wostream& out) = 0;
	
protected:
	Expression() { }
};

class ParseTree::Identifier: public Expression
{
public:
	virtual ~Identifier() { }
	
	const std::wstring& name() const { return _name; }
	Identifier& name(const std::wstring& value) { _name = value; return *this; }
	
	const SourceProperty& soureProperty() const { return _source; };
	Identifier& soureProperty(const SourceProperty& value) { _source = value; return *this; }
	
	virtual void print(std::wostream& out);
	
private:
	std::wstring _name;
	SourceProperty _source;
};

class ParseTree::InlineStatement: public Expression, public Statement
{
public:
	virtual ~InlineStatement() { }
	
	virtual void print(std::wostream& out);
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
