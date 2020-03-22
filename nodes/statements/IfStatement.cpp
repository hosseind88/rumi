#include "IfStatement.h"
#include <llvm/IR/BasicBlock.h>
#include "../../Context.h"

IfStatement::~IfStatement() {
  delete i;
  if (e)
    delete e;
  delete exp;
}
void IfStatement::codegen(CodegenContext *cc){
  llvm::Function *f = cc->builder->GetInsertBlock()->getParent();

  llvm::BasicBlock *ifB = llvm::BasicBlock::Create(cc->context, "if", f),
    *elseB,
    *mergeB = llvm::BasicBlock::Create(cc->context, "ifcont");

  if(this->e)
    elseB = llvm::BasicBlock::Create(cc->context, "else");
  else
    elseB = mergeB;

  llvm::Value *cond = this->exp->exprGen(cc);

  cond = cc->builder->CreateICmpNE(cond, llvm::ConstantInt::get(llvm::Type::getInt64Ty(cc->context), 0, false), // TODO, maybe improve?
    "ifcond");

  cc->builder->CreateCondBr(cond, ifB, elseB);

  // If Body
  cc->block.push_back(new CodegenBlockContext(ifB));
  cc->builder->SetInsertPoint(ifB);
  this->i->codegen(cc);
  if (cc->builder->GetInsertPoint()
          ->getPrevNonDebugInstruction()
          ->getOpcode() != llvm::Instruction::Br) {
    cc->builder->CreateBr(mergeB);
  }
  cc->block.pop_back();

  // Else
  if(this->e){
    cc->block.push_back(new CodegenBlockContext(elseB));
    f->getBasicBlockList().push_back(elseB);
    cc->builder->SetInsertPoint(elseB);
    this->e->codegen(cc);
    if (cc->builder->GetInsertPoint()
            ->getPrevNonDebugInstruction()
            ->getOpcode() != llvm::Instruction::Br) {
      cc->builder->CreateBr(mergeB);
    }
    cc->block.pop_back();
  }

  f->getBasicBlockList().push_back(mergeB);
  cc->builder->SetInsertPoint(mergeB);
}

void IfStatement::compile(Context *cc){
  auto is = this; // TODO lazy
  is->exp->compile(cc);
  is->i->compile(cc);
  if (is->e)
    is->e->compile(cc);
}