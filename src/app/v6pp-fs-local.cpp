/**
 * @file v6pp-fs-cli.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-10 20:15:16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iomanip>
#include <iostream>

#include "argparse.hpp"
#include "v6pp_vfs.hpp"

void cli(v6pp::FileSystem& fs) {
  if (1) {
    std::cout << "Unix V6++ File System Shell" << std::endl;
    std::cout << "Presented by 2053642 Boyu Li, CSE, CEIE, Tongji Univ."
              << std::endl;
    std::cout << "For available utilities, type \"help\"." << std::endl;
    std::cout << std::endl;
  }

  while (1) {
    std::cout << "fswizard@fswizard:" << fs._getcwd() << ":$ ";
    std::string cli_cmd;
    std::vector<std::string> cli_args;

    if (1) {
      std::string cli;
      std::string cli_seg = "";
      bool has_cmd = false;

      std::getline(std::cin, cli);
      for (char ch : cli) {
        if (ch == ' ') {
          if (cli_seg.length() == 0) continue;
          if (!has_cmd) {
            has_cmd = true;
            cli_cmd = cli_seg;
          } else
            cli_args.emplace_back(cli_seg);
          cli_seg = "";
        } else {
          cli_seg += ch;
        }
      }
      if (cli_seg.length() > 0) {
        if (!has_cmd)
          cli_cmd = cli_seg;
        else
          cli_args.emplace_back(cli_seg);
      }
    }

    if (cli_cmd == "help") {
      std::cout << "Available commands: " << std::endl;
      std::cout << std::left;
      std::cout << "quit" << std::endl;
#define COMMAND(comm) std::cout << #comm << std::endl;
#include "commands.inc"
#undef COMMAND
      std::cout << std::endl;
      continue;
    }
    if (cli_cmd == "quit") break;
#define COMMAND(comm)     \
  if (cli_cmd == #comm) { \
    fs.comm(cli_args);    \
    continue;             \
  }
#include "commands.inc"
#undef COMMAND

    std::cout << "Unknown command. Type \"help\" for help." << std::endl;
  }
}

/**
 * @brief
 *
 * V6++磁盘文件系统交互式终端程序。
 *
 * @param argc
 * @param argv
 * @return int
 */

int main(int argc, char** argv) {
  std::string image_path;

  if (1) {
    std::map<std::string, std::string> result;
    ArgParseRule rule;
    rule.add_rule("image", aptype_is_str | apshow_strict);

    if (rule.accept(argc, argv, &result)) {
      std::cerr << "Error: " << rule.error() << std::endl;
      return -1;
    }

    image_path = result["image"];
  }

  v6pp::FileSystemConfig config;
  config.asker_ = [&](const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    if (!std::cin.good()) {
      std::cin.clear();
      throw std::runtime_error("Bad pipe.");
    }
    return line;
  };
  config.speaker_ = [&](const std::string& msg) {
    std::cout << msg << std::endl;
  };
  config.disk_path_ = image_path;

  v6pp::FileSystem fs(config);
  cli(fs);

  return 0;
}