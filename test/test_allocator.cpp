#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/allocator.hpp"
#include <cmath>
using namespace std;

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
TEST_CASE("Best-Fit Allocation", "[bestfit]") {
    initialize_memory();
    set_strategy(BestFit);
    int id1 = allocate(10);
    int id2 = allocate(20);
    free_block(id1); // creates free block of size 10
    int id3 = allocate(8); // should go into size 10 block
    REQUIRE(id3 != -1);
}

TEST_CASE("Fragmentation stats after allocation and freeing", "[fragmentation]") {
    initialize_memory();
    set_strategy(FirstFit);

    // 模擬碎片化情況
    int id1 = allocate(100); // [0-99] Used
    int id2 = allocate(200); // [100-299] Used
    int id3 = allocate(150); // [300-449] Used
    free_block(id2);         // [100-299] Free
    int id4 = allocate(50);  // [100-149] Used, [150-299] Free
    int id5 = allocate(30);  // [150-179] Used, [180-299] Free

    size_t total_free = 0;
    size_t max_free_block = 0;
    int fragment_count = 0;

    for (const auto& block : memory) {
        if (!block.used) {
            total_free += block.size;
            max_free_block = std::max(max_free_block, block.size);
            fragment_count++;
        }
    }

    double fragmentation_ratio = 1.0 - (double)max_free_block / total_free;

    REQUIRE(total_free == 694);         // 120 + 574
    REQUIRE(max_free_block == 574);     // 最大空閒區塊
    REQUIRE(fragment_count == 2);       // 兩個空閒區塊
    REQUIRE(fabs(fragmentation_ratio - (1.0 - 574.0 / 694.0)) < 1e-6); // 碎片率接近
}


