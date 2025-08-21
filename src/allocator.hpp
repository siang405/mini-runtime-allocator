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
