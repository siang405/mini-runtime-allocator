# Mini Runtime Allocator

This is a simple memory allocator simulator written in C++. It mimics a heap allocator with basic allocation and deallocation functions, simulating a contiguous memory region. The project is designed to demonstrate how memory allocation strategies work in a runtime system.

## Features

* Fixed-size memory (default: 1024 bytes)
* First-Fit allocation strategy (default)
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
> show
[0 - 299] Free Size: 300
[300 - 499] Used (ID: 2) Size: 200
[500 - 1023] Free Size: 524
```
## Testing

This project uses [Catch2](https://github.com/catchorg/Catch2), a modern C++ testing framework, allowing you to build and run tests with no additional installation.
Run this after Building:
```bash
ctest --verbose
```
## Future Extensions

* Add best-fit allocation strategy 
* Visual memory map output
* Fragmentation analysis
* Unit tests using Catch2

---

MIT License © 2025
