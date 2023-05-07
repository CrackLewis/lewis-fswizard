/**
 * @file makeimage.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-07 12:40:57
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>

#include "argparse.hpp"
#include "v6pp_disk.hpp"

using namespace v6pp;

/**
 * @brief
 *
 * ¾µÏñ´´½¨³ÌÐò¡£
 *
 * @return int
 */
int main(int argc, char** argv) {
  std::string image_path;
  std::string kernel_path;
  std::string boot_path;
  std::string rootfs_path;

  if (1) {
    std::map<std::string, std::string> cli_params;
    ArgParseRule rule;
    rule.add_rule("image", aptype_is_str | apshow_strict);
    rule.add_rule("kernel", aptype_is_str | apshow_strict);
    rule.add_rule("boot", aptype_is_str | apshow_strict);
    rule.add_rule("rootfs", aptype_is_str | apshow_strict);

    if (rule.accept(argc, argv, &cli_params)) {
      std::cout << "Error: " << rule.error() << std::endl;
    } else {
      image_path = cli_params["image"];
      kernel_path = cli_params["kernel"];
      boot_path = cli_params["boot"];
      rootfs_path = cli_params["rootfs"];
    }
  }

  
  return 0;
}