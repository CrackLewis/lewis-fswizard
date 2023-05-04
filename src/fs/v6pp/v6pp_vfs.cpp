/**
 * @file v6pp_vfs.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-03 14:19:15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "exceptions.hpp"
#include "v6pp_vfs.hpp"

using namespace v6pp;

/**
 * @brief
 *
 * �����ļ�ϵͳ�����ش��̡�
 *
 * @param config
 */
FileSystem::FileSystem(const FileSystemConfig& config) : config_(config) {
  std::fstream ftest(config.disk_path_,
                     std::ios::in | std::ios::out | std::ios::binary);
  // ���ļ��������ҽ����ǻ��Ǳе�����ɡ�
  if (!ftest.is_open()) {
    throw std::runtime_error("v6pp::FileSystem: cannot open disk file: " +
                             config.disk_path_);
  }

  // У���ļ��ߴ硣
  ftest.seekg(0, std::ios::end);
  i32 fsize = ftest.tellg();
  ftest.close();
  // ���Լ���Ӳ�̡�
  try {
    if (fsize != DiskProps::get_disk_size()) {
      // �ߴ粻�ԣ���������Ҫô����Ҫô��ʽ����
      if (!config.format_on_disksize_validation_failure_) {
        throw std::runtime_error("v6pp::FileSystem: bad disksize.");
      }

      // �����ļ���С��
      try {
        std::filesystem::resize_file(config.disk_path_,
                                     DiskProps::get_disk_size());
      } catch (std::filesystem::filesystem_error& e) {
        throw std::runtime_error("v6pp::FileSystem: error resizing diskfile: " +
                                 config.disk_path_);
      }

      // ��ʽ����
      disk_ = new Disk(config.disk_path_);
      format();
      disk_->load();
    } else {
      // �ߴ���ȷ��ֱ�Ӽ��ء�
      disk_ = new Disk(config.disk_path_);
      disk_->load();
    }
  } catch (FileSystemException& e) {
    throw std::runtime_error("v6pp::FileSystem: " + e.what());
  }

  inode_idx_stack_.push_back(Disk::IDX_ROOT_INODE);
}

FileSystem::~FileSystem() {
  if (disk_) {
    delete disk_;
    disk_ = nullptr;
  }
}

i32 FileSystem::cd(const ArgPack& args) {
  if (args.size() > 1) {
    config_.speaker_("Usage: cd | cd [PATH]");
    return 0;
  }
  if (args.size() == 0) {
    return pwd();
  }

  // ·���ֶκͼ�顣
  bool is_absolute = !(args[0][0] == '/' || args[0][0] == '\\');
  std::vector<std::string> pathsegs;
  std::string buf = "";
  for (char ch : args[0]) {
    if (ch == '/' || ch == '\\') {
      // �÷ָ���ǰ��ȷ��·���������߸÷ָ����Ǿ���·����ʶ
      if (buf.length() > 0 || pathsegs.empty())
        pathsegs.push_back(buf), buf = "";
      else {
        config_.speaker_("cd: paths with adjacent delimiters are invalid.");
        return -1;
      }
    } else {
      buf += ch;
    }
  }
  if (buf.length() > 0) pathsegs.push_back(buf);

  if (is_absolute) {
    inode_idx_stack_.clear();
    inode_idx_stack_.push_back(Disk::IDX_ROOT_INODE);
  }

  // �𼶳�����ת��
  try {
    for (auto seg : pathsegs) {
      if (seg == ".")
        continue;
      else if (seg == "..") {
        // �������һ����ת�ᱻ�Ե���
        if (inode_idx_stack_.size() > 1) inode_idx_stack_.pop_back();
      } else {
        auto curdir = disk_->read_inode_directory(
            disk_->inodes_[inode_idx_stack_.back()]);
        i32 found = 0;
        for (i32 idx = 0; idx < curdir->length_; ++idx) {
          if (seg == curdir->entries_[idx].name_) {
            found = 1;
            Inode& inode = disk_->inodes_[curdir->entries_[idx].inode_id_];
            if (inode.file_type_ != FileType::DIR) {
              config_.speaker_("cd: not a directory: " + seg);
              return -1;
            } else {
              inode_idx_stack_.push_back(curdir->entries_[idx].inode_id_);
            }
          }
        }

        if (!found) {
          config_.speaker_("cd: cannot find directory: " + seg);
          return -1;
        }
      }
    }
  } catch (FileSystemException& e) {
    config_.speaker_("cd: invalid path: " + args[0]);
    return -1;
  }

  return 0;
}

i32 FileSystem::pwd(const ArgPack& args) {
  if (args.size() != 0) {
    config_.speaker_("Usage: pwd");
    return 0;
  }

  if (inode_idx_stack_.size() == 1) {
    config_.speaker_("/");
    return 0;
  }

  try {
    std::string path = "";
    for (i32 sidx = 1; sidx < inode_idx_stack_.size(); ++sidx) {
      // ��ǰһ��Ŀ¼����Ŀ¼��Ѱ�ұ���Ŀ¼�����ơ�
      auto predir = disk_->read_inode_directory(
          disk_->inodes_[inode_idx_stack_[sidx - 1]]);
      i32 found = 0;
      for (i32 diridx = 0; diridx < predir->length_; ++diridx) {
        if (inode_idx_stack_[sidx] == predir->entries_[diridx].inode_id_) {
          found = 1;
          (path += '/') += predir->entries_[diridx].name_;
        }
      }
      if (!found)
        throw FileSystemException("FileSystem::pwd: directory missing.");
    }

    config_.speaker_(path);
  } catch (FileSystemException& e) {
    throw std::runtime_error(e.what());
  }
  return 0;
}

i32 FileSystem::mkdir(const ArgPack& args) {
  if (args.size() != 1) {
    config_.speaker_("Usage: mkdir [NEWDIR]");
    return 0;
  }

  
}