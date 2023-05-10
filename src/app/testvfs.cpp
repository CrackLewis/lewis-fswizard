/**
 * @file testvfs.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-10 12:05:51
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <csignal>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include "argparse.hpp"
#include "exceptions.hpp"
#include "v6pp_vfs.hpp"

// �ӱ���ɾ�����Ծ���
void destroy_image(const std::string& copy_path) {
  try {
    if (std::filesystem::exists(copy_path)) std::filesystem::remove(copy_path);
  } catch (std::filesystem::filesystem_error& e) {
    throw std::runtime_error(e.what());
  }
}

// ����ָ������Ĭ�ϲ����ǡ�
void copy_image(const std::string& src_path, const std::string& dest_path) {
  try {
    std::filesystem::copy_file(src_path, dest_path);
  } catch (std::filesystem::filesystem_error& e) {
    throw std::runtime_error(e.what());
  }
}

void refresh_image(const std::string& src_path, const std::string& dest_path) {
  destroy_image(dest_path);
  copy_image(src_path, dest_path);
}

static std::string __copy_image_path = "";
static bool __destroy_copy_on_exit = false;
static bool __image_created = false;

void interrupt_handler(int signal) {
  if (__destroy_copy_on_exit) destroy_image(__copy_image_path);
}

void exit_handler() {
  if (__destroy_copy_on_exit) destroy_image(__copy_image_path);
}

void subshell(v6pp::FileSystem& fs, const std::vector<std::string>& comms) {
  while (1) {
    std::cout << "fswizard@fswizard:" << fs._getcwd() << ":$";
    std::string cli, cli_cmd = "";
    std::getline(std::cin, cli);

    std::vector<std::string> cli_args;
    std::string cli_seg = "";
    bool has_cmd = false;
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

    if (cli_cmd == "quit") break;

#define REGOPT(opt) \
  if (comm == #opt && cli_cmd == #opt) fs.opt(cli_args);

    for (auto comm : comms) {
      REGOPT(ls);
      REGOPT(cd);
      REGOPT(pwd);

      REGOPT(mkdir);
      REGOPT(rmdir);

      REGOPT(touch);
      REGOPT(rm);
      REGOPT(cp);
      REGOPT(mv);

      REGOPT(upload);
      REGOPT(download);
      REGOPT(format);
    }

#undef REGOPT
  }
}

void test_cd(v6pp::FileSystem& fs) {
  subshell(fs, {"cd", "ls", "pwd", "mkdir", "rmdir", "touch", "rm", "cp", "mv",
                "upload", "download", "format"});
}

/**
 * @brief
 * �ó�������ָ���ľ��񴴽�һ�����Ը������ڸ����Ͻ��в��ԣ�
 * ������ֱ��д������������ʱ��������û�ѡ������Ƿ����ٲ��Ը�����
 *
 * destroy_on_exit=true, refresh=true: ��Ԫ���ԣ����񲻱�����
 * destroy_on_exit=true, refresh=false: ���ϲ��ԣ����񲻱�����
 * destroy_on_exit=false: ��������
 */

#define REG_TEST(testname, tester, refresh)                              \
  try {                                                                  \
    if (!__image_created)                                                \
      copy_image(image_path, __copy_image_path), __image_created = true; \
    std::cout << "==== Before test: " << #testname << std::endl;         \
    {                                                                    \
      v6pp::FileSystem vfs(config);                                      \
      tester(vfs);                                                       \
    }                                                                    \
    std::cout << "==== After test: " << #testname << std::endl;          \
    if (refresh) refresh_image(image_path, __copy_image_path);           \
  } catch (FileSystemException & e) {                                    \
    std::cout << "Test failed with exception (" << #testname             \
              << "): " << e.what() << std::endl;                         \
  }

int main(int argc, char** argv) {
  std::string image_path;
  std::signal(SIGINT, interrupt_handler);
  atexit(exit_handler);

  if (1) {
    ArgParseRule rule;
    rule.add_rule("image", aptype_is_str | apshow_strict);
    rule.add_rule("save_on_exit", aptype_opt_only | apshow_once);
    std::map<std::string, std::string> result;

    if (rule.accept(argc, argv, &result)) {
      std::cout << "Error: " << rule.error() << std::endl;
      return -1;
    }

    image_path = result["image"];
    __destroy_copy_on_exit = !(result.count("save_on_exit"));

    __copy_image_path = image_path + ".testimg";

    // �����ʱ�����ַ���ļ���˵���ϴ���ɾ�ˣ����Զ�ɾ����
    if (std::filesystem::exists(__copy_image_path))
      std::filesystem::remove(__copy_image_path);
  }

  v6pp::FileSystemConfig config;
  config.asker_ = [&](const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    if (!std::cin.good()) {
      std::cin.clear();
      throw std::runtime_error("Bad pipe!");
    }
    return line;
  };
  config.speaker_ = [&](const std::string& msg) {
    std::cout << msg << std::endl;
  };
  config.disk_path_ = __copy_image_path;

  // ͨ��REG_TEST��Ӳ���������
  REG_TEST(CdTest, test_cd, false);

  return 0;
}

#undef REG_TEST