#include <Passes/LlvmStoreLoadCombine.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
using namespace llvm;

char LlvmStoreLoadCombine::ID = 0;

static RegisterPass<LlvmStoreLoadCombine> X("StoreLoadCombine", "Store / Load Combine Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);



LlvmStoreLoadCombine::LlvmStoreLoadCombine()
: BasicBlockPass(ID)
{
}

LlvmStoreLoadCombine::~LlvmStoreLoadCombine()
{
}

bool LlvmStoreLoadCombine::runOnBasicBlock(BasicBlock& BB)
{
	for (BasicBlock::iterator i = BB.begin(), end = BB.end(); i != end;) {
		
		// store ty Y X, load ty Y X
		if(StoreInst *store = dyn_cast<StoreInst>(i++)) {
			Value* ptr = store->getPointerOperand();
			for(Value::use_iterator use = ptr->use_begin(), end = ptr->use_end(); use != end; ++use) {
				if(LoadInst* load = dyn_cast<LoadInst>(*use)) {
					load->replaceAllUsesWith(store->getValueOperand());
					load->eraseFromParent();
				}
			}
		}
		
	}
	return true;
}
