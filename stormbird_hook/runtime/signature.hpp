// stormbird project
// Copyright (c) 2023 <https://github.com/yretenai/stormbird>
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "signature_engine.hpp"

namespace stormbird_hook {
	MAKE_SIGNATURE(RTTI_FACTORY_CTOR, "48 89 41 08 48 89 41 10 48 89 41 18 48 89 41 20 48 89 41 28 48 89 41 30 48 89 41 38 48 89 41 40 48 8b c1 48 89 ?? ?? ?? ?? ?? c3")
} // namespace stormbird_hook
