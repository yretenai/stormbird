//
// Created by yretenai on 2020-08-08.
//

#ifndef LIBSTORMBIRD_DECIMASTREAMINDEX_H
#define LIBSTORMBIRD_DECIMASTREAMINDEX_H

// .idx files found on HZD PC.

#include "export.h"
#include <standard_dragon/Array.h>
#include <string>

namespace stormbird {
    class STORMBIRD_EXPORT DecimaStreamIndex {
      public:
#pragma pack(push, 1)
        typedef uint8_t DecimaStreamChecksum[0x10];
        typedef struct DECIMA_STREAM_INDEX_ENTRY {
            std::string path;
            DecimaStreamChecksum Checksum;
            uint64_t hash;
            uint64_t offset;
            uint64_t size;
        } DecimaStreamIndexEntry;
#pragma pack(pop)
        dragon::Array<DecimaStreamIndexEntry> Entries;
        explicit DecimaStreamIndex(dragon::Array<char> buffer);
        void dump_info();
    };
} // namespace stormbird

#endif // LIBSTORMBIRD_DECIMASTREAMINDEX_H
