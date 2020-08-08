//
// Created by yretenai on 2020-08-08.
//

#ifndef LIBSTORMBIRD_EXPORT_H
#define LIBSTORMBIRD_EXPORT_H

#ifdef _WIN32
#include "export_win32.h"
#else
#include "export_clang.h"
#endif

#endif //LIBSTORMBIRD_EXPORT_H
