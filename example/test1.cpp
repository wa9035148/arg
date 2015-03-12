#include <cstdio>

#include "arg.h"

int main(int argc, char** argv) {
  arg_begin("INT1 INT2", 2);
  arg_i(int_option, 0, "An option which takes an integer value.");
  arg_b(bool_option, false, "An option which takes an bool value.");
  arg_end;

  for (int i=0; i<args.size(); i++) {
    try {
      std::cout << "args[" << i << "] = " << args.as<int>(i) << std::endl;
    } catch (const std::invalid_argument& e) {
      std::cerr << "argument is not integer!" << std::endl;
      std::exit(1);
    }
  }

  std::cout << "int_option = " << int_option << std::endl;
  std::cout << "bool_option = " << bool_option << std::endl;
}
