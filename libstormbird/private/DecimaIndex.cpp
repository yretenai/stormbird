//
// Created by yretenai on 2020-08-04.
//

#include "DecimaIndex.h"

#include <cstdint>
#include <standard_dragon/dragon.h>

#define DECIMA_BIN_MAGIC (uint32_t)0x20304050

stormbird::DecimaIndex::DecimaIndex(const std::filesystem::path& path) {
    Stream = std::make_shared<std::ifstream>(std::ifstream(path, std::ios::binary | std::ios::in));
    int32_t offset = 0;
    dragon::Array<char> buffer(sizeof(DecimaIndexHeader) + 4, nullptr);
    Stream->read(buffer.data(), buffer.size());
    if (buffer.lpcast<uint32_t>(&offset) != DECIMA_BIN_MAGIC) {
        ELOG("Cache binary has wrong magic!");
        return;
    }
    Header = buffer.lpcast<DecimaIndexHeader>(&offset);
    dragon::Array<DecimaIndexRecord> record_buffer(Header.record_count, nullptr);
    dragon::Array<DecimaIndexBlock> block_buffer(Header.block_count, nullptr);
    Stream->read(reinterpret_cast<char*>(record_buffer.data()), record_buffer.byte_size());
    Stream->read(reinterpret_cast<char*>(block_buffer.data()), block_buffer.byte_size());
    for(auto record : record_buffer) {
        if(Records.contains(record.hash)) {
            ELOG("Duplicate record? " << std::setfill('0') << std::setw(16) << std::hex << record.hash << std::setw(0) << " defined twice");
        }
        Records[record.hash] = record;
    }

    for(auto block : block_buffer) {
        Blocks[block.block_offset] = block;
    }

    std::filesystem::path idx_path(path);
    idx_path.replace_extension(".idx");
    if(std::filesystem::exists(idx_path)) {
        LOG("Cache Index file found! " << idx_path.filename());
        StreamIndex = std::make_shared<DecimaStreamIndex>(DecimaStreamIndex(dragon::read_file(idx_path)));
    }
}

stormbird::DecimaIndex::~DecimaIndex() {
    Stream->close();
}

dragon::Array<char> stormbird::DecimaIndex::read_file(uint64_t hash) {
    return dragon::Array<char>();
}

bool stormbird::DecimaIndex::file_exists(uint64_t hash) {
    return Records.contains(hash);
}

void stormbird::DecimaIndex::dump_info() {
    for(auto pair : Records) {
        LOG("Record " << std::setfill('0') << std::setw(16) << std::hex << pair.second.hash);
    }

    if(StreamIndex != nullptr) {
        LOG("DecimaStreamIndex is present");
        StreamIndex->dump_info();
    } else {
        LOG("DecimaStreamIndex is not present");
    }
}
