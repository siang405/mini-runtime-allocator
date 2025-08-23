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
    int target_index = -1;

    if (current_strategy == FirstFit) {
        for (size_t i = 0; i < memory.size(); ++i) {
            if (!memory[i].used && memory[i].size >= size) {
                target_index = i;
                break;
            }
        }
    } else if (current_strategy == BestFit) {
        size_t best_size = SIZE_MAX;
        for (size_t i = 0; i < memory.size(); ++i) {
            if (!memory[i].used && memory[i].size >= size && memory[i].size < best_size) {
                best_size = memory[i].size;
                target_index = i;
            }
        }
    } else if (current_strategy == WorstFit) {
        size_t worst_size = 0;
        for (size_t i = 0; i < memory.size(); ++i) {
            if (!memory[i].used && memory[i].size >= size && memory[i].size > worst_size) {
                worst_size = memory[i].size;
                target_index = i;
            }
        }
    }

    if (target_index != -1) {
        size_t start = memory[target_index].start;
        size_t old_size = memory[target_index].size;

        int id = next_id++;

        // 覆蓋原本的 block
        memory[target_index].used = true;
        memory[target_index].size = size;
        memory[target_index].id = id;

        size_t leftover = old_size - size;
        if (leftover > 0) {
            memory.insert(memory.begin() + target_index + 1,
                          Block(start + size, leftover, false, 0));
        }

        return id;
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
