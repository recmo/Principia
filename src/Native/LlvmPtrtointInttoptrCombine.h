#pragma once
#include <llvm/Pass.h>
#include <llvm/IR/BasicBlock.h>

/// Disable typeinfo on this class since LLVM does not provide it
class LlvmPtrtointInttoptrCombine: public llvm::BasicBlockPass
{
public:
	static char ID;
	explicit LlvmPtrtointInttoptrCombine();
	virtual ~LlvmPtrtointInttoptrCombine();
	virtual bool runOnBasicBlock(llvm::BasicBlock &BB);
};
