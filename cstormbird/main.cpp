//
// Created by yretenai on 2020-08-04.
//

#include "Stormbird.h"
#include "standard_dragon/dragon.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        if (argc > 0) {
            eprintf("Usage: %s path_to_bin_dir\n", argv[0]);
        } else {
            eprintf("Usage: cstormbird path_to_bin_dir");
        }
        return -1;
    }
    return 0;
}
