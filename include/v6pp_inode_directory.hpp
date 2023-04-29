/**
 * @file v6pp_inode_directory.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-28 23:18:21
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef V6PP_INODE_DIRECTORY_HPP_
#define V6PP_INODE_DIRECTORY_HPP_

#include "defines.hpp"

namespace v6pp {

/**
 * @brief
 *
 * 系统文件表项，指代一个文件或目录。
 * size = 32bytes.
 */
class DirectoryEntry {
 public:
  static const int DIRSIZE = 28;

 public:
  /**
   * @brief
   * 对应文件的inode编号。
   */
  i32 inode_id_;
  /**
   * @brief
   * 对应文件的名称。
   */
  char name_[DIRSIZE];
} __attribute__((packed));

/**
 * @brief
 *
 * inode对应的目录项。是一个操作辅助结构。
 */
class InodeDirectory {
 public:
  ~InodeDirectory() {
    if (entries_) delete[] entries_;
  }

  // TODO: InodeDirectory constructors and methods

 public:
  size_t length_ = 0;
  // 本目录的儿子表项们。
  DirectoryEntry* entries_ = nullptr;
} __attribute__((packed));

}  // namespace v6pp

#endif