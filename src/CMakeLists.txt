add_library(allocator allocator.cpp)
add_executable(runtime main.cpp)
target_link_libraries(runtime allocator)
