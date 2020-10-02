//
// Created by yretenai on 2020-08-04.
//

#include "Stormbird.h"
#include "standard_dragon/dragon.h"

using namespace stormbird;

int main(int argc, char** argv) {
    if (argc < 3) {
        if (argc > 0) {
            eprintf("Usage: %s path_to_bin_dir path_to_output_dir\n", argv[0]);
        } else {
            eprintf("Usage: cstormbird path_to_bin_dir path_to_output_dir\n");
        }
        return -1;
    }
    auto decima = DecimaCache(std::filesystem::path(argv[1]));

    std::filesystem::path out(argv[2]);
    if (!std::filesystem::exists(out)) {
        std::filesystem::create_directories(out);
    }

    for (std::shared_ptr<DecimaIndex> index : decima.Entries) {
        std::shared_ptr<DecimaStreamIndex> stream_index = index->StreamIndex;
        std::map<uint64_t, size_t> hash_table;
        if (stream_index != nullptr) {
            hash_table = stream_index->Table;
        }

        for (std::pair<uint64_t, DecimaIndex::DecimaIndexRecord> record : index->Records) {
            std::filesystem::path target = std::filesystem::path(out);
            if (hash_table.contains(record.first)) {
                DecimaStreamIndex::DecimaStreamIndexEntry entry = stream_index->Entries[hash_table[record.first]];
                LOG(entry.path);
                std::string unwrapped = stormbird::DecimaCache::unwrap_path(entry.path);
                target.append(unwrapped);
            } else {
                std::stringstream s;
                s << "__unknown/" << std::setfill('0') << std::setw(16) << std::hex << record.first << ".bin";
                LOG("cache:" << s.str());
                target.append(s.str());
            }
            std::filesystem::path dir = target.parent_path();
            if (!std::filesystem::exists(dir)) {
                std::filesystem::create_directories(dir);
            }
            dragon::Array<char> data = index->read_file(record.first);
            dragon::write_file(target, &data);
        }
    }
    return 0;
}
