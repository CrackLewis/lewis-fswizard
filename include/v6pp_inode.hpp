/**
 * @file v6pp_inode.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-28 23:18:29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef V6PP_INODE_HPP_
#define V6PP_INODE_HPP_

#include "defines.hpp"

namespace v6pp {

/**
 * @brief
 *
 * V6++�ļ�������㡣
 */
class Inode {
 public:
  // TODO: Inode methods
  void format();

 public:
  /**
   * @brief
   * 0-1: Ȩ������ļ�������ʶ
   */
  u16 prot_others_ : 3;
  u16 prot_group_ : 3;
  u16 prot_owner_ : 3;

  u16 is_vtx_ : 1;
  u16 is_gid_ : 1;
  u16 is_uid_ : 1;

  u16 ilarg_ : 1;

  /**
   * @brief
   * 2-3: �ļ�����ռ�б�ʶ
   */
  u16 file_type_ : 2;
  u16 ialloc_ : 1;
  u16 pad0_ = 0;

  u32 d_nlink_;
  u16 d_uid_;
  u16 d_gid_;

  u32 d_size_ = 0;

  /**
   * @brief
   *
   * �̿��������
   * 6��ֱ��������2��һ�����������2���������������
   */
  u32 idx_direct_[6];
  u32 idx_indirect_[2];
  u32 idx_secondary_indirect_[2];

  i32 d_atime_;
  i32 d_mtime_;

} __attribute__((packed));

}  // namespace v6pp

#endif