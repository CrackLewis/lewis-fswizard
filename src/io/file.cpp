/**
 * @file file.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 13:44:28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "io_file.hpp"

using namespace io;

FileBase::FileBase(const std::string& filepath)
    : file_path_(filepath), is_good_(1), error_() {}

FileBase::~FileBase() {}