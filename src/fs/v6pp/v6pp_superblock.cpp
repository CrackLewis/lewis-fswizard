/**
 * @file v6pp_superblock.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 12:56:55
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "v6pp_superblock.hpp"

using namespace v6pp;

char* SuperBlock::data() { return (char*)(this); }

const char* SuperBlock::data() const { return (const char*)(this); }

bool SuperBlock::load(io::FileBase& file) {
  file.seekg(SUPER_BLOCK_OFFSET, io::FileBase::FILE_SET);
  file.read(data(), sizeof(SuperBlock));
  if (!file.good()) {
    file.error();
    return false;
  }
  return true;
}

bool SuperBlock::update(io::FileBase& file) {
  file.seekp(SUPER_BLOCK_OFFSET, io::FileBase::FILE_SET);
  file.write(data(), sizeof(SuperBlock));
  if (!file.good()) {
    file.error();
    return false;
  }
  return true;
}

void SuperBlock::format() {
  SuperBlock copy;
  copy.s_ninode_ = 0;
  copy.s_nfree_ = 0;
  copy.s_ronly_ = 0;
  copy.s_ilock_ = copy.s_flock_ = 0;
  memcpy(this, &copy, sizeof(SuperBlock));
}