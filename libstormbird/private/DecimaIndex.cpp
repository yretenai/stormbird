//
// Created by yretenai on 2020-08-04.
//

#include "DecimaIndex.h"

#include <cstdint>
#include <ooz/kraken.h>
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
    Blocks = dragon::Array<DecimaIndexBlock>(Header.block_count, nullptr);
    Stream->read(reinterpret_cast<char*>(record_buffer.data()), record_buffer.byte_size());
    Stream->read(reinterpret_cast<char*>(Blocks.data()), Blocks.byte_size());
    for (DecimaIndexRecord record : record_buffer) {
        if (Records.contains(record.hash)) {
            ELOG("Duplicate record? " << std::setfill('0') << std::setw(16) << std::hex << record.hash << std::setw(0) << " defined twice");
        }
        Records[record.hash] = record;
    }

    std::filesystem::path idx_path(path);
    idx_path.replace_extension(".idx");
    if (std::filesystem::exists(idx_path)) {
        LOG("Cache Index file found! " << idx_path.filename());
        StreamIndex = std::make_shared<DecimaStreamIndex>(DecimaStreamIndex(dragon::read_file(idx_path)));
    }
}

stormbird::DecimaIndex::~DecimaIndex() { Stream->close(); }

// not thread safe.
dragon::Array<char> stormbird::DecimaIndex::read_file(uint64_t hash) {
    if (!file_exists(hash))
        return dragon::Array<char>();
    DecimaIndexRecord record = Records[hash];
    uint32_t alignment = record.offset / Header.max_block_size;
    uint32_t last_alignment = (record.offset + record.size) / Header.max_block_size;
    uint32_t block_count = last_alignment - alignment + 1;
    dragon::Array<char> dec_block_buffer(block_count * Header.max_block_size + Header.max_block_size, nullptr);
    for (uint32_t i = 0; i < block_count; ++i) {
        DecimaIndexBlock block = Blocks[i];
        Stream->seekg(block.offset, std::ios::beg); // add mutex here probably.
        dragon::Array<char> block_buffer(block.size, nullptr);
        Stream->read(reinterpret_cast<char*>(block_buffer.data()), block_buffer.byte_size());
        Kraken_Decompress(reinterpret_cast<const uint8_t*>(block_buffer.data()), block_buffer.byte_size(),
                          reinterpret_cast<uint8_t*>(dec_block_buffer.data() + (i * Header.max_block_size)), block.block_size);
    }
    return dragon::Array<char>(dec_block_buffer.data() + (record.offset % Header.max_block_size), record.size, nullptr);
}

bool stormbird::DecimaIndex::file_exists(uint64_t hash) { return Records.contains(hash); }

void stormbird::DecimaIndex::dump_info() {
    for (std::pair<uint64_t, DecimaIndexRecord> pair : Records) {
        LOG("Record " << std::setfill('0') << std::setw(16) << std::hex << pair.second.hash);
    }

    if (StreamIndex != nullptr) {
        LOG("DecimaStreamIndex is present");
        StreamIndex->dump_info();
    } else {
        LOG("DecimaStreamIndex is not present");
    }
}
