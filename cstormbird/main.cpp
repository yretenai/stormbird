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
    if(!std::filesystem::exists(out)) {
       std::filesystem::create_directories(out);
    }

    for (std::shared_ptr<DecimaIndex> index : decima.Entries) {
        if (index->StreamIndex != nullptr) {
            for (DecimaStreamIndex::DecimaStreamIndexEntry entry : index->StreamIndex->Entries) {
                LOG(entry.path);
                std::string unwrapped = stormbird::DecimaCache::unwrap_path(entry.path);
                std::filesystem::path combined(out);
                combined.append(unwrapped);
                std::filesystem::path dir = combined.parent_path();
                if(!std::filesystem::exists(dir)) {
                    std::filesystem::create_directories(dir);
                }
                dragon::Array<char> data = index->read_file(entry.hash);
                dragon::write_file(combined, &data);
            }
        }
    }
    return 0;
}
