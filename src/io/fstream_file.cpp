/**
 * @file fstream_file.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 14:04:43
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "exceptions.hpp"
#include "io_fstream_file.hpp"

using namespace io;

FstreamFile::FstreamFile(const std::string& filepath) : FileBase(filepath) {
  stream_.open(filepath, std::ios::in | std::ios::out | std::ios::binary);
  if (!stream_.is_open()) {
    throw FileSystemException("cannot open " + filepath);
  }
}

FstreamFile::~FstreamFile() { stream_.close(); }

FileBase& FstreamFile::read(char* dest_addr, size_t rdsize) {
  stream_.read(dest_addr, rdsize);
  if (!stream_.good()) {
    is_good_ = 0;
    error_ = "FstreamFile::read(" + std::to_string(u64(dest_addr)) + ", " +
             std::to_string(rdsize) + ") failed";
  }
  return *this;
}

FileBase& FstreamFile::write(const char* src_addr, size_t wrsize) {
  stream_.write(src_addr, wrsize);
  if (!stream_.good()) {
    is_good_ = 0;
    error_ = "FstreamFile::write(" + std::to_string(u64(src_addr)) + ", " +
             std::to_string(wrsize) + ") failed";
  }
  return *this;
}

FileBase& FstreamFile::seekg(i32 offset, u32 seekdir) {
  switch (seekdir) {
    case FILE_SET:
      stream_.seekg(offset, std::ios::beg);
      break;
    case FILE_CUR:
      stream_.seekg(offset, std::ios::cur);
      break;
    case FILE_END:
      stream_.seekg(offset, std::ios::end);
      break;
  }
  return *this;
}

FileBase& FstreamFile::seekp(i32 offset, u32 seekdir) {
  switch (seekdir) {
    case FILE_SET:
      stream_.seekp(offset, std::ios::beg);
      break;
    case FILE_CUR:
      stream_.seekp(offset, std::ios::cur);
      break;
    case FILE_END:
      stream_.seekp(offset, std::ios::end);
      break;
  }
  return *this;
}

i32 FstreamFile::tellg() { return stream_.tellg(); }

i32 FstreamFile::tellp() { return stream_.tellp(); }

bool FstreamFile::good() { return !!is_good_; }

std::string FstreamFile::error() {
  std::string ret = error_;

  // 还原文件流状态。
  is_good_ = 1;
  error_ = "";
  stream_.clear();

  return ret;
}