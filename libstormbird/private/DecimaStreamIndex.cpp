//
// Created by yretenai on 2020-08-08.
//

#include "DecimaStreamIndex.h"

#include <DecimaCache.h>
#include <algorithm>
#include <standard_dragon/dragon.h>

#define DECIMA_IDX_MAGIC (uint32_t)0x10203040

stormbird::DecimaStreamIndex::DecimaStreamIndex(dragon::Array<char> buffer) {
    int32_t offset = 0;
    if (buffer.lpcast<uint32_t>(&offset) != DECIMA_IDX_MAGIC) {
        ELOG("Cache index has wrong magic!");
        return;
    }

    Entries = dragon::Array<DecimaStreamIndexEntry>(buffer.lpcast<int32_t>(&offset), nullptr);

    for (int32_t i = 0; i < Entries.size(); i++) {
        int32_t length = buffer.lpcast<int32_t>(&offset);
        Entries[i].path = std::string(buffer.lpslice(&offset, length).data(), length);
        std::copy_n(buffer.lpslice(&offset, 0x10).data(), 0x10, Entries[i].Checksum);
        Entries[i].offset = buffer.lpcast<uint64_t>(&offset);
        Entries[i].size = buffer.lpcast<uint64_t>(&offset);
        Entries[i].hash = stormbird::DecimaCache::get_hash(Entries[i].path);
    }
}

void stormbird::DecimaStreamIndex::dump_info() {
    for (DecimaStreamIndexEntry entry : Entries) {
        LOG("Entry " << entry.path << " " << std::setfill('0') << std::hex << std::setw(16) << entry.hash);
    }
}
