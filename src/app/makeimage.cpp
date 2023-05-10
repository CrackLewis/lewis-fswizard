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

#include <filesystem>
#include <fstream>
#include <iostream>

#include "argparse.hpp"
#include "v6pp_disk.hpp"

using namespace v6pp;

void prepare_diskfile(const std::string& path) {
  std::ofstream ftest(path, std::ios::out | std::ios::binary);
  if (!ftest.is_open())
    throw std::runtime_error("Error: cannot open diskfile: " + path);

  ftest.seekp(DiskProps::get_disk_size(), std::ios::beg);
  ftest.put('\0');
  ftest.close();
}

/**
 * @brief
 *
 * 镜像创建程序。
 *
 * @return int
 */
int main(int argc, char** argv) {
  std::string image_path;
  std::string kernel_path;
  std::string boot_path;
  std::string rootfs_path;

  // 处理命令行参数。
  if (1) {
    std::map<std::string, std::string> cli_params;
    ArgParseRule rule;
    rule.add_rule("image", aptype_is_str | apshow_strict);
    rule.add_rule("kernel", aptype_is_str | apshow_strict);
    rule.add_rule("boot", aptype_is_str | apshow_strict);
    rule.add_rule("rootfs", aptype_is_str | apshow_strict);

    if (rule.accept(argc, argv, &cli_params)) {
      std::cout << "Error: " << rule.error() << std::endl;
      return -1;
    } else {
      image_path = cli_params["image"];
      kernel_path = cli_params["kernel"];
      boot_path = cli_params["boot"];
      rootfs_path = cli_params["rootfs"];
    }
  }

  // 创建磁盘文件并使其具有指定大小。
  prepare_diskfile(image_path);

  return 0;
}