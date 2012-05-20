#include <Parser/ParseTree.h>

ParseTree::ParseTree()
: _topLevel(new Scope)
{
}

ParseTree::~ParseTree()
{
	delete _topLevel;
}

void ParseTree::print(std::wostream& out)
{
	_topLevel->print(out);
}

ParseTree::Scope::Scope()
: _children()
{
}

ParseTree::Scope::~Scope()
{
	foreach(StatementOrScope* child, _children)
		delete child;
}

void ParseTree::Scope::print(std::wostream& out, int indentation)
{
	foreach(StatementOrScope* child, _children) {
		for(int i = 0; i < indentation; ++i)
			out << L"\t";
		child->print(out, indentation + 1);
		if(dynamic_cast<Statement*>(child))
			out << endl;
	}
}


ParseTree::Statement::Statement()
: _type(-1)
, _out()
, _in()
{
}

ParseTree::Statement::~Statement()
{
	foreach(Identifier* child, _out)
		delete child;
	foreach(Expression* child, _in)
		delete child;
}

void ParseTree::Statement::print(std::wostream& out, int indentation)
{
	foreach(Identifier* child, _out) {
		child->print(out);
		out << L" ";
	}
	out << ((_type == NodeType::Call) ? L"≔ " : L"↦ ");
	foreach(Expression* child, _in) {
		child->print(out);
		if(child != _in.back())
			out << L" ";
	}
}

void ParseTree::InlineStatement::print(std::wostream& out)
{
	out << "(";
	Statement::print(out);
	out << ")";
}

void ParseTree::Constant::print(std::wostream& out)
{
	out << _value;
}

void ParseTree::Identifier::print(std::wostream& out)
{
	out << _name;
}
