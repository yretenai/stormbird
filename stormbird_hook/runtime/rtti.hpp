#include <cstdint>
#include <array>

#pragma once

namespace stormbird_hook {
#pragma pack(push, 1)
	template<typename T>
	struct Array {
		T* array;
		uint32_t count;
		uint32_t capacity;

		[[nodiscard]] auto
		operator[](uint32_t index) const -> T & {
			if(index > capacity) {
				return {};
			}

			return array[index];
		}

		struct iterator {
			using iterator_category [[maybe_unused]] = std::forward_iterator_tag;
			using difference_type [[maybe_unused]] = std::ptrdiff_t;
			using value_type = T;
			using pointer = value_type *;
			using reference = value_type &;

			explicit iterator(pointer ptr): array_ptr(ptr) { }

			auto
			operator*() const noexcept -> reference {
				return *array_ptr;
			}

			auto
			operator->() const noexcept -> pointer {
				return array_ptr;
			}

			auto
			operator++() noexcept -> iterator & {
				array_ptr++;
				return *this;
			}

			auto
			operator++(int) noexcept -> iterator { // NOLINT(cert-dcl21-cpp)
				iterator tmp = *this;
				array_ptr++;
				return tmp;
			}

			friend auto
			operator==(const iterator &lhs, const iterator &rhs) noexcept -> bool {
				return lhs.array_ptr == rhs.array_ptr;
			};

			friend auto
			operator!=(const iterator &lhs, const iterator &rhs) noexcept -> bool {
				return lhs.array_ptr != rhs.array_ptr;
			};

			pointer array_ptr;
		};

		[[nodiscard]] auto
		begin() const noexcept -> iterator {
			return iterator(array);
		}

		[[nodiscard]] auto
		end() const noexcept -> iterator {
			return iterator(array + count);
		}
	};

	enum class RTTIType : uint8_t {
		Primitive,
		Reference,
		Container,
		Enum,
		Class,
		Bitset,
		Struct
	};

	struct RTTIBase {
		// todo!
		uint16_t type_id;
		uint16_t category_type_id;
		RTTIType rtti_type;
	};

	struct RTTIPrimitive {
		RTTIBase base;
		uint8_t padding;
		uint16_t total_size;
		uint8_t size;
		uint8_t count;
		uint16_t unknown1;
		uint32_t unknown2;
		const char* name;
		RTTIBase* parent;
	};

	struct RTTIReferenceBaseData {
		const char* name;
		uint16_t unknown1;
		uint8_t unknown2;
		uint8_t unknown3;
		uint32_t padding;
		// list of function pointers follows here.
		// ref has at least 2, but can have up to 8.
		// container has at least 4, but can have up to 32.
	};

	// container is a reference
	struct RTTIReference {
		RTTIBase base;
		std::array<uint8_t, 3> padding;
		RTTIBase* type;
		RTTIReferenceBaseData* data;
	};

	using RTTIContainer = RTTIReference;

	struct RTTIEnumValue {
		uint64_t value;
		const char* name;
		std::array<uint8_t, 24> padding;
	};

	// bitset is an enum
	struct RTTIEnum {
		RTTIBase base;
		uint8_t size;
		uint16_t member_count;
		uint32_t unknown1;
		uint32_t unknown2;
		const char* name;
		RTTIEnumValue* values;
		void* alloc_info;
	};

	using RTTIBitset = RTTIEnum;

	struct RTTIClass;

	struct RTTIBaseClass {
		RTTIClass* type;
		uint32_t offset;
		uint32_t padding;
	};

	struct RTTIClassMember {
		RTTIBase* type;
		uint16_t offset;
		uint16_t flags;
		uint32_t unknown;
		const char* name;
		void* get;
		void* set;
		void* unknown1;
		void* unknown2;
	};

	struct RTTIClassFunction {
		uint64_t return_type;
		const char* name;
		const char* args;
		void* func;
	};

	struct RTTIClassEvent {
		RTTIBase* type;
		void* func;
	};

	struct RTTIBaseClassEvent {
		uint64_t unknown1;
		RTTIBase* type;
		RTTIBase* base_class;
	};

	struct RTTIClass {
		RTTIBase base;
		uint8_t base_count;
		uint8_t member_count;
		uint8_t function_count;
		uint8_t event_count;
		uint8_t base_event_count;
		uint8_t unknown1;
		uint8_t unknown2;
		uint16_t unknown3;
		uint16_t unknown4;
		uint32_t size;
		uint16_t alignment;
		uint16_t flags;
		void* ctor;
		void* dtor;
		void* cctor;
		void* cdtor;
		const char* name;
		uint32_t hash; // hash of the schema???
		uint32_t unknown5;
		RTTIClass* first_child;
		RTTIClass* next_sibling;
		RTTIBaseClass* bases;
		RTTIClassMember* members;
		RTTIClassFunction* functions;
		RTTIClassEvent* events;
		RTTIBaseClassEvent* base_events;
		void* exports;
	};

	struct RTTIStruct {
		RTTIBase base;
		uint8_t unknown1;
		uint16_t unknown2;
		void* unknown3;
		uint32_t size;
	};

	struct RTTIRecord {
		uint64_t hash;
		RTTIBase* rtti;
	};

	struct RuntimeRTTIRecord {
		uint64_t hash;
		uint64_t unknown;
		RTTIBase* rtti;
	};

	struct RTTIRefChain {
		uint64_t hash;
		RTTIBase* type;
		RTTIBase* ref_type;
	};

	struct RTTIRefRecord {
		uint64_t hash;
		RTTIReferenceBaseData* data;
		Array<RTTIRefChain> rtti;
	};

	struct CoreRTTIInfo {
		RTTIBase* rtti;
		void* ctor;
		void* dtor;
	};

	struct CoreRTTIRecord {
		uint64_t hash;
		const char* name;
		CoreRTTIInfo* rtti;
	};

	struct RTTIFactory {
		void** vtable;
		Array<RTTIRecord> rtti;
		Array<RuntimeRTTIRecord> runtime_rtti; // usually POD types, very confusing layout.
		Array<RTTIRefRecord> rtti_refs; // todo: this one is confusing
		Array<CoreRTTIRecord> core_rtti;
	};
#pragma pack(pop)
}
