/**
 * @file argparse.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-07 12:53:54
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <regex>

#include "argparse.hpp"

ArgParseRule::ArgParseRule(const std::map<std::string, unsigned int>& _rule) {
  for (auto& _ : _rule) {
    add_rule(_.first, _.second);
  }
}

ArgParseRule::ArgParseRule() {}

#define ARGRULE(target, type, hbit, lbit) \
  target[arg] = static_cast<type>(rule & ((1u << hbit) - (1u << lbit)));

void ArgParseRule::add_rule(const std::string& arg, unsigned int rule) {
  ARGRULE(typerule_, ArgParseTyperule, 20, 0)
  ARGRULE(showrule_, ArgParseShowrule, 22, 20)
}

#undef ARGRULE

#define ARGERROR(info) \
  do {                 \
    error_ = info;     \
    return -1;         \
  } while (0)

#define PRMCHECK(trule, rgex, tname)                                     \
  case ArgParseTyperule::trule: {                                        \
    if (!std::regex_match(param, std::regex(rgex)))                      \
      ARGERROR(std::string("invalid ") + tname + " value at position " + \
               std::to_string(argi + 1) + ": " + param);                 \
    break;                                                               \
  }

int ArgParseRule::accept(const int argc, const char* const* const argv,
                         std::map<std::string, std::string>* result) const {
  result->clear();
  error_ = "";

  for (int argi = 1; argi < argc; ++argi) {
    std::string argt = argv[argi];
    if (argt[0] != '-') ARGERROR("excess parameter: " + argt);

    // remove leading bar
    std::reverse(argt.begin(), argt.end());
    while (argt.back() == '-') argt.pop_back();
    std::reverse(argt.begin(), argt.end());
    if (argt.length() <= 0)
      ARGERROR("argument without content at position " + std::to_string(argi));

    // what the heck is this argument ?!
    if (!typerule_.count(argt) || !showrule_.count(argt))
      ARGERROR("unrecognized argument: " + std::string(argv[argi]));

    // check for duplicates
    if (showrule_.at(argt) == ArgParseShowrule::apshow_once &&
        result->count(argt))
      ARGERROR("duplicate argument at position " + std::to_string(argi) + ": " +
               argt);

    // if argument does not have a parameter
    if (argi >= argc - 1 || argv[argi + 1][0] == '-') {
      // if argument requires a parameter then go erroneous
      if (typerule_.at(argt) != ArgParseTyperule::aptype_free &&
          typerule_.at(argt) != ArgParseTyperule::aptype_opt_only)
        ARGERROR(("missing parameter at position " + std::to_string(argi)) +
                 ": " + argt);

      (*result)[argt] = "";
    } else {  // has a parameter
      std::string param = argv[argi + 1];
      // strip quotes for the parameter
      while (param.back() == '\"') param.pop_back();
      std::reverse(param.begin(), param.end());
      while (param.back() == '\"') param.pop_back();
      std::reverse(param.begin(), param.end());

      switch (typerule_.at(argt)) {
        case ArgParseTyperule::aptype_opt_only: {
          ARGERROR("excess parameter at position " + std::to_string(argi + 1) +
                   ": " + param);
        }
          PRMCHECK(aptype_free, ".+", "string")
          PRMCHECK(aptype_is_str, ".+", "string")
          PRMCHECK(aptype_is_int, "-?[1-9]\\d*|0", "integer")
          PRMCHECK(aptype_is_uint, "[1-9]\\d*|0", "unsigned integer")
          PRMCHECK(aptype_is_char, ".?", "char")
          PRMCHECK(aptype_is_bool, "[tT]rue|[fF]alse", "boolean")
          PRMCHECK(aptype_is_real, "(-?\\d+)(\\.\\d+)?", "floating")
        default: {
          ARGERROR("corrupted argparse rule");
        }
      }
      (*result)[argt] = param;
      argi++;
    }  // if (argi)
  }

  // check strict arguments
  for (auto& i : showrule_) {
    if (i.second == apshow_strict && !(result->count(i.first))) {
      ARGERROR("missing parameter: " + i.first);
    }
  }

  return 0;
}

std::string ArgParseRule::error() const { return error_; }

#undef PRMCHECK
#undef ARGERROR
