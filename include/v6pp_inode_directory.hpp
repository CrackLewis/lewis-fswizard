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
 * ϵͳ�ļ����ָ��һ���ļ���Ŀ¼��
 * size = 32bytes.
 */
class DirectoryEntry {
 public:
  static const int DIRSIZE = 28;

 public:
  /**
   * @brief
   * ��Ӧ�ļ���inode��š�
   */
  i32 inode_id_;
  /**
   * @brief
   * ��Ӧ�ļ������ơ�
   */
  char name_[DIRSIZE];
} __attribute__((packed));

/**
 * @brief
 *
 * inode��Ӧ��Ŀ¼���һ�����������ṹ��
 */
class InodeDirectory {
 public:
  ~InodeDirectory() {
    if (entries_) delete[] entries_;
  }

  // TODO: InodeDirectory constructors and methods

 public:
  size_t length_ = 0;
  // ��Ŀ¼�Ķ��ӱ����ǡ�
  DirectoryEntry* entries_ = nullptr;
} __attribute__((packed));

}  // namespace v6pp

#endif