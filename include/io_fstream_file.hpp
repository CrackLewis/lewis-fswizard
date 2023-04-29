/**
 * @file io_fstream_file.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 13:45:14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef IO_FSTREAM_FILE_HPP_
#define IO_FSTREAM_FILE_HPP_

#include <fstream>

#include "io_file.hpp"

namespace io {

/**
 * @brief
 *
 * ʹ��C++��׼���е�fstreamʵ���ļ���д�Ĵ����ļ���ʽ��
 */
class FstreamFile : public FileBase {
 public:
  explicit FstreamFile(const std::string& filepath);

  virtual ~FstreamFile();

  virtual FileBase& read(char* dest_addr, size_t rdsize) override;

  virtual FileBase& write(const char* src_addr, size_t wrsize) override;

  virtual FileBase& seekg(i32 offset, u32 seekdir) override;

  virtual FileBase& seekp(i32 offset, u32 seekdir) override;

  virtual i32 tellg() override;

  virtual i32 tellp() override;

  virtual bool good() override;

  virtual std::string error() override;

 protected:
  std::fstream stream_;
};

}  // namespace io

#endif