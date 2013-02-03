#include <Passes/LlvmPtrtointInttoptrCombine.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
using namespace llvm;

char LlvmPtrtointInttoptrCombine::ID = 0;

static RegisterPass<LlvmPtrtointInttoptrCombine> X("PtrtointInttoptrCombine", "PtrToInt / IntToPtr Combine Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

LlvmPtrtointInttoptrCombine::LlvmPtrtointInttoptrCombine(): BasicBlockPass(ID)
{
}

LlvmPtrtointInttoptrCombine::~LlvmPtrtointInttoptrCombine()
{
}

bool LlvmPtrtointInttoptrCombine::runOnBasicBlock(BasicBlock& BB)
{
	for (BasicBlock::iterator i = BB.begin(), end = BB.end(); i != end;) {
		
		// inttoptr(ptrtoint ty Y) to ty -> Y 
		if(IntToPtrInst *i2p = dyn_cast<IntToPtrInst>(i++)) {
			if(PtrToIntInst *p2i = dyn_cast<PtrToIntInst>(i2p->getOperand(0))) {
				if(p2i->getSrcTy() == i2p->getDestTy()){
					i2p->replaceAllUsesWith(p2i->getOperand(0));
					i2p->eraseFromParent();
					if(p2i->use_empty()) {
						// If this is the only use of the cast delete it.
						p2i->eraseFromParent();
					}
				}
			}
		}
		
	}
	return true;
}
