/**
 * @file exceptions.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 14:34:45
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <map>
#include <stdexcept>

class FileSystemException : public std::exception {
 public:
  explicit FileSystemException(const std::string& statement);

  std::string what();

  template <class ValTy>
  void set_kv(const std::string& key, ValTy value) {
    dict_[key] = std::to_string(value);
  }

 protected:
  std::string stmt_;
  std::map<std::string, std::string> dict_;
};

#endif