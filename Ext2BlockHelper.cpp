#include <iostream>
#include <sstream>

#include "Ext2BlockHelper.hpp"

Ext2BlockHelper::Ext2BlockHelper(char* ext2_dump) {
    ext2_dump_ = ext2_dump;
    super_block_ = (ext2_super_block *) (ext2_dump + SUPER_BLOCK_OFFSET);

    if (super_block_->s_magic != EXT2_SUPER_MAGIC) {
        std::cerr << "Super Block not found at offset " << SUPER_BLOCK_OFFSET << std::endl;
        exit(1);
    }

    std::cout << *this << std::endl;
}

ext2_super_block* Ext2BlockHelper::get_super_block() {
    return super_block_;
}

char *Ext2BlockHelper::get_block(unsigned int block_number) {
    if (block_number > super_block_->s_blocks_count) {
        std::cerr << "Block number exceeds block count." << std::endl;
        exit(1);
    }

    return ext2_dump_ + (block_number * get_block_size_in_bytes());
}

char *Ext2BlockHelper::get_block_after(const char *after) {
    long offset = after - ext2_dump_;
    unsigned int current_block = offset / get_block_size_in_bytes();
    return get_block(current_block + 1);
}

void Ext2BlockHelper::to_stream(std::ostream &os) {
    os << "######################" << std::endl;
    os << "Superblock information" << std::endl;

    // First some general stuff about the file system
    os << "* Located in block group " << super_block_->s_block_group_nr << std::endl;
    os << "* Ext2 revision: " << get_ext2_revision() << std::endl;
    os << "* Volume name: " << super_block_->s_volume_name << std::endl;
    os << "* State: " << get_human_readable_state() << std::endl;

    // Now the interesting stuff ^^
    os << "* Block size: " << super_block_->s_log_block_size << " (" << get_block_size_in_bytes() << " bytes)" << std::endl;
    os << "* Total Blocks: " << super_block_->s_blocks_count << " (" << super_block_->s_free_blocks_count << " free, "
       << get_used_block_count() << " used, " << super_block_->s_r_blocks_count << " reserved for super user)" << std::endl;
    os << "* Blocks per group: " << super_block_->s_blocks_per_group << std::endl;

    os << "* Total Inodes: " << super_block_->s_inodes_count << " (" << super_block_->s_free_inodes_count << " free, "
       << get_used_inode_count() << " used)" << std::endl;
    os << "* Inodes per group: " << super_block_->s_inodes_per_group << std::endl;
}

std::string Ext2BlockHelper::get_ext2_revision() {
    std::stringstream string_stream;
    string_stream << super_block_->s_rev_level << "." << super_block_->s_minor_rev_level;
    return string_stream.str();
}

std::string Ext2BlockHelper::get_human_readable_state() {
    switch (super_block_->s_state) {
        case EXT2_VALID_FS:
            return "Unmounted cleanly";
        case EXT2_ERROR_FS:
            return "Errors detected";
        default:
            return "Unknown";
    }
}

long Ext2BlockHelper::get_block_size_in_bytes() {
    return 1024 << super_block_->s_log_block_size;
}

unsigned int Ext2BlockHelper::get_used_block_count() {
    return super_block_->s_blocks_count - super_block_->s_free_blocks_count;
}

unsigned int Ext2BlockHelper::get_used_inode_count() {
    return super_block_->s_inodes_count - super_block_->s_free_inodes_count;
}