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
 * Unix V6++�����顣
 */
class SuperBlock {
 public:
  static constexpr u32 SUPER_BLOCK_OFFSET =
      DiskProps::BLOCKS_BOOT_AND_KERNEL * DiskProps::BLOCK_SIZE;

 public:
  char* data();

  const char* data() const;
  // �Ӵ����ļ����볬���顣
  bool load(io::FileBase& file);
  // ������ļ�д�볬���顣
  bool update(io::FileBase& file);
  // ���ó����飬���ڸ�ʽ��������
  void format();

 public:
  /**
   * @brief
   *
   * 0-835: �̿��Inode��Դ��������׷�١�
   */

  // ���Inode��ռ���̿�����
  u32 s_isize_ = DiskProps::BLOCKS_INODE_ZONE;
  // �̿�������
  u32 s_fsize_ = DiskProps::get_disk_blocks();
  // ֱ�ӹ���Ŀ����̿�������
  u32 s_nfree_ = 0;
  // ֱ�ӹ���Ŀ����̿�������
  u32 s_free_[100] = {0};
  // ֱ�ӹ���Ŀ������Inode������
  u32 s_ninode_ = 0;
  // ֱ�ӹ���Ŀ������Inode������
  u32 s_inode_[100] = {0};
  // ���������̿��������־��
  u32 s_flock_;
  // ��������Inode���־��
  u32 s_ilock_;
  // �ڴ���SuperBlock�������޸ı�־��
  u32 s_fmod_;
  // ���ļ�ϵͳֻ�ܶ�����
  u32 s_ronly_;
  // ������ʱ�䡣
  u32 s_time_;

  /**
   * @brief
   *
   * 836-1023:
   * ��ԭ��������padding�����������ڼ�¼һЩ������Ϣ��
   */
  // ������������ = 20160
  u32 p_size_sectors_ =
      DiskProps::CYLINDERS * DiskProps::HEADS * DiskProps::SECTORS_PER_TRACK;
  // Inode����ʼ������ = 200
  u32 p_off_inodes_ = DiskProps::BLOCKS_BOOT_AND_KERNEL;
  // Inode�������� = 822
  u32 p_size_inodes_ = DiskProps::BLOCKS_INODE_ZONE;
  // ��������ʼ������ = 1024
  u32 p_off_data_ = p_off_inodes_ + p_size_inodes_;
  // ������������ = 16976
  u32 p_size_data_ =
      p_size_sectors_ - p_off_data_ - DiskProps::BLOCKS_SWAP_ZONE;
  // ��������ʼ������ = 18000
  u32 p_off_swap_ = p_off_data_ + p_size_data_;
  // ������������ = 2160
  u32 p_size_swap_ = DiskProps::BLOCKS_SWAP_ZONE;
  // padding���Ƿ��޸ģ�Ĭ���Ѿ����޸��ˡ�
  u32 p_mod_ = 1;
  // ������;��
  u32 p_blank_[39] = {0};
} __attribute__((packed));

}  // namespace v6pp

#endif