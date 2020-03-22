#include "WhileStatement.h"
#include "../../Context.h"


WhileStatement::~WhileStatement(){
    delete w;
    delete exp;
  }
void WhileStatement::codegen(Context *cc){
  // TODO

  llvm::Function *f = cc->builder->GetInsertBlock()->getParent();

  llvm::BasicBlock
    *condB = llvm::BasicBlock::Create(cc->context, "whilecond", f),
    *whileB = llvm::BasicBlock::Create(cc->context, "while"),
    *mergeB = llvm::BasicBlock::Create(cc->context, "whilecont");

  cc->builder->CreateBr(condB);

  // While cond
  cc->blocks.push_back(new BlockContext(condB));
  cc->builder->SetInsertPoint(condB);
  llvm::Value *cond = this->exp->exprGen(cc);
  cond = cc->builder->CreateICmpNE(cond, llvm::ConstantInt::get(llvm::Type::getInt64Ty(cc->context), 0, false), "whilecond"); // TODO improve?
  cc->builder->CreateCondBr(cond, whileB, mergeB);
  cc->blocks.pop_back();

  // While Body
  f->getBasicBlockList().push_back(whileB);
  cc->blocks.push_back(new BlockContext(whileB));
  cc->builder->SetInsertPoint(whileB);
  this->w->codegen(cc);
  if (cc->builder->GetInsertPoint()
          ->getPrevNonDebugInstruction()
          ->getOpcode() != llvm::Instruction::Br) {
    cc->builder->CreateBr(condB);
  }
  cc->blocks.pop_back();

  // Cont
  f->getBasicBlockList().push_back(mergeB);
  cc->builder->SetInsertPoint(mergeB);
}

void WhileStatement::compile(Context *cc){
  auto ws = this; // TODO lazy
  ws->exp->compile(cc);
  ws->w->compile(cc);
  return;
}

