/**
 * @file v6pp-fs-server.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-13 16:32:31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>

#include "argparse.hpp"

int main(int argc, char** argv) {
  std::string image_path;
  int port = 7777;
  int max_sessions = 4;
  if (1) {
    ArgParseRule rule;
    rule.add_rule("image", aptype_is_str | apshow_strict);
    rule.add_rule("port", aptype_is_uint | apshow_once);
    rule.add_rule("max_sessions", aptype_is_uint | apshow_once);

    std::map<std::string, std::string> result;
    if (rule.accept(argc, argv, &result)) {
      std::cout << "Error: " << rule.error() << std::endl;
      return -1;
    }
    image_path = result["image"];
    if (result.count("port")) port = atoi(result["port"].data());
    if (result.count("max_sessions"))
      max_sessions = atoi(result["max_sessions"].data());
  }

  
  return 0;
}