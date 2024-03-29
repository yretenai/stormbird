// stormbird project
// Copyright (c) 2023 <https://github.com/yretenai/stormbird>
// SPDX-License-Identifier: MPL-2.0

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <array>
#include <string_view>

namespace stormbird_hook {
	constexpr static const char *settings_name = R"(.\stormbird.ini)";
	constexpr static const char *settings_namespace = "stormbird";

#define LOAD_SETTING_BOOL(name) (settings.name = GetPrivateProfileIntA(settings_namespace, #name, static_cast<int>(settings.name), settings_name) != 0)
#define LOAD_SETTING_INT(name) (settings.name = GetPrivateProfileIntA(settings_namespace, #name, settings.name, settings_name))
#define LOAD_SETTING(name) \
	if (GetPrivateProfileStringA(settings_namespace, #name, NULL, settings.name.data(), static_cast<DWORD>(settings.name.size()), settings_name) == NULL) { \
		settings.name[0] = '\0'; \ 
	}

#define SAVE_SETTING_BOOL(name) WritePrivateProfileStringA(settings_namespace, #name, name ? "1" : "0", settings_name)
#define SAVE_SETTING_INT(name) WritePrivateProfileStringA(settings_namespace, #name, std::to_string(name).c_str(), settings_name)
#define SAVE_SETTING(name) WritePrivateProfileStringA(settings_namespace, #name, name.data(), settings_name)

	struct settings {
		bool load_renderdoc = false; // disable by default because it kills ReShade and performance in general.
		bool dump_rtti = false; // disable by default for clutter reasons

		std::array<char, MAX_PATH + 1> exe_name {}; // name of the exe we are patching, used to find the exe in the same directory.
		std::array<char, MAX_PATH + 1> renderdoc_path {}; // path to renderdoc/dll

		// load the settings from the ini file
		static auto
		load() -> settings {
			settings settings;

			LOAD_SETTING_BOOL(load_renderdoc);
			LOAD_SETTING_BOOL(dump_rtti);
			LOAD_SETTING(exe_name)
			LOAD_SETTING(renderdoc_path)

			settings.exe_name[MAX_PATH] = '\0';
			settings.renderdoc_path[MAX_PATH] = '\0';

			return settings;
		}

		// save the settings to the ini file
		void
		save() {
			exe_name[MAX_PATH] = '\0';
			renderdoc_path[MAX_PATH] = '\0';
			SAVE_SETTING_BOOL(load_renderdoc);
			SAVE_SETTING_BOOL(dump_rtti);
			SAVE_SETTING(exe_name);
			SAVE_SETTING(renderdoc_path);
		}
	};
} // namespace stormbird_hook
