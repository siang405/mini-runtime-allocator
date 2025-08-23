# Design Document: Mini Runtime Allocator

## Overview

This project implements a simple dynamic memory allocator that simulates a heap with fixed-size memory (1024 bytes). It supports memory allocation and deallocation using different strategies.

## Memory Model

* Simulated memory is represented by a `std::vector<Block>`.
* Each `Block` stores:

  * `start`: starting address (offset from 0)
  * `size`: size of the block
  * `used`: whether the block is in use
  * `id`: a unique allocation ID (non-zero if used)

```cpp
struct Block {
    size_t start;
    size_t size;
    bool used;
    int id;
};
```

## Allocation Strategies

### First-Fit (default)

* Traverse memory from the beginning.
* Allocate in the first block large enough to satisfy the request.
* If the block is larger than requested, it is split into used and free parts.

### Best-Fit

* Traverse the entire memory to find the smallest free block that can satisfy the request.
* If found, allocate and split if necessary.
* Reduces external fragmentation compared to First-Fit, but requires a full scan.

### Worst-Fit

* Traverse the entire memory to find the largest free block that can satisfy the request.
* If found, allocate and split if necessary.
* Leaves larger free blocks, potentially reducing the number of unusable fragments.

### Buddy System (new)

* Memory is divided into blocks of size 2^k.
* Allocation size is rounded up to the nearest power of two.
* If a block is larger than required, it is recursively split into two buddies.
* On deallocation, if both buddies are free and the same size, they are merged back together.
* Provides fast split/merge operations and reduces external fragmentation, at the cost of internal fragmentation.

### Strategy Selection

A global variable `current_strategy` stores the selected allocation strategy.

```cpp
enum AllocationStrategy {
    FirstFit,
    BestFit,
    WorstFit,
    Buddy
};

extern AllocationStrategy current_strategy;
void set_strategy(AllocationStrategy strategy);
```

## Allocation Logic

### Unified Allocation (First/Best/Worst)

* Strategies only select a target block index.
* The core allocation logic handles marking used, splitting, and assigning IDs.

### Buddy Allocation

* Rounds size to next power of two.
* Recursively splits blocks until reaching the required size.
* Marks block as used.
* On free, merges buddies back together if both are free and equal size.

## Deallocation Logic

* For First/Best/Worst:

  * Mark the block as free.
  * Merge with adjacent free blocks (coalescing).
* For Buddy:

  * Mark as free.
  * Attempt recursive buddy merge until no further merge is possible.

## Example Allocation Flow

1. `alloc 200` → allocates \[0–199]
2. `alloc 300` → allocates \[200–499]
3. `free 1` → marks \[0–199] as free
4. `alloc 100` with Best-Fit → uses \[0–199]
5. `strategy buddy`
6. `alloc 100` with Buddy → rounds to 128, allocates \[0–127]
7. `free 2` → Buddy merge restores larger free blocks

## Benchmarking (new)

* Added a benchmarking framework that runs random allocation/free sequences.
* Benchmarks all strategies (First-Fit, Best-Fit, Worst-Fit, Buddy).
* Outputs CSV files (`benchmark_first.csv`, `benchmark_best.csv`, `benchmark_worst.csv`, `benchmark_buddy.csv`).
* Each CSV contains:

  * step, total\_free, max\_free, fragments, fragmentation\_ratio
* Supports visualization with Python (`plot_benchmark.py`), comparing fragmentation ratio curves across strategies.
* The Python script also computes **average fragmentation ratio** for each strategy and saves a summary plot (`benchmark_comparison.png`).

## ASCII Visualization (new)

* Added ASCII visualization of memory layout.
* CLI command `visual` prints memory map:

  * `#` = used block
  * `.` = free block
* Example:

```
[ASCII Memory Map]
###....................
```

## Testing

* Unit tested with [Catch2](https://github.com/catchorg/Catch2)
* Tests include:

  * Basic allocation and deallocation
  * Strategy-specific behaviors
  * Split correctness for Best-Fit, Worst-Fit, and Buddy
  * Edge cases (allocation failure, invalid free)
  * Memory sum consistency (must always equal 1024)
  * Fragmentation statistics under different strategies
  * Buddy merge behavior

## Future Work

* Add internal fragmentation reporting
* Interactive/graphical memory visualizer
* Implement more advanced allocators (Slab, TLSF)
* Implement memory compaction (defragmentation)

---

Last Updated: 2025-08-23 design
