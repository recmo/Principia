#pragma once
#include <DFG/DataFlowGraph.h>
#include <Passes/Value.h>
#include <Passes/StackMachineProperty.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>


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
	std::map<std::shared_ptr<Node>, llvm::Function*> _declarations;
	std::map<std::shared_ptr<Node>, llvm::Function*> _wrappers;
	std::map<std::shared_ptr<Node>, llvm::Value*> _closures;
	
	void buildDeclareFunction(std::shared_ptr<Node> closureNode);
	void buildFunctionBody(std::shared_ptr<Node> closureNode);
	void buildInstruction(const StackMachineProperty::Instruction* instruction);
	void buildCall(const StackMachineProperty::CallInstruction* call);
	void buildAlloc(const StackMachineProperty::AllocateInstruction* alloc);
	void buildStore(const StackMachineProperty::StoreInstruction* store);
	void buildRet(const StackMachineProperty::ReturnInstruction* ret);
	void buildBuiltin(const StackMachineProperty::CallInstruction* call);
	
	// For reference counting
	void buildReference(llvm::Value* value);
	void buildDereference(llvm::Value* value);
	
	void buildDefaultClosure(std::shared_ptr<Node> closureNode);
	void buildWrapper(std::shared_ptr<Node> closureNode);
	llvm::Value* buildConstant(const Value& value);
	llvm::FunctionType* buildFunctionType(int inArity, int outArity);
	void buildTrace(uint64 value) { buildTrace(_builder.getInt64(value)); }
	void buildTrace(llvm::Value* value);
	
	llvm::Value* buildMalloc(int size); 
};
