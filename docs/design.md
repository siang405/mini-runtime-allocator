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

### Previous Design

Originally, each strategy contained its own allocation logic. Both Best-Fit and Worst-Fit duplicated the splitting and marking logic from First-Fit.
This led to inconsistencies: in particular, Best-Fit and Worst-Fit could produce **stale free blocks** due to reference invalidation after `std::vector::insert`.

### Current Design (Refactored)

All strategies now share a **unified allocation routine** for marking used blocks and splitting leftovers.
The strategy is only responsible for choosing the target block index (`target_index`).

The Buddy system is implemented separately because it enforces power-of-two block sizes and requires recursive splitting and merging.

### Pseudocode (Unified Logic for First/Best/Worst)

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

### Pseudocode (Buddy System)

```cpp
req_size = round_up_to_power_of_two(request)
find a free block with size >= req_size
while block.size > req_size:
    split block into two buddies
    keep one, mark the other as free
mark final block as used, assign id
return id
```

### Buddy Free (Merge)

```cpp
mark block as free
while (buddy exists && buddy is free && buddy.size == block.size):
    merge block with buddy
    double block.size
```

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
4. `alloc 100` with Best-Fit → uses \[0–199] because it’s the smallest available block ≥ 100
5. `strategy buddy`
6. `alloc 100` with Buddy → rounds up to 128, allocates a 128-byte block
7. `free 2` → Buddy merge restores larger free blocks

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

* Add detailed fragmentation reporting
* Interactive memory visualizer
* Support additional strategies (e.g., Slab Allocator)
* Implement memory compaction (defragmentation)

---

Last Updated: 2025-08-23 design
