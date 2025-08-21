# Mini Runtime Allocator

This is a simple memory allocator simulator written in C++. It mimics a heap allocator with basic allocation and deallocation functions, simulating a contiguous memory region. The project is designed to demonstrate how memory allocation strategies work in a runtime system.

## Features

* Fixed-size memory (default: 1024 bytes)
* First-Fit allocation strategy (default)
* Best-Fit allocation strategy (new!)
* Command-line interface (CLI)
* Supports `alloc`, `free`, `show`, and `exit` commands

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

| Command        | Description                   |
| -------------- | ----------------------------- |
| `alloc <size>` | Allocate memory block of size |
| `free <id>`    | Free block by allocation ID   |
| `show`         | Show current memory layout    |
|`strategy <name>`| Switch strategy to first or best |
| `exit`         | Exit the program              |

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
> show
[0 - 99] Used (ID: 3) Size: 100  
[100 - 299] Free Size: 200  
[300 - 499] Used (ID: 2) Size: 200  
[500 - 1023] Free Size: 524  

```

## Testing

This project uses [Catch2](https://github.com/catchorg/Catch2), a modern C++ testing framework. After building, you can run tests with:

```bash
ctest --verbose
```

## Future Extensions

* Add more allocation strategies (e.g. Worst-Fit, Buddy System)
* Visual memory map output
* Fragmentation analysis
* Enhanced CLI experience

---

MIT License © 2025
