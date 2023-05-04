/**
 * @file v6pp_superblock.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-28 23:18:39
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef V6PP_SUPERBLOCK_HPP_
#define V6PP_SUPERBLOCK_HPP_

#include "defines.hpp"
#include "io_file.hpp"

namespace v6pp {

/**
 * @brief
 *
 * Unix V6++超级块。
 */
class SuperBlock {
 public:
  static constexpr u32 SUPER_BLOCK_OFFSET =
      DiskProps::BLOCKS_BOOT_AND_KERNEL * DiskProps::BLOCK_SIZE;

 public:
  char* data();

  const char* data() const;
  // 从磁盘文件载入超级块。
  bool load(io::FileBase& file);
  // 向磁盘文件写入超级块。
  bool update(io::FileBase& file);
  // 重置超级块，用于格式化工作。
  void format();

 public:
  /**
   * @brief
   *
   * 0-835: 盘块和Inode资源管理，更新追踪。
   */

  // 外存Inode区占用盘块数。
  u32 s_isize_ = DiskProps::BLOCKS_INODE_ZONE;
  // 盘块总数。
  u32 s_fsize_ = DiskProps::get_disk_blocks();
  // 直接管理的空闲盘块数量。
  u32 s_nfree_ = 0;
  // 直接管理的空闲盘块索引表。
  u32 s_free_[100] = {0};
  // 直接管理的空闲外存Inode数量。
  u32 s_ninode_ = 0;
  // 直接管理的空闲外存Inode索引表。
  u32 s_inode_[100] = {0};
  // 封锁空闲盘块索引表标志。
  u32 s_flock_;
  // 封锁空闲Inode表标志。
  u32 s_ilock_;
  // 内存中SuperBlock副本被修改标志。
  u32 s_fmod_;
  // 本文件系统只能读出。
  u32 s_ronly_;
  // 最后更新时间。
  u32 s_time_;

  /**
   * @brief
   *
   * 836-1023:
   * 在原镜像里是padding区。这里用于记录一些辅助信息。
   */
  // 磁盘总扇区数 = 20160
  u32 p_size_sectors_ =
      DiskProps::CYLINDERS * DiskProps::HEADS * DiskProps::SECTORS_PER_TRACK;
  // Inode区起始扇区号 = 200
  u32 p_off_inodes_ = DiskProps::BLOCKS_BOOT_AND_KERNEL;
  // Inode区扇区数 = 822
  u32 p_size_inodes_ = DiskProps::BLOCKS_INODE_ZONE;
  // 数据区起始扇区号 = 1024
  u32 p_off_data_ = p_off_inodes_ + p_size_inodes_;
  // 数据区扇区数 = 16976
  u32 p_size_data_ =
      p_size_sectors_ - p_off_data_ - DiskProps::BLOCKS_SWAP_ZONE;
  // 交换区起始扇区号 = 18000
  u32 p_off_swap_ = p_off_data_ + p_size_data_;
  // 交换区扇区数 = 2160
  u32 p_size_swap_ = DiskProps::BLOCKS_SWAP_ZONE;
  // padding区是否被修改：默认已经被修改了。
  u32 p_mod_ = 1;
  // 保留用途。
  u32 p_blank_[39] = {0};
} __attribute__((packed));

}  // namespace v6pp

#endif