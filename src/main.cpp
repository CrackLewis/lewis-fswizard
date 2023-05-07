/**
 * @file main.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-04 11:12:54
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>
#include <iostream>

#include "exceptions.hpp"
#include "v6pp_disk.hpp"

using namespace std;

int main(int argc, char** argv) {
  try {
    v6pp::Disk disk("../etc/c.img");
    disk.load();

    i32 ret;
    std::vector<i32> blocks;
    while (1) {
      ret = disk.alloc_block();
      if (ret == -1) {
        break;
      }
      blocks.push_back(ret);
    }
    std::cout << blocks.size() << std::endl;
  } catch (FileSystemException& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
} 