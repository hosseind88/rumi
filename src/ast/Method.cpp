#include "Method.h"
#include "Arg.h"
#include "PointerType.h"
#include "../base.h"
#include "Named.h"
#include "Struct.h"
#include "ast.h"

Method::Method(std::string structName, std::string methodName, Function *f)
  : structName(structName)
  , methodName(methodName)
  , f(f)
{}

void Method::compile(CC *cc){
  f->compile(cc);
}

void Method::prepare(CC *cc){
  // ensure struct exist
  Named *named = cc->lookup(structName);
  if(!named || !named->isType){
    graceFulExit(dbg, structName + " is not a type");
  }
  StructType *st = dynamic_cast<StructType *>(named->type);
  if(!st){
    graceFulExit(dbg, structName + " is not a struct type");
  }
  // register ourself to the struct
  st->addMethod(cc, this);

  // set name of f
  // struct$method
  f->id = structName + "$" + methodName;

  // add self as an argument to f
  Arg *selfArg = new Arg();
  selfArg->id = "self";
  selfArg->type = new PointerType(st);
  f->args.insert(f->args.begin() , selfArg);

  f->prepare(cc);
}

void Method::codegen(CC *cc){
  f->codegen(cc);
}
