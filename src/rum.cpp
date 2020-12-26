#include "base.h"
#include <ostream>

#include "parser/parser.h"

int main(int argc, char **argv) {
  CompileContext cc(argc, argv);

  for (Source *s : cc.sources) {
    cc.debug(Verbosity::LOW) << *s << std::endl;
  }

  cc.debug(Verbosity::MEDIUM) << "Context created." << std::endl;

  for(Source *s: cc.sources){
    cc.load(s);
    Token *t;
    t = cc.parser.parseTop(s);
    while(t){
      cc.debug(Verbosity::NONE) << *t << std::endl;
      t = cc.parser.parseTop(s, t->epos + 1);
    }
  }


  return 0;
}
