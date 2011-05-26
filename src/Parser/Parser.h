#pragma once
#include <fixups.h>
class DataFlowGraph;
class Edge;
class Source;
namespace quex { class Token; }

class Parser
{
public:
	Parser();
	Parser(DataFlowGraph* ir);
	
	const DataFlowGraph* dataFlowGraph() const { return _dfg; }
	DataFlowGraph* dataFlowGraph() { return _dfg; }
	
	Parser& parse(const string& filename);
	Parser& parse(std::wifstream input);
	
	void pushScope();
	void popScope();
	
private:
	typedef quex::Token Token;
	typedef map<string, Edge*> Scope;
	DataFlowGraph* _dfg;
	vector<Scope> _scopeStack;
	string _filename;
	Token* _token;
	
	enum Type {
		undetermined,
		call,
		closure
	};
	struct Expression {
		Expression();
		Type type;
		vector<Edge*> out;
		vector<Edge*> in;
	};
	vector<Expression> _expressionStack;
	
	string lexeme();
	Source source(bool hasLexeme = true);
	void parseIdentifier();
	void parseQuotation();
	void parseNumber();
	void parseCall();
	void parseClosure();
	void parseStatementSeparator();
	void parseBlockBegin();
	void parseBlockEnd();
	void parseBracketOpen();
	void parseBracketClose();
	void parseFailure();
	Edge* finishNode();
};


