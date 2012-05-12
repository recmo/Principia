#include "LlvmCompiler.h"
#include "ClosureProperty.h"
#include "OrderProperty.h"
#include "StackProperty.h"
#include "ReturnStackProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <DFG/Edge.h>
#include <Interpreter/Value.h>
#include <Parser/IdentifierProperty.h>
#include <Parser/ConstantProperty.h>
#include <stdint.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Attributes.h>
#include <llvm/Analysis/MemoryBuiltins.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/PassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>

std::wostream& operator<<(std::wostream& out, const llvm::StringRef& string)
{
	out << string.data();
	return out;
}

std::wostream& operator<<(std::wostream& out, const llvm::Twine& string)
{
	out << string.str().data();
	return out;
}

LlvmCompiler::LlvmCompiler(DataFlowGraph* dfg)
: _dfg(dfg)
, _context(llvm::getGlobalContext())
, _module(new llvm::Module("Program", _context))
, _builder(_context)
, _stack()
{
	llvm::InitializeNativeTarget();
}

void LlvmCompiler::compile()
{
	// Declare malloc
	llvm::FunctionType* mallocType = llvm::FunctionType::get(_builder.getVoidTy()->getPointerTo(), _builder.getInt64Ty(), false);
	_malloc = llvm::Function::Create(mallocType, llvm::GlobalValue::ExternalLinkage, "malloc", _module);
	
	// Declare the functions
	foreach(const Node* closure, _dfg->nodes()) {
		if(closure->type() != NodeType::Closure)
			continue;
		buildFunction(closure);
	}
	
	// Build the functions
	foreach(const Node* closure, _dfg->nodes()) {
		if(closure->type() != NodeType::Closure)
			continue;
		buildFunctionBody(closure);
	}
	
	// Print the result
	_module->dump();
	
	llvm::verifyModule(*_module);
	
	// Construct the JIT
	std::string error;
	llvm::ExecutionEngine* ee = llvm::EngineBuilder(_module).setErrorStr(&error).create();
	if(!ee) {
		wcerr << "ERROR " << error.data() << endl;
		return;
	}
	
	// Optimize
	llvm::FunctionPassManager fpm(_module);
	fpm.add(new llvm::TargetData(*(ee->getTargetData())));
	fpm.add(llvm::createBasicAliasAnalysisPass());
	fpm.add(llvm::createInstructionCombiningPass());
	fpm.add(llvm::createReassociatePass());
	fpm.add(llvm::createGVNPass());
	fpm.add(llvm::createCFGSimplificationPass());
	fpm.doInitialization();
	
	llvm::Function* func = _module->getFunction("test");
	
	func->dump();
	fpm.run(*func);
	func->dump();
	
	
	wcerr << "EXEC = " << endl;
	vector<llvm::GenericValue> args;
	args.push_back(llvm::GenericValue((void*)1));
	llvm::GenericValue r = ee->runFunction(_module->getFunction("test"), args);
	
	r.IntVal.dump();
	wcerr << " s!!!" << endl;
	
	//void* fp = ee->getPointerToFunction(_module->getFunction("test"));
	//wcerr << fp << endl;
	
	_module->dump();
}

void LlvmCompiler::buildFunction(const Node* closureNode)
{
	wcerr << endl << endl;
	wcerr << "FUNC " << closureNode << endl;
	
	// Construct the function type
	llvm::FunctionType* functionType = buildFunctionType(closureNode->outArrity() - 1, closureNode->inArrity());
	
	// Construct the function
	std::string name = "";
	if(closureNode->out(0)->has<IdentifierProperty>())
		name = encodeUtf8(closureNode->out(0)->get<IdentifierProperty>().value());
	llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, name, _module);
	
	// All functions are pure
	function->addFnAttr(llvm::Attribute::ReadOnly);
	
	// Use the fastest calling convention, required for tail calling
	function->setCallingConv(llvm::CallingConv::Fast);
	
	// Store the declaration
	_declarations[closureNode] = function;
}

void LlvmCompiler::buildFunctionBody(const Node* closureNode)
{
	llvm::Function* function = _declarations[closureNode];
	
	// Construct the body
	llvm::BasicBlock *body = llvm::BasicBlock::Create(_context, "entry", function);
	_builder.SetInsertPoint(body);
	
	// Clear the variable stack
	_stack.clear();
	
	// Add the closure to the stack
	llvm::Argument* arg0 = function->arg_begin();
	const vector<const Edge*> closureEdges = closureNode->get<ClosureProperty>().edges();
	for(int i = 0; i < closureEdges.size(); ++i) {
		llvm::Value* elementPtr = _builder.CreateGEP(arg0, _builder.getInt64(i));
		_stack.push_back(_builder.CreateLoad(elementPtr));
	}
	function->arg_begin()->setName("closure");
	
	// Add the arguments to the stack, skipping the first argument
	int index = 1;
	for(llvm::Function::arg_iterator argi = ++function->arg_begin(); argi != function->arg_end(); ++argi, ++index) {
		if(closureNode->out(index)->has<IdentifierProperty>())
			argi->setName(encodeUtf8(closureNode->out(index)->get<IdentifierProperty>().value()));
		_stack.push_back(argi);
	}
	
	// Construct the function body
	foreach(const Node* node, closureNode->get<OrderProperty>().nodes()) {
		buildNode(node);
	}
	
	// Construct the return value
	wcerr << "RETURNS" << endl;
	vector<llvm::Value*> results;
	const vector<int>& resultStackpos = closureNode->get<ReturnStackProperty>().positions();
	for(int i = 0; i < closureNode->inArrity(); ++i) {
		if(resultStackpos[i] == -1)
			results.push_back(buildConstant(closureNode->in(i)->get<ConstantProperty>().value()));
		else {
			assert(resultStackpos[i] < _stack.size());
			results.push_back(_stack[resultStackpos[i]]);
		}
		
	}
	_builder.CreateAggregateRet(results.data(), results.size());
}

void LlvmCompiler::buildNode(const Node* node)
{
	wcerr << "NODE " << node << endl;
	if(node->type() == NodeType::Call) {
		if(node->in(0)->has<ConstantProperty>() && node->in(0)->get<ConstantProperty>().value().kind == Value::Builtin)
			buildBuiltin(node);
		else
			buildCall(node);
	} else
		buildClosure(node);
}

void LlvmCompiler::buildCall(const Node* callNode)
{
	wcerr << "CALL " << callNode << endl;
	
	// Fetch the argument values
	vector<const Edge*> inputs = callNode->in();
	const vector<int>& stackPositions = callNode->get<StackProperty>().positions();
	vector<llvm::Value*> args;
	for(int i = 0; i < stackPositions.size(); ++i) {
		if(stackPositions[i] == -1)
			args.push_back(buildConstant(inputs[i]->get<ConstantProperty>().value()));
		else
			args.push_back(_stack[stackPositions[i]]);
	}
	
	// Cast the first argument to closure pointer
	args[0] = _builder.CreateIntToPtr(args[0], _builder.getInt64Ty()->getPointerTo());
	
	// Fetch the function pointer
	llvm::FunctionType* funcType = buildFunctionType(callNode->inArrity() - 1, callNode->outArrity());
	llvm::Value* closurePtr = _builder.CreateBitCast(args[0], _builder.getInt64Ty()->getPointerTo());
	llvm::Value* funcPtrIntPtr = _builder.CreateGEP(closurePtr, _builder.getInt64(0));
	llvm::Value* funcPtrInt = _builder.CreateLoad(funcPtrIntPtr);
	llvm::Value* funcPtr = _builder.CreateIntToPtr(funcPtrInt, funcType->getPointerTo());
	
	// Create the call
	llvm::Value* result = _builder.CreateCall(funcPtr, args);
	
	// Unpack the results to the stack
	for(int i = 0; i < callNode->outArrity(); ++i) {
		_stack.push_back(_builder.CreateExtractValue(result, i));
	}
	
	wcerr << " END CALL " << endl;
}

void LlvmCompiler::buildBuiltin(const Node* callNode)
{
	wcerr << "BUILTIN " << callNode << endl;
	
	// Construct the arguments
	vector<const Edge*> inputs = callNode->in();
	wcerr << inputs << endl;
	const vector<int>& stackPositions = callNode->get<StackProperty>().positions();
	wcerr << stackPositions << endl;
	vector<llvm::Value*> args;
	for(int i = 1; i < stackPositions.size(); ++i) {
		if(stackPositions[i] == -1)
			args.push_back(buildConstant(inputs[i]->get<ConstantProperty>().value()));
		else
			args.push_back(_stack[stackPositions[i]]);
	}
	
	// Dispatch on the name
	std::wstring name = callNode->in(0)->get<IdentifierProperty>().value();
	if(name == L"add") {
		wcerr << "ADD" << endl;
		std::string name;
		if(callNode->out(0)->has<IdentifierProperty>())
			name = encodeUtf8(callNode->out(0)->get<IdentifierProperty>().value());
		llvm::Value* result = _builder.CreateAdd(args[0], args[1], name);
		_stack.push_back(result);
	}
}

void LlvmCompiler::buildClosure(const Node* closureNode)
{
	wcerr << "CLOSURE " << closureNode << endl;
	
	/// TODO: Return constant if the closure is empty
	
	// Construct the inputs
	const vector<const Edge*>& inputs = closureNode->get<ClosureProperty>().edges();
	wcerr << "closure edges = " <<  inputs << endl;
	const vector<int>& stackPositions = closureNode->get<StackProperty>().positions();
	wcerr << stackPositions << endl;
	wcerr << _stack << endl;
	vector<llvm::Value*> args;
	for(int i = 0; i < stackPositions.size(); ++i) {
		wcerr << inputs[i] << " to " << stackPositions[i] << endl;
		if(stackPositions[i] == -1)
			args.push_back(buildConstant(inputs[i]->get<ConstantProperty>().value()));
		else
			args.push_back(_stack[stackPositions[i]]);
	}
	
	// Construct the closure
	int closureSize = args.size() + 1;
	llvm::Value* closure = buildMalloc(_builder.getInt64Ty());
	
	// Store the function pointer
	llvm::Function* func = _declarations[closureNode];
	llvm::Value* funcPtr = _builder.CreatePtrToInt(func, _builder.getInt64Ty());
	_builder.CreateStore(funcPtr, _builder.CreateGEP(closure, _builder.getInt64(0)));
	
	// Store the rest of the closure
	for(int i = 0; i < args.size(); ++i)
		_builder.CreateStore(args[i], _builder.CreateGEP(closure, _builder.getInt64(i + 1)));
	
	// Push the closure
	llvm::Value* closurePtr = _builder.CreatePtrToInt(closure, _builder.getInt64Ty());
	_stack.push_back(closurePtr);
}

llvm::Value* LlvmCompiler::buildConstant(const Value& value)
{
	switch(value.kind) {
	case Value::Integer:
		return _builder.getInt64(value.integer());
	case Value::Function: {
		const Closure* closure = value.closure();
		return _builder.getInt64(reinterpret_cast<uint64>(closure));
	}
	default:
		wcerr << "Could not build constant " <<  value << endl;
		return _builder.getInt64(0);
		throw L"Invalid constant";
	}
}

llvm::FunctionType* LlvmCompiler::buildFunctionType(int inArrity, int outArrity)
{
	// Construct the argument type
	std::vector<llvm::Type*> params;
	params.push_back(_builder.getInt64Ty()->getPointerTo()); // Pointer to closure
	for(int i = 0; i < inArrity; ++i)
		params.push_back(_builder.getInt64Ty());

	// Construct the return type
	std::vector<llvm::Type*> returnTypes;
	for(int i = 0; i < outArrity; ++i) {
		llvm::Type* type = _builder.getInt64Ty();
		returnTypes.push_back(type);
	}
	llvm::StructType* returns = llvm::StructType::create(_context, returnTypes);
	
	// Construct the function type
	llvm::FunctionType* functionType = llvm::FunctionType::get(returns, params, false);
	return functionType;
}

llvm::Value* LlvmCompiler::buildMalloc(llvm::Type* type)
{
	
	// Allocate the space
	size_t size = (type->getScalarSizeInBits() + 7)/ 8;
	llvm::Value* space = _builder.CreateCall(_malloc, _builder.getInt64(size), "space");
	assert(llvm::isMalloc(space));
	
	llvm::Value* pointer = _builder.CreateBitCast(space, type->getPointerTo());
	
	pointer->dump();
	return pointer;
}
