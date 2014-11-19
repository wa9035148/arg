#include <cstdio>
#include <cstdio>

#include "arg.h"

int main(int argc, char** argv) {
  arg_begin("IN1 IN2", 2);
  arg_i(int_argument).desc("an argument which takes an integer");
  arg_end;

  for (int i=0; i<args.size(); i++)
    std::cout << "args[" << i << "] = " << args[i] << std::endl;

  std::cout << "int_argument = " << int_argument << std::endl;
}
