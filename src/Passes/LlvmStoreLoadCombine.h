#pragma once
#include <llvm/Pass.h>
#include <llvm/BasicBlock.h>

class LlvmStoreLoadCombine: public llvm::BasicBlockPass
{
public:
	static char ID;
	explicit LlvmStoreLoadCombine(): BasicBlockPass(ID) { }
	virtual bool runOnBasicBlock(llvm::BasicBlock &BB);
};
