#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/allocator.hpp"

TEST_CASE("Memory allocator basic operations", "[allocator]") {
    initialize_memory();

    SECTION("Allocate and free a single block") {
        int id = allocate(100);
        REQUIRE(id != -1);
        REQUIRE(free_block(id));
    }

    SECTION("Allocate multiple blocks") {
        int a = allocate(200);
        int b = allocate(300);
        REQUIRE(a != -1);
        REQUIRE(b != -1);
        REQUIRE(free_block(a));
        REQUIRE(free_block(b));
    }

    SECTION("Allocation failure") {
        int id1 = allocate(800);
        int id2 = allocate(300); // should fail
        REQUIRE(id1 != -1);
        REQUIRE(id2 == -1);
    }

    SECTION("Free invalid ID") {
        REQUIRE_FALSE(free_block(999)); // not exist
    }
}
