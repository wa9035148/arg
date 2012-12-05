#ifndef _ARG_H_
#define _ARG_H_

// Shortest option parser for C++
// https://github.com/anjn/arg


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>

#define arg_begin(...) arg arg(argc,argv,##__VA_ARGS__)
#define arg_end   arg.check()
#define argi(var,...)  int var; arg(var,#var,##__VA_ARGS__)
#define argd(var,...)  double var; arg(var,#var,##__VA_ARGS__)
#define argb(var,...)  bool var; arg(var,#var,##__VA_ARGS__)
#define args(var,...)  char var[1024]; arg(var,#var,##__VA_ARGS__)

namespace arg_helper {
  struct helper {
    static bool is_positive_integer(char *c) {
      while (*c != 0) {
        if (!isdigit(*c)) return false;
        c++;
      }
      return true;
    }
    static bool is_integer(char *c) {
      if (*c == '-') c++;
      return is_positive_integer(c);
    }
    static bool is_double(char *c) {
      if (*c == '-') c++;
      while (*c != 0) {
        if (!isdigit(*c) && *c != '.') return false;
        if (*c == '.') { c++; break; }
        c++;
      }
      return is_positive_integer(c);
    }
  };
  
  // abstract parser
  template<typename T>
  struct parser {
    static void assign(T& target, T value);
    static void parse(T& target, char *value);
    static bool valid(char *value);
    static bool require_value();
  };

  template<>
  struct parser<int> {
    static void assign(int& target, int value) {
      target = value;
    }
    static void parse(int& target, char *value) {
      assign(target, atoi(value));
    }
    static bool valid(char *value) {
      return helper::is_integer(value);
    }
    static bool require_value() {
      return true;
    }
  };
  
  template<>
  struct parser<double> {
    static void assign(double& target, double value) {
      target = value;
    }
    static void parse(double& target, char *value) {
      assign(target, atof(value));
    }
    static bool valid(char *value) {
      return helper::is_double(value);
    }
    static bool require_value() {
      return true;
    }
  };

  template<>
  struct parser<bool> {
    static void assign(bool& target, bool value) {
      target = value;
    }
    static void parse(bool& target, char *value) {
      assign(target, true);
    }
    static bool valid(char *value) {
      return true;
    }
    static bool require_value() {
      return false;
    }
  };

  template<>
  struct parser<char*> {
    static void assign(char*& target, char* value) {
      strcpy(target, value);
    }
    static void parse(char*& target, char *value) {
      assign(target, value);
    }
    static bool valid(char *value) {
      return strlen(value) > 0;
    }
    static bool require_value() {
      return true;
    }
  };
  
  template<int N>
  struct parser<char[N]> {
    static void parse(char target[N], char *value) {
      strncpy(target, value, N);
    }
    static bool valid(char *value) {
      return strlen(value) > 0;
    }
    static bool require_value() {
      return true;
    }
  };
}

class arg {
private:
  typedef std::pair<char*,int> arg_val; // arg_value, arg_position
  typedef std::map<char*,arg_val> arg_map; // option_name -> arg_val
  arg_map options;
  std::set<char*> parsed_options;
  std::vector<arg_val> args;
  const char *EMPTY_ARG;
  const char *progname;
  std::vector<std::string> errors;
  std::vector<std::string> help_strs;
  int require_arg_min;
  int require_arg_max;
  
#define ERROR(...) do {                         \
    std::ostringstream err;                     \
    err << __VA_ARGS__;                         \
    errors.push_back(err.str());                \
  } while (0)
  
public:
  bool replace_underscore_with_hyphen;

  arg(int& argc, char** argv): 
    EMPTY_ARG(""), progname(argv[0]),
    require_arg_min(-1), require_arg_max(-1),
    replace_underscore_with_hyphen(true) {
    parse_arg(argc, argv);
  }
  arg(int& argc, char** argv, const char* prog): 
    EMPTY_ARG(""), progname(prog),
    require_arg_min(-1), require_arg_max(-1),
    replace_underscore_with_hyphen(true) {
    parse_arg(argc, argv);
  }
  arg(int& argc, char** argv, const int arg_min):
    EMPTY_ARG(""), progname(argv[0]),
    require_arg_min(arg_min), require_arg_max(-1),
    replace_underscore_with_hyphen(true) {
    parse_arg(argc, argv);
  }
  arg(int& argc, char** argv, const int arg_min, const int arg_max):
    EMPTY_ARG(""), progname(argv[0]),
    require_arg_min(arg_min), require_arg_max(arg_max),
    replace_underscore_with_hyphen(true) {
    parse_arg(argc, argv);
  }
  arg(int& argc, char** argv, const char* prog, const int arg_min):
    EMPTY_ARG(""), progname(prog),
    require_arg_min(arg_min), require_arg_max(-1),
    replace_underscore_with_hyphen(true) {
    parse_arg(argc, argv);
  }
  arg(int& argc, char** argv, const char* prog, const int arg_min, const int arg_max):
    EMPTY_ARG(""), progname(prog),
    require_arg_min(arg_min), require_arg_max(arg_max),
    replace_underscore_with_hyphen(true) {
    parse_arg(argc, argv);
  }
  ~arg() {
    check();
  }
  
  // for option with value
  // with long opt
  template<typename T>
  void operator()(T& target, const char* lopt) {
    parse(target, lopt, 0, NULL);
  }
  template<typename T>
  void operator()(T& target, const char* lopt, const char* help) {
    parse(target, lopt, 0, help);
  }
  template<typename T>
  void operator()(T& target, const char* lopt, T defval) {
    parse(target, lopt, 0, NULL, defval);
  }
  template<typename T>
  void operator()(T& target, const char* lopt, const char* help, T defval) {
    parse(target, lopt, 0, help, defval);
  }
  void operator()(char target[], const char* lopt, const char* help, const char* defval) {
    char* s = (char*) target;
    parse(s, lopt, 0, help, (char*) defval);
  }
  // with short opt
  template<typename T>
  void operator()(T& target, const char sopt) {
    parse(target, NULL, sopt, NULL);
  }
  template<typename T>
  void operator()(T& target, const char sopt, const char* help) {
    parse(target, NULL, sopt, help);
  }
  template<typename T>
  void operator()(T& target, const char sopt, const T defval) {
    parse(target, NULL, sopt, NULL, defval);
  }
  template<typename T>
  void operator()(T& target, const char sopt, const char* help, T defval) {
    parse(target, NULL, sopt, help, defval);
  }
  void operator()(char target[], const char sopt, const char* help, const char* defval) {
    char* s = (char*) target;
    parse(s, NULL, sopt, help, (char*) defval);
  }
  // with long opt & short opt
  template<typename T>
  void operator()(T& target, const char* lopt, const char sopt) {
    parse(target, lopt, sopt, NULL);
  }
  template<typename T>
  void operator()(T& target, const char* lopt, const char sopt, const char* help) {
    parse(target, lopt, sopt, help);
  }
  template<typename T>
  void operator()(T& target, const char* lopt, const char sopt, T defval) {
    parse(target, lopt, sopt, NULL, defval);
  }
  template<typename T>
  void operator()(T& target, const char* lopt, const char sopt, const char* help, T defval) {
    parse(target, lopt, sopt, help, defval);
  }
  void operator()(char target[], const char* lopt, const char sopt, const char* help, const char* defval) {
    char* s = (char*) target;
    parse(s, lopt, sopt, help, (char*) defval);
  }

  void help() {
    fprintf(stderr, "Usage: %s\n\n", progname);
    for (std::vector<std::string>::iterator h = help_strs.begin(); 
         h != help_strs.end(); h++) {
      fprintf(stderr, "%s\n", h->c_str());
    }
    exit(0);
  }
  
  // error check
  void check() {
    // check help
    bool show_help;
    operator()(show_help, "help", 'h', "Show this message", false);
    if (show_help) help();
    
    // check unknow option
    for (arg_map::iterator o = options.begin(); o != options.end(); o++) {
      if (parsed_options.find(o->first) == parsed_options.end()) {
        ERROR("unknown option '" << o->first << "'!");
      }
    }
    // check num args
    int argc = *argc_ptr - 1;
    if (require_arg_min >= 0 && require_arg_max >= 0) {
      if (argc < require_arg_min || argc > require_arg_max) {
        if (require_arg_min == require_arg_max)
          ERROR("invalid argument " << argc << " for " << require_arg_min);
        else
          ERROR("invalid argument " << argc << " for " << 
                require_arg_min << "-" << require_arg_max);
      }
    } else if (require_arg_min >= 0) {
      if (argc < require_arg_min)
        ERROR("invalid argument " << argc << " for " << require_arg_min << "-");
    }
    // check error
    if (errors.size() > 0) {
      for (std::vector<std::string>::iterator err = errors.begin(); 
           err != errors.end(); err++) {
        fprintf(stderr, "error: %s\n", err->c_str());
      }
      exit(1);
    }
  }
  
  // argument access
  char *operator[](int i) {
    return args.at(i).first;
  }
  int size() {
    return args.size();
  }
  
private:
  int* argc_ptr;
  char** argv_ptr;
  
  // parse arguments
  void parse_arg(int& argc, char** argv) {
    int argpos = 1;
    char *opt = NULL;
    arg_val empty((char*) EMPTY_ARG, -1);
    while (argpos < argc) {
      char* arg = argv[argpos];
      arg_val val(arg,argpos);
      
      if (strncmp(arg, "--", 2) == 0 || 
          (arg[0] == '-' && isalpha(arg[1]))) {
        // option name
        if (opt != NULL) options[opt] = empty;
        // TODO handle "-I."
        opt = arg;
      } else if (opt != NULL) {
        // option argument
        options[opt] = val;
        opt = NULL;
      } else {
        // argument
        args.push_back(val);
      }
      argpos++;
    }
    if (opt != NULL) options[opt] = empty;
    // TODO handle "-I."
    
    // overwrite argc/argv
    argc_ptr = &argc;
    argv_ptr = argv;
    overwrite_args();
  }
  
  void overwrite_args() {
    *argc_ptr = 1 + args.size();
    for (size_t i = 0; i < args.size(); i++) 
      argv_ptr[1+i] = args[i].first;
  }
  
  // parse each option
  template<typename T>
  bool parse_opt(T& target, const char* lopt, const char sopt,
                 const char* help, std::string option_name) {
    // generate help message
    {
      std::ostringstream h;
      h << "  ";
      if (sopt != 0 && lopt != NULL)
        h << "-" << sopt << ", --" << lopt;
      else if (sopt != 0)
        h << "-" << sopt;
      else
        h << "    --" << lopt;
      int len = 30 - h.str().size();
      for (int i=0; i<len; i++) h << " ";
      h << help;
      help_strs.push_back(h.str());
    }
    
    // parse option value
    for (arg_map::iterator o = options.begin(); o != options.end(); o++) {
      char *name  = o->first;
      char *value = o->second.first;
      if ((lopt != NULL && strlen(name) > 2 && strcmp(name+2, lopt) == 0) ||
          (strlen(name) == 2 && name[1] == sopt)) {
        if (arg_helper::parser<T>::require_value() && value == EMPTY_ARG) {
          // empty
          ERROR(option_name << " requires value!");
        } else if (!arg_helper::parser<T>::valid(value)) {
          // invalid input format
          ERROR("input '" << value << "' is invalid for " << option_name << "!");
        } else {
          arg_helper::parser<T>::parse(target, value);
        }
        
        if (!arg_helper::parser<T>::require_value() && value != EMPTY_ARG) {
          // insert to args
          int argpos = o->second.second;
          for (std::vector<arg_val>::iterator a=args.begin(); a!=args.end(); a++) {
            if (argpos < a->second) {
              args.insert(a, o->second);
              break;
            }
          }
          overwrite_args();
        }
        
        parsed_options.insert(name);
        return true;
      }
    }
    return false;
  }

  std::string fix_long_option(const char *lopt) {
    std::string str(lopt);
    for (std::string::iterator it = str.begin(); it != str.end(); it++) {
      if (replace_underscore_with_hyphen && *it == '_') *it = '-';
    }
    return str;
  }
  
  std::string optname(const char* lopt, const char sopt) {
    std::ostringstream name;
    if (lopt != NULL && sopt != 0)
      name << "option '-" << sopt << ",--" << lopt << "'";
    else if (lopt != NULL)
      name << "option '--" << lopt << "'";
    else
      name << "option '-" << sopt << "'";
    return name.str();
  }
  
  template<typename T>
  void parse(T& target, const char* lopt, const char sopt, const char* help) {
    std::string lopt_fix = fix_long_option(lopt);
    std::string name = optname(lopt_fix.c_str(), sopt);
    if (!parse_opt(target, lopt_fix.c_str(), sopt, help, name))
      ERROR(name << " was not specified!");
  }

  template<typename T>
  void parse(T& target, const char* lopt, const char sopt,
             const char* help, T defval) {
    std::string lopt_fix = fix_long_option(lopt);
    std::string name = optname(lopt_fix.c_str(), sopt);
    if (!parse_opt(target, lopt_fix.c_str(), sopt, help, name))
      arg_helper::parser<T>::assign(target, defval);
  }
  
#undef ERROR
  
};

#endif /* _ARG_H_ */
