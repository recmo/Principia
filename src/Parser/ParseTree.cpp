#include <Parser/ParseTree.h>
#include <algorithm>
#include <functional>

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

void ParseTree::Node::insertAfter(ParseTree::Node* child, ParseTree::Node* newChild)
{
	assert(child != nullptr);
	insertChild(newChild, child->_indexInParent + 1);
}

void ParseTree::Node::insertBefore(ParseTree::Node* child, ParseTree::Node* newChild)
{
	assert(child != nullptr);
	insertChild(newChild, child->_indexInParent);
}

void ParseTree::Node::removeChild(uint position)
{
	assert(position < _children.size());
	ParseTree::Node* child = _children[position];
	assert(child->_parent == this);
	_children.erase(_children.begin() + position);
	for(uint i = position; i < _children.size(); ++i)
		_children[i]->_indexInParent = i;
	child->_parent = nullptr;
	child->_indexInParent = 0;
}

void ParseTree::Node::removeChild(ParseTree::Node* child)
{
	assert(child != nullptr);
	assert(child->_parent == this);
	auto i = std::find(_children.begin(), _children.end(), child);
	assert(i != _children.end());
	_children.erase(i);
	for(uint i = 0; i < _children.size(); ++i)
		_children[i]->_indexInParent = i;
	child->_parent = nullptr;
	child->_indexInParent = 0;
}

void ParseTree::Node::swapChild(uint position, ParseTree::Node* newChild)
{
	assert(newChild != nullptr);
	assert(newChild->_parent == nullptr);
	assert(position < _children.size());
	_children[position]->_parent = nullptr;
	_children[position]->_indexInParent = 0;
	_children[position] = newChild;
	_children[position]->_parent = this;
	_children[position]->_indexInParent = position;
}

void ParseTree::Node::swapChild(ParseTree::Node* currentChild, ParseTree::Node* newChild)
{
	assert(currentChild != nullptr);
	assert(currentChild->_parent == this);
	assert(newChild != nullptr);
	assert(newChild->_parent == nullptr);
	auto i = std::find(_children.begin(), _children.end(), currentChild);
	assert(i != _children.end());
	*i = newChild;
	newChild->_parent = this;
	newChild->_indexInParent = i - _children.begin();
	currentChild->_parent = nullptr;
	currentChild->_indexInParent = 0;
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

bool ParseTree::Statement::isInline() const
{
	if(!parent())
		return false;
	return !parent()->isA<Scope>();
}

ParseTree::Scope* ParseTree::Statement::associatedScope(bool create)
{
	if(!create) {
		if(isInline())
			return nullptr;
		if(parent() == nullptr)
			return nullptr;
		if(!parent()->isA<ParseTree::Scope>())
			return nullptr;
	}
	assert(!isInline());
	assert(parent() != nullptr);
	assert(parent()->isA<ParseTree::Scope>());
	
	// Check if there already is an associated scope
	if(nextSibbling() != nullptr)
		if(nextSibbling()->isA<ParseTree::Scope>())
			return nextSibbling()->to<ParseTree::Scope>();
	if(!create)
		return nullptr;
	
	// Create one
	ParseTree::Scope* scope = new ParseTree::Scope;
	ParseTree::Scope* parent = _parent->to<ParseTree::Scope>();
	parent->insertAfter(this, scope);
	assert(scope->associatedStatement() == this);
	return scope;
}

void ParseTree::Statement::print(std::wostream& out, uint indentation) const
{
	if(isInline())
		out << "(";
	bool outbounds = true;
	for(Node* child: children()) {
		if(outbounds && !(child->isA<Identifier>() && child->to<Identifier>()->outbound())) {
			switch(_type) {
				case Call: out << L"≔"; break;
				case Closure: out << L"↦"; break;
				case Precondition: out << L"∵"; break;
				case Postcondition: out << L"∴"; break;
				case Axiom: out << L"⊨"; break;
				case Assertion: out << L"⊢"; break;
			}
			out << " ";
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

void ParseTree::IdentifierVisible::print(std::wostream& out, uint indentation) const
{
	assert(_identifier);
	out << L"→" << _identifier->name();
}

void ParseTree::IdentifierLookup::print(std::wostream& out, uint indentation) const
{
	assert(_identifier);
	out << L"←" << _identifier->name();
}
