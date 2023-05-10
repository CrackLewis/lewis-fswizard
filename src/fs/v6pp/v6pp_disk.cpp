/**
 * @file v6pp_disk.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-29 13:29:14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>
#include <functional>

#include "defines.hpp"
#include "exceptions.hpp"
#include "io_fstream_file.hpp"
#include "util_time.hpp"
#include "v6pp_block.hpp"
#include "v6pp_disk.hpp"
#include "v6pp_inode.hpp"
#include "v6pp_inode_directory.hpp"
#include "v6pp_superblock.hpp"

using namespace v6pp;
using namespace io;

/**
 * @brief
 *
 * ��ʼ��V6++���̣��򿪺�У��V6++�����ļ���
 * ע�⣺��Ҫͨ��load�ֶ����ش��̡�
 *
 * @param filepath
 */
Disk::Disk(const std::string& filepath) {
  file_ = new FstreamFile(filepath);
  // У���ļ��ߴ硣
  file_->seekg(0, FileBase::FILE_END);
  u32 fsize = file_->tellg();

  if (fsize != DiskProps::get_disk_size()) {
    delete file_;
    file_ = nullptr;

    auto ex = FileSystemException("invalid diskfile size");
    ex.set_kv("fname", filepath);
    ex.set_kv("actual_size", fsize);
    ex.set_kv("expected_size", DiskProps::get_disk_size());
    throw ex;
  }
}

/**
 * @brief
 *
 * ���������inode��д�ش��̣����رմ����ļ���
 * ����V6++���̵Ļ���ڡ�
 */
Disk::~Disk() {
  if (file_) {
    update();
    delete file_;
    file_ = nullptr;
  }
}

/**
 * @brief
 *
 * ������̳������inode����׼����ʼ���̷���
 */
void Disk::load() {
  // ��ȡ�����顣
  if (!superblock_.load(*file_)) {
    throw FileSystemException("Disk::load: superblock reading failed");
  }

  // ��ȡInode����
  u32 inode_off = superblock_.p_off_inodes_ * DiskProps::BLOCK_SIZE;
  u32 inode_size = superblock_.p_size_inodes_ * DiskProps::BLOCK_SIZE;

  file_->seekg(inode_off, FileBase::FILE_SET);
  file_->read((char*)inodes_, inode_size);
  if (!file_->good()) {
    throw FileSystemException(file_->error());
  }
  if (file_->tellg() != inode_off + inode_size) {
    auto ex = FileSystemException("Disk::load: broken inode area");
    ex.set_kv("actual_bytes", file_->tellg() - inode_off);
    ex.set_kv("expected_bytes", inode_size);
    throw ex;
  }

  // ԭ���߽�VFS�ʹ����߼��ۺϳ���һ��FileSystemAdapter��
  // ���������Ƿֿ���Ƶģ����Գ�ʼ���û�·����һ���ŵ�VFSʵ�֡�
}

/**
 * @brief
 *
 * ��ʽ�������ļ���
 *
 * ���������������ڣ�
 * - ���賬���顣
 * - �ͷ����е�inode��
 * - �����еĿ����̿����ӵ��̿�������
 * - ���������ø��ļ�inode��
 * - ����һЩ��Ҫ���ļ���
 *
 * ��ʽ�������������������ںˣ�Ҳ���᳹��Ĩ���������ݡ�
 */
void Disk::format() {
  // TODO: ������ܽ��������ļ�ϵͳʵ�֡�
}

/**
 * @brief
 *
 * ���������inodeд�ش��̡�
 */
void Disk::update() {
  // ������д����̡�
  if (!superblock_.update(*file_)) {
    throw FileSystemException("Disk::update: superblock update failed");
  }

  // inodeд����̡�
  u32 inode_off = superblock_.p_off_inodes_ * DiskProps::BLOCK_SIZE;
  u32 inode_size = superblock_.p_size_inodes_ * DiskProps::BLOCK_SIZE;
  file_->seekp(inode_off, FileBase::FILE_SET);
  file_->write((char*)inodes_, inode_size);
  if (!file_->good()) {
    throw FileSystemException(file_->error());
  }
  if (file_->tellp() != inode_off + inode_size) {
    auto ex = FileSystemException("Disk::update: broken inode area");
    ex.set_kv("actual_bytes", file_->tellp() - inode_off);
    ex.set_kv("expected_bytes", inode_size);
    throw ex;
  }
}

/**
 * @brief
 *
 * ���ں��ļ�д������ļ���
 */
void Disk::write_kernel(const std::string& kernel_path) {
  u32 kernel_size = DiskProps::BLOCK_SIZE * DiskProps::BLOCKS_KERNEL;
  char* buf = new char[kernel_size];

  std::fstream fkern(kernel_path, std::ios::in | std::ios::binary);
  if (!fkern.is_open()) {
    throw FileSystemException("Disk::write_kernel: cannot open " + kernel_path);
  }

  fkern.clear();
  fkern.seekg(0, std::ios::beg);
  fkern.read(buf, kernel_size);

  write_blocks(buf, DiskProps::BLOCKS_BOOTLOAD, DiskProps::BLOCKS_KERNEL);

  delete[] buf;
}

void Disk::write_bootloader(const std::string& bootloader_path) {
  u32 bootloader_size = DiskProps::BLOCK_SIZE * DiskProps::BLOCKS_BOOTLOAD;
  char* buf = new char[bootloader_size];

  std::fstream fboot(bootloader_path, std::ios::in | std::ios::binary);
  if (!fboot.is_open()) {
    throw FileSystemException("Disk::write_bootloader: cannot open " +
                              bootloader_path);
  }

  fboot.clear();
  fboot.seekg(0, std::ios::beg);
  fboot.read(buf, bootloader_size);

  write_blocks(buf, 0, DiskProps::BLOCKS_BOOTLOAD);

  delete[] buf;
}

bool Disk::read_block(Block& block, i32 block_idx) {
  return read_blocks(block.data(), block_idx, 1);
}

bool Disk::read_blocks(char* dest, i32 block_idx, i32 block_cnt) {
  bool arg_check = true;
  // ������顣
  arg_check &= (dest != nullptr);
  arg_check &= (block_cnt > 0);
  arg_check &= (block_idx >= 0);
  arg_check &= (block_idx + block_cnt <= DiskProps::get_disk_blocks());

  if (!arg_check) {
    auto ex = FileSystemException("Disk::read_blocks: invalid arguments");
    ex.set_kv("dest", u64(dest));
    ex.set_kv("block_idx", block_idx);
    ex.set_kv("block_cnt", block_cnt);
    throw ex;
  }

  // ִ�ж��������
  file_->seekg(block_idx * DiskProps::BLOCK_SIZE, FileBase::FILE_SET);
  file_->read(dest, block_cnt * DiskProps::BLOCK_SIZE);
  return (file_->good()) ? true : (file_->error(), false);
}

bool Disk::write_block(const Block& block, i32 block_idx) {
  return write_blocks(block.data(), block_idx, 1);
}

bool Disk::write_blocks(const char* src, i32 block_idx, i32 block_cnt) {
  bool arg_check = true;
  // ��������
  arg_check &= (src != nullptr);
  arg_check &= (block_idx >= 0);
  arg_check &= (block_cnt > 0);
  arg_check &= (block_idx + block_cnt <= DiskProps::get_disk_blocks());

  if (!arg_check) {
    auto ex = FileSystemException("Disk::write_blocks: invalid arguments");
    ex.set_kv("src", u64(src));
    ex.set_kv("block_idx", block_idx);
    ex.set_kv("block_cnt", block_cnt);
    throw ex;
  }

  // ִ��д�������
  file_->seekp(block_idx * DiskProps::BLOCK_SIZE, FileBase::FILE_SET);
  file_->write(src, block_cnt * DiskProps::BLOCK_SIZE);
  return (file_->good()) ? true : (file_->error(), false);
}

bool Disk::read_file(char* dest, Inode& inode) {
  DiskBlockTraversalMixin mixin;

  mixin.direct_block_process_ = [&](i32 file_offset, i32 blk_idx) {
    read_blocks(dest + file_offset, blk_idx, 1);
  };
  mixin.failure_handler_ = [](Inode&, i32, const std::string& errmsg) {
    throw FileSystemException(errmsg);
  };

  return traverse_blocks_over_inode(inode, mixin);
}

bool Disk::write_file(const char* src, Inode& inode, i32 fsize) {
  // V6++�ļ�������С��

  if (fsize > FSIZE_MAX) {
    throw FileSystemException("Disk::write_file: maximum file size exceeded.");
  }
  // д�ļ�ǰ����Ҫ�����ļ���ԭ���ݡ�
  free_inode_blocks(inode);

  i32 size_remaining = fsize;
  inode.d_size_ = size_remaining;
  inode.ilarg_ = !!(size_remaining > sizeof(Block) * 6);
  // ��ΪV6++Ŀǰû�����û�������Ȩ��Ŀǰ����ν��
  inode.prot_owner_ = inode.prot_group_ = inode.prot_others_ = 7;

  DiskBlockTraversalMixin mixin;
  // ��ΪҪ����д�ļ���������Ҫ�������̿顣
  mixin.block_allocator_ = [&](i32 old_blk_idx) { return alloc_block(); };
  mixin.direct_block_process_ = [&](i32 file_offset, i32 blk_idx) {
    write_blocks(src + file_offset, blk_idx, 1);
  };
  // Ĭ���ǲ�д�ؼ��������ģ�������Ҫ�ֶ�д�ء�
  mixin.indirect_block_teardown_ = [&](const char* pblk, i32 blk_idx) {
    write_blocks(pblk, blk_idx, 1);
  };
  mixin.failure_handler_ = [&](Inode&, i32, const std::string& errmsg) {
    throw FileSystemException(errmsg);
  };

  return traverse_blocks_over_inode(inode, mixin);
}

i32 Disk::alloc_block() {
  i32 ret = -1;
  if (superblock_.s_nfree_ == 0) {
    // ����һ���̿��Ѿ��þ���
    ret = -1;
  } else if (superblock_.s_nfree_ >= 2) {
    // ���ζ�����ǰ�����̿鲻�������̿顣
    ret = superblock_.s_free_[--superblock_.s_nfree_];
  } else {
    // ����������ǰ�����̿��������̿顣
    ret = superblock_.s_free_[0];
    Block b;
    // ��������һ��������Ӧ�ù�ܶ�ȡ0���������������
    if (ret != 0)
      read_block(b, ret);
    else
      ret = -1;
    // ��Ϊ�����̿�洢s_nfree��s_free��
    // ����ʹ��memcpyһ���Ը��ǡ�
    memcpy(&superblock_.s_nfree_, b.data(), 101 * sizeof(u32));
  }

  if (ret >= i32(DiskProps::get_disk_blocks())) {
    auto ex = FileSystemException("Disk::alloc_block: invalid block index");
    ex.set_kv("ret", ret);
    throw ex;
  }
  return ret;
}

void Disk::free_block(i32 idx) {
  if (idx < 0 || idx >= i32(DiskProps::get_disk_blocks())) {
    auto ex = FileSystemException("Disk::free_block: invalid block index");
    ex.set_kv("idx", idx);
    throw ex;
  }

  // �����ǰ�޿����̿顣
  if (superblock_.s_nfree_ == 0) {
    superblock_.s_free_[0] = 0;
    superblock_.s_nfree_ = 1;
  }

  if (superblock_.s_nfree_ < 100) {
    // ����һ����ǰ�����̿�����δ����
    superblock_.s_free_[superblock_.s_nfree_++] = idx;
  } else {
    // ���ζ�����ǰ�����̿�����������
    // ����ǰ����д����̿飬�����̿���Ϊ�������������̿顣
    Block b;
    memcpy(b.data(), &superblock_.s_nfree_, 101 * sizeof(u32));
    write_block(b, idx);

    superblock_.s_nfree_ = 1;
    superblock_.s_free_[0] = idx;
  }
}

/**
 * @brief
 *
 * ����һ������inode��
 *
 * @return i32
 */
i32 Disk::alloc_inode() {
  auto find_free_inodes = [&]() {
    // ��Inode�������������н�㡣
    for (u32 idx = IDX_ROOT_INODE + 1,
             idx_end = sizeof(inodes_) / sizeof(Inode) - 1;
         idx <= idx_end && superblock_.s_ninode_ < 100; ++idx) {
      if (inodes_[idx].ialloc_ == 0) {
        superblock_.s_inode_[superblock_.s_ninode_++] = idx;
      }
    }
  };

  if (superblock_.s_ninode_ == 0) {
    find_free_inodes();
  }

  if (superblock_.s_ninode_ > 0) {
    // ȡ��һ������inode��
    i32 res = superblock_.s_inode_[--superblock_.s_ninode_];

    /**
     * @brief
     *
     * ����777Ȩ�޺ͷ���ʱ�䡣
     */
    inodes_[res].ialloc_ = 1;
    inodes_[res].prot_owner_ = 7;
    inodes_[res].prot_group_ = 7;
    inodes_[res].prot_others_ = 7;
    inodes_[res].d_size_ = 0;
    inodes_[res].d_nlink_ = 1;
    inodes_[res].is_gid_ = 0;
    inodes_[res].is_uid_ = 0;
    inodes_[res].d_uid_ = 0;
    inodes_[res].d_gid_ = 0;

    i32 tstamp = Time::stamp();
    inodes_[res].d_atime_ = tstamp;
    inodes_[res].d_mtime_ = tstamp;

    // ���inode�þ�������Ҫ���²��ҡ�
    if (superblock_.s_ninode_ == 0) {
      find_free_inodes();
    }

    return res;
  }

  // ����ʧ�ܡ�
  return -1;
}

void Disk::free_inode(i32 idx, bool free_blocks) {
  Inode& inode = inodes_[idx];
  if (free_blocks) free_inode_blocks(inode);

  inode.format();
  if (superblock_.s_ninode_ < 100) {
    superblock_.s_inode_[superblock_.s_ninode_++] = idx;
  }
}

void Disk::free_inode_blocks(Inode& inode) {
  DiskBlockTraversalMixin mixin;
  mixin.direct_block_teardown_ = [&](i32 file_offset, i32 blk_idx) {
    free_block(blk_idx);
  };
  mixin.indirect_block_teardown_ = [&](const char* pblk, i32 blk_idx) {
    free_block(blk_idx);
  };

  traverse_blocks_over_inode(inode, mixin);
  inode.d_size_ = 0u;
  for (i32 idx = 0; idx < 10; ++idx) *(inode.idx_direct_ + idx) = 0;
}

/**
 * @brief
 *
 * inode�ڱ����������ݿ顣
 *
 * ATTENTION:
 * mixin����Ĭ�ϲ�д�������̿顣
 * �����Ҫ�޸��ļ��������mixin������д�������̿顣
 */
bool Disk::traverse_blocks_over_inode(Inode& inode,
                                      const DiskBlockTraversalMixin& mixin) {
  // һЩ������
  static const i32 ENTRIES_PER_BLOCK = sizeof(Block) / sizeof(u32);
  static const i32 IDXS_DIRECT = 6;
  static const i32 IDXS_L1 = 2;
  static const i32 IDXS_L2 = 2;

  i32 size_remaining = inode.d_size_;
  u32 idx_block_l1[ENTRIES_PER_BLOCK];
  u32 idx_block_l2[ENTRIES_PER_BLOCK];

  try {
    /**
     * @brief
     *
     * ����6��ֱ�������̿顣
     */
    for (i32 idx = 0; size_remaining > 0 && idx < IDXS_DIRECT; ++idx) {
      // ѡ���Ը���ֱ��������Ĭ�ϲ����¡�
      i32 new_blk_idx = mixin.block_allocator_(inode.idx_direct_[idx]);
      if (new_blk_idx < 0)
        throw FileSystemException(
            "direct block allocation failed while traversing");
      inode.idx_direct_[idx] = new_blk_idx;
      // ���ݿ���������һ������ֻ�ú�������
      mixin.direct_block_setup_(idx * sizeof(Block), inode.idx_direct_[idx]);
      mixin.direct_block_process_(idx * sizeof(Block), inode.idx_direct_[idx]);
      mixin.direct_block_teardown_(idx * sizeof(Block), inode.idx_direct_[idx]);

      size_remaining -= sizeof(Block);
    }

    /**
     * @brief
     *
     * ����2��һ����������̿顣
     */
    for (i32 idx1 = 0; idx1 < IDXS_L1 && size_remaining > 0; ++idx1) {
      // ѡ���Ը���һ�����������Ĭ�ϲ����¡�
      i32 new_blk_idx = mixin.block_allocator_(inode.idx_indirect_[idx1]);
      if (new_blk_idx < 0)
        throw FileSystemException(
            "indirect block allocation failed while traversing");
      inode.idx_indirect_[idx1] = new_blk_idx;

      // �������̿鵽�������顣
      read_blocks((char*)idx_block_l1, inode.idx_indirect_[idx1], 1);
      mixin.indirect_block_setup_((char*)idx_block_l1,
                                  inode.idx_indirect_[idx1]);
      // ��������һ��������������������ݿ顣
      for (i32 idx = 0; idx < ENTRIES_PER_BLOCK && size_remaining > 0; ++idx) {
        i32 new_blk_idx = mixin.block_allocator_(idx_block_l1[idx]);
        if (new_blk_idx < 0)
          throw FileSystemException(
              "direct block allocation failed while traversing (under indirect "
              "block)");
        idx_block_l1[idx] = new_blk_idx;

        u32 blk_offset = DiskProps::BLOCK_SIZE *
                         (IDXS_DIRECT + ENTRIES_PER_BLOCK * idx1 + idx);

        mixin.direct_block_setup_(blk_offset, idx_block_l1[idx]);
        mixin.direct_block_process_(blk_offset, idx_block_l1[idx]);
        mixin.direct_block_teardown_(blk_offset, idx_block_l1[idx]);

        size_remaining -= sizeof(Block);
      }  // for(idx)
      mixin.indirect_block_teardown_((char*)idx_block_l1, new_blk_idx);
    }  // for(idx1)

    /**
     * @brief
     *
     * ����2��������������̿顣
     */
    for (i32 idx2 = 0; idx2 < IDXS_L2 && size_remaining > 0; ++idx2) {
      // ѡ���Ը��¶������������Ĭ�ϲ����¡�
      i32 new_blk_idx =
          mixin.block_allocator_(inode.idx_secondary_indirect_[idx2]);
      if (new_blk_idx < 0)
        throw FileSystemException(
            "secondary indirect block allocation failed while traversing");
      inode.idx_secondary_indirect_[idx2] = new_blk_idx;

      read_blocks((char*)idx_block_l2, inode.idx_secondary_indirect_[idx2], 1);
      mixin.indirect_block_setup_((char*)idx_block_l2,
                                  inode.idx_secondary_indirect_[idx2]);

      // �������һ��������
      for (i32 idx1 = 0; idx1 < ENTRIES_PER_BLOCK && size_remaining > 0;
           ++idx1) {
        // ѡ���Ը���һ�����������Ĭ�ϲ����¡�
        i32 new_blk_idx = mixin.block_allocator_(idx_block_l2[idx1]);
        if (new_blk_idx < 0)
          throw FileSystemException(
              "indirect block allocation failed while traversing (under "
              "secondary indirect block)");
        idx_block_l2[idx1] = new_blk_idx;

        read_blocks((char*)idx_block_l1, idx_block_l2[idx1], 1);
        mixin.indirect_block_setup_((char*)idx_block_l1, idx_block_l2[idx1]);

        // ����һ�����������������ݿ顣
        for (i32 idx = 0; size_remaining > 0 && idx < ENTRIES_PER_BLOCK;
             ++idx) {
          i32 new_blk_idx = mixin.block_allocator_(idx_block_l1[idx]);
          if (new_blk_idx < 0)
            throw FileSystemException(
                "direct block allocation failed while traversing (under "
                "secondary indirect block)");
          idx_block_l1[idx] = new_blk_idx;

          u32 blk_offset = DiskProps::BLOCK_SIZE *
                           (IDXS_DIRECT + IDXS_L1 * ENTRIES_PER_BLOCK +
                            idx2 * ENTRIES_PER_BLOCK * ENTRIES_PER_BLOCK +
                            idx1 * ENTRIES_PER_BLOCK + idx);

          mixin.direct_block_setup_(blk_offset, idx_block_l1[idx]);
          mixin.direct_block_process_(blk_offset, idx_block_l1[idx]);
          mixin.direct_block_teardown_(blk_offset, idx_block_l1[idx]);

          size_remaining -= sizeof(Block);
        }  // for(idx)
        mixin.indirect_block_teardown_((char*)idx_block_l1, new_blk_idx);
      }  // for(idx1)

      mixin.indirect_block_teardown_((char*)idx_block_l2, new_blk_idx);
    }  // for(idx2)

    return true;
  } catch (FileSystemException& e) {
    mixin.failure_handler_(inode, size_remaining, e.what());
    return false;
  }
}

bool Disk::traverse_inode_tree(Inode& inode,
                               const DiskInodeTravesalMixin& mixin) {
  // TODO: not implemented lol.
  return false;
}

std::unique_ptr<InodeDirectory> Disk::read_inode_directory(
    Inode& inode, bool ignore_ftype_check, int dir_stride) {
  auto ret = std::make_unique<InodeDirectory>();

  if (!ignore_ftype_check && inode.file_type_ != FileType::DIR) {
    auto ex = FileSystemException(
        "Disk::read_inode_directory: inode is not a directory.");
    ex.set_kv("inode_idx", (&inode - inodes_));
    throw ex;
  }

  i32 dirfile_size = inode.d_size_;
  i32 alloc_size = dirfile_size + sizeof(DirectoryEntry) * dir_stride;
  i32 entries_cnt = (alloc_size / DiskProps::BLOCK_SIZE +
                     !!(alloc_size % DiskProps::BLOCK_SIZE)) *
                    DiskProps::BLOCK_SIZE / sizeof(DirectoryEntry);

  ret->length_ = dirfile_size / sizeof(DirectoryEntry);
  ret->entries_ = new DirectoryEntry[entries_cnt];
  read_file((char*)ret->entries_, inode);

  return std::move(ret);
}