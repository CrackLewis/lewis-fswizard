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
 * �����ļ�ϵͳ���û�����ӿڡ�
 *
 * �ýӿ����û����񣬷�װ���̲���ϸ�ڡ�
 * �ϸ���������������һ��Ӧ����������е��ļ�ϵͳ�쳣��
 */
class FileSystemBase {
 public:
  /**
   * @brief
   *
   * �����в�������������������
   */
  using ArgPack = std::vector<std::string>;

 public:
  /**
   * @brief
   *
   * ���汻��Ϊ�ǽ��д��̻�����������С�����
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
   * �п�Ҳ����д������Unix�ļ�ָ�����棬����ʵ����û�����ˡ�
   * E.g. chown/chmod/grep/head/tail/cat
   */
};

#endif