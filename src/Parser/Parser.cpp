#include "Parser.h"
#include <Unicode/convert.h>
#include <Unicode/exceptions.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <functional>
#include <limits>

namespace Parser {

typedef std::vector<std::shared_ptr<Node>> Stack;

Node lexer(std::wistream& stream)
{
	Stack stack;
	uint indentation_depth = 0;
	uint statement_depth = 0;
	
	// TOKEN: BEGIN MODULE
	//std::wcerr << "BEGIN MODULE\n";
	stack.push_back(std::make_shared<Node>(Module));
	
	while(stream.good()) {
		wchar_t c = stream.get();
		
		// Statements separator and indentation
		if(c == statement_separator) {
			// All sub statements must be closed
			if(statement_depth > 1) {
				throw runtime_error(L"Missing )");
			} else if(statement_depth == 1) {
				
				// TOKEN: END STATEMENT
				//std::wcerr << "END STATEMENT\n";
				assert(stack.back()->kind == Statement);
				stack.pop_back();
				statement_depth = 0;
				
			}
			
			// Indentation mode:
			uint new_identation_depth = 0;
			while(stream.peek() == indentation) {
				++new_identation_depth;
				stream.get();
			}
			// wcerr << "INDENTATION " << new_identation_depth << "\n";
			if(new_identation_depth > indentation_depth + 1) {
				throw runtime_error(L"Can not open more than one scope at a time.");
			} else if(new_identation_depth == indentation_depth + 1) {
				// TOKEN: BEGIN SCOPE
				//std::wcerr << "BEGIN SCOPE\n";
				assert(stack.back()->kind == Module || stack.back()->kind == Scope);
				stack.back()->children.push_back(std::make_shared<Node>(Scope));
				stack.push_back(stack.back()->children.back());
				++indentation_depth;
			} else if(new_identation_depth == indentation_depth) {
				// Same scope
			} else {
				while(new_identation_depth < indentation_depth) {
					// TOKEN: END SCOPE
					//std::wcerr << "END SCOPE\n";
					assert(stack.back()->kind == Scope);
					stack.pop_back();
					--indentation_depth;
				}
			}
		} else if(c == indentation) {
			throw runtime_error(L"Can not have indentation here.");
		
		} else if(c == identifier_separator) {
			// Nothing
		
		// Substatements
		} else if(c == substatement_begin) {
			
			if(statement_depth == 0) {
				// TOKEN: BEGIN STATEMENT
				//std::wcerr << "BEGIN STATEMENT\n";
				assert(stack.back()->kind == Module || stack.back()->kind == Scope);
				assert(statement_depth == 0);
				stack.back()->children.push_back(std::make_shared<Node>(Statement));
				stack.push_back(stack.back()->children.back());
				statement_depth = 1;
			}
			
			// TOKEN: BEGIN SUBSTATEMENT
			//std::wcerr << "BEGIN SUB-STATEMENT\n";
			assert(stack.back()->kind == Statement || stack.back()->kind == SubStatement);
			assert(statement_depth >= 1);
			stack.back()->children.push_back(std::make_shared<Node>(SubStatement));
			stack.push_back(stack.back()->children.back());
			++statement_depth;
			
		} else if(c == substatement_end) {
			if(statement_depth <= 1) {
				throw runtime_error(L"Unmatched ) found.");
			}
			
			// TOKEN: END SUBSTATEMENT
			//std::wcerr << "END SUB-STATEMENT\n";
			assert(stack.back()->kind == SubStatement);
			assert(statement_depth >= 2);
			stack.pop_back();
			--statement_depth;
		
		// Quote mode
		} else if(c == quote_begin) {
			auto node = std::make_shared<Node>(Quote);
			uint nesting = 1;
			while(true) {
				if(!stream.good())
					throw runtime_error(L"Found unmatched “.");
				c = stream.get();
				if(c == quote_begin)
					++nesting;
				if(c == quote_end)
					--nesting;
				if(nesting == 0)
					break;
				node->quote += c;
			}
			
			if(statement_depth == 0) {
				// TOKEN: BEGIN STATEMENT
				//std::wcerr << "BEGIN STATEMENT\n";
				assert(stack.back()->kind == Module || stack.back()->kind == Scope);
				assert(statement_depth == 0);
				stack.back()->children.push_back(std::make_shared<Node>(Statement));
				stack.push_back(stack.back()->children.back());
				statement_depth = 1;
			}
			
			// TOKEN: QUOTE(content)
			//std::wcerr << "QUOTE \"" << node->identifier << "\"\n";
			assert(stack.back()->kind == Statement || stack.back()->kind == SubStatement);
			stack.back()->children.push_back(node);
			
		} else if(c == quote_end) {
			throw runtime_error(L"Found unmatched ”.");
		
		// Identifier mode
		} else {
			auto node = std::make_shared<Node>(Identifier);
			node->identifier += c;
			while(true) {
				if(!stream.good())
					break;
				c = stream.peek();
				if(c == statement_separator || c == identifier_separator || c == substatement_end)
					break;
				c = stream.get();
				if(c == quote_begin || c == quote_end || c == indentation)
					throw runtime_error(L"Unexpected character.");
				node->identifier += c;
			}
			
			if(statement_depth == 0) {
				// TOKEN: BEGIN STATEMENT
				//std::wcerr << "BEGIN STATEMENT\n";
				assert(stack.back()->kind == Module || stack.back()->kind == Scope);
				assert(statement_depth == 0);
				stack.back()->children.push_back(std::make_shared<Node>(Statement));
				stack.push_back(stack.back()->children.back());
				statement_depth = 1;
			}
			
			// TOKEN: IDENTIFIER(content)
			//std::wcerr << "IDENTIFIER " << node->identifier << "\n";
			assert(stack.back()->kind == Statement || stack.back()->kind == SubStatement);
			stack.back()->children.push_back(node);
		}
	}
	if(statement_depth == 1) {
		// TOKEN: END STATEMENT
		//std::wcerr << "END STATEMENT\n";
		assert(stack.back()->kind == Statement);
		stack.pop_back();
	}
	while(indentation_depth--) {
		// TOKEN: END SCOPE
		//std::wcerr << "END SCOPE\n";
		assert(stack.back()->kind == Scope);
		stack.pop_back();
	}
	
	// TOKEN: END MODULE
	//std::wcerr << "END MODULE\n";
	assert(stack.back()->kind == Module);
	return *(stack.back());
}

Node parseFile(const std::wstring& filename)
{
	std::wifstream stream(encodeLocal(filename));
	Node n = lexer(stream);
	n.filename = filename;
	deanonymize(n);
	tag_binding_sites(n);
	undebruijn(n);
	bind(n);
	return n;
}

Node parseString(const std::wstring& contents)
{
	std::wstringstream stream(contents);
	Node n = lexer(stream);
	n.filename = L"<string>";
	deanonymize(n);
	tag_binding_sites(n);
	undebruijn(n);
	bind(n);
	return n;
}

Node paserStream(std::wistream& stream)
{
	Node n = lexer(stream);
	n.filename = L"<stream>";
	deanonymize(n);
	tag_binding_sites(n);
	undebruijn(n);
	bind(n);
	return n;
}

void visit(Node& node, std::function<void(Node&)> visitor)
{
	visitor(node);
	for(const std::shared_ptr<Node>& child: node.children)
		visit(*child, visitor);
}

void visit(Stack& stack, std::function<void(const Stack&)> visitor)
{
	visitor(stack);
	const std::shared_ptr<Node>& node = stack.back();
	for(const std::shared_ptr<Node>& child: node->children) {
		stack.push_back(child);
		visit(stack, visitor);
		stack.pop_back();
	}
}

void visit(Node& node, std::function<void(const Stack&)> visitor)
{
	Stack stack;
	stack.push_back(std::make_shared<Node>(node));
	visit(stack, visitor);
}

void deanonymize(Node& module)
{
	// Give all substatements a unique identifier.
	uint counter = 1;
	const std::wstring base(L"anon-");
	visit(module, [&](Node& node) {
		if(node.kind == SubStatement) {
			std::wstringstream name;
			name << base;
			name << counter;
			node.identifier = name.str();
			++counter;
		}
	});
}

void tag_binding_sites(Node& module)
{
	visit(module, [](Node& node) {
		if((node.kind == Statement || node.kind == SubStatement)
			&& node.children.size() >= 1
			&& node.children[0]->kind == Identifier
			&& node.children[0]->identifier.size() == 1
			&& node.children[0]->identifier[0] == closure) {
			for(uint i = 1; i < node.children.size(); ++i) {
				node.children[i]->is_binding_site = true;
			}
		}
	});
}

void undebruijn(Node& module)
{
	constexpr uint none = std::numeric_limits<uint>().max();
	visit(module, [](const Stack& stack) {
		const std::shared_ptr<Node>& node = stack.back();
		if(node->kind == Identifier
			&& node->identifier.size() == 1) {
			uint number = none;
			switch(node->identifier[0]) {
				case de_bruijn_1: number = 1; break;
				case de_bruijn_2: number = 2; break;
				default: break;
			}
			if(number != none) {
				assert(number < stack.size());
				const uint index = stack.size() - number - 1;
				assert(stack[index]->kind = SubStatement);
				
				if(node->is_binding_site) {
					assert(!stack[index]->is_binding_site);
					stack[index]->binding_site = node;
				} else {
					assert(stack[index]->is_binding_site);
					node->binding_site = stack[index];
				}
			}
		}
	});
}

std::shared_ptr<Node> find_symbol(const std::wstring& identifier, std::shared_ptr<Node> node)
{
	if(node->kind == Identifier
		&& node->is_binding_site
		&& node->identifier == identifier)
		return node;
	if(node->kind == Statement || node->kind == SubStatement) {
		for(std::shared_ptr<Node> child: node->children) {
			std::shared_ptr<Node> binding_site = find_symbol(identifier, child);
			if(binding_site)
				return binding_site;
		}
	}
	return std::shared_ptr<Node>();
}

void bind(Node& module)
{
	// Bind identifiers
	visit(module, [](const Stack& stack) {
		
		const std::shared_ptr<Node>& node = stack.back();
		
		if(node->kind != Identifier)
			return;
		if(node->is_binding_site)
			return;
		if(node->binding_site.lock())
			return;
		
		std::wcerr << "BIND " << node->identifier << "\n";
		
	});
}


} // namespace Parser
