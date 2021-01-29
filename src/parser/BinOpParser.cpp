#include "BinOpParser.h"
#include "Symbols.h"
#include "../Source.h"
#include "../ast/BinOp.h"
#include <iostream>

int precSymbol(Symbol op){
  switch(op){
  case s_andand:
  case s_pipepipe:
    return 0;
  case s_eqeq:
  case s_neq:
  case s_gt:
  case s_gte:
  case s_lt:
  case s_lte:
    return 1;
  case s_plus:
  case s_minus:
    return 2;
  case s_mult:
  case s_div:
  case s_rem:
    return 3;
  }
  std::cout << "UNKNOWN OP IN PREC " << std::endl;
  exit(1);
}

BinOpToken::BinOpToken(Symbol op, Token *lhs, Token *rhs, CC *cc,
                       Source *s, int pos, int epos)
  : op(op)
  , lhs(lhs)
  , rhs(rhs)
    {
  this -> cc = cc;
  this -> s = s;
  this -> spos = pos;
  this -> epos = epos;

  prec = precSymbol(op);
}

AST *BinOpToken::toAST(CC* cc){
  if(BinOpToken *r = dynamic_cast<BinOpToken*>(rhs)){
    // handle the precedence
    if(r->prec < prec){
      // swap the op
      auto tmpO = op;
      op = r->op;
      r->op = tmpO;

      auto hs = lhs;
      lhs = rhs;
      rhs = hs;

      hs = r->lhs;
      r->lhs = r->rhs;
      r->rhs = hs;

      hs = r->lhs;
      r->lhs = rhs;
      rhs = hs;
    }
  }
  std::string ops = symbolDesc(op);
  return new BinOp((Expression *)lhs->toAST(cc), ops, (Expression *)rhs->toAST(cc));
}

std::string BinOpToken::desc(){
  return "(" + lhs -> desc() + " " + symbolDesc(op) + " " + rhs -> desc() + ")";
}

ParseResult BinOpParser::scheme(CC *cc, Source *s, int pos){
  ParseState *ps = s->resolveState(pos);
  if(ps->hasParser("binOp")) return ps->getToken("binOp"); // recursive
  ps->setToken("binOp", 0);

  Token* tmp =  innerscheme(cc, s, pos).token;

  // tmp ((lhs, op), rhs)

  if(tmp){
    TupleToken *t = (TupleToken*) tmp; 
    TupleToken *tt = (TupleToken *) t->t1;
    tmp = new BinOpToken(((SymbolToken *)tt->t2)->sb, tt->t1, t->t2, cc, s, t->spos, t->epos);
  }

  ps->setToken("binOp", tmp);
  return ParseResult(tmp);
}

ParseResult BinOpParser::innerscheme(CC *cc, Source *s, int pos){
  // exp op exp
  auto v1 = vp.parse(cc, s, pos);

  // op = +?
  auto tmp = v1 >> addp;
  if(tmp) return tmp >> vp;

  // op = -?
  tmp = v1 >> minusp;
  if(tmp) return tmp >> vp;

  // op = *?
  tmp = v1 >> multp;
  if(tmp) return tmp >> vp;

  // op = /?
  tmp = v1 >> divp;
  if(tmp) return tmp >> vp;

  // op = %?
  tmp = v1 >> remp;
  if(tmp) return tmp >> vp;

  // op = ==?
  tmp = v1 >> eqp;
  if(tmp) return tmp >> vp;

  // op = !=?
  tmp = v1 >> neqp;
  if(tmp) return tmp >> vp;

  // op = >?
  tmp = v1 >> gtp;
  if(tmp) return tmp >> vp;

  // op = >=?
  tmp = v1 >> gtep;
  if(tmp) return tmp >> vp;

  // op = <?
  tmp = v1 >> ltp;
  if(tmp) return tmp >> vp;

  // op = <=?
  tmp = v1 >> ltep;
  if(tmp) return tmp >> vp;

  // op = &&?
  tmp = v1 >> andp;
  if(tmp) return tmp >> vp;

  // op = ||?
  tmp = v1 >> orp;
  if(tmp) return tmp >> vp;

  return 0;
}


BinOpParser::BinOpParser()
  : addp(s_plus)
  , minusp(s_minus)
  , multp(s_mult)
  , divp(s_div)
  , remp(s_rem)
  , eqp(s_eqeq)
  , neqp(s_neq)
  , gtp(s_gt)
  , gtep(s_gte)
  , ltp(s_lt)
  , ltep(s_lte)
  , andp(s_andand)
  , orp(s_pipepipe)
{}
