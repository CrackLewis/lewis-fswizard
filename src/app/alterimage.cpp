/**
 * @file alterimage.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-10 14:39:42
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>
#include <iostream>

#include "argparse.hpp"
#include "v6pp_disk.hpp"

/**
 * @brief
 *
 * 修改其他来源的V6++镜像，使其兼容本程序。
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
      std::cout << "Error: " << rule.error() << std::endl;
      return -1;
    }

    image_path = result["image"];
  }

  std::fstream fimg(image_path,
                    std::ios::in | std::ios::out | std::ios::binary);
  if (!fimg.is_open()) {
    std::cout << "Cannot open " << image_path << std::endl;
    return -1;
  }

  v6pp::SuperBlock sb;
  i32 off_padding = 836;
  i32 off_superblock = 102400;
  i32 size_padding = 1024 - 836;

  fimg.seekp(off_superblock + off_padding, std::ios::beg);
  fimg.write((char*)(&sb) + off_padding, size_padding);
  fimg.close();

  return 0;
}