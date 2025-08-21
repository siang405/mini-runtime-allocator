# Mini Runtime Allocator - Design Notes

## Overview

This project implements a mini heap allocator simulator in C++, using a vector of `Block` structures to represent a contiguous region of memory.

## Data Structures

### `Block`

```cpp
struct Block {
    size_t start;    // Starting address
    size_t size;     // Size of block
    bool used;       // Usage flag
    int id;          // Allocation ID (0 if free)
};
```

### `memory`

```cpp
std::vector<Block> memory;
```

This vector holds all current memory blocks (used or free). Initially it contains one big free block of size 1024.

## Core Functions

### `allocate(size_t size)`

* Uses **First-Fit strategy**: traverses from left to right, picks the first block with enough space.
* If a block is larger than needed, it is split into a used block and a remaining free block.
* Returns a unique ID or -1 if allocation fails.

### `free_block(int id)`

* Marks the block as unused.
* Merges with adjacent free blocks if possible.

### `show_memory()`

* Prints a formatted view of the memory layout.

## CLI Interface

The main program provides a command-line interface with commands:

* `alloc <size>`: allocate a block
* `free <id>`: deallocate by ID
* `show`: print memory layout
* `exit`: quit the program

## Design Constraints

* No dynamic system memory is used; memory is simulated.
* Fragmentation and coalescing are handled manually.
* First-Fit is the only strategy implemented before commit `best-fit`.

## Next Steps

* Introduce allocation strategy abstraction (enum switch)
* Add `best-fit` selection logic
* Separate logic into strategy-aware dispatcher
* Optional: memory visualization / JSON log

---

This design is intentionally kept simple for educational and demonstrative purposes.
