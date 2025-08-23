#include "allocator.hpp"
#include <iostream>
#include <chrono>
#include <fstream>

using namespace std;

vector<Block> memory;
const size_t MEMORY_SIZE = 1024;
int next_id = 1;

Block::Block(size_t s, size_t sz, bool u, int i) : start(s), size(sz), used(u), id(i) {}

AllocationStrategy current_strategy = FirstFit;

size_t next_power_of_two(size_t n) {
    if (n == 0) return 1;
    size_t power = 1;
    while (power < n) power <<= 1;
    return power;
}

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
    }else if (current_strategy == Buddy) {
        size_t req_size = next_power_of_two(size);

        // find the first free block big enough
        for (size_t i = 0; i < memory.size(); ++i) {
            if (!memory[i].used && memory[i].size >= req_size) {
                target_index = i;
                break;
            }
        }

        if (target_index != -1) {
            size_t start = memory[target_index].start;
            size_t block_size = memory[target_index].size;

            // recursively split until block_size == req_size
            while (block_size > req_size) {
                block_size /= 2;
                // replace current block with first half
                memory[target_index].size = block_size;
                // insert second half after it
                memory.insert(memory.begin() + target_index + 1,
                              Block(start + block_size, block_size, false, 0));
            }

            int id = next_id++;
            memory[target_index].used = true;
            memory[target_index].id = id;
            return id;
        }

        return -1; // no block found
    }

    // existing unified logic for FirstFit/BestFit/WorstFit...
    if (target_index != -1) {
        size_t start = memory[target_index].start;
        size_t old_size = memory[target_index].size;

        int id = next_id++;

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

            if (current_strategy == Buddy) {
                size_t block_size = memory[i].size;
                size_t block_start = memory[i].start;

                bool merged = true;
                while (merged) {
                    merged = false;
                    size_t buddy_start = block_start ^ block_size;

                    // search for buddy
                    for (size_t j = 0; j < memory.size(); ++j) {
                        if (j == i) continue;
                        if (!memory[j].used &&
                            memory[j].size == block_size &&
                            memory[j].start == buddy_start) {
                            // merge
                            size_t new_start = min(block_start, buddy_start);
                            block_size *= 2;

                            // erase the higher index first to keep `i` valid
                            if (j > i) {
                                memory.erase(memory.begin() + j);
                                memory.erase(memory.begin() + i);
                            } else {
                                memory.erase(memory.begin() + i);
                                memory.erase(memory.begin() + j);
                                i = j; // adjust index
                            }

                            // insert merged block
                            memory.insert(memory.begin() + i,
                                Block(new_start, block_size, false, 0));

                            block_start = new_start;
                            merged = true;
                            break;
                        }
                    }
                }
            } else {
                // normal merging
                if (i + 1 < memory.size() && !memory[i + 1].used) {
                    memory[i].size += memory[i + 1].size;
                    memory.erase(memory.begin() + i + 1);
                }
                if (i > 0 && !memory[i - 1].used) {
                    memory[i - 1].size += memory[i].size;
                    memory.erase(memory.begin() + i);
                }
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

void show_memory_ascii(int width) {
    cout << "\n[ASCII Memory Map]\n";
    vector<char> canvas(width, '_');

    for (const auto& block : memory) {
        int start = (block.start * width) / MEMORY_SIZE;
        int end = ((block.start + block.size) * width) / MEMORY_SIZE;
        for (int i = start; i < end && i < width; i++) {
            canvas[i] = block.used ? '#' : '.';
        }
    }

    for (char c : canvas) cout << c;
    cout << "\n";
}

void run_benchmarks(int ops, int max_alloc) {
    std::vector<std::pair<AllocationStrategy, std::string>> strategies = {
        {FirstFit, "first"},
        {BestFit,  "best"},
        {WorstFit, "worst"},
        {Buddy,    "buddy"}
    };

    for (auto& [strat, name] : strategies) {
        initialize_memory();
        set_strategy(strat);

        std::vector<int> allocated;
        allocated.reserve(ops);

        using namespace std::chrono;
        auto start_time = high_resolution_clock::now();

        std::ofstream log("benchmark_" + name + ".csv");
        log << "step,total_free,max_free,fragments,fragmentation_ratio\n";

        for (int i = 0; i < ops; i++) {
            if ((rand() % 2 == 0) && !allocated.empty()) {
                int idx = rand() % allocated.size();
                int id = allocated[idx];
                if (free_block(id)) {
                    allocated.erase(allocated.begin() + idx);
                }
            } else {
                int size = 1 + rand() % max_alloc;
                int id = allocate(size);
                if (id != -1) allocated.push_back(id);
            }

            if (i % 50 == 0) {
                size_t total_free = 0;
                size_t max_free = 0;
                int fragments = 0;
                for (const auto& b : memory) {
                    if (!b.used) {
                        total_free += b.size;
                        max_free = std::max(max_free, b.size);
                        fragments++;
                    }
                }
                double frag_ratio = 0.0;
                if (total_free > 0 && max_free > 0 && fragments > 1)
                    frag_ratio = 1.0 - (double)max_free / total_free;

                log << i << "," << total_free << "," << max_free << ","
                    << fragments << "," << frag_ratio << "\n";
            }
        }

        auto end_time = high_resolution_clock::now();
        auto duration = duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        log.close();
        std::cout << "[Benchmark Finished] Strategy=" << name
                  << " Ops=" << ops
                  << " Time=" << duration << " ms\n"
                  << "Results saved to benchmark_" << name << ".csv\n";
    }
}
