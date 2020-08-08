//
// Created by yretenai on 2020-08-08.
//

#ifndef LIBSTORMBIRD_DECIMACACHE_H
#define LIBSTORMBIRD_DECIMACACHE_H

#include <string>
#include <vector>
#include <map>
#include <istream>
#include <cstdint>
#include <memory>
#include <standard_dragon/Array.h>
#include "DecimaIndex.h"
#include "export.h"

// Management class for collecting index files.

namespace stormbird {
    class STORMBIRD_EXPORT DecimaCache {
    private:
        std::map<uint64_t, int32_t> FileMap;
        std::map<uint64_t, std::string> HashMap;
    public:
        dragon::Array<std::shared_ptr<DecimaIndex>> Entries;
        DecimaCache();
        ~DecimaCache();
        dragon::Array<char> read_file(std::string path);
        dragon::Array<char> read_file(uint64_t hash);
        // bool write_file(std::string path, dragon::Array<char> data); // TODO
        // bool write_file(uint64_t path, dragon::Array<char> data); // TODO
        bool file_exists(std::string hash);
        bool file_exists(uint64_t hash);
        uint64_t get_hash(std::string hash);
    };
}

#endif //LIBSTORMBIRD_DECIMACACHE_H
