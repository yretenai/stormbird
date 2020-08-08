//
// Created by yretenai on 2020-08-08.
//

#ifndef LIBSTORMBIRD_DECIMASTREAMINDEX_H
#define LIBSTORMBIRD_DECIMASTREAMINDEX_H

// .idx files found on HZD PC.

#include <standard_dragon/Array.h>
#include <string>
#include "export.h"

namespace stormbird {
    class STORMBIRD_EXPORT DecimaStreamIndex {
    public:
        typedef struct DECIMA_STREAM_INDEX_ENTRY {
            std::string path;
            // TODO
        } DecimaStreamIndexEntry;
        dragon::Array<DecimaStreamIndexEntry> Entries;
        DecimaStreamIndex();
    };
}


#endif //LIBSTORMBIRD_DECIMASTREAMINDEX_H
