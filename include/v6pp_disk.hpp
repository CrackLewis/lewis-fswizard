/**
 * @file v6pp_disk.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 13:05:17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef V6PP_DISK_HPP_
#define V6PP_DISK_HPP_

#include <functional>
#include <memory>
#include <vector>

#include "defines.hpp"
#include "io_file.hpp"
#include "v6pp_block.hpp"
#include "v6pp_inode.hpp"
#include "v6pp_inode_directory.hpp"
#include "v6pp_superblock.hpp"

namespace v6pp {

class DiskBlockTraversalMixin {
 public:
  // �̿��������
  std::function<i32(i32 old_block_idx)> block_allocator_ =
      [](i32 old_block_idx) { return old_block_idx; };

  // ���ݿ�ǰ��/�м�/��̲�����
  std::function<void(i32 fileoff, i32 block_idx)> direct_block_setup_ =
      [](...) {};
  std::function<void(i32 fileoff, i32 block_idx)> direct_block_process_ =
      [](...) {};
  std::function<void(i32 fileoff, i32 block_idx)> direct_block_teardown_ =
      [](...) {};

  // ������ǰ��/�м�/��̲�����
  std::function<void(const char* pblk, i32 block_idx)> indirect_block_setup_ =
      [](...) {};
  std::function<void(const char* pblk, i32 block_idx)>
      indirect_block_teardown_ = [](...) {};

  // ���ϴ���
  std::function<void(Inode& inode, i32 size_remaining, const std::string& msg)>
      failure_handler_ = [](...) {};
};

class DiskInodeTravesalMixin {
 public:
  // �ļ�Ŀ¼���ı���˳��
  enum TraverseOrder {
    PRE_ORDER,   // ǰ�����
    POST_ORDER,  // �������
  };

 public:
  TraverseOrder order_ = PRE_ORDER;
  // �����˳����ӡ�
  std::function<bool(i32 cur_idx, i32 father_idx)> traverse_border_ = [](...) {
    return false;
  };
  // Ŀ¼���ӡ�
  std::function<void(i32 cur_idx, i32 father_idx)> directory_handler_ =
      [](...) {};
  // ��Ŀ¼���ļ����ַ��豸�����豸�����ӡ�
  std::function<void(i32 cur_idx, i32 father_idx)> file_handler_ = [](...) {};
  // ���ϴ���
  std::function<void(Inode& inode, const std::string& errmsg)>
      failure_handler_ = [](Inode&, const std::string& errmsg) {
        throw FileSystemException(errmsg);
      };
};

/**
 * @brief
 *
 * Unix V6++���̶���
 *
 * ���̶����װ�˴������ļ������ʵʩϸ�ڡ�
 */
class Disk {
 public:
  // ���ļ���Inode��š�
  static constexpr u32 IDX_ROOT_INODE = 1u;
  static constexpr u32 FSIZE_MAX =
      DiskProps::BLOCK_SIZE * (6 + 2 * 128 + 2 * 128 * 128);

 public:
  explicit Disk(const std::string& filepath);

  ~Disk();

  /**
   * @brief
   *
   * �����������ں�������
   */
  void load();
  void format();
  void update();
  void write_kernel(const std::string& kernel_path);
  void write_bootloader(const std::string& bootloader_path);

  /**
   * @brief
   *
   * ���̿��д������
   */
  bool read_block(Block& block, i32 block_idx);
  bool read_blocks(char* dest, i32 block_idx, i32 block_cnt);
  bool write_block(const Block& block, i32 block_idx);
  bool write_blocks(const char* src, i32 block_idx, i32 block_cnt);

  /**
   * @brief
   *
   * �ļ���д������
   */
  bool read_file(char* dest, Inode& inode);
  bool write_file(const char* src, Inode& inode, i32 fsize);

  /**
   * @brief
   *
   * ���̿��Inode��Դ����
   */
  i32 alloc_block();
  void free_block(i32 idx);
  i32 alloc_inode();
  void free_inode(i32 idx, bool free_blocks = false);
  void free_inode_blocks(Inode& inode);

  /**
   * @brief
   *
   * ���̿��inode����������
   *
   */
  bool traverse_blocks_over_inode(Inode& inode,
                                  const DiskBlockTraversalMixin& mixin);
  bool traverse_inode_tree(Inode& root, const DiskInodeTravesalMixin& mixin);

  std::unique_ptr<InodeDirectory> read_inode_directory(
      Inode& inode, bool ignore_ftype_check = false, int dir_stride = 0);

 public:
  // �����ļ���
  io::FileBase* file_;
  // ��������ڴ渱����
  SuperBlock superblock_;
  // ����Inode�����ڴ渱����
  Inode inodes_[DiskProps::BLOCKS_INODE_ZONE * DiskProps::BLOCK_SIZE /
                sizeof(Inode)];
};

}  // namespace v6pp

#endif