/**
 * @file vfs.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-02 18:11:38
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef FS_USER_HPP_
#define FS_USER_HPP_

#include <string>
#include <vector>

#include "defines.hpp"

/**
 * @brief
 *
 * 虚拟文件系统的用户界面接口。
 *
 * 该接口向用户服务，封装磁盘操作细节。
 * 严格意义上来讲，这一层应当处理掉所有的文件系统异常。
 */
class FileSystemBase {
 public:
  /**
   * @brief
   *
   * 命令行参数包。不带程序名。
   */
  using ArgPack = std::vector<std::string>;

 public:
  /**
   * @brief
   *
   * 下面被认为是进行磁盘基本操作的最小命令集。
   */

  virtual ~FileSystemBase();

  virtual i32 cd(const ArgPack& args) = 0;
  virtual i32 pwd(const ArgPack& args) = 0;

  virtual i32 mkdir(const ArgPack& args) = 0;
  virtual i32 rmdir(const ArgPack& args) = 0;

  virtual i32 touch(const ArgPack& args) = 0;
  virtual i32 rm(const ArgPack& args) = 0;
  virtual i32 cp(const ArgPack& args) = 0;
  virtual i32 mv(const ArgPack& args) = 0;

  virtual i32 ls(const ArgPack& args) = 0;

  virtual i32 upload(const ArgPack& args) = 0;
  virtual i32 download(const ArgPack& args) = 0;

  virtual i32 format(const ArgPack& args) = 0;

  /**
   * @brief
   *
   * 有空也可以写其他的Unix文件指令玩玩，但我实在是没精力了。
   * E.g. chown/chmod/grep/head/tail/cat
   */
};

#endif