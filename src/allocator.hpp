#pragma once
#include <vector>
#include <cstddef>

struct Block {
    size_t start;
    size_t size;
    bool used;
    int id;

    Block(size_t s, size_t sz, bool u, int i);
};

void initialize_memory();
int allocate(size_t size);
bool free_block(int id);

void show_memory();
void show_fragmentation_stats();
void show_memory_ascii(int width = 64);

enum AllocationStrategy {
    FirstFit,
    BestFit,
    WorstFit,
    Buddy
};

extern AllocationStrategy current_strategy;
void set_strategy(AllocationStrategy strategy);

extern std::vector<Block> memory;
extern const size_t MEMORY_SIZE;

void run_benchmarks(int ops = 1000, int max_alloc = 200);
