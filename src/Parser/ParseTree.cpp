#include <Parser/ParseTree.h>

ParseTree::ParseTree()
: _top(new Scope)
{
}

ParseTree::~ParseTree()
{
	delete _top;
}

void ParseTree::print(std::wostream& out) const
{
	_top->print(out);
}

void ParseTree::uniqueifyNames()
{
	std::map<string, uint> idCounter;
	std::function<void(Node*)> recurse = [&](Node* node) {
		// Fix up identifiers
		if(node->isA<Identifier>()) {
			Identifier* id = node->to<Identifier>();
			if(id->outbound()) {
				string name = id->name();
				uint count = idCounter[name]++;
				if(count) {
					wchar number = '0' + count; /// @todo
					name.append(&number, 1);
					id->name(name);
				}
			}
		}
		
		for(Node* child: node->children())
			recurse(child);
	};
	recurse(_top);
}

ParseTree::Node::Node()
: _parent(nullptr)
, _indexInParent(-1)
, _children()
{
}

ParseTree::Node::~Node()
{
	for(Node* child: _children)
		delete child;
}

ParseTree::Node* ParseTree::Node::prevSibbling() const
{
	if(!_parent)
		return nullptr;
	if(_indexInParent == 0)
		return nullptr;
	return _parent->child(_indexInParent - 1);
}

ParseTree::Node* ParseTree::Node::nextSibbling() const
{
	if(!_parent)
		return nullptr;
	if(_indexInParent + 1 == _parent->numChildren())
		return nullptr;
	return _parent->child(_indexInParent + 1);
}

ParseTree::Node* ParseTree::Node::firstChild() const
{
	if(_children.empty())
		return nullptr;
	return _children.front();
}

ParseTree::Node* ParseTree::Node::lastChild() const
{
	if(_children.empty())
		return nullptr;
	return _children.back();
}

ParseTree::Node* ParseTree::Node::child(uint index) const
{
	assert(index < _children.size());
	return _children[index];
}

void ParseTree::Node::appendChild(ParseTree::Node* child)
{
	assert(child != nullptr);
	assert(child->_parent == nullptr);
	child->_indexInParent = _children.size();
	child->_parent = this;
	_children.push_back(child);
}

void ParseTree::Node::insertChild(ParseTree::Node* child, uint position)
{
	assert(child != nullptr);
	assert(child->_parent == nullptr);
	assert(position <= _children.size());
	child->_parent = this;
	_children.insert(_children.begin() + position, child);
	for(uint i = position; i < _children.size(); ++i)
		_children[i]->_indexInParent = i;
}

bool ParseTree::Node::validate() const
{
	if(parent() && parent()->child(_indexInParent) != this)
			return false;
	for(Node* child: _children) {
		if(child->parent() != this)
			return false;
		if(!child->validate())
			return false;
	}
	return true;
}

ParseTree::Scope* ParseTree::Node::enclosingScope() const
{
	if(!_parent)
		return nullptr;
	if(_parent->isA<Scope>())
		return _parent->to<Scope>();
	return _parent->enclosingScope();
}

void ParseTree::Scope::print(std::wostream& out, uint indentation) const
{
	for(Node* child: children()) {
		if(!child->isA<Scope>())
			for(uint i = 0; i < indentation; ++i)
				out << L"\t";
		child->print(out, indentation + 1);
		if(!child->isA<Scope>())
			out << endl;
	}
}

ParseTree::Statement* ParseTree::Scope::associatedStatement() const
{
	ParseTree::Node* prev = prevSibbling();
	if(!prev)
		return nullptr;
	if(!prev->isA<Statement>())
		return nullptr;
	return prev->to<Statement>();
}

void ParseTree::Proposition::print(std::wostream& out, uint /*indentation*/) const
{
	switch(_kind) {
	case Precondition: out << L"∵"; break;
	case Postcondition: out << L"∴"; break;
	case Axiom: out << L"⊨"; break;
	case Assertion: out << L"⊢"; break;
	}
	out << " ";
	if(firstChild())
		firstChild()->print(out);
}

bool ParseTree::Statement::isInline() const
{
	if(!parent())
		return false;
	return !parent()->isA<Scope>();
}

void ParseTree::Statement::print(std::wostream& out, uint indentation) const
{
	if(isInline())
		out << "(";
	bool outbounds = true;
	for(Node* child: children()) {
		if(outbounds && !(child->isA<Identifier>() && child->to<Identifier>()->outbound())) {
			out << ((_type == NodeType::Call) ? L"≔ " : L"↦ ");
			outbounds = false;
		}
		child->print(out, indentation);
		if(child != lastChild())
			out << " ";
	}
	if(isInline())
		out << ")";
}

void ParseTree::Constant::print(std::wostream& out, uint /*indentation*/) const
{
	out << _value;
}

void ParseTree::Identifier::print(std::wostream& out, uint /*indentation*/) const
{
	if(bindingSite())
		out << bindingSite();
	else
		out << _name;
}

ParseTree::Statement& ParseTree::Statement::addOut(ParseTree::Identifier* value)
{
	assert(value != nullptr);
	appendChild(value);
	value->setOutbound();
	return *this;
}

ParseTree::Statement& ParseTree::Statement::addIn(ParseTree::Node* value)
{
	assert(value != nullptr);
	appendChild(value);
	return *this;
}

std::vector<ParseTree::Identifier*> ParseTree::Statement::out() const
{
	std::vector<ParseTree::Identifier*> result;
	for(Node* child: children())
		if(child->isA<ParseTree::Identifier>() && child->to<ParseTree::Identifier>()->outbound())
			result.push_back(child->to<ParseTree::Identifier>());
	return result;
}

std::vector<ParseTree::Node*> ParseTree::Statement::in() const
{
	std::vector<ParseTree::Node*> result;
	for(Node* child: children()) {
		if(child->isA<ParseTree::Identifier>() && child->to<ParseTree::Identifier>()->outbound())
			continue;
		result.push_back(child);
	}
	return result;
}
