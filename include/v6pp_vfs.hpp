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
  // 磁盘、内核、引导文件路径。
  // 默认值假定当前路径在./build中。
  std::string disk_path_ = "../etc/c.img";
  std::string kernel_path_ = "../etc/kernel.bin";
  std::string bootloader_path_ = "../etc/boot.bin";
  // 如果磁盘文件尺寸错误，是否格式化整个磁盘。
  bool format_on_disksize_validation_failure_ = true;
  // 用户交互钩子。
  std::function<std::string(const std::string&)> asker_ = [](...) {
    return "";
  };
  // 输出钩子。
  std::function<void(const std::string&)> speaker_ = [](...) {};
  // 日志钩子。
  std::function<void(const std::string&, LogLevel)> logger_ = [](...) {};
};

class FileSystem : public FileSystemBase {
 public:
  explicit FileSystem(const FileSystemConfig& config);

  ~FileSystem();

  i32 cd(const ArgPack& args = {}) override;
  i32 pwd(const ArgPack& args = {}) override;

  i32 mkdir(const ArgPack& args = {}) override;
  i32 rmdir(const ArgPack& args = {}) override;

  i32 touch(const ArgPack& args = {}) override;
  i32 rm(const ArgPack& args = {}) override;
  i32 cp(const ArgPack& args = {}) override;
  i32 mv(const ArgPack& args = {}) override;

  i32 ls(const ArgPack& args = {}) override;

  i32 upload(const ArgPack& args = {}) override;
  i32 download(const ArgPack& args = {}) override;

  i32 format(const ArgPack& args = {}) override;

 protected:
  Disk* disk_;
  std::vector<i32> inode_idx_stack_;
  const FileSystemConfig& config_;
};

}  // namespace v6pp

#endif