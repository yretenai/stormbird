//
// Created by yretenai on 2020-08-04.
//

#ifndef LIBSTORMBIRD_DECIMAINDEX_H
#define LIBSTORMBIRD_DECIMAINDEX_H

// .bin files found on HZD PC and PS4.
// TODO: get bin files from DS.

#include <memory>
#include <standard_dragon/Array.h>
#include <map>
#include "DecimaStreamIndex.h"
#include "export.h"

namespace stormbird {

    class STORMBIRD_EXPORT DecimaIndex {
    public:
        typedef struct DECIMA_INDEX_ENTRY {
            // TODO
        } DecimaIndexEntry;
        std::shared_ptr<DecimaStreamIndex> StreamIndex;
        std::map<uint64_t, DecimaIndexEntry> Entries;
        DecimaIndex();
        ~DecimaIndex();
        dragon::Array<char> read_file(uint64_t hash);
        bool file_exists(uint64_t hash);

    };
}


#endif //LIBSTORMBIRD_DECIMAINDEX_H
