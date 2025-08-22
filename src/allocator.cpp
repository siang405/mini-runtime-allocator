#include "allocator.hpp"
#include <iostream>
using namespace std;

vector<Block> memory;
const size_t MEMORY_SIZE = 1024;
int next_id = 1;

Block::Block(size_t s, size_t sz, bool u, int i) : start(s), size(sz), used(u), id(i) {}

AllocationStrategy current_strategy = FirstFit;

void set_strategy(AllocationStrategy strategy) {
    current_strategy = strategy;
}

void initialize_memory() {
    memory.clear();
    next_id = 1;
    memory.push_back(Block(0, MEMORY_SIZE, false, 0));
}

int allocate(size_t size) {
    if (current_strategy == FirstFit) {
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
    } else if (current_strategy == BestFit) {
        int best_index = -1;
        size_t best_size = SIZE_MAX;
        for (size_t i = 0; i < memory.size(); ++i) {
            auto& block = memory[i];
            if (!block.used && block.size >= size) {
                if (block.size < best_size) {
                    best_size = block.size;
                    best_index = i;
                }
            }
        }
        if (best_index != -1) {
            auto& block = memory[best_index];
            if (block.size > size) {
                memory.insert(memory.begin() + best_index + 1,
                    Block(block.start + size, block.size - size, false, 0));
                block.size = size;
            }
            block.used = true;
            block.id = next_id++;
            return block.id;
        }
    } else if (current_strategy == WorstFit) {
        int worst_index = -1;
        size_t worst_size = 0;
        for (size_t i = 0; i < memory.size(); ++i) {
            auto& block = memory[i];
            if (!block.used && block.size >= size) {
                if (block.size > worst_size) {
                    worst_size = block.size;
                    worst_index = i;
                }
            }
        }
        if (worst_index != -1) {
            auto& block = memory[worst_index];
            if (block.size > size) {
                memory.insert(memory.begin() + worst_index + 1,
                    Block(block.start + size, block.size - size, false, 0));
                block.size = size;
            }
            block.used = true;
            block.id = next_id++;
            return block.id;
        }
    }

    return -1;  // Allocation failed
}



bool free_block(int id) {
    for (size_t i = 0; i < memory.size(); ++i) {
        if (memory[i].id == id && memory[i].used) {
            memory[i].used = false;
            memory[i].id = 0;

            // Merge with next
            if (i + 1 < memory.size() && !memory[i + 1].used) {
                memory[i].size += memory[i + 1].size;
                memory.erase(memory.begin() + i + 1);
            }

            // Merge with previous
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
    cout << "\nMemory Layout:\n";
    for (const auto& block : memory) {
        cout << "[" << block.start << " - " << (block.start + block.size - 1)
                  << "] " << (block.used ? "Used" : "Free")
                  << (block.used ? (" (ID: " + to_string(block.id) + ")") : "")
                  << " Size: " << block.size << "\n";
    }
}

void show_fragmentation_stats() {
    size_t total_free = 0;
    size_t largest_free_block = 0;
    int fragment_count = 0;

    for (const auto& block : memory) {
        if (!block.used) {
            total_free += block.size;
            largest_free_block = max(largest_free_block, block.size);
            fragment_count++;
        }
    }

    double fragmentation = 0.0;
    if (total_free > 0 && largest_free_block > 0 && fragment_count > 1) {
        fragmentation = 1.0 - (double)largest_free_block / total_free;
    }

    cout << "\n[Fragmentation Stats]\n";
    cout << "Total Free Memory     : " << total_free << " bytes\n";
    cout << "Largest Free Block    : " << largest_free_block << " bytes\n";
    cout << "Number of Fragments   : " << fragment_count << "\n";
    cout << "External Fragmentation: " << fragmentation * 100 << "%\n";
}
