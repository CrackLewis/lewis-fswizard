/**
 * @file argparse.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-07 12:47:53
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ARGPARSE_HPP_
#define ARGPARSE_HPP_

// 挺久以前写的命令行参数解析器了，移植过来。

#include <map>
#include <string>
#include <vector>

/**
 * @brief ArgParseTyperule
 *
 * ArgParseTyperule indicates what rule the appendice parameter of an argument
 * shall follow.
 *
 * We assume that NO MORE THAN one appendice parameter is needed per argument.
 *
 * ATTENTION: The element value SHALL be a 2-based power and not exceed 0xfffff.
 */
enum ArgParseTyperule : unsigned int {
  aptype_free = 0u,        // accepts 0 or 1 param
  aptype_is_str = 1u,      // accepts 1 param of any content
  aptype_is_int = 2u,      // this is deliberate, do not ask me why.
  aptype_is_uint = 4u,     // accepts 1 param which is an unsigned decimal
  aptype_is_char = 8u,     // accepts 1 param of 1 length
  aptype_is_bool = 0x10u,  // accepts 1 param of bool value (case insensitive)
  aptype_is_real = 0x20u,  // accepts 1 param of real value (fixed only)
  aptype_opt_only = 0x80000u,  // accepts 0 param
  aptype_vmax = 0xffffffffu    // upper bound
};

/**
 * @brief ArgParseShowrule
 *
 * ArgParseShowrule indicates how many times of appearance each argument shall
 * follow.
 *
 * If an argument appears more than once, then only the last appearance is
 * effective.
 */
enum ArgParseShowrule : unsigned int {
  apshow_free = 0u,           // allow any times of presence
  apshow_once = 0x100000u,    // allow no more than once
  apshow_strict = 0x200000u,  // allow once only
  apshow_vmax = 0xffffffffu   // upper bound
};

class ArgParseRule {
 public:
  ArgParseRule(const std::map<std::string, unsigned int>& _rule);
  ArgParseRule();

  void add_rule(const std::string& arg, unsigned int rule);

  int accept(const int argc, const char* const* const argv,
             std::map<std::string, std::string>* result) const;
  std::string error() const;

 private:
  std::map<std::string, ArgParseTyperule> typerule_;
  std::map<std::string, ArgParseShowrule> showrule_;

  mutable std::string error_ = "";
};

#endif