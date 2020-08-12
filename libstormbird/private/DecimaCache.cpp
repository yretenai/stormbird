//
// Created by yretenai on 2020-08-08.
//

#include "DecimaCache.h"

#include <murmur3/murmur3.h>
#include <utility>

stormbird::DecimaCache::DecimaCache(std::filesystem::path bin_path) {
    // todo
}

dragon::Array<char> stormbird::DecimaCache::read_file(std::string path) { return read_file(get_hash(std::move(path))); }

dragon::Array<char> stormbird::DecimaCache::read_file(uint64_t hash) {
    if (!file_exists(hash)) {
        return dragon::Array<char>();
    }

    return Entries[FileMap[hash]]->read_file(hash);
}

bool stormbird::DecimaCache::file_exists(std::string path) { return file_exists(get_hash(std::move(path))); }

bool stormbird::DecimaCache::file_exists(uint64_t hash) { return FileMap.contains(hash); }

uint64_t stormbird::DecimaCache::get_hash(std::string hash) {
    uint64_t hashed[2] = {0, 0};
    std::string stripped_path = hash.substr(hash.find(':') + 1);
    std::transform(stripped_path.begin(), stripped_path.end(), stripped_path.begin(),
                   [](unsigned char c) -> unsigned char { return std::tolower(c); });
    MurmurHash3_x64_128(stripped_path.c_str(), stripped_path.size() + 1, 42, &hashed);
    return hashed[0];
}
