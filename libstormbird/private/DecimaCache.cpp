//
// Created by yretenai on 2020-08-08.
//

#include "DecimaCache.h"

stormbird::DecimaCache::DecimaCache() {

}

stormbird::DecimaCache::~DecimaCache() {

}

dragon::Array<char> stormbird::DecimaCache::read_file(std::string path) {
    return dragon::Array<char>();
}

dragon::Array<char> stormbird::DecimaCache::read_file(uint64_t hash) {
    return dragon::Array<char>();
}

bool stormbird::DecimaCache::file_exists(std::string hash) {
    return false;
}

bool stormbird::DecimaCache::file_exists(uint64_t hash) {
    return false;
}

uint64_t stormbird::DecimaCache::get_hash(std::string hash) {
    return 0;
}
