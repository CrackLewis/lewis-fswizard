/**
 * @file testimage.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-07 16:07:48
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * 本文件主要是对v6pp::Disk进行一些功能性测试。
 *
 */

#include <csignal>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include "argparse.hpp"
#include "exceptions.hpp"
#include "v6pp_disk.hpp"

// 从本地删除测试镜像。
void destroy_image(const std::string& copy_path) {
  try {
    if (std::filesystem::exists(copy_path)) std::filesystem::remove(copy_path);
  } catch (std::filesystem::filesystem_error& e) {
    throw std::runtime_error(e.what());
  }
}

// 复制指定镜像，默认不覆盖。
void copy_image(const std::string& src_path, const std::string& dest_path) {
  try {
    std::filesystem::copy_file(src_path, dest_path);
  } catch (std::filesystem::filesystem_error& e) {
    throw std::runtime_error(e.what());
  }
}

void refresh_image(const std::string& src_path, const std::string& dest_path) {
  destroy_image(dest_path);
  copy_image(src_path, dest_path);
}

static std::string __copy_image_path = "";
static bool __destroy_copy_on_exit = false;
static bool __image_created = false;

void interrupt_handler(int signal) {
  if (__destroy_copy_on_exit) destroy_image(__copy_image_path);
}

void exit_handler() {
  if (__destroy_copy_on_exit) destroy_image(__copy_image_path);
}

// 盘块分配与释放测试。
void test_block_allocation(v6pp::Disk& disk) {
  disk.load();
  i32 ret;
  std::vector<i32> disk_blocks;
  // 不断申请盘块，直到盘块枯竭。
  while ((ret = disk.alloc_block()) != -1) {
    disk_blocks.push_back(ret);
  }
  std::cout << "Allocated blocks: " << disk_blocks.size() << std::endl;
  // 再释放回所有盘块。
  for (auto i : disk_blocks) disk.free_block(i);
  disk_blocks.clear();
  std::cout << "Reallocating. " << std::endl;
  // 再申请一遍。
  while ((ret = disk.alloc_block()) != -1) {
    disk_blocks.push_back(ret);
  }
  std::cout << "Allocated blocks: " << disk_blocks.size() << std::endl;
}

// Inode分配与释放测试。
void test_inode_allocation(v6pp::Disk& disk) {
  disk.load();
  i32 ret;
  std::vector<i32> disk_inodes;

  while ((ret = disk.alloc_inode()) != -1) {
    disk_inodes.push_back(ret);
  }
  std::cout << "Allocated inodes: " << disk_inodes.size() << std::endl;
  // 再释放回所有盘块。
  for (auto i : disk_inodes) disk.free_inode(i);
  disk_inodes.clear();
  std::cout << "Reallocating. " << std::endl;
  // 再申请一遍。
  while ((ret = disk.alloc_inode()) != -1) {
    disk_inodes.push_back(ret);
  }
  std::cout << "Allocated inodes: " << disk_inodes.size() << std::endl;
}

// 文件树遍历测试。
void test_tree(v6pp::Disk& disk) {
  disk.load();

  using namespace v6pp;
  std::function<void(const std::string&, Inode&, i32)> traverse =
      [&](const std::string fname, Inode& inode, i32 tab) {
        if (inode.file_type_ == FileType::DIR) {
          std::cout << std::setw(tab + 4) << ' ' << fname << "/" << std::endl;
          auto dir = disk.read_inode_directory(inode);

          for (i32 dirent_idx = 0; dirent_idx < dir->length_; ++dirent_idx) {
            const DirectoryEntry& dirent = dir->entries_[dirent_idx];
            traverse(dirent.name_, disk.inodes_[dirent.inode_id_], tab + 4);
          }
        } else {
          std::cout << std::setw(tab + 4) << ' ' << fname << " ("
                    << inode.d_size_ << " B) " << std::endl;
        }
      };

  traverse("", disk.inodes_[Disk::IDX_ROOT_INODE], 0);
}

/**
 * @brief
 * 该程序会基于指定的镜像创建一个测试副本，在副本上进行测试，
 * 而不是直接写镜像。用例结束时，会根据用户选项决定是否销毁测试副本。
 *
 * destroy_on_exit=true, refresh=true: 单元测试，镜像不保留。
 * destroy_on_exit=true, refresh=false: 集合测试，镜像不保留。
 * destroy_on_exit=false: 镜像保留。
 */

#define REG_TEST(testname, tester, refresh)                              \
  try {                                                                  \
    if (!__image_created)                                                \
      copy_image(image_path, __copy_image_path), __image_created = true; \
    std::cout << "==== Before test: " << #testname << std::endl;         \
    {                                                                    \
      v6pp::Disk disk(__copy_image_path);                                \
      tester(disk);                                                      \
    }                                                                    \
    std::cout << "==== After test: " << #testname << std::endl;          \
    if (refresh) refresh_image(image_path, __copy_image_path);           \
  } catch (FileSystemException & e) {                                    \
    std::cout << "Test failed with exception (" << #testname             \
              << "): " << e.what() << std::endl;                         \
  }

int main(int argc, char** argv) {
  std::string image_path;
  std::signal(SIGINT, interrupt_handler);
  atexit(exit_handler);

  if (1) {
    ArgParseRule rule;
    rule.add_rule("image", aptype_is_str | apshow_strict);
    rule.add_rule("save_on_exit", aptype_opt_only | apshow_once);
    std::map<std::string, std::string> result;

    if (rule.accept(argc, argv, &result)) {
      std::cout << "Error: " << rule.error() << std::endl;
      return -1;
    }

    image_path = result["image"];
    __destroy_copy_on_exit = !(result.count("save_on_exit"));

    __copy_image_path = image_path + ".testimg";

    // 如果临时镜像地址有文件，说明上次忘删了，会自动删掉。
    if (std::filesystem::exists(__copy_image_path))
      std::filesystem::remove(__copy_image_path);
  }

  // 通过REG_TEST添加测试用例。
  // REG_TEST(BlockAllocationTest, test_block_allocation, true);
  // REG_TEST(InodeAllocationTest, test_inode_allocation, true);
  // REG_TEST(FileTreeTest, test_tree, true);

  return 0;
}

#undef REG_TEST