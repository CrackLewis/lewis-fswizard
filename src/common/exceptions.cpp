/**
 * @file exceptions.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 16:17:22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "exceptions.hpp"

FileSystemException::FileSystemException(const std::string& statement)
    : stmt_(statement) {}

std::string FileSystemException::what() {
  std::string result = stmt_;
  bool comma = false;
  result += "(";
  for (auto&& pair : dict_) {
    if (comma)
      result += ",";
    else
      comma = true;
    result += pair.first + ": " + pair.second;
  }
  result += ")";
  return result;
}