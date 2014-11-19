#pragma once

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <libgen.h> // basename

#include "arg_helper.h"

namespace arg {

typedef std::vector<std::string>::iterator iterator;

struct desc {
  const std::string& value;
  explicit desc(const std::string& _value): value(_value) {}
};

struct parser {
  typedef std::vector<std::string> str_vec;

  std::string prog_name;
  str_vec arg_list;
  const std::string usage;
  const int min_arg_count;
  const int max_arg_count;
  str_vec help_messages;
  bool show_help;
  std::string indent;

  parser(int argc, char** argv, const std::string& usage_ = "",
         int min_ = -1, int max_ = -1):
    usage(usage_),
    min_arg_count(min_),
    max_arg_count(std::max(min_arg_count,max_)),
    prog_name(basename(argv[0])),
    show_help(false), indent("    ")
  {
    // initialize argument vector
    for (int i=1; i<argc; i++) arg_list.push_back(argv[i]);

    // define default help option
    def(show_help, "help", false).desc("Show this help and exit.");
  }

  void check() {
    // help flag passed?
    if (show_help) help();

    // check invalid option
    for (iterator it = arg_list.begin(); it != arg_list.end(); ++it) {
      if ("--" == *it) {
        // remove first "--"
        erase(it);
        // don't check after "--"
        break;
      } else if ("--" == (*it).substr(0,2)) {
        // found unknown option
        helper::util::error("unknown option '" + *it + "'!");
      }
    }

    // check the number of arguments
    if (min_arg_count >= 0 && arg_list.size() < min_arg_count) {
      std::ostringstream message;
      message <<
        "the number of arguments is less than expected! (expected at least: "
        << min_arg_count << ", passed: " << arg_list.size() << ")";
      helper::util::error(message.str());
      help();
    }
    if (max_arg_count >= 0 && arg_list.size() > max_arg_count) {
      std::ostringstream message;
      message <<
        "the number of arguments is greater than expected! (expected at most: "
        << min_arg_count << ", passed: " << arg_list.size() << ")";
      helper::util::error(message.str());
    }
  }

  // show help message and then exit
  void help() {
    //std::cerr << "usage: " << prog_name << " " << usage << std::endl;
    std::cerr << "USAGE" << std::endl;
    std::cerr << indent << prog_name << " " << usage << std::endl << std::endl;
    std::cerr << "OPTIONS" << std::endl;
    for (iterator it = help_messages.begin();
         it != help_messages.end(); it++) {
      std::cerr << *it;
    }
    std::exit(1);
  }

  // container delegations
  iterator begin() { return arg_list.begin(); }
  iterator end() { return arg_list.end(); }
  iterator erase(const iterator& pos) {
    return arg_list.erase(pos);
  }
  iterator erase(const iterator& first, const iterator& last) {
    return arg_list.erase(first, last);
  }
  const std::string& operator[](int i) {
    if (i >= arg_list.size())
      helper::util::error("index is out of range!");
    return arg_list[i];
  }

  template<typename T>
  bool parse(T& var, const std::string& name) {
    for (iterator it = arg_list.begin(); it != arg_list.end(); ++it) {
      if (name == *it) {
        // get value string
        iterator value_it = helper::option<T>::require_value() ? it+1 : it;
        if (value_it == arg_list.end())
          helper::util::error("option '" + name + "' requires value!");
        // cast string to value
        try {
          var = helper::option<T>::cast(*value_it);
        } catch (const std::invalid_argument& ex) {
          std::string opt_desc = helper::option<T>::name();
          if (!opt_desc.empty()) opt_desc = " " + opt_desc;
          helper::util::error(std::string(ex.what()) + "! given '" + *value_it +
              "' to the option '" + name + opt_desc + "'");
        }
        erase(it, value_it+1);
        return true; // match
      } else if ("--" == *it) {
        // ignore options after "--"
        break;
      }
    }
    return false;
  }

  template<typename T>
  struct arg_info {
    parser* p;
    T& var;
    const std::string& name;
    const std::string longname;
    std::vector<std::string> desc_;
    bool has_default_value;
    std::string default_value;

    arg_info(parser* _p, T& _var, const std::string& _name):
      p(_p), var(_var), name(_name),
      longname("--" + helper::util::replace_all(name, '_', '-')),
      has_default_value(false), default_value("") {}

    // do parse at dtor
    ~arg_info() {
      p->parse(*this);
    }

    arg_info<T>& desc(const std::string& _desc) {
      desc_.push_back(_desc);
      return *this;
    }

    const std::vector<std::string>& desc() const { return desc_; }
  };

  template<typename T>
  void gen_help(const arg_info<T>& ai) {
    std::ostringstream str;

    // display option name
    str << indent << ai.longname << " " << helper::option<T>::name();

    // display default value
    if (!ai.default_value.empty()) {
      str << "   (default: " << ai.default_value << ")";
    }
    str << std::endl;

    // display message
    if (!ai.desc().empty()) {
      for (std::vector<std::string>::const_iterator it = ai.desc().begin();
          it != ai.desc().end(); it++) {
        str << indent << indent << *it << std::endl;
      }
      str << std::endl;
    }

    help_messages.push_back(str.str());
  }

  template<typename T>
  void parse(arg_info<T>& ai) {
    gen_help<T>(ai);
    if (parse(ai.var, ai.longname) || ai.has_default_value) return;
    if (!show_help) helper::util::error("option '" + ai.longname + "' is required!");
  }

  template<typename T>
  arg_info<T> def(T& var, const std::string& name) {
    arg_info<T> ai(this, var, name);
    return ai;
  }

  template<typename T>
  arg_info<T> def(T& var, const std::string& name, const T& defval) {
    arg_info<T> ai(this, var, name);
    var = defval;
    ai.has_default_value = true;
    return ai;
  }

  template<typename T>
  arg_info<T> def(T& var, const std::string& name, const char* defval) {
    arg_info<T> ai(this, var, name);
    try {
      var = helper::option<T>::cast(defval);
    } catch (const std::invalid_argument& ex) {
      std::string opt_desc = helper::option<T>::name();
      if (!opt_desc.empty()) opt_desc = " " + opt_desc;
      helper::util::error(std::string(ex.what()) + "! given '" + defval +
          "' for the default value '" + ai.longname + opt_desc + "'");
    }
    ai.has_default_value = true;
    ai.default_value = std::string(defval);
    return ai;
  }
};

}
