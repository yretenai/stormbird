project(cstormbird)

add_executable(cstormbird main.cpp)
target_compile_definitions(cstormbird PRIVATE DRAGON_LIBRARY_NAME="cstormbird")
target_link_libraries(cstormbird stormbird)
target_include_directories(cstormbird PUBLIC ${STORMBIRD_SOURCE_DIR}/public)
