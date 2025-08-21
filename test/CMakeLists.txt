add_executable(test_allocator test_allocator.cpp)
target_include_directories(test_allocator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_allocator allocator)
add_test(NAME AllocatorTests COMMAND test_allocator)
