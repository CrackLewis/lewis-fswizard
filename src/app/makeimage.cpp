/**
 * @file makeimage.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-07 12:40:57
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <filesystem>
#include <fstream>
#include <iostream>

#include "argparse.hpp"
#include "v6pp_vfs.hpp"

using namespace v6pp;

void prepare_diskfile(const std::string& path) {
  std::ofstream ftest(path, std::ios::out | std::ios::binary);
  if (!ftest.is_open())
    throw std::runtime_error("Error: cannot open diskfile: " + path);

  ftest.seekp(DiskProps::get_disk_size() - 1u, std::ios::beg);
  ftest.put('\0');
  ftest.close();
}

void write_binaries(const std::string& image_path,
                    const std::string& bootloader_path,
                    const std::string& kernel_path) {
  try {
    v6pp::Disk disk(image_path);

    disk.write_bootloader(bootloader_path);
    disk.write_kernel(kernel_path);
  } catch (FileSystemException& e) {
    throw std::runtime_error(e.what());
  }
}

void format_diskfile(const std::string& image_path) {
  try {
    v6pp::FileSystemConfig config;
    config.disk_path_ = image_path;
    config.asker_ = [&](const std::string&) { return "y"; };
    config.speaker_ = [&](const std::string& m) {
      std::cout << "format: " << m << std::endl;
    };

    v6pp::FileSystem fs(config);
    fs.format();
  } catch (FileSystemException& e) {
    throw std::runtime_error("format: " + e.what());
  }
}

void write_rootfs(const std::string& image_path, std::string rootfs_path) {
  namespace fs = std::filesystem;

  try {
    v6pp::FileSystemConfig config;
    config.disk_path_ = image_path;
    config.speaker_ = [&](const std::string& m) {
      std::cout << "write_rootfs: " << m << std::endl;
    };

    v6pp::FileSystem fs(config);

    while (rootfs_path.back() == '\\' || rootfs_path.back() == '/')
      rootfs_path.pop_back();

    std::function<void(std::string, std::string)> trav_local =
        [&](std::string local_path, std::string v6pp_path) {
          for (const auto& dirent : fs::directory_iterator(local_path)) {
            if (dirent.is_directory()) {
              std::string dname = dirent.path().filename().string();
              fs.mkdir({v6pp_path + dname});
              trav_local(dirent.path().string(), v6pp_path + dname + '/');
            } else {
              fs.upload({dirent.path().string(),
                         v6pp_path + dirent.path().filename().string()});
            }
          }
        };

    trav_local(rootfs_path, "/");
  } catch (FileSystemException& e) {
    throw std::runtime_error("write_rootfs: " + e.what());
  } catch (fs::filesystem_error& e) {
    throw std::runtime_error("write_rootfs: " + std::string(e.what()));
  }
}

/**
 * @brief
 *
 * 镜像创建程序。
 *
 * @return int
 */
int main(int argc, char** argv) {
  std::string image_path;
  std::string kernel_path;
  std::string boot_path;
  std::string rootfs_path;

  // 处理命令行参数。
  if (1) {
    std::map<std::string, std::string> cli_params;
    ArgParseRule rule;
    rule.add_rule("image", aptype_is_str | apshow_strict);
    rule.add_rule("kernel", aptype_is_str | apshow_strict);
    rule.add_rule("boot", aptype_is_str | apshow_strict);
    rule.add_rule("rootfs", aptype_is_str | apshow_strict);

    if (rule.accept(argc, argv, &cli_params)) {
      std::cout << "Error: " << rule.error() << std::endl;
      return -1;
    } else {
      image_path = cli_params["image"];
      kernel_path = cli_params["kernel"];
      boot_path = cli_params["boot"];
      rootfs_path = cli_params["rootfs"];
    }
  }

  // 创建磁盘文件并使其具有指定大小。
  prepare_diskfile(image_path);

  // 写入引导和内核文件。
  write_binaries(image_path, boot_path, kernel_path);

  // 格式化磁盘文件。
  format_diskfile(image_path);

  // 写入根文件目录。（注意：目录内不能有/dev，否则报错）
  write_rootfs(image_path, rootfs_path);

  return 0;
}