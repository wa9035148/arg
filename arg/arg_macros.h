#pragma once

#define arg_begin(...)                      arg::parser args(argc,argv,##__VA_ARGS__)
#define arg_end                             args.check()

// for global variables
#define arg_g(var,...)                      args.def(var,#var,##__VA_ARGS__)

#define arg_i(var,...)   int var;           args.def(var,#var,##__VA_ARGS__)
#define arg_l(var,...)   long int var;      args.def(var,#var,##__VA_ARGS__)
#define arg_ll(var,...)  long long int var; args.def(var,#var,##__VA_ARGS__)
#define arg_d(var,...)   double var;        args.def(var,#var,##__VA_ARGS__)
#define arg_b(var,...)   bool var;          args.def(var,#var,##__VA_ARGS__)
#define arg_s(var,...)   std::string var;   args.def(var,#var,##__VA_ARGS__)

#define arg_iv(var,...)  std::vector<int> var;         args.def(var,#var,##__VA_ARGS__)
#define arg_dv(var,...)  std::vector<double> var;      args.def(var,#var,##__VA_ARGS__)
#define arg_sv(var,...)  std::vector<std::string> var; args.def(var,#var,##__VA_ARGS__)
