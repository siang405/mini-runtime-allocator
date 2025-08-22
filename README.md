# Mini Runtime Allocator

This is a simple memory allocator simulator written in C++. It mimics a heap allocator with basic allocation and deallocation functions, simulating a contiguous memory region. The project is designed to demonstrate how memory allocation strategies work in a runtime system.

## Features

* Fixed-size memory (default: 1024 bytes)
* First-Fit allocation strategy (default)
* Best-Fit allocation strategy
* Worst-Fit allocation strategy (new!)
* Command-line interface (CLI)
* Supports `alloc`, `free`, `show`, `strategy`, `stats`, and `exit` commands

## Usage

### Build (with CMake)

```bash
mkdir build
cd build
cmake ..
make
```

### Run

```bash
./src/runtime
```

### CLI Commands

| Command           | Description                                    |
| ----------------- | ---------------------------------------------- |
| `alloc <size>`    | Allocate memory block of given size            |
| `free <id>`       | Free block by allocation ID                    |
| `show`            | Show current memory layout                     |
| `strategy <name>` | Switch strategy to `first`, `best`, or `worst` |
| `stats`           | Show fragmentation statistics                  |
| `exit`            | Exit the program                               |

### Example

```
> alloc 300
Allocated ID: 1
> alloc 200
Allocated ID: 2
> show
[0 - 299] Used (ID: 1) Size: 300  
[300 - 499] Used (ID: 2) Size: 200  
[500 - 1023] Free Size: 524  
> free 1
Freed ID: 1
> strategy best
Switched to Best-Fit strategy
> alloc 100
Allocated ID: 3
> strategy worst
Switched to Worst-Fit strategy
> alloc 50
Allocated ID: 4
> show
[0 - 99] Used (ID: 3) Size: 100  
[100 - 299] Free Size: 200  
[300 - 499] Used (ID: 2) Size: 200  
[500 - 549] Used (ID: 4) Size: 50  
[550 - 1023] Free Size: 474  
```

## Testing

This project uses [Catch2](https://github.com/catchorg/Catch2), a modern C++ testing framework. After building, you can run tests with:

```bash
ctest --verbose
```

## Future Extensions

* Add more allocation strategies (e.g. Buddy System)
* Visual memory map output
* Fragmentation analysis
* Enhanced CLI experience

---

MIT License © 2025
