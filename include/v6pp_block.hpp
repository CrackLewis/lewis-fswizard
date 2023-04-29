/**
 * @file v6pp_block.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-28 23:17:29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef V6PP_BLOCK_HPP_
#define V6PP_BLOCK_HPP_

#include "defines.hpp"

namespace v6pp {

/**
 * @brief
 *
 * Unix V6++´ÅÅÌ¿é¡£
 */
class Block {
 public:
  char* data();

  const char* data() const;

 protected:
  byte data_[DiskProps::BLOCK_SIZE] = {0};
} __attribute__((packed));

}  // namespace v6pp

#endif  // V6PP_BLOCK_HPP_