#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/allocator.hpp"
#include <cmath>
using namespace std;

//
// Basic functionality tests
//
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
        REQUIRE_FALSE(free_block(999)); // non-existent id
    }
}

//
// BestFit test
//
TEST_CASE("BestFit correctly splits block", "[bestfit]") {
    initialize_memory();
    set_strategy(BestFit);

    int id1 = allocate(200);   // [0-199] Used
    int id2 = allocate(300);   // [200-499] Used
    free_block(id1);           // [0-199] Free

    int id3 = allocate(100);   // should fit into [0-199]
    REQUIRE(id3 != -1);

    // Verify split result
    REQUIRE(memory[0].used == true);    // [0-99] Used
    REQUIRE(memory[0].size == 100);
    REQUIRE(memory[1].used == false);   // [100-199] Free
    REQUIRE(memory[1].size == 100);
}

//
// WorstFit test
//
TEST_CASE("WorstFit chooses largest free block", "[worstfit]") {
    initialize_memory();
    int id1 = allocate(100);  // [0-99]
    int id2 = allocate(200);  // [100-299]
    int id3 = allocate(300);  // [300-599]

    REQUIRE(free_block(id1)); // [0-99] Free
    REQUIRE(free_block(id3)); // [300-599] Free

    set_strategy(WorstFit);
    int id4 = allocate(250);  // should fit into [300-599]
    REQUIRE(id4 != -1);

    REQUIRE(memory[2].used == true);    // [300-549] Used
    REQUIRE(memory[2].size == 250);
    REQUIRE(memory[3].used == false);   // [550-599] Free
}

//
// Memory consistency check (sum must always equal MEMORY_SIZE)
//
TEST_CASE("Total memory always sums to MEMORY_SIZE", "[consistency]") {
    initialize_memory();
    set_strategy(FirstFit);

    int a = allocate(123);
    int b = allocate(456);
    free_block(a);
    int c = allocate(100);

    size_t sum = 0;
    for (const auto& block : memory) sum += block.size;
    REQUIRE(sum == 1024);
}

//
// Fragmentation test
//
TEST_CASE("Fragmentation stats after allocation and freeing", "[fragmentation]") {
    initialize_memory();
    set_strategy(FirstFit);

    // Simulate fragmentation
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
    REQUIRE(max_free_block == 574);     // largest free block
    REQUIRE(fragment_count == 2);       // two free blocks
    REQUIRE(fabs(fragmentation_ratio - (1.0 - 574.0 / 694.0)) < 1e-6);
}

//
// Fragmentation test under BestFit and WorstFit
//
TEST_CASE("Fragmentation under BestFit and WorstFit", "[fragmentation-strategy]") {
    initialize_memory();
    set_strategy(BestFit);

    int id1 = allocate(100);
    int id2 = allocate(200);
    int id3 = allocate(150);
    free_block(id2);
    int id4 = allocate(50);
    int id5 = allocate(30);

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

    REQUIRE(total_free + (100+50+30+150) <= 1024); // total check
    REQUIRE(fragment_count >= 1); // at least one fragment
}
