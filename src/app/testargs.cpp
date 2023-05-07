/**
 * @file testargs.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-07 13:22:08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cstring>
#include <iostream>

#include "argparse.hpp"

std::string ask(const std::string& prompt) {
  std::cout << prompt;
  std::string rd;
  std::getline(std::cin, rd);
  return rd;
}

int main() {
  if (1) {
    std::cout << "This is the command line argument parser testing program. "
              << std::endl;
    std::cout << "To quit the program, press Ctrl-C. " << std::endl;
  }

  while (1) {
    ArgParseRule rule;

    // load default rule or input customized rule.
    if (tolower(ask("Use default argparse rule? [y/n] ")[0]) == 'y') {
      rule.add_rule("rule1", aptype_free | apshow_free);
      rule.add_rule("rule2", aptype_free | apshow_once);
      rule.add_rule("rule3", aptype_free | apshow_strict);
      rule.add_rule("rule4", aptype_is_bool | apshow_free);
      rule.add_rule("rule5", aptype_is_char | apshow_free);
      rule.add_rule("rule6", aptype_is_int | apshow_free);
      rule.add_rule("rule7", aptype_is_real | apshow_free);
      rule.add_rule("rule8", aptype_is_str | apshow_free);
      rule.add_rule("rule9", aptype_is_uint | apshow_free);
      rule.add_rule("rule10", aptype_opt_only | apshow_free);
    } else {
      int rulec = atoi(ask("Number of rules: ").c_str());
      for (int idx = 0; idx < rulec; ++idx) {
        std::string rulename =
            ask("Name of the rule #" + std::to_string(idx) + ": ");
        unsigned int rulemask = static_cast<unsigned int>(
            atoi(ask("Mask code of the rule #" + std::to_string(idx) + ": ")
                     .c_str()));
        rule.add_rule(rulename, rulemask);
      }
    }

    std::cout << "Testing towards the rule you specified has started."
              << std::endl;
    std::cout << "Type your arguments separated by spaces in a line for each "
                 "testcase. "
              << std::endl;
    std::cout << "Type an empty line to finish testing." << std::endl;

    std::string answer, argtext = "";
    while ((answer = ask(">>> ")) != "") {
      std::vector<char*> argv_cont;
      for (char ch : answer) {
        if (ch == ' ') {
          if (argtext != "") {
            char* new_argv = new char[argtext.length() + 1]();
            strcpy(new_argv, argtext.data());
            argv_cont.push_back(new_argv);
            argtext = "";
          }
        } else {
          argtext += ch;
        }
      }
      if (argtext != "") {
        char* new_argv = new char[argtext.length() + 1]();
        strcpy(new_argv, argtext.data());
        argv_cont.push_back(new_argv);
        argtext = "";
      }

      std::map<std::string, std::string> result;
      int ret = rule.accept(argv_cont.size(), argv_cont.data(), &result);
      if (ret) {
        std::cout << "Error: " << rule.error() << std::endl;
      } else {
        std::cout << "Parse result: " << std::endl;
        for (auto _ : result) {
          std::cout << _.first << ": " << _.second << std::endl;
        }
      }

      for (auto argv : argv_cont) delete[] argv;
    }

    std::cout << "Finished testing. Preparing next round. " << std::endl;
  }
  return 0;
}