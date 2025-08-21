# Design Document: Mini Runtime Allocator

## Overview

This project implements a simple dynamic memory allocator that simulates a heap with fixed-size memory (1024 bytes). It supports memory allocation and deallocation using different strategies.

## Memory Model

- Simulated memory is represented by a `std::vector<Block>`.
- Each `Block` stores:
  - `start`: starting address (offset from 0)
  - `size`: size of the block
  - `used`: whether the block is in use
  - `id`: a unique allocation ID (non-zero if used)

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

- Traverse memory from the beginning.
- Allocate in the first block large enough to satisfy the request.
- If the block is larger than requested, it is split into used and free parts.

### Best-Fit (added in Phase 2)

- Traverse the entire memory to find the smallest free block that can satisfy the request.
- If found, allocate and split if necessary.
- More efficient in terms of fragmentation than First-Fit, but slightly more expensive in time.

### Strategy Selection

A global variable `current_strategy` stores the selected allocation strategy.

```cpp
enum AllocationStrategy {
    FirstFit,
    BestFit
};

extern AllocationStrategy current_strategy;
void set_strategy(AllocationStrategy strategy);
```

## Allocation Logic

Shared logic:
- Search for a suitable block based on the strategy.
- If the block is larger than the requested size, split it.
- Assign a unique allocation ID (`next_id++`).

### First-Fit Logic (simplified):

```cpp
for (block in memory)
    if (block.free && block.size >= size)
        split if necessary
        mark used
        return id
```

### Best-Fit Logic:

```cpp
find block with smallest size ≥ requested size
split if necessary
mark used
return id
```

## Deallocation Logic

- Mark the block as free.
- Try merging with adjacent free blocks (coalescing).
- Keep memory as compact as possible.

## Example Allocation Flow

1. `alloc 200` → allocates [0–199]
2. `alloc 300` → allocates [200–499]
3. `free 1` → marks [0–199] as free
4. `alloc 100` with Best-Fit → uses [0–199] because it's the smallest available block ≥ 100

## Testing

- Unit tested with [Catch2](https://github.com/catchorg/Catch2)
- Tests include:
  - Allocating and freeing
  - Strategy-specific behaviors
  - Edge cases (allocation failure, invalid free)

## Future Work

- Add fragmentation reporting
- Interactive memory visualizer
- Support for additional strategies (e.g., Worst-Fit, Buddy Allocation)
- Memory compaction (defragmentation)

---

Last Updated: 2025-08-22
