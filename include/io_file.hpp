/**
 * @file io_file.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 13:15:01
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef IO_FILE_HPP_
#define IO_FILE_HPP_

#include <string>

#include "defines.hpp"

namespace io {

/**
 * @brief
 *
 * 磁盘文件基类。
 *
 * 本程序中涉及的所有磁盘文件均为二进制随机读写文件。
 */
class FileBase {
 public:
  static const u32 FILE_SET = 0;
  static const u32 FILE_CUR = 1;
  static const u32 FILE_END = 2;

 public:
  explicit FileBase(const std::string& filepath);

  virtual ~FileBase();

  virtual FileBase& read(char* dest_addr, size_t rdsize) = 0;

  virtual FileBase& write(const char* src_addr, size_t wrsize) = 0;

  virtual FileBase& seekg(i32 offset, u32 seekdir) = 0;

  virtual FileBase& seekp(i32 offset, u32 seekdir) = 0;

  virtual i32 tellg() = 0;

  virtual i32 tellp() = 0;

  virtual bool good() = 0;

  virtual std::string error() = 0;

 protected:
  std::string file_path_;
  std::string error_;
  i32 is_good_ = 1;
};

};  // namespace io

#endif