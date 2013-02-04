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


ParseTree::StatementOrScope* ParseTree::StatementOrScope::nextSibbling()
{
	if(!_parent)
		return 0;
	int max = _parent->children().size() - 1;
	if(_index >= max)
		return 0;
	return _parent->children()[_index + 1];
}

ParseTree::StatementOrScope* ParseTree::StatementOrScope::prevSibbling()
{
	if(!_parent)
		return 0;
	if(_index <= 0)
		return 0;
	return _parent->children()[_index - 1];
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
	for(Proposition* prop: _propositions) {
		for(int i = 0; i < indentation; ++i)
			out << L"\t";
		prop->print(out);
		out << endl;
	}
	for(StatementOrScope* child: _children) {
		for(int i = 0; i < indentation; ++i)
			out << L"\t";
		child->print(out, indentation + 1);
		if(dynamic_cast<Statement*>(child))
			out << endl;
	}
}

void ParseTree::Proposition::print(std::wostream& out)
{
	switch(_kind) {
	case Precondition: out << L"∵"; break;
	case Postcondition: out << L"∴"; break;
	case Axiom: out << L"⊨"; break;
	case Assertion: out << L"⊢"; break;
	}
	out << " ";
	if(_condition)
		_condition->print(out);
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

ParseTree::Expression* ParseTree::Statement::first()
{
	if(!_out.empty())
		return _out.front();
	if(!_in.empty())
		return _in.front();
	return 0;
}

ParseTree::Expression* ParseTree::Statement::last()
{
	if(!_in.empty())
		return _in.back();
	if(!_out.empty())
		return _out.back();
	return 0;
}

ParseTree::Expression* ParseTree::Expression::nextSibbling()
{
	if(!_parent)
		return 0;
	if(isOutbound()) {
		int index = - _index - 1;
		int max = _parent->out().size() - 1;
		if(index >= max) {
			if(!_parent->in().empty())
				return _parent->in().front();
			return 0;
		}
		return _parent->out()[index + 1];
	} else {
		int max = _parent->in().size() - 1;
		if(_index >= max)
			return 0;
		return _parent->in()[_index + 1];
	}
}

ParseTree::Expression* ParseTree::Expression::prevSibbling()
{
	if(!_parent)
		return 0;
	if(isOutbound()) {
		int index = - _index - 1;
		if(index <= 0)
			return 0;
		return _parent->out()[index - 1];
	} else {
		if(_index <= 0) {
			if(!_parent->out().empty())
				return _parent->out().back();
			return 0;
		}
		return _parent->in()[_index - 1];
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
