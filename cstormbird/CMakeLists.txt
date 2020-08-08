project(cstormbird)

add_executable(cstormbird main.cpp)
target_link_libraries(cstormbird stormbird)
target_include_directories(cstormbird PUBLIC ${STORMBIRD_SOURCE_DIR}/public)
