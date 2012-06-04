#pragma once
#include <DFG/DataFlowGraph.h>
#include <Interpreter/Value.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Value.h>
#include "Passes/StackVMProperty.h"


class LlvmCompiler
{
public:
	LlvmCompiler(DataFlowGraph* dfg);
	~LlvmCompiler() { }
	
	void compile();
	
private:
	DataFlowGraph* _dfg;
	llvm::LLVMContext& _context;
	llvm::Module* _module;
	llvm::IRBuilder<> _builder;
	llvm::Function* _malloc;
	llvm::Function* _trace;
	std::vector<llvm::Value*> _stack;
	std::map<const Node*, llvm::Function*> _declarations;
	std::map<const Node*, llvm::Function*> _wrappers;
	std::map<const Node*, llvm::Value*> _closures;
	
	void buildDeclareFunction(const Node* closureNode);
	void buildFunctionBody(const Node* closureNode);
	void buildInstruction(const StackVMProperty::Instruction* instruction);
	void buildCall(const StackVMProperty::CallInstruction* call);
	void buildAlloc(const StackVMProperty::AllocateInstruction* alloc);
	void buildStore(const StackVMProperty::StoreInstruction* store);
	void buildRet(const StackVMProperty::ReturnInstruction* ret);
	void buildBuiltin(const StackVMProperty::CallInstruction* call);
	
	void buildDefaultClosure(const Node* closureNode);
	void buildWrapper(const Node* closureNode);
	llvm::Value* buildConstant(const Value& value);
	llvm::FunctionType* buildFunctionType(int inArrity, int outArrity);
	void buildTrace(uint64 value) { buildTrace(_builder.getInt64(value)); }
	void buildTrace(llvm::Value* value);
	
	llvm::Value* buildMalloc(int size); 
};
