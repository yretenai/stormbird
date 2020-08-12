//
// Created by yretenai on 2020-08-04.
//

#ifndef LIBSTORMBIRD_DECIMAINDEX_H
#define LIBSTORMBIRD_DECIMAINDEX_H

// .bin files found on HZD PC and PS4.
// TODO: get bin files from DS.

#include "DecimaStreamIndex.h"
#include "export.h"
#include <filesystem>
#include <istream>
#include <map>
#include <memory>
#include <standard_dragon/Array.h>

namespace stormbird {
    class STORMBIRD_EXPORT DecimaIndex {
      public:
#pragma pack(push, 1)
        typedef struct DECIMA_INDEX_RECORD {
            int32_t file_id;
            int32_t unknown1;
            int64_t hash;
            int64_t offset;
            int32_t size;
            int32_t unknown2;
        } DecimaIndexRecord;
        typedef struct DECIMA_INDEX_BLOCK {
            int64_t block_offset;
            int32_t block_size;
            int32_t unknown1;
            int64_t offset;
            int32_t size;
            int32_t unknown2;
        } DecimaIndexBlock;
        typedef struct DECIMA_INDEX_HEADER {
            int32_t load_priority;
            int64_t compressed_size;
            int64_t decompressed_size;
            int64_t record_count;
            int32_t block_count;
            int32_t max_block_size;
        } DecimaIndexHeader;
#pragma pack(pop)

        DecimaIndexHeader Header = DecimaIndexHeader();
        std::shared_ptr<std::ifstream> Stream;
        std::shared_ptr<DecimaStreamIndex> StreamIndex;
        std::map<uint64_t, DecimaIndexRecord> Records;
        dragon::Array<DecimaIndexBlock> Blocks;
        explicit DecimaIndex(const std::filesystem::path& path);
        ~DecimaIndex();
        dragon::Array<char> read_file(uint64_t hash);
        bool file_exists(uint64_t hash);
        void dump_info();
    };
} // namespace stormbird

#endif // LIBSTORMBIRD_DECIMAINDEX_H
