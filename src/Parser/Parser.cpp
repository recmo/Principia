#include "Parser.h"
#include <Unicode/convert.h>
#include <Unicode/exceptions.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <functional>
#include <limits>
#include <algorithm>

// TODO: Make `Symbol` and uint instead of a pair.

namespace Parser {

typedef std::vector<std::shared_ptr<Node>> Stack;

std::shared_ptr<Node> lexer(std::wistream& stream)
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
	return stack.back();
}

std::shared_ptr<Node> parseFile(const std::wstring& filename)
{
	std::wifstream stream(encodeLocal(filename));
	std::shared_ptr<Node> n = lexer(stream);
	n->filename = filename;
	parse(n);
	return n;
}

std::shared_ptr<Node> parseString(const std::wstring& contents)
{
	std::wstringstream stream(contents);
	std::shared_ptr<Node> n = lexer(stream);
	n->filename = L"<string>";
	parse(n);
	return n;
}

std::shared_ptr<Node> paserStream(std::wistream& stream)
{
	std::shared_ptr<Node> n = lexer(stream);
	n->filename = L"<stream>";
	parse(n);
	return n;
}

void visit(std::shared_ptr<Node> node, std::function<void(Node&)> visitor)
{
	visitor(*node);
	for(const std::shared_ptr<Node>& child: node->children)
		visit(child, visitor);
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

void visit(std::shared_ptr<Node>node, std::function<void(const Stack&)> visitor)
{
	Stack stack;
	stack.push_back(node);
	visit(stack, visitor);
}

void tag(std::shared_ptr<Node> module)
{
	visit(module, [](Node& node) {
		if((node.kind == Statement || node.kind == SubStatement)
			&& node.children.size() >= 1
			&& node.children[0]->kind == Identifier
			&& node.children[0]->identifier.size() == 1
			&& node.children[0]->identifier[0] == closure) {
			node.is_closure = true;
			node.children[0]->is_closure = true;
			for(uint i = 1; i < node.children.size(); ++i) {
				node.children[i]->is_binding_site = true;
			}
		}
	});
}

void deanonymize(std::shared_ptr<Node> module)
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

void undebruijn(std::shared_ptr<Node> module)
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
					stack[index]->is_substatement_bound = true;
				} else {
					assert(stack[index]->is_binding_site);
					node->binding_site = stack[index];
					stack[index]->is_substatement_bound = true;
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

void bind(std::shared_ptr<Node> module)
{
	// Bind identifiers
	visit(module, [](const Stack& stack) {
		const std::shared_ptr<Node>& node = stack.back();
		if(node->kind != Identifier || node->is_binding_site || node->is_closure
			|| node->binding_site.lock())
			return;
		
		// Id node
		const std::shared_ptr<Node> id_node = node;
		const std::wstring id = id_node->identifier;
		
		// Binding node
		std::shared_ptr<Node> binding_node;
		
		// Go up the stack
		assert(stack.size() >= 2);
		uint stack_index = stack.size() - 2;
		
		while(stack_index < stack.size()) {
			
			// Find out position in the stack
			const std::shared_ptr<Node> child = stack[stack_index + 1];
			const std::shared_ptr<Node> parent = stack[stack_index];
			uint child_index = std::distance(parent->children.begin(), std::find(
				parent->children.begin(), parent->children.end(), child));
			assert(child_index < parent->children.size());
			
			// Scan backward
			for(uint i = child_index - 1; i < parent->children.size(); --i) {
				binding_node = find_symbol(id, parent->children[i]);
				if(binding_node)
					break;
			}
			if(binding_node)
				break;
			
			// Scan forward
			for(uint i = child_index + 1; i < parent->children.size(); ++i) {
				binding_node = find_symbol(id, parent->children[i]);
				if(binding_node)
					break;
			}
			if(binding_node)
				break;
				
			// Go up and try again
			--stack_index;
		}
		
		// Upsert a global symbol
		if(!binding_node) {
			std::shared_ptr<Node> module = stack[0];
			assert(module->kind == Module);
			
			for(std::shared_ptr<Node> global: module->globals) {
				if(global->identifier == id) {
					binding_node = global;
					break;
				}
			}
			if(!binding_node) {
				binding_node = std::make_shared<Node>(Identifier);
				binding_node->identifier = id;
				binding_node->is_binding_site = true;
				module->globals.push_back(binding_node);
			}
		}
		
		// Bind  the node
		assert(binding_node);
		id_node->binding_site = binding_node;
		
	});
}

void parse(std::shared_ptr<Node> module)
{
	tag(module);
	deanonymize(module);
	undebruijn(module);
	bind(module);
}

void print(std::shared_ptr<Node> module)
{
	uint i = 0;
	std::function<void(const Parser::Node& n)> r = [&](const Parser::Node& n) -> void {
		
		for(uint j = 0; j < i; ++j)
			std::wcerr << "\t";
		if(n.kind == Parser::Module)
			std::wcerr << "Module";
		if(n.kind == Parser::Scope)
			std::wcerr << "Scope";
		if(n.kind == Parser::Statement)
			std::wcerr << "Statement";
		if(n.kind == Parser::SubStatement)
			std::wcerr << "SubStatement";
		if(n.kind == Parser::Identifier)
			std::wcerr << "Identifier";
		if(n.kind == Parser::Quote)
			std::wcerr << "Quote";
		std::wcerr << " " << n.quote << n.identifier << n.filename;
		if(n.is_binding_site)
			std::wcerr << " binding";
		if(std::shared_ptr<Parser::Node> site = n.binding_site.lock())
			std::wcerr << " bound to " << site->identifier;
		std::wcerr << "\n";
		
		++i;
		for(const std::shared_ptr<Parser::Node>& c: n.globals)
			r(*c);
		for(const std::shared_ptr<Parser::Node>& c: n.children)
			r(*c);
		--i;
	};
	r(*module);
}

// TODO: (λ · (print “0” ret)) produces a lambda of arity one, but
//       we can equally argue it should be a lambda of arity two that
//       ignores its argument. (Although this can also be done as
//       (λ · _ (print “0” ret))

uint count_arguments(const Node& node)
{
	uint count = node.children.size();
	for(const auto& child: node.children)
		if(child->kind == SubStatement && !child->is_substatement_bound)
			--count;
	count -= node.is_closure ? 2 : 1;
	return count;
}

Program compile(std::shared_ptr<Node> module)
{
	Program p;
	
	// Add unresolved symbols to p.symbols_import
	for(auto i: module->globals) {
		p.symbols_import.push_back(i->identifier);
		const Symbol symbol = std::make_pair(0, p.symbols_import.size() - 1);
		i->bind_index = symbol;
		p.symbols[symbol] = i->identifier;
	}
	
	// Add top level symbols to p.symbols_export
	for(auto i: module->children) {
		Node& node = *i;
		if((node.kind == Statement || node.kind == SubStatement)
			&& node.is_closure
			&& node.children.size() > 2
			&& node.children[1]->kind == Identifier) {
			if(std::find(p.symbols_export.begin(), p.symbols_export.end(),
				node.children[1]->identifier) != p.symbols_export.end())
				continue;
			p.symbols_export.push_back(node.children[1]->identifier);
			p.closures.push_back(count_arguments(node));
			node.closure_index = p.closures.size() + 1;
			for(uint i = 1; i < node.children.size(); ++i)
				node.children[i]->bind_index = std::make_pair(node.closure_index, i - 1);
		}
	}
	
	// Add all other closures
	visit(module, [&](Node& node) {
		if(node.kind == Statement || node.kind == SubStatement) {
			if(node.is_closure && node.closure_index == 0) {
				p.closures.push_back(count_arguments(node));
				node.closure_index = p.closures.size() + 1;
				for(uint i = 1; i < node.children.size(); ++i)
					node.children[i]->bind_index = std::make_pair(node.closure_index, i - 1);
			}
		}
	});
	
	// Add all the calls
	visit(module, [&](Node& node) {
		if(node.kind == Statement || node.kind == SubStatement) {
			if(!node.is_closure) {
				std::vector<std::pair<uint,uint>> call;
				for(std::shared_ptr<Node> c: node.children) {
					if(c->kind == SubStatement && !c->is_substatement_bound)
						continue;
					if(c->kind == Quote) {
						p.constants.push_back(c->quote);
						call.push_back(std::make_pair(1, p.constants.size() - 1));
						continue;
					}
					std::shared_ptr<Node> bind = c->binding_site.lock();
					assert(bind);
					call.push_back(bind->bind_index);
				}
				node.call_index = p.calls.size();
				p.calls.push_back(call);
			}
		}
	});
	
	// Add all the symbols and their identifiers (this is debug info)
	visit(module, [&](Node& node) {
		if(node.kind == Identifier) {
			p.symbols[node.bind_index] = node.identifier;
		}
	});
	std::wcerr << p.symbols << "\n";
	
	// Associate closures with calls
	// The associated call is the first call after the closure.
	// Note: This implies that closures and calls alternate
	constexpr uint none = std::numeric_limits<uint>().max();
	p.closure_call.resize(p.calls.size(), none);
	uint current_closure_index = none;
	visit(module, [&](Node& node) {
		if(node.kind == Statement || node.kind == SubStatement) {
			if(node.is_closure) {
				assert(current_closure_index == none);
				current_closure_index = node.closure_index - 2;
			} else {
				if(current_closure_index != none) {
					p.closure_call[current_closure_index] = node.call_index;
					current_closure_index = none;
				} else {
					std::wcerr << "Unassociated call\n";
				}
			}
		}
	});
	
	return p;
}

void write(std::wostream& out, const Program& p)
{
	for(uint i = 0; i < p.symbols_import.size(); ++i) {
		out << p.symbols_import[i];
		if(i != p.symbols_import.size() - 1)
			out << " ";
	}
	out << "\n";
	for(uint i = 0; i < p.symbols_export.size(); ++i) {
		out << p.symbols_export[i];
		if(i != p.symbols_export.size() - 1)
			out << " ";
	}
	out << "\n";
	out << p.constants.size() << "\n";
	for(const auto s: p.constants)
		out << s.size() << " " << s << "\n";
	for(uint i = 0; i < p.closures.size(); ++i) {
		out << p.closures[i];
		if(i != p.closures.size() - 1)
			out << " ";
	}
	out << "\n";
	for(uint i = 0; i < p.closure_call.size(); ++i) {
		out << p.closure_call[i];
		if(i != p.closure_call.size() - 1)
			out << " ";
	}
	out << "\n";
	for(const auto s: p.calls) {
		for(uint i = 0; i < s.size(); ++i) {
			out << s[i].first << " " << s[i].second;
			if(i != s.size() - 1)
				out << " ";
		}
		out << "\n";
	}
}

Program read(std::wistream& in)
{
	// TODO
	return Program();
}

} // namespace Parser
