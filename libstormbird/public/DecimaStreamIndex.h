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
        typedef uint8_t DecimaStreamHash[0x10];
        typedef struct DECIMA_STREAM_INDEX_ENTRY {
            std::string path;
            DecimaStreamHash hash;
            uint64_t offset;
            uint64_t size;
        } DecimaStreamIndexEntry;
        dragon::Array<DecimaStreamIndexEntry> Entries;
        DecimaStreamIndex(dragon::Array<char> buffer);
    };
} // namespace stormbird

#endif // LIBSTORMBIRD_DECIMASTREAMINDEX_H
