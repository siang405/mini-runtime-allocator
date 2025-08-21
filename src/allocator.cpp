#include "allocator.hpp"
#include <iostream>

std::vector<Block> memory;
const size_t MEMORY_SIZE = 1024;
int next_id = 1;

Block::Block(size_t s, size_t sz, bool u, int i) : start(s), size(sz), used(u), id(i) {}

void initialize_memory() {
    memory.clear();
    memory.push_back(Block(0, MEMORY_SIZE, false, 0));
}

int allocate(size_t size) {
    for (auto it = memory.begin(); it != memory.end(); ++it) {
        if (!it->used && it->size >= size) {
            size_t leftover = it->size - size;
            int id = next_id++;
            it->used = true;
            it->size = size;
            it->id = id;
            if (leftover > 0) {
                memory.insert(it + 1, Block(it->start + size, leftover, false, 0));
            }
            return id;
        }
    }
    return -1;
}

bool free_block(int id) {
    for (size_t i = 0; i < memory.size(); ++i) {
        if (memory[i].id == id && memory[i].used) {
            memory[i].used = false;
            memory[i].id = 0;
            if (i + 1 < memory.size() && !memory[i + 1].used) {
                memory[i].size += memory[i + 1].size;
                memory.erase(memory.begin() + i + 1);
            }
            if (i > 0 && !memory[i - 1].used) {
                memory[i - 1].size += memory[i].size;
                memory.erase(memory.begin() + i);
            }
            return true;
        }
    }
    return false;
}

void show_memory() {
    std::cout << "\nMemory Layout:\n";
    for (const auto& block : memory) {
        std::cout << "[" << block.start << " - " << (block.start + block.size - 1)
                  << "] " << (block.used ? "Used" : "Free")
                  << (block.used ? (" (ID: " + std::to_string(block.id) + ")") : "")
                  << " Size: " << block.size << "\n";
    }
}
