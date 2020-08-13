//
// Created by yretenai on 2020-08-08.
//

#include "DecimaCache.h"

#include <murmur3/murmur3.h>
#include <standard_dragon/dragon.h>
#include <utility>

stormbird::DecimaCache::DecimaCache(const std::filesystem::path& bin_path) {
    std::vector<std::filesystem::path> found_files;
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(bin_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            found_files.push_back(entry.path());
        }
    }
    Entries = dragon::Array<std::shared_ptr<DecimaIndex>>(found_files.size(), nullptr);
    uint32_t i = 0;
    for (const std::filesystem::path& path : found_files) {
        LOG("Cache file found! " << path.filename());
        Entries[i] = std::make_shared<DecimaIndex>(path);
        for (std::pair<uint64_t, DecimaIndex::DecimaIndexRecord> pair : Entries[i]->Records) {
            if (FileMap.contains(pair.first)) {
                ELOG("Duplicate record? " << std::setfill('0') << std::setw(16) << std::hex << pair.first << std::setw(0) << " defined twice");
            }
            FileMap[pair.first] = i;
        }
        ++i;
    }
}

dragon::Array<char> stormbird::DecimaCache::read_file(const std::string& path) { return read_file(get_hash(path)); }

dragon::Array<char> stormbird::DecimaCache::read_file(uint64_t hash) {
    if (!file_exists(hash)) {
        return dragon::Array<char>();
    }

    return Entries[FileMap[hash]]->read_file(hash);
}

bool stormbird::DecimaCache::file_exists(const std::string& path) { return file_exists(get_hash(path)); }

bool stormbird::DecimaCache::file_exists(uint64_t hash) { return FileMap.contains(hash); }

std::string stormbird::DecimaCache::unwrap_path(const std::string& path) { return path.substr(path.find(':') + 1); }

std::string stormbird::DecimaCache::path_type(const std::string& path) { return path.substr(0, path.find(':')); }

uint64_t stormbird::DecimaCache::get_hash(const std::string& path) {
    uint64_t hash[2] = {0, 0};
    std::string stripped_path = unwrap_path(path);
    std::transform(stripped_path.begin(), stripped_path.end(), stripped_path.begin(),
                   [](unsigned char c) -> unsigned char { return std::tolower(c); });
    MurmurHash3_x64_128(stripped_path.c_str(), (int32_t)stripped_path.size() + 1, 42, &hash);
    return hash[0];
}
