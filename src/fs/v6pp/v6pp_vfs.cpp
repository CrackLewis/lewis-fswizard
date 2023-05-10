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
#include "util_time.hpp"
#include "v6pp_vfs.hpp"

using namespace v6pp;

/**
 * @brief
 *
 * 建立文件系统，加载磁盘。
 *
 * @param config
 */
FileSystem::FileSystem(const FileSystemConfig& config) : config_(config) {
  std::fstream ftest(config.disk_path_,
                     std::ios::in | std::ios::out | std::ios::binary);
  // 连文件都不让我进，那还是毙掉程序吧。
  if (!ftest.is_open()) {
    throw std::runtime_error("v6pp::FileSystem: cannot open disk file: " +
                             config.disk_path_);
  }

  // 校验文件尺寸。
  ftest.seekg(0, std::ios::end);
  i32 fsize = ftest.tellg();
  ftest.close();
  // 尝试加载硬盘。
  try {
    if (fsize != DiskProps::get_disk_size()) {
      // 尺寸不对：根据配置要么报错，要么格式化。
      if (!config.format_on_disksize_validation_failure_) {
        throw std::runtime_error("v6pp::FileSystem: bad disksize.");
      }

      // 重置文件大小。
      try {
        std::filesystem::resize_file(config.disk_path_,
                                     DiskProps::get_disk_size());
      } catch (std::filesystem::filesystem_error& e) {
        throw std::runtime_error("v6pp::FileSystem: error resizing diskfile: " +
                                 config.disk_path_);
      }

      // 格式化。
      disk_ = new Disk(config.disk_path_);
      format();
      disk_->load();
    } else {
      // 尺寸正确，直接加载。
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
    config_.speaker_("Usage: cd [PATH]");
    return 0;
  }
  if (args.size() == 0) {
    return pwd();
  }

  // 逐级尝试跳转。
  try {
    auto&& new_inode_idx_stack = _pwalk(args[0], true);
    inode_idx_stack_ = new_inode_idx_stack;
  } catch (FileSystemException& e) {
    config_.speaker_("cd: " + e.what());
    return -1;
  }

  return 0;
}

i32 FileSystem::pwd(const ArgPack& args) {
  if (args.size() != 0) {
    config_.speaker_("Usage: pwd");
    return 0;
  }

  std::string path = _getcwd();
  config_.speaker_(path);
  return 0;
}

i32 FileSystem::mkdir(const ArgPack& args) {
  if (args.size() != 1) {
    config_.speaker_("Usage: mkdir NEWDIR");
    return 0;
  }

  try {
    _touch(args[0], FileType::DIR);
  } catch (FileSystemException& e) {
    config_.speaker_("mkdir: " + e.what());
    return -1;
  }
  return 0;
}

i32 FileSystem::rmdir(const ArgPack& args) {
  if (args.size() != 1) {
    config_.speaker_("Usage: rmdir DIR");
    return 0;
  }

  try {
    _rmfile(args[0], FileType::DIR);
  } catch (FileSystemException& e) {
    config_.speaker_("rmdir: " + e.what());
    return -1;
  }
  return 0;
}

i32 FileSystem::touch(const ArgPack& args) {
  if (args.size() != 1) {
    config_.speaker_("Usage: touch NEWFILE");
    return 0;
  }

  try {
    _touch(args[0], FileType::NORMAL);
  } catch (FileSystemException& e) {
    config_.speaker_("touch: " + e.what());
    return -1;
  }
  return 0;
}

i32 FileSystem::rm(const ArgPack& args) {
  if (args.size() != 1) {
    config_.speaker_("Usage: rm PATH");
    return 0;
  }

  try {
    auto idx_stk = _pwalk(args[0], false);
    Inode& inode = disk_->inodes_[idx_stk.back()];

    // 目录类型：需要和用户确认。
    if (inode.file_type_ == FileType::DIR) {
      // 删除的目录不能是当前目录或其祖先目录。
      if (inode_idx_stack_.size() >= idx_stk.size() &&
          idx_stk.back() == inode_idx_stack_[idx_stk.size() - 1]) {
        throw FileSystemException(
            "removing a super-directory of the current working directory is "
            "prohibited");
      }

      // 和用户作确认。
      auto confirm =
          config_.asker_("Warning: sure to remove the directory \"" + args[0] +
                         "\" and all of its content? [Y/N]");
      if (confirm.length() != 1 || tolower(confirm.front()) != 'y') {
        config_.speaker_("Abort.");
        return -1;
      }

      // 递归删除所有子目录和子文件。
      std::function<void(Inode&, std::string)> remove_recurse =
          [&](Inode& inode, std::string cwd) {
            auto dir = disk_->read_inode_directory(inode);
            while (cwd.back() == '/' || cwd.back() == '\\') cwd.pop_back();

            for (i32 idx = 0; idx < dir->length_; ++idx) {
              Inode& subnode = disk_->inodes_[dir->entries_[idx].inode_id_];
              if (subnode.file_type_ == FileType::DIR) {
                remove_recurse(subnode, cwd + '/' + dir->entries_[idx].name_);
              }
              disk_->free_inode_blocks(subnode);
              disk_->free_inode(dir->entries_[idx].inode_id_);
            }
            // 直接写空目录可以更快地清空文件。
            // 为什么不调_rmfile？试试有多慢:)
            disk_->write_file((char*)dir->entries_, inode, 0);
          };

      remove_recurse(inode, args[0]);
      disk_->free_inode_blocks(inode);
      _rmfile(args[0], FileType::DIR);
    }  // if(inode.file_type_==DIR)
    else {
      _rmfile(args[0], FileType(inode.file_type_));
    }
  } catch (FileSystemException& e) {
    config_.speaker_("rm: " + e.what());
    return -1;
  }
  return 0;
}

// TODO: 算了，要是做文件树移动和复制的话，肝要废了。留给后人解决罢。

i32 FileSystem::cp(const ArgPack& args) {
  if (args.size() != 2) {
    config_.speaker_("Usage: cp SRCFILE DESTFILE");
    return -1;
  }

  try {
    auto src_idx_stk = _pwalk(args[0], false);

    Inode& src_inode = disk_->inodes_[src_idx_stk.back()];
    if (src_inode.file_type_ != FileType::NORMAL)
      throw FileSystemException("source file is not a normal file.");
    Inode& dst_inode = _touch(args[1], FileType::NORMAL);

    char* fbuf = new char[(src_inode.d_size_ + DiskProps::BLOCK_SIZE)];
    disk_->read_file(fbuf, src_inode);
    disk_->write_file(fbuf, dst_inode, src_inode.d_size_);
    delete[] fbuf;
    // 这里比较懒，没用在线的方式实现文件写入，或许大神您能实现。
  } catch (FileSystemException& e) {
    config_.speaker_("cp: " + e.what());
    return -1;
  }
  return 0;
}

i32 FileSystem::mv(const ArgPack& args) {
  if (args.size() != 2) {
    config_.speaker_("Usage: mv SRCFILE DESTFILE");
    return -1;
  }

  try {
    auto src_idx_stk = _pwalk(args[0], false);

    Inode& src_inode = disk_->inodes_[src_idx_stk.back()];
    if (src_inode.file_type_ != FileType::NORMAL)
      throw FileSystemException("source file is not a normal file: " + args[0]);
    Inode& dst_inode = _touch(args[1], FileType::NORMAL);

    char* fbuf = new char[(src_inode.d_size_ + DiskProps::BLOCK_SIZE)];
    disk_->read_file(fbuf, src_inode);
    disk_->write_file(fbuf, dst_inode, src_inode.d_size_);
    delete[] fbuf;

    _rmfile(args[0], FileType::NORMAL);
  } catch (FileSystemException& e) {
    config_.speaker_("mv: " + e.what());
    return -1;
  }
  return 0;
}

i32 FileSystem::ls(const ArgPack& args) {
  if (args.size() > 1) {
    config_.speaker_("Usage: ls [PATH]");
    return -1;
  }

  try {
    Inode& inode =
        disk_->inodes_[args.size() == 0 ? inode_idx_stack_.back()
                                        : _pwalk(args[0], true).back()];
    auto dir = disk_->read_inode_directory(inode);

    struct LsEntry {
      std::string fname_;
      i32 fsize_ = 0;
      i32 ftype_;
      i32 inode_id_ = 0;
      i32 block_id_[10] = {};
    };
    std::vector<LsEntry> ls_entries;

    for (i32 idx = 0; idx < dir->length_; ++idx) {
      Inode& sub_inode = disk_->inodes_[dir->entries_[idx].inode_id_];
      LsEntry entry;
      entry.fname_ = dir->entries_[idx].name_;
      entry.fsize_ = sub_inode.d_size_;
      entry.ftype_ = sub_inode.file_type_;
      entry.inode_id_ = dir->entries_[idx].inode_id_;
      memcpy(entry.block_id_, sub_inode.idx_direct_, 10 * sizeof(i32));

      ls_entries.emplace_back(entry);
    }

    char logbuf[120];
    sprintf(logbuf, "%6s%28s%10s%6s%60s", "FType", "FileName", "FileSize",
            "Inode", "BlockID");
    config_.speaker_(logbuf);

    for (auto& entry : ls_entries) {
      sprintf(logbuf, "%6d%28s%10d%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d",
              entry.ftype_, entry.fname_.data(), entry.fsize_, entry.inode_id_,
              entry.block_id_[0], entry.block_id_[1], entry.block_id_[2],
              entry.block_id_[3], entry.block_id_[4], entry.block_id_[5],
              entry.block_id_[6], entry.block_id_[7], entry.block_id_[8],
              entry.block_id_[9]);
      config_.speaker_(logbuf);
    }
  } catch (FileSystemException& e) {
    config_.speaker_("ls: " + e.what());
    return -1;
  }
  return 0;
}

i32 FileSystem::upload(const ArgPack& args) {
  if (args.size() != 2) {
    config_.speaker_("Usage: upload LOCALPATH DISKPATH");
    return 0;
  }

  try {
    std::fstream flocal(args[0], std::ios::in | std::ios::binary);
    if (!flocal.is_open()) {
      config_.speaker_("upload: cannot open local file: " + args[0]);
      return -1;
    }

    Inode& inode = _touch(args[1], FileType::NORMAL);

    flocal.clear();
    flocal.seekg(0, std::ios::end);
    i32 fsize = flocal.tellg();

    // 文件大小检查目前没啥意义，但还是象征性写一下。
    if (fsize > i32(Disk::FSIZE_MAX)) {
      config_.speaker_("upload: local file size too large (" +
                       std::to_string(fsize) + " B)");
      return -1;
    }

    i32 size_remaining = fsize;
    inode.d_size_ = size_remaining;
    inode.ilarg_ = !!(size_remaining > i32(6 * sizeof(Block)));
    inode.prot_owner_ = inode.prot_group_ = inode.prot_others_ = 7;

    flocal.seekg(0, std::ios::beg);

    // 遍历盘块，写入磁盘文件。
    DiskBlockTraversalMixin mixin;
    mixin.block_allocator_ = [&](i32 old_blk_idx) {
      return disk_->alloc_block();
    };
    mixin.direct_block_process_ = [&](i32 fileoff, i32 blk_idx) {
      Block b;
      flocal.read(b.data(), sizeof(b));
      disk_->write_block(b, blk_idx);
    };
    mixin.indirect_block_teardown_ = [&](const char* pblk, i32 blk_idx) {
      disk_->write_blocks(pblk, blk_idx, 1);
    };

    disk_->traverse_blocks_over_inode(inode, mixin);
  } catch (FileSystemException& e) {
    config_.speaker_("upload: " + e.what());
    return -1;
  }
  return 0;
}

i32 FileSystem::download(const ArgPack& args) {
  if (args.size() != 2) {
    config_.speaker_("Usage: download LOCALPATH DISKPATH");
    return -1;
  }

  try {
    Inode& inode = disk_->inodes_[_pwalk(args[1], FileType::NORMAL).back()];

    std::fstream flocal(args[0], std::ios::out | std::ios::binary);
    if (!flocal.is_open()) {
      config_.speaker_("download: cannot open local file: " + args[0]);
      return -1;
    }

    flocal.clear();
    flocal.seekp(0, std::ios::beg);

    DiskBlockTraversalMixin mixin;
    mixin.direct_block_process_ = [&](i32, i32 blk_idx) {
      Block b;
      disk_->read_block(b, blk_idx);
      i32 wrlen = std::min(sizeof(b), (size_t)inode.d_size_ - flocal.tellp());
      flocal.write(b.data(), wrlen);
    };
    mixin.failure_handler_ = [&](Inode&, i32, const std::string& msg) {
      throw FileSystemException(msg);
    };
    disk_->traverse_blocks_over_inode(inode, mixin);
  } catch (FileSystemException& e) {
    config_.speaker_("download: " + e.what());
    return -1;
  }
  return 0;
}

i32 FileSystem::format(const ArgPack& args) {
  if (args.size() != 0) {
    config_.speaker_("Usage: format");
    return -1;
  }

  try {
    auto confirm = config_.asker_(
        "Warning: all data will be lost on formatting. Are you sure? [y/N]");
    if (confirm.length() != 1 || tolower(confirm[0]) != 'y') {
      config_.speaker_("Abort.");
      return -1;
    }

    disk_->superblock_.format();
    // 因为超级块已经重置，所以磁盘此时已经丧失了对所有inode和磁盘块的追踪，
    // 因此直接遍历释放一遍inode和盘块就行了。
    for (i32 idx = Disk::IDX_ROOT_INODE;
         idx < DiskProps::BLOCKS_INODE_ZONE * sizeof(Block) / sizeof(Inode);
         ++idx)
      disk_->free_inode(idx);
    for (i32 idx = disk_->superblock_.p_off_data_;
         idx < disk_->superblock_.p_off_data_ + disk_->superblock_.p_size_data_;
         ++idx)
      disk_->free_block(idx);

    inode_idx_stack_.clear();
    inode_idx_stack_.push_back(Disk::IDX_ROOT_INODE);

    Inode& root = disk_->inodes_[Disk::IDX_ROOT_INODE];
    root.prot_owner_ = root.prot_group_ = root.prot_others_ = 7;
    root.d_nlink_ = 1;
    root.file_type_ = FileType::DIR;
    root.d_size_ = 0;
    root.d_mtime_ = Time::stamp();
    root.d_atime_ = Time::stamp();
    root.is_gid_ = root.is_uid_ = 0;
    root.d_gid_ = root.d_uid_ = 0;
    root.ialloc_ = 1;

    mkdir({"dev"});
    _touch("/dev/tty1", FileType::CHAR_DEV);
    _touch("/dingzhen", FileType::NORMAL);

    config_.speaker_("Formatting complete.");
  } catch (FileSystemException& e) {
    // 格式化寄了的话，啥也别玩了吧。。。
    throw std::runtime_error("format: " + e.what());
  }
  return 0;
}

std::vector<i32> FileSystem::_pwalk(const std::string& path,
                                    bool to_directory) {
  std::vector<i32> ret = inode_idx_stack_;
  // 路径分段和检查。
  bool is_absolute = (path[0] == '/' || path[0] == '\\');
  std::vector<std::string> pathsegs;
  std::string buf = "";
  for (char ch : path) {
    if (ch == '/' || ch == '\\') {
      // 该分隔符前有确切路径名
      if (buf.length() > 0)
        pathsegs.push_back(buf), buf = "";
      else if (!pathsegs.empty())
        throw FileSystemException(
            "FileSystem::_pwalk: adjacent delimiters are not permitted.");
    } else {
      buf += ch;
    }
  }
  if (buf.length() > 0) pathsegs.push_back(buf);

  if (is_absolute) {
    ret.clear();
    ret.push_back(Disk::IDX_ROOT_INODE);
  }

  // 逐级尝试跳转。
  for (auto seg : pathsegs) {
    if (seg == ".")
      continue;
    else if (seg == "..") {
      // 多余的上一级跳转会被吃掉。
      if (ret.size() > 1) ret.pop_back();
    } else {
      auto curdir =
          disk_->read_inode_directory(disk_->inodes_[ret.back()], true);
      i32 found = 0;
      for (i32 idx = 0; idx < curdir->length_; ++idx) {
        if (seg == curdir->entries_[idx].name_) {
          found = 1;
          Inode& inode = disk_->inodes_[curdir->entries_[idx].inode_id_];
          if (to_directory && inode.file_type_ != FileType::DIR) {
            throw FileSystemException("FileSystem::_pwalk: not a directory: " +
                                      seg);
          } else {
            ret.push_back(curdir->entries_[idx].inode_id_);
          }
        }  // if(seg==name_)
      }    // for(idx)

      if (!found)
        throw FileSystemException(
            "FileSystem::_pwalk: cannot find directory: " + seg);
    }  // else
  }    // for(seg:pathsegs)

  return ret;
}

std::string FileSystem::_getcwd() {
  if (inode_idx_stack_.size() == 1) return "/";
  std::string path = "";

  try {
    for (i32 sidx = 1; sidx < inode_idx_stack_.size(); ++sidx) {
      // 在前一级目录的子目录内寻找本级目录的名称。
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
        throw std::runtime_error("FileSystem::_getcwd: directory missing.");
    }
  } catch (FileSystemException& e) {
    throw std::runtime_error("FileSystem::_getcwd: " + e.what());
  }

  return path;
}

Inode& FileSystem::_touch(const std::string& path, FileType ftype) {
  i32 last_delim = -1;
  for (i32 idx = path.length() - 1; idx >= 0; --idx) {
    if (path[idx] == '/' || path[idx] == '\\') {
      last_delim = idx;
      break;
    }
  }

  // 目录名超过长度限制。
  if (i32(path.length()) - last_delim >= i32(sizeof(DirectoryEntry::name_))) {
    throw FileSystemException(
        "FileSystem::_touch: file or directory name exceeds length limit.");
  }

  Inode& parent_inode =
      disk_->inodes_[(last_delim == -1)
                         ? (inode_idx_stack_.back())
                         : (_pwalk(path.substr(0, last_delim), true).back())];
  std::string fname = path.substr(last_delim + 1);
  // dir_stride=1，因为有可能需要写入一个新目录项。
  auto parent_dir = disk_->read_inode_directory(parent_inode, false, 1);
  // 目录下检查是否有同名文件。

  for (i32 idx = 0; idx < parent_dir->length_; ++idx) {
    if (parent_dir->entries_[idx].name_ == fname) {
      throw FileSystemException(
          "FileSystem::_touch: file or directory already exists: " + fname);
    }
  }

  i32 new_idx = disk_->alloc_inode();
  if (new_idx < 0)
    throw FileSystemException("FileSystem::_touch: out of inode.");
  Inode& new_inode = disk_->inodes_[new_idx];

  new_inode.file_type_ = ftype;
  parent_dir->entries_[parent_dir->length_].inode_id_ = new_idx;
  memset((parent_dir->entries_[parent_dir->length_].name_), 0,
         sizeof(DirectoryEntry::name_));
  memcpy((parent_dir->entries_[parent_dir->length_].name_), fname.data(),
         fname.length() + 1);
  parent_dir->length_++;

  // 写回去。
  disk_->write_file((char*)(parent_dir->entries_), parent_inode,
                    (parent_dir->length_) * sizeof(DirectoryEntry));

  return new_inode;
}

/**
 * @brief
 *
 * 尝试删除单个文件。
 * 如果指定ftype=DIR，则目录必须为空。
 *
 * @param path
 */
void FileSystem::_rmfile(const std::string& path, FileType ftype) {
  // 必须是一个已存在的目录。
  auto idx_stk = _pwalk(path, ftype == FileType::DIR);
  if (idx_stk.size() == 1) {
    throw FileSystemException(
        "FileSystem::_rmfile: removing root directory is prohibited.");
  }
  Inode& inode_tar = disk_->inodes_[idx_stk.back()];
  Inode& inode_fa = disk_->inodes_[*-- --idx_stk.end()];

  if (ftype == FileType::DIR) {
    // 目录不能是VFS当前访问的目录。
    if (idx_stk.back() == inode_idx_stack_.back())
      throw FileSystemException(
          "FileSystem::_rmfile: the directory specified is currently being "
          "visited by VFS.");
    // 目录不能为非空。
    auto tar_dir = disk_->read_inode_directory(inode_tar);
    if (tar_dir->length_ != 0)
      throw FileSystemException(
          "FileSystem::_rmfile: cannot remove a non-empty directory: " + path);
  }

  // 从父目录删除对应的表项。
  auto fa_dir = disk_->read_inode_directory(inode_fa);
  for (i32 idx = 0, del = -1; idx < fa_dir->length_; ++idx) {
    if (fa_dir->entries_[idx].inode_id_ == idx_stk.back()) {
      del = idx;
    } else if (~del) {
      memcpy(fa_dir->entries_ + idx - 1, fa_dir->entries_ + idx,
             sizeof(DirectoryEntry));
    }
  }
  fa_dir->length_--;

  // 将对文件所在目录的修改写回。
  disk_->write_file((char*)fa_dir->entries_, inode_fa,
                    fa_dir->length_ * sizeof(DirectoryEntry));

  // 将子文件删除，并释放该文件占用的inode资源。
  disk_->free_inode_blocks(inode_tar);
  disk_->free_inode(idx_stk.back());
}