/**
 * @file v6pp_block.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 12:28:43
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "v6pp_block.hpp"

using namespace v6pp;

char* Block::data() { return (char*)data_; }

const char* Block::data() const { return (const char*)data_; }