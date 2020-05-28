#ifndef EXT2SUPERBLOCKREADER_HPP
#define EXT2SUPERBLOCKREADER_HPP

#include <vector>
#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>

class Ext2SuperBlockHelper {
public:
    explicit Ext2SuperBlockHelper(const std::vector<char>& ext2_dump);

    void to_stream(std::ostream& os);

    std::string get_ext2_revision();
    std::string get_volume_name();
    std::string get_human_readable_state();

    long get_block_size_in_bytes();
private:
    void find_super_block(const std::vector<char>& ext2_dump);

    long offset = 0;
    ext2_super_block* found_super_block = nullptr;
};

inline std::ostream &operator <<(std::ostream& os, Ext2SuperBlockHelper& ext2_super_block_helper) {
    ext2_super_block_helper.to_stream(os);
    return os;
}

#endif