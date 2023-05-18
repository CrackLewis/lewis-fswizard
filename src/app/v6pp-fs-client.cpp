/**
 * @file v6pp-fs-client.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-13 19:35:22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>

#include "argparse.hpp"
#include "net_api.hpp"

int main(int argc, char** argv) {
  std::string addr = "127.0.0.1";
  int port = 7777;
  if (1) {
    ArgParseRule rule;
    rule.add_rule("addr", aptype_is_str | apshow_once);
    rule.add_rule("port", aptype_is_uint | apshow_once);

    std::map<std::string, std::string> result;
    if (rule.accept(argc, argv, &result)) {
      std::cout << "Error: " << rule.error() << std::endl;
      return -1;
    }
    if (result.count("addr")) addr = result["addr"];
    if (result.count("port")) port = atoi(result["port"].data());
  }

  return 0;
}