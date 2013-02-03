#include "LlvmCompiler.h"
#include "ClosureProperty.h"
#include "NativeProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <DFG/Edge.h>
#include <Passes/Value.h>
#include <Passes/Closure.h>
#include "EscapeProperty.h"
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
#include <llvm/PassManager.h>
#include <llvm/PassManagers.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Attributes.h>

// Passes
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/CodeGen/Passes.h>
#include <Passes/LlvmPtrtointInttoptrCombine.h>
#include <Passes/LlvmStoreLoadCombine.h>

#define debug false

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

extern "C" {
	void trace(uint64 val)
	{
		wcerr << "TRACE: " << val << " " << std::hex << val << std::dec << endl;
	}
}


LlvmCompiler::LlvmCompiler(DataFlowGraph* dfg)
: _dfg(dfg)
, _context(llvm::getGlobalContext())
, _module(new llvm::Module("Program", _context))
, _builder(_context)
, _malloc(0)
, _trace(0)
, _stack()
, _declarations()
, _wrappers()
, _closures()
{
	llvm::InitializeNativeTarget();
	
	// Declare malloc
	llvm::FunctionType* mallocType = llvm::FunctionType::get(_builder.getInt8PtrTy(), _builder.getInt64Ty(), false);
	_malloc = llvm::Function::Create(mallocType, llvm::GlobalValue::ExternalLinkage, "malloc", _module);
	
	// Declate trace
	llvm::FunctionType* traceFuncType = llvm::FunctionType::get(_builder.getVoidTy(), _builder.getInt64Ty(), false);
	_trace = llvm::Function::Create(traceFuncType, llvm::GlobalValue::ExternalLinkage, "trace", _module);
}

void LlvmCompiler::compile()
{
	// Declare the functions
	foreach(const Node* closure, _dfg->nodes()) {
		if(closure->type() != NodeType::Closure)
			continue;
		
		// Declare the functions
		buildDeclareFunction(closure);
		
		// Build an empty default closure if we can
		if(closure->get<ClosureProperty>().edges().empty())
			buildDefaultClosure(closure);
	}
	
	// Build the functions
	foreach(const Node* closure, _dfg->nodes()) {
		if(closure->type() != NodeType::Closure)
			continue;
		buildFunctionBody(closure);
	}
	
	// Build wrappers to call from C++
	foreach(const Node* closure, _dfg->nodes()) {
		if(closure->type() != NodeType::Closure)
			continue;
		if(!closure->out(0)->has<IdentifierProperty>())
			continue;
		buildWrapper(closure);
	}
	
	if(debug) {
		// Print the result
		wcerr << endl << endl << "==============================================================================" << endl;
		_module->dump();
		wcerr << endl << endl << "==============================================================================" << endl;
	}
	
	// Verify the code
	if(llvm::verifyModule(*_module)) {
		wcerr << "ERRORS!!!" << endl;
		throw L"Errors in produced LLVM code";
	}
	
	// Optimize
	llvm::PassManager mpm;
	mpm.add(llvm::createEarlyCSEPass());
	mpm.add(new LlvmPtrtointInttoptrCombine);
	mpm.add(new LlvmStoreLoadCombine);
	mpm.add(llvm::createInstructionSimplifierPass());
	mpm.add(llvm::createFunctionInliningPass());
	mpm.add(llvm::createEarlyCSEPass());
	mpm.add(llvm::createDeadStoreEliminationPass());
	mpm.add(llvm::createInstructionCombiningPass());
	
	// mpm.add(llvm::createPromoteMemoryToRegisterPass());
	// mpm.add(llvm::createReassociatePass());
	// mpm.add(llvm::createScalarReplAggregatesPass());
	// mpm.add(llvm::createFunctionInliningPass());
	// mpm.add(llvm::createTailCallEliminationPass());
	mpm.run(*_module);
	
	if(debug) {
		// Print the result
		wcerr << endl << endl << "==============================================================================" << endl;
		_module->dump();
		wcerr << endl << endl << "==============================================================================" << endl;
	}
	
	// Construct the JIT
	std::string error;
	llvm::ExecutionEngine* ee = llvm::EngineBuilder(_module)
	.setErrorStr(&error)
	.setEngineKind(llvm::EngineKind::JIT)
	.setOptLevel(llvm::CodeGenOpt::Aggressive)
	.create();
	
	if(!ee) {
		wcerr << "ERROR " << error.data() << endl;
		return;
	}
	
	/*
	// Link the trace function if it is not optimized away
	llvm::Function* trace = _module->getFunction("trace");
	if(trace)
		ee->addGlobalMapping(trace, (void*)&trace);
	
	// Link the malloc function if it is not optimized away
	llvm::Function* malloc = _module->getFunction("malloc");
	if(malloc)
		ee->addGlobalMapping(malloc, (void*)&malloc);
	*/
	
	// Compile to native!
	if(debug)
		wcerr << "Compiling to native" << endl;
	foreach(Node* closure, _dfg->nodes()) {
		if(closure->type() != NodeType::Closure)
			continue;
		if(!closure->out(0)->has<IdentifierProperty>())
			continue;
		void* voidPtr = ee->getPointerToFunction(_wrappers[closure]);
		if(voidPtr == 0) {
			wcerr << "Closure " << closure << " got optimized away!" << endl;
			continue;
		}
		NativeProperty::Function funcPtr = reinterpret_cast<NativeProperty::Function>(voidPtr);
		int numClosure = closure->get<ClosureProperty>().edges().size();
		closure->set(NativeProperty(funcPtr, numClosure, closure->outArrity() - 1, closure->inArrity()));
	}
	if(debug)
		wcerr << "DONE!" << endl;
}

void LlvmCompiler::buildDeclareFunction(const Node* closureNode)
{
	if(debug)
		wcerr << "DEC FUNC " << closureNode << endl;
	
	// Construct the function type
	llvm::FunctionType* functionType = buildFunctionType(closureNode->outArrity() - 1, closureNode->inArrity());
	
	// Construct the function
	std::string name = "";
	if(closureNode->out(0)->has<IdentifierProperty>())
		name = encodeUtf8(closureNode->out(0)->get<IdentifierProperty>().value());
	llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::PrivateLinkage, name, _module);
	
	// All functions are pure
	function->addFnAttr(llvm::Attributes::ReadOnly);
	
	// Use the fastest calling convention, required for tail calling
	function->setCallingConv(llvm::CallingConv::Fast);
	
	// We do not use exceptions anywhere
	function->setDoesNotThrow();
	
	// Store the declaration
	_declarations[closureNode] = function;
}

void LlvmCompiler::buildDefaultClosure(const Node* closureNode)
{
	// The closure is in this case a pointer to a structure
	// containing only a pointer to the function all cast to
	// int64's
	if(debug)
		wcerr << "DEFC " << closureNode << endl;
	llvm::Constant* zero = llvm::ConstantExpr::getIntegerValue(_builder.getInt64Ty(), llvm::APInt(64, 0));
	assert(closureNode->get<ClosureProperty>().edges().empty());
	llvm::Function* func = _declarations[closureNode];
	llvm::Constant* funcPtr = llvm::ConstantExpr::getGetElementPtr(func, zero);
	llvm::Constant* funcPtrInt = llvm::ConstantExpr::getPtrToInt(funcPtr, _builder.getInt64Ty());
	std::string name = "";
	if(closureNode->out(0)->has<IdentifierProperty>())
		name = encodeUtf8(closureNode->out(0)->get<IdentifierProperty>().value()) + "_closure";
	llvm::GlobalVariable* closure = new llvm::GlobalVariable(*_module, _builder.getInt64Ty(), true, llvm::GlobalValue::PrivateLinkage, funcPtrInt, name);
	llvm::Value* closurePtr = _builder.CreateGEP(closure, _builder.getInt64(0));
	llvm::Value* closurePtrInt = _builder.CreatePtrToInt(closurePtr, _builder.getInt64Ty());
	_closures[closureNode] = closurePtrInt;
}

void LlvmCompiler::buildWrapper(const Node* closureNode)
{
	// Build a wrapper using safe calling conventions
	// void function_wrapper(int64* closure, int64* inputs, int64* outputs);
	if(debug)
		wcerr << "WRAP " << closureNode << endl;
	
	// Give it a name
	std::string name = "";
	if(closureNode->out(0)->has<IdentifierProperty>())
		name = encodeUtf8(closureNode->out(0)->get<IdentifierProperty>().value()) + "_wrapper";
	
	// Construct the function type
	std::vector<llvm::Type*> argumentTypes;
	argumentTypes.push_back(_builder.getInt64Ty()->getPointerTo());
	argumentTypes.push_back(_builder.getInt64Ty()->getPointerTo());
	argumentTypes.push_back(_builder.getInt64Ty()->getPointerTo());
	llvm::FunctionType* functionType = llvm::FunctionType::get(_builder.getVoidTy(), argumentTypes, false);
	
	// Declare the function
	llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, name, _module);
	function->setCallingConv(llvm::CallingConv::C);
	function->setVisibility(llvm::GlobalValue::DefaultVisibility);
	_wrappers[closureNode] = function;
	
	// Construct the function body
	llvm::BasicBlock *body = llvm::BasicBlock::Create(_context, "entry", function);
	_builder.SetInsertPoint(body);
	llvm::Function::arg_iterator ai = function->arg_begin();
	llvm::Value* closure = ai++;
	llvm::Value* inputs = ai++;
	llvm::Value* outputs = ai;
	closure->setName("closure");
	inputs->setName("inputs");
	outputs->setName("outputs");
	
	// Load the input variables
	vector<llvm::Value*> inputValues;
	inputValues.push_back(closure);
	for(int i = 0; i < closureNode->outArrity() - 1; ++i) {
		llvm::Value* ptr = _builder.CreateGEP(inputs, _builder.getInt64(i));
		llvm::Value* value = _builder.CreateLoad(ptr);
		inputValues.push_back(value);
	}
	
	// Create the call to the native function
	llvm::Function* native = _declarations[closureNode];
	llvm::CallInst* result = _builder.CreateCall(native, inputValues);
	result->setCallingConv(llvm::CallingConv::Fast);
	result->setOnlyReadsMemory();
	result->setDoesNotThrow();
	result->setTailCall();
	
	// Unpack the result value and store
	if(closureNode->inArrity() == 1) {
		llvm::Value* ptr = _builder.CreateGEP(outputs, _builder.getInt64(0));
		_builder.CreateStore(result, ptr);
	} else {
		for(int i = 0; i < closureNode->inArrity(); ++i) {
			llvm::Value* value = _builder.CreateExtractValue(result, i);
			llvm::Value* ptr = _builder.CreateGEP(outputs, _builder.getInt64(i));
			_builder.CreateStore(value, ptr);
			inputValues.push_back(value);
		}
	}
	
	// Return
	_builder.CreateRetVoid();
}

void LlvmCompiler::buildFunctionBody(const Node* closureNode)
{
	if(debug)
		wcerr << "FUNC " << endl;
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
		// The first element of the closure is always the function pointer,
		// the closure edges start at index 1
		llvm::Value* elementPtr = _builder.CreateGEP(arg0, _builder.getInt64(i + 1));
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
	foreach(const StackMachineProperty::Instruction* instruction, closureNode->get<StackMachineProperty>().instructions())
		buildInstruction(instruction);
}

void LlvmCompiler::buildInstruction(const StackMachineProperty::Instruction* instruction)
{
	const StackMachineProperty::CallInstruction* call = dynamic_cast<const StackMachineProperty::CallInstruction*>(instruction);
	if(call)
		buildCall(call);
	const StackMachineProperty::AllocateInstruction* alloc = dynamic_cast<const StackMachineProperty::AllocateInstruction*>(instruction);
	if(alloc)
		buildAlloc(alloc);
	const StackMachineProperty::StoreInstruction* store = dynamic_cast<const StackMachineProperty::StoreInstruction*>(instruction);
	if(store)
		buildStore(store);
	const StackMachineProperty::ReturnInstruction* ret = dynamic_cast<const StackMachineProperty::ReturnInstruction*>(instruction);
	if(ret)
		buildRet(ret);
}

void LlvmCompiler::buildCall(const StackMachineProperty::CallInstruction* call)
{
	if(call->node()->in(0)->has<ConstantProperty>() && call->node()->in(0)->get<ConstantProperty>().value().kind == Value::Builtin) {
		buildBuiltin(call);
		return;
	}
	
	if(debug)
		wcerr << "CALL " << call->node() << endl;
	
	// Fetch the argument values
	vector<const Edge*> inputs = call->node()->in();
	const vector<int>& stackPositions = call->arguments();
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
	llvm::Value* funcPtr;
	if(call->node()->in(0)->has<ConstantProperty>()) {
		const Node* closureNode = call->node()->in(0)->get<ConstantProperty>().value().closure()->node();
		funcPtr = _declarations[closureNode];
	} else {
		
		// Fetch the function pointer
		llvm::FunctionType* funcType = buildFunctionType(call->node()->inArrity() - 1, call->node()->outArrity());
		llvm::Value* closurePtr = _builder.CreateBitCast(args[0], _builder.getInt64Ty()->getPointerTo());
		llvm::Value* funcPtrIntPtr = _builder.CreateGEP(closurePtr, _builder.getInt64(0));
		llvm::Value* funcPtrInt = _builder.CreateLoad(funcPtrIntPtr);
		funcPtr = _builder.CreateIntToPtr(funcPtrInt, funcType->getPointerTo());
	}
	
	// Create the call
	std::string name = "";
	if(call->node()->in(0)->has<IdentifierProperty>())
		name = encodeUtf8(call->node()->in(0)->get<IdentifierProperty>().value());
	llvm::CallInst* result = _builder.CreateCall(funcPtr, args, name);
	result->setCallingConv(llvm::CallingConv::Fast);
	result->setOnlyReadsMemory();
	result->setDoesNotThrow();
	result->setTailCall();
	
	// Unpack the results to the stack
	if (call->numReturns() == 1) {
		_stack.push_back(result);
	} else
		for(int i = 0; i < call->numReturns(); ++i)
			_stack.push_back(_builder.CreateExtractValue(result, i));
	
	if(debug)
		wcerr << " END CALL " << endl;
}

void LlvmCompiler::buildAlloc(const StackMachineProperty::AllocateInstruction* alloc)
{
	if(debug)
		wcerr << "CLOSURE " << alloc->closure() << endl;
	
	const vector<const Edge*>& inputs = alloc->closure()->get<ClosureProperty>().edges();
	if(debug)
		wcerr << "closure edges = " <<  inputs << endl;
	
	// Return a constant when the closure is empty
	if(inputs.empty()) {
		_closures[alloc->closure()]->dump();
		_stack.push_back(_closures[alloc->closure()]);
		return;
	}
	
	// Allocate the closure
	int closureSize = alloc->closure()->get<ClosureProperty>().edges().size() + 1;
	llvm::Value* closure = 0;
	if(alloc->closure()->has<EscapeProperty>() && !alloc->closure()->get<EscapeProperty>().escapes()) {
		// Stack allocat the closure
		closure = _builder.CreateAlloca(_builder.getInt64Ty(), _builder.getInt64(closureSize));
	} else {
		// Heap allocat the closure
		closure = buildMalloc(closureSize);
	}
	
	// Push the closure
	llvm::Value* closurePtr = _builder.CreatePtrToInt(closure, _builder.getInt64Ty());
	_stack.push_back(closurePtr);
	
	// Store the function pointer
	llvm::Function* func = _declarations[alloc->closure()];
	llvm::Value* funcPtr = _builder.CreatePtrToInt(func, _builder.getInt64Ty());
	_builder.CreateStore(funcPtr, _builder.CreateGEP(closure, _builder.getInt64(0)));
}

void LlvmCompiler::buildStore(const StackMachineProperty::StoreInstruction* store)
{
	llvm::Value* closure = _builder.CreateIntToPtr(_stack[store->closure()], _builder.getInt64Ty()->getPointerTo());
	llvm::Value* value = _stack[store->value()];
	int slot = store->slot() + 1;
	llvm::Value* pointer = _builder.CreateGEP(closure, _builder.getInt64(slot));
	_builder.CreateStore(value, pointer);
}

void LlvmCompiler::buildRet(const StackMachineProperty::ReturnInstruction* ret)
{
	// Construct the return value
	if(debug)
		wcerr << "RETURNS" << endl;
	const vector<int>& resultStackpos = ret->returns();
	if(resultStackpos.size() == 1) {
		if(resultStackpos[0] == -1)
			_builder.CreateRet(buildConstant(ret->closure()->in(0)->get<ConstantProperty>().value()));
		else {
			assert(resultStackpos[0] < _stack.size());
			_builder.CreateRet(_stack[resultStackpos[0]]);
		}
	} else {
		vector<llvm::Value*> results;
		for(int i = 0; i < ret->closure()->inArrity(); ++i) {
			if(resultStackpos[i] == -1)
				results.push_back(buildConstant(ret->closure()->in(i)->get<ConstantProperty>().value()));
			else {
				assert(resultStackpos[i] < _stack.size());
				results.push_back(_stack[resultStackpos[i]]);
			}
		}
		_builder.CreateAggregateRet(results.data(), results.size());
	}
}

void LlvmCompiler::buildBuiltin(const StackMachineProperty::CallInstruction* call)
{
	if(debug)
		wcerr << "BUILTIN " << call->node() << endl;
	
	// Construct the arguments
	vector<const Edge*> inputs = call->node()->in();
	const vector<int>& stackPositions = call->arguments();
	vector<llvm::Value*> args;
	for(int i = 1; i < stackPositions.size(); ++i) {
		if(stackPositions[i] == -1)
			args.push_back(buildConstant(inputs[i]->get<ConstantProperty>().value()));
		else
			args.push_back(_stack[stackPositions[i]]);
	}
	
	// Dispatch on the name
	std::wstring name = call->node()->in(0)->get<IdentifierProperty>().value();
	if(name == L"if") {
		std::string name;
		if(call->node()->out(0)->has<IdentifierProperty>())
			name = encodeUtf8(call->node()->out(0)->get<IdentifierProperty>().value());
		llvm::Value* condition = _builder.CreateICmpNE(args[0], _builder.getInt64(0), name + "_cond");
		llvm::Value* result = _builder.CreateSelect(condition, args[1], args[2], name);
		_stack.push_back(result);
	} else  if(name == L"add") {
		std::string name;
		if(call->node()->out(0)->has<IdentifierProperty>())
			name = encodeUtf8(call->node()->out(0)->get<IdentifierProperty>().value());
		llvm::Value* result = _builder.CreateAdd(args[0], args[1], name);
		_stack.push_back(result);
	} else if(name == L"sub") {
		std::string name;
		if(call->node()->out(0)->has<IdentifierProperty>())
			name = encodeUtf8(call->node()->out(0)->get<IdentifierProperty>().value());
		llvm::Value* result = _builder.CreateSub(args[0], args[1], name);
		_stack.push_back(result);
	} else if(name == L"mul") {
		std::string name;
		if(call->node()->out(0)->has<IdentifierProperty>())
			name = encodeUtf8(call->node()->out(0)->get<IdentifierProperty>().value());
		llvm::Value* result = _builder.CreateMul(args[0], args[1], name);
		_stack.push_back(result);
	} else if(name == L"div") {
		std::string name;
		if(call->node()->out(0)->has<IdentifierProperty>())
			name = encodeUtf8(call->node()->out(0)->get<IdentifierProperty>().value());
		llvm::Value* dividend = _builder.CreateSDiv(args[0], args[1], name);
		llvm::Value* remainder = _builder.CreateSRem(args[0], args[1], name);
		_stack.push_back(dividend);
		_stack.push_back(remainder);
	} else if(name == L"head") {
		
		
	} else {
		wcerr << "Do not know builtin " << name  << endl;
		assert(false);
	}
}

llvm::Value* LlvmCompiler::buildConstant(const Value& value)
{
	switch(value.kind) {
	case Value::Integer:
		return _builder.getInt64(value.integer());
	case Value::Function: {
		const Node* closureNode = value.closure()->node();
		assert(closureNode->get<ClosureProperty>().edges().empty());
		return _closures[closureNode];
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
	llvm::Type* returns = 0;
	if(outArrity == 0) {
		returns = _builder.getVoidTy();
	} else if (outArrity == 1) {
		returns = _builder.getInt64Ty();
	} else {
		std::vector<llvm::Type*> returnTypes;
		for(int i = 0; i < outArrity; ++i) {
			llvm::Type* type = _builder.getInt64Ty();
			returnTypes.push_back(type);
		}
		returns = llvm::StructType::get(_context, returnTypes);
	}
	
	// Construct the function type
	llvm::FunctionType* functionType = llvm::FunctionType::get(returns, params, false);
	return functionType;
}

void LlvmCompiler::buildTrace(llvm::Value* value)
{
	if(value->getType()->isPointerTy())
		value = _builder.CreatePtrToInt(value, _builder.getInt64Ty());
	_builder.CreateCall(_trace, value);
}

llvm::Value* LlvmCompiler::buildMalloc(int size)
{
	// Allocate the space
	llvm::Value* space = _builder.CreateCall(_malloc, _builder.getInt64(size * 8) , "space");
	// assert(llvm::isMalloc(space));
	llvm::Value* pointer = _builder.CreateBitCast(space, _builder.getInt64Ty()->getPointerTo());
	return pointer;
}
