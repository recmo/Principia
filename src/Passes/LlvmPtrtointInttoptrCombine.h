#pragma once
#include <llvm/Pass.h>
#include <llvm/BasicBlock.h>

class LlvmPtrtointInttoptrCombine: public llvm::BasicBlockPass
{
public:
	static char ID;
	explicit LlvmPtrtointInttoptrCombine(): BasicBlockPass(ID) { }
	virtual bool runOnBasicBlock(llvm::BasicBlock &BB);
};
