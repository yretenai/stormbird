//
// Created by yretenai on 2020-08-08.
//

#include "DecimaStreamIndex.h"

#include <algorithm>
#include <standard_dragon/dragon.h>
#include <murmur3/murmur3.h>

#define DECIMA_IDX_MAGIC (uint32_t)0x10203040

stormbird::DecimaStreamIndex::DecimaStreamIndex(dragon::Array<char> buffer) {
    int32_t offset = 0;
    if (buffer.lpcast<uint32_t>(&offset) != DECIMA_IDX_MAGIC) {
        ELOG("Cache index has wrong magic!");
        return;
    }

    Entries = dragon::Array<DecimaStreamIndexEntry>(buffer.lpcast<int32_t>(&offset), nullptr);

    uint64_t hash[2] = { 0, 0 };
    for (int32_t i = 0; i < Entries.size(); i++) {
        int32_t length = buffer.lpcast<int32_t>(&offset);
        Entries[i].path = std::string(buffer.lpslice(&offset, length).data(), length);
        std::copy_n(buffer.lpslice(&offset, 0x10).data(), 0x10, Entries[i].Checksum);
        Entries[i].offset = buffer.lpcast<uint64_t>(&offset);
        Entries[i].size = buffer.lpcast<uint64_t>(&offset);
        std::string stripped_path = Entries[i].path.substr(Entries[i].path.find(':') + 1);
        std::transform(stripped_path.begin(), stripped_path.end(), stripped_path.begin(),
                       [](unsigned char c) -> unsigned char { return std::tolower(c); });
        MurmurHash3_x64_128(stripped_path.c_str(), stripped_path.size() + 1, 42, &hash);
        Entries[i].hash = hash[0]; // decima only uses upper 32-bits.
    }
}

void stormbird::DecimaStreamIndex::dump_info() {
    for(auto entry : Entries) {
        LOG("Entry " << entry.path << " " << std::setfill('0') << std::hex << std::setw(16) << entry.hash);
    }
}
