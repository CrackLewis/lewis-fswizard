CC = g++
SRCS = src/common/argparse.cpp \
		src/common/defines.cpp \
		src/common/exceptions.cpp \
		src/common/vfs.cpp \
		src/fs/v6pp/v6pp_block.cpp \
		src/fs/v6pp/v6pp_disk.cpp \
		src/fs/v6pp/v6pp_inode_directory.cpp \
		src/fs/v6pp/v6pp_inode.cpp \
		src/fs/v6pp/v6pp_superblock.cpp \
		src/fs/v6pp/v6pp_vfs.cpp \
		src/io/file.cpp \
		src/io/fstream_file.cpp \
		src/util/stringcast.cpp \
		src/util/time.cpp 

INCLUDE = include
CFLAGS = -g -O2 -I$(INCLUDE)
TARGETDIR = build

.PHONY: makeimage
makeimage:
	$(CC) $(CFLAGS) -o $(TARGETDIR)/makeimage.exe $(SRCS) src/app/makeimage.cpp 

.PHONY: v6ppfscli
v6ppfscli:
	$(CC) $(CFLAGS) -o $(TARGETDIR)/v6pp-fs-cli.exe $(SRCS) src/app/v6pp-fs-cli.cpp 

.PHONY: all
all: makeimage v6ppfscli