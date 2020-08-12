//
// Created by yretenai on 2020-08-08.
//

#ifndef LIBSTORMBIRD_DECIMACACHE_H
#define LIBSTORMBIRD_DECIMACACHE_H

#include "DecimaIndex.h"
#include "export.h"
#include <cstdint>
#include <istream>
#include <map>
#include <memory>
#include <standard_dragon/Array.h>
#include <string>
#include <vector>

// Management class for collecting index files.

namespace stormbird {
    class STORMBIRD_EXPORT DecimaCache {
      private:
        std::map<uint64_t, int32_t> FileMap;

      public:
        dragon::Array<std::shared_ptr<DecimaIndex>> Entries;
        DecimaCache(std::filesystem::path bin_path);
        dragon::Array<char> read_file(std::string path);
        dragon::Array<char> read_file(uint64_t hash);
        // bool write_file(std::string path, dragon::Array<char> data); // TODO
        // bool write_file(uint64_t hash, dragon::Array<char> data); // TODO
        bool file_exists(std::string path);
        bool file_exists(uint64_t hash);
        static uint64_t get_hash(std::string hash);
    };
} // namespace stormbird

#endif // LIBSTORMBIRD_DECIMACACHE_H
