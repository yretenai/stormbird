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
        explicit DecimaCache(const std::filesystem::path& bin_path);
        dragon::Array<char> read_file(const std::string& path);
        dragon::Array<char> read_file(uint64_t hash);
        // bool write_file(std::string path, dragon::Array<char> data); // TODO
        // bool write_file(uint64_t hash, dragon::Array<char> data); // TODO
        bool file_exists(const std::string& path);
        bool file_exists(uint64_t hash);
        static std::string unwrap_path(const std::string& path);
        static std::string path_type(const std::string& path);
        static uint64_t get_hash(const std::string& path);
    };
} // namespace stormbird

#endif // LIBSTORMBIRD_DECIMACACHE_H
