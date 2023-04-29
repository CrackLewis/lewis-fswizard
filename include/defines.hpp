/**
 * @file defines.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-28 23:20:24
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef DEFINES_HPP_
#define DEFINES_HPP_

#include <cstdint>

using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using byte = uint8_t;

enum FileType {
  NORMAL = 0,
  CHAR_DEV = 1,
  DIR = 2,
  BLOCK_DEV = 3,
  MAX = 4,
};

namespace v6pp {

/**
 * @brief
 *
 * V6++磁盘的固定属性。
 */
class DiskProps {
 public:
  /**
   * @brief
   *
   * 设备属性。
   */

  constexpr static size_t BLOCK_SIZE = 512u;
  constexpr static size_t SECTOR_SIZE = 512u;
  constexpr static size_t CYLINDERS = 20u;
  constexpr static size_t HEADS = 16u;
  constexpr static size_t SECTORS_PER_TRACK = 63u;

  /**
   * @brief
   *
   * 磁盘分区属性。
   *
   * 扇区       内容
   * 0          引导文件扇区
   * 1-199      内核文件扇区
   * 200-201    SuperBlock扇区
   * 202-1023   Inode扇区
   * 1024-2183  交换扇区
   * 2184+      文件扇区
   */
  constexpr static size_t BLOCKS_BOOTLOAD = 1ul;
  constexpr static size_t BLOCKS_KERNEL = 199ul;
  constexpr static size_t BLOCKS_SUPERBLOCK = 2ul;
  constexpr static size_t BLOCKS_INODE_ZONE = 822ul;
  constexpr static size_t BLOCKS_SWAP_ZONE = 2160ul;

  constexpr static size_t BLOCKS_BOOT_AND_KERNEL =
      BLOCKS_BOOTLOAD + BLOCKS_KERNEL;

 public:
  DiskProps() = delete;

  DiskProps(const DiskProps&) = delete;

  DiskProps(DiskProps&&) = delete;

  // 20,160
  static constexpr size_t get_disk_blocks() {
    return CYLINDERS * SECTORS_PER_TRACK * HEADS;
  }

  // 10,321,900 bytes = 10,079 KiB = 9.843 MiB
  static constexpr size_t get_disk_size() {
    return BLOCK_SIZE * get_disk_blocks();
  }
};

}  // namespace v6pp

#endif  // DEFINES_HPP_