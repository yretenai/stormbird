// stormbird project
// Copyright (c) 2023 <https://github.com/yretenai/stormbird>
// SPDX-License-Identifier: MPL-2.0

#include <chrono>
#include <fstream>
#include <memory>
#include <ostream>
#include <unordered_set>

#include "rtti.hpp"
#include "runtime.hpp"
#include "settings.hpp"
#include "signature.hpp"
#include "signature_engine.hpp"

#include <MinHook.h>
#include <nlohmann/json.hpp>

using namespace nlohmann;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmicrosoft-cast"

namespace {
	std::ofstream g_output;
	HMODULE g_renderdoc = nullptr;
	HMODULE g_game_module = nullptr;
	bool g_minhook_initialized = false;
	stormbird_hook::settings g_settings;
	std::thread g_rtti_dump_thread;
} // namespace

namespace stormbird_hook {
	auto
	get_game() -> HMODULE {
		HMODULE module = nullptr;

		// if the exe name is set, use that
		if (strnlen_s(g_settings.exe_name.data(), g_settings.exe_name.size()) > 0) {
			module = GetModuleHandleA(g_settings.exe_name.data());
			if (module != nullptr) {
				g_output << "[stormbird] found " << std::string_view(g_settings.exe_name.data()) << '\n';
			}
		}

		g_output.flush();

		// not found
		return module;
	}

	struct rtti_json {
		json::array_t rtti;
		std::unordered_set<uint64_t> visited {};
	};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-bounds-pointer-arithmetic"
	using rtti_factory_ctor_t = RTTIFactory *(*) (RTTIFactory *);
	rtti_factory_ctor_t fwd_rtti_factory_ctor = nullptr;

	RTTIFactory *rtti_factory = nullptr;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotInitializedField"

	auto
	get_rtti_name(RTTIBase *rtti) -> std::string { // NOLINT(*-no-recursion)
		if (rtti == nullptr) {
			return "<null>";
		}

		switch (rtti->rtti_type) {
			case RTTIType::Primitive:
				{
					auto ptr = reinterpret_cast<RTTIPrimitive *>(rtti);
					return ptr->name;
				}
			case RTTIType::Reference:
			case RTTIType::Container:
				{
					auto ptr = reinterpret_cast<RTTIReference *>(rtti);
					std::string name = ptr->data->name;
					auto type = get_rtti_name(ptr->type);
					return name + "<" + type + ">";
				}
			case RTTIType::Enum:
			case RTTIType::Bitset:
				{
					auto ptr = reinterpret_cast<RTTIEnum *>(rtti);
					return ptr->name;
				}
			case RTTIType::Class:
				{
					auto ptr = reinterpret_cast<RTTIClass *>(rtti);
					return ptr->name;
				}
			case RTTIType::Struct: break;
		}

		return {};
	}

	void
	visit_rtti(rtti_json &result, RTTIBase *rtti);

	void
	visit_rtti_primitive(rtti_json &result, json &obj, RTTIPrimitive *primitive) {
		obj["total_size"] = primitive->total_size;
		obj["size"] = primitive->size;
		obj["count"] = primitive->count;
		obj["unknown2"] = primitive->unknown1;
		obj["unknown2"] = primitive->unknown2;
		obj["name"] = std::string(primitive->name);
		if (primitive->parent != reinterpret_cast<RTTIBase *>(primitive)) {
			obj["parent_addr"] = reinterpret_cast<uint64_t>(primitive->parent);
			obj["parent"] = get_rtti_name(primitive->parent);

						visit_rtti(result, primitive->parent);
		}
	}

	void
	visit_rtti_reference(rtti_json &result, json &obj, RTTIReference *reference) {
		obj["name"] = get_rtti_name(reinterpret_cast<RTTIBase *>(reference));
		obj["container_name"] = std::string(reference->data->name);
		obj["type_name"] = get_rtti_name(reinterpret_cast<RTTIBase *>(reference->type));
		obj["type_addr"] = reinterpret_cast<uint64_t>(reference->type);
		obj["unknown1"] = reference->data->unknown1;
		obj["unknown2"] = reference->data->unknown2;
		obj["unknown3"] = reference->data->unknown3;

		visit_rtti(result, reference->type);
	}

	void
	visit_rtti_enum(json &obj, RTTIEnum *enum_rtti) {
		obj["name"] = std::string(enum_rtti->name);
		obj["size"] = enum_rtti->size;
		obj["member_count"] = enum_rtti->member_count;
		obj["unknown1"] = enum_rtti->unknown1;
		obj["unknown2"] = enum_rtti->unknown2;
		json::array_t values;

		for (auto i = 0; i < enum_rtti->member_count; i++) {
			auto enum_value = enum_rtti->values[i];
			json value;
			value["value"] = enum_value.value;
			value["name"] = std::string(enum_value.name);
			values.emplace_back(value);
		}

		obj["values"] = values;
	}

	void
	visit_rtti_class(rtti_json &result, json &obj, RTTIClass *class_rtti) {
		obj["name"] = std::string(class_rtti->name);
		obj["base_count"] = class_rtti->base_count;
		obj["member_count"] = class_rtti->member_count;
		obj["function_count"] = class_rtti->function_count;
		obj["event_count"] = class_rtti->event_count;
		obj["base_event_count"] = class_rtti->base_event_count;
		obj["unknown1"] = class_rtti->unknown1;
		obj["unknown2"] = class_rtti->unknown2;
		obj["unknown3"] = class_rtti->unknown3;
		obj["unknown4"] = class_rtti->unknown4;
		obj["hash"] = class_rtti->hash;
		obj["unknown5"] = class_rtti->unknown5;
		obj["size"] = class_rtti->size;
		obj["alignment"] = class_rtti->alignment;
		obj["flags"] = class_rtti->flags;

		if (class_rtti->first_child != nullptr) {
			std::unordered_set<uint64_t> visited {};
			json::array_t descendants;
			RTTIClass *descendant_rtti = class_rtti->first_child;
			while (descendant_rtti != nullptr) {
				if (!visited.emplace(reinterpret_cast<uint64_t>(descendant_rtti)).second) {
					break;
				}

				json descendant_obj;
				descendant_obj["name"] = get_rtti_name(reinterpret_cast<RTTIBase *>(descendant_rtti));
				descendant_obj["addr"] = reinterpret_cast<uint64_t>(descendant_rtti);
				descendants.emplace_back(descendant_obj);
				visit_rtti(result, reinterpret_cast<RTTIBase*>(descendant_rtti));
				descendant_rtti = descendant_rtti->next_sibling;
			}
			obj["descendants"] = descendants;
		}

		if (class_rtti->bases != nullptr && class_rtti->base_count > 0) {
			json::array_t bases;
			for (auto i = 0; i < class_rtti->base_count; i++) {
				json base_obj;
				auto base = class_rtti->bases[i];
				base_obj["name"] = get_rtti_name(reinterpret_cast<RTTIBase *>(base.type));
				base_obj["addr"] = reinterpret_cast<uint64_t>(base.type);
				base_obj["offset"] = base.offset;
				visit_rtti(result, reinterpret_cast<RTTIBase*>(base.type));
				bases.emplace_back(base_obj);
			}
			obj["bases"] = bases;
		}

		if (class_rtti->members != nullptr && class_rtti->member_count > 0) {
			json::array_t members;
			for (auto i = 0; i < class_rtti->member_count; i++) {
				json member_obj;
				auto member = class_rtti->members[i];
				member_obj["type_name"] = get_rtti_name(reinterpret_cast<RTTIBase *>(member.type));
				member_obj["type_addr"] = reinterpret_cast<uint64_t>(member.type);
				member_obj["name"] = std::string(member.name);
				member_obj["offset"] = member.offset;
				member_obj["flags"] = member.flags;
				member_obj["unknown"] = member.unknown;
				visit_rtti(result, reinterpret_cast<RTTIBase*>(member.type));
				members.emplace_back(member_obj);
			}
			obj["members"] = members;
		}

		if (class_rtti->functions != nullptr && class_rtti->function_count > 0) {
			json::array_t functions;
			for (auto i = 0; i < class_rtti->function_count; i++) {
				json function_obj;
				auto function = class_rtti->functions[i];
				function_obj["type"] = function.return_type;
				function_obj["name"] = std::string(function.name);
				function_obj["args"] = std::string(function.args);
				functions.emplace_back(function_obj);
			}
			obj["functions"] = functions;
		}

		if (class_rtti->events != nullptr && class_rtti->event_count > 0) {
			json::array_t events;
			for (auto i = 0; i < class_rtti->event_count; i++) {
				json event_obj;
				auto event = class_rtti->events[i];
				event_obj["name"] = get_rtti_name(reinterpret_cast<RTTIBase *>(event.type));
				event_obj["addr"] = reinterpret_cast<uint64_t>(reinterpret_cast<RTTIBase *>(event.type));
				visit_rtti(result, reinterpret_cast<RTTIBase*>(event.type));
				events.emplace_back(event_obj);
			}
			obj["events"] = events;
		}

		if (class_rtti->base_events != nullptr && class_rtti->base_event_count > 0) {
			json::array_t base_events;
			for (auto i = 0; i < class_rtti->base_event_count; i++) {
				json base_event_obj;
				auto base_event = class_rtti->base_events[i];
				base_event_obj["unknown1"] = base_event.unknown1;
				base_event_obj["name"] = get_rtti_name(reinterpret_cast<RTTIBase *>(base_event.type));
				base_event_obj["addr"] = reinterpret_cast<uint64_t>(base_event.type);
				base_event_obj["type_name"] = get_rtti_name(reinterpret_cast<RTTIBase *>(base_event.base_class));
				base_event_obj["type_addr"] = reinterpret_cast<uint64_t>(base_event.base_class);
				visit_rtti(result, reinterpret_cast<RTTIBase*>(base_event.type));
				visit_rtti(result, reinterpret_cast<RTTIBase*>(base_event.base_class));
				base_events.emplace_back(base_event_obj);
			}
			obj["base_events"] = base_events;
		}
	}

	void
	visit_rtti(rtti_json &result, RTTIBase *rtti) {
		if (rtti == nullptr) {
			return;
		}

		if (!result.visited.emplace(reinterpret_cast<uint64_t>(rtti)).second) {
			return;
		}

		json obj;
		obj["type_id"] = rtti->type_id;
		obj["category_type_id"] = rtti->category_type_id;
		obj["type"] = static_cast<uint8_t>(rtti->rtti_type);
		obj["addr"] = reinterpret_cast<uint64_t>(rtti);

		switch (rtti->rtti_type) {
			case RTTIType::Primitive:
				{
					visit_rtti_primitive(result, obj, reinterpret_cast<RTTIPrimitive *>(rtti));
					break;
				}
			case RTTIType::Reference:
			case RTTIType::Container:
				{
					visit_rtti_reference(result, obj, reinterpret_cast<RTTIReference *>(rtti));
					break;
				}
			case RTTIType::Enum:
			case RTTIType::Bitset:
				{
					visit_rtti_enum(obj, reinterpret_cast<RTTIEnum *>(rtti));
					break;
				}
			case RTTIType::Class:
				{
					visit_rtti_class(result, obj, reinterpret_cast<RTTIClass *>(rtti));
					break;
				}
			case RTTIType::Struct: break;
		}

		result.rtti.emplace_back(obj);
	}

#pragma clang diagnostic pop

	void
	dump_rtti() {
		using namespace std::chrono_literals;

		g_output << "[rtti] sleeping by 5 seconds to give the game a chance to "
					"set up...\n";
		std::this_thread::sleep_for(5s);

		g_output << "[rtti] dumping...\n";

		g_output.flush();

		rtti_json rtti;

		for (const auto &rtti_record : rtti_factory->rtti) {
			visit_rtti(rtti, rtti_record.rtti);
		}

		for (const auto &ref_rtti_record : rtti_factory->rtti_refs) {
			for (const auto &rtti_record : ref_rtti_record.rtti) {
				visit_rtti(rtti, rtti_record.type);
				visit_rtti(rtti, rtti_record.ref_type);
			}
		}

		for (const auto &rtti_record : rtti_factory->core_rtti) {
			visit_rtti(rtti, rtti_record.rtti->rtti);
		}

		std::ofstream json_data;

		json_data.open("./rtti.json");
		json j = rtti.rtti;
		auto json_text = j.dump();
		json_data.write(json_text.c_str(), static_cast<std::streamsize>(json_text.size()));
		json_data.flush();
		json_data.close();

		g_output.flush();
	}

	auto
	rtti_factory_ctor(RTTIFactory *arg1) -> RTTIFactory * {
		auto factory = reinterpret_cast<RTTIFactory *>(fwd_rtti_factory_ctor(arg1));
		factory->core_rtti = { nullptr, 0, 0 };
		factory->rtti = { nullptr, 0, 0 };
		factory->rtti_refs = { nullptr, 0, 0 };
		factory->runtime_rtti = { nullptr, 0, 0 };

		rtti_factory = factory;
		g_output << "[stormbird] starting rtti dump thread\n";
		g_rtti_dump_thread = std::thread(dump_rtti);

		g_output.flush();

		return factory;
	}

	void
	null_func() { }

	void
	init_minhook() {
		if (g_minhook_initialized) {
			return;
		}

		if (MH_Initialize() != MH_OK) {
			g_output << "[stormbird] failed to initialize minhook\n";
			return;
		}

		g_output.flush();

		g_minhook_initialized = true;
	}

	void
	create_hook(const std::string_view &name, std::ostream &output, HMODULE game, const hex_signature &signature, LPVOID detour, LPVOID *original) {
		output << "[stormbird] searching for " << name << " pointer\n";
		auto pointers = scan(game, signature);
		if (pointers.empty()) {
			output << "[stormbird] could not find " << name << " pointer, aborting\n";
			return;
		}

		if (pointers.size() > 1) {
			output << "[stormbird] found " << pointers.size() << " " << name << " pointers, too many. aborting\n";
			return;
		}

		output << "[stormbird] found " << name << " pointer at " << std::hex << reinterpret_cast<uint64_t>(pointers[0]) << std::dec << "\n";

		init_minhook();

		if (MH_CreateHook(pointers[0], detour, original) != MH_OK) {
			output << "[stormbird] failed to create " << name << " hook\n";
			return;
		}

		if (MH_EnableHook(pointers[0]) != MH_OK) {
			output << "[stormbird] failed to enable " << name << " hook\n";
			return;
		}

		output << "[stormbird] created " << name << " hook\n";
	}

#pragma clang diagnostic pop

	namespace runtime {
		void
		init() {
			// this runs on the main thread

			g_output.open("./stormbird.log");
			g_output << "[stormbird] init\n";

			g_settings = settings::load();

			g_game_module = get_game();
			if (g_game_module == nullptr) {
				g_settings.save();
				g_output << "[stormbird] game not found, set exe_name in ini.\n";
				return;
			}

			if (g_settings.load_renderdoc) {
				g_output << "[stormbird] loading renderdoc\n";
				if (std::filesystem::exists("renderdoc.dll")) {
					g_output << "[stormbird] loaded local renderdoc\n";
					g_renderdoc = LoadLibraryA("renderdoc.dll");
				} else {
					auto renderdoc_path = std::filesystem::path(g_settings.renderdoc_path.data());
					if (renderdoc_path.empty()) {
						g_output << "[stormbird] renderdoc.dll not found\n";
					} else {
						if (std::filesystem::exists(renderdoc_path)) {
							g_output << "[stormbird] loaded " << renderdoc_path << "\n";
							g_renderdoc = LoadLibraryA(g_settings.renderdoc_path.data());
						} else {
							g_output << "[stormbird] renderdoc.dll not found\n";
						}
					}
				}
			}

			if (g_settings.dump_rtti) {
				create_hook("rtti", g_output, g_game_module, RTTI_FACTORY_CTOR_SIGNATURE, &rtti_factory_ctor, reinterpret_cast<LPVOID *>(&fwd_rtti_factory_ctor));
				rtti_factory = nullptr;
			}

			g_output << "[stormbird] init complete\n";

			g_output.flush();
		}

		void
		fini() {
			g_settings.save();
			g_output << "[stormbird] fini\n";

			if (g_renderdoc != nullptr) {
				g_output << "[stormbird] unloading renderdoc\n";
				FreeLibrary(g_renderdoc);
			}

			if (g_rtti_dump_thread.joinable()) {
				g_rtti_dump_thread.join();
			}

			g_output << "[stormbird] fini complete\n";
			g_output.flush();
			g_output.close();
		}
	} // namespace runtime
} // namespace stormbird_hook

#pragma clang diagnostic pop
