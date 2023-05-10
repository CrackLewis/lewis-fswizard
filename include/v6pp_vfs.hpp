/**
 * @file v6pp_vfs.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-03 13:41:35
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef V6PP_VFS_HPP_
#define V6PP_VFS_HPP_

#include <functional>

#include "v6pp_disk.hpp"
#include "vfs.hpp"

namespace v6pp {

class FileSystemConfig {
 public:
  enum LogLevel {
    INFO,
    WARN,
    ERROR,
    FATAL,
    MAX,
  };

 public:
  // ���̡��ںˡ������ļ��������ļ�ϵͳ·����
  // Ĭ��ֵ�ٶ���ǰ·����./build�С�
  std::string disk_path_ = "../etc/c.img";
  std::string kernel_path_ = "../etc/kernel.bin";
  std::string bootloader_path_ = "../etc/boot.bin";
  std::string rootfs_path = "../etc/rootfs";
  // ��������ļ��ߴ�����Ƿ��ʽ���������̡�
  bool format_on_disksize_validation_failure_ = true;
  // �û��������ӡ�
  std::function<std::string(const std::string&)> asker_ = [](...) {
    return "";
  };
  // ������ӡ�
  std::function<void(const std::string&)> speaker_ = [](...) {};
  // ��־���ӡ�
  std::function<void(const std::string&, LogLevel)> logger_ = [](...) {};
};

class FileSystem : public FileSystemBase {
 public:
  explicit FileSystem(const FileSystemConfig& config);

  ~FileSystem();

#define COMMAND(comm) i32 comm(const ArgPack& args = {}) override;
#include "commands.inc"
#undef COMMAND

  std::string _getcwd();

 protected:
  std::vector<i32> _pwalk(const std::string& path, bool to_directory);
  Inode& _touch(const std::string& path, FileType ftype);
  void _rmfile(const std::string& path, FileType ftype);

 protected:
  Disk* disk_;
  std::vector<i32> inode_idx_stack_;
  const FileSystemConfig& config_;
};

}  // namespace v6pp

#endif