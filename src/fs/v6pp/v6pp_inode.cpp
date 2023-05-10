/**
 * @file v6pp_inode.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 12:48:57
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cstring>

#include "v6pp_inode.hpp"

using namespace v6pp;

void Inode::format() {
  ialloc_ = 0;
  ilarg_ = 0;
  d_size_ = 0;
  memset(idx_direct_, 0, sizeof(idx_direct_));
  memset(idx_indirect_, 0, sizeof(idx_indirect_));
  memset(idx_secondary_indirect_, 0, sizeof(idx_secondary_indirect_));
}