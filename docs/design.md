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

### Best-Fit (Phase 2)

* Traverse the entire memory to find the smallest free block that can satisfy the request.
* If found, allocate and split if necessary.
* Reduces external fragmentation compared to First-Fit, but requires a full scan.

### Worst-Fit (Phase 3)

* Traverse the entire memory to find the largest free block that can satisfy the request.
* If found, allocate and split if necessary.
* Leaves larger free blocks, potentially reducing the number of unusable fragments.

### Strategy Selection

A global variable `current_strategy` stores the selected allocation strategy.

```cpp
enum AllocationStrategy {
    FirstFit,
    BestFit,
    WorstFit
};

extern AllocationStrategy current_strategy;
void set_strategy(AllocationStrategy strategy);
```

## Allocation Logic

### Previous Design

Originally, each strategy contained its own allocation logic. Both Best-Fit and Worst-Fit duplicated the splitting and marking logic from First-Fit.
This led to inconsistencies: in particular, Best-Fit and Worst-Fit could produce **stale free blocks** due to reference invalidation after `std::vector::insert`.

### Current Design (Refactored)

All strategies now share a **unified allocation routine**.
The strategy is only responsible for choosing the target block index (`target_index`), while the split and update logic is centralized:

* Search for a suitable block based on the selected strategy.
* Mark it as used, shrink it to requested size, assign allocation ID.
* If there is leftover space, insert a new free block immediately after.

This guarantees consistent behavior across all strategies.

### Pseudocode (Unified Logic)

```cpp
find target_index according to strategy

if target_index != -1:
    save start, old_size
    mark block as used with requested size
    assign id
    if leftover > 0:
        insert a new free block after target_index
    return id

return -1 // allocation failed
```

## Deallocation Logic

* Mark the block as free.
* Merge with adjacent free blocks (coalescing).
* Keeps memory layout compact and prevents fragmentation growth.

## Example Allocation Flow

1. `alloc 200` → allocates \[0–199]
2. `alloc 300` → allocates \[200–499]
3. `free 1` → marks \[0–199] as free
4. `alloc 100` with Best-Fit → uses \[0–199] because it’s the smallest available block ≥ 100
5. `strategy worst`
6. `alloc 100` with Worst-Fit → chooses the largest free block

## Testing

* Unit tested with [Catch2](https://github.com/catchorg/Catch2)
* Tests include:

  * Basic allocation and deallocation
  * Strategy-specific behaviors
  * Split correctness for Best-Fit and Worst-Fit
  * Edge cases (allocation failure, invalid free)
  * Memory sum consistency (must always equal 1024)
  * Fragmentation statistics under different strategies

## Future Work

* Add detailed fragmentation reporting
* Interactive memory visualizer
* Support additional strategies (e.g., Buddy Allocation)
* Implement memory compaction (defragmentation)

---

Last Updated: 2025-08-23 design
