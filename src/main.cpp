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

    if (1) {
      // ≥¨º∂øÈ≤‚ ‘°£
      v6pp::SuperBlock sb = disk.superblock_;
      0;
    }
  } catch (FileSystemException& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}