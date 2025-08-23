# Mini Runtime Allocator

This is a simple memory allocator simulator written in C++. It mimics a heap allocator with basic allocation and deallocation functions, simulating a contiguous memory region. The project is designed to demonstrate how memory allocation strategies work in a runtime system.

## Features

* Fixed-size memory (default: 1024 bytes)
* Allocation strategies:

  * First-Fit
  * Best-Fit
  * Worst-Fit
  * Buddy System (power-of-two splitting & merging)
* Command-line interface (CLI)
* Supports `alloc`, `free`, `show`, `strategy`, `stats`, `visual`, `benchmark`, and `exit` commands
* Benchmarking framework with CSV output for analysis
* ASCII visualization of memory layout

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

| Command           | Description                                                 |
| ----------------- | ----------------------------------------------------------- |
| `alloc <size>`    | Allocate memory block of given size                         |
| `free <id>`       | Free block by allocation ID                                 |
| `show`            | Show current memory layout                                  |
| `strategy <name>` | Switch strategy to `first`, `best`, `worst`, or `buddy`     |
| `stats`           | Show fragmentation statistics                               |
| `visual`          | Show ASCII visualization of memory (used = `#`, free = `.`) |
| `benchmark`       | Run benchmark for all strategies, output CSV + summary      |
| `exit`            | Exit the program                                            |

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
> visual
[ASCII Memory Map]
###.................................................
> benchmark
[Benchmark Finished] Strategy=first Ops=1000 Time=12 ms
[Benchmark Finished] Strategy=best Ops=1000 Time=13 ms
[Benchmark Finished] Strategy=worst Ops=1000 Time=12 ms
[Benchmark Finished] Strategy=buddy Ops=1000 Time=9 ms
Results saved to benchmark_first.csv, benchmark_best.csv, benchmark_worst.csv, benchmark_buddy.csv
```

## Benchmark & Visualization

* Benchmark generates CSV files for all strategies:

  * `benchmark_first.csv`
  * `benchmark_best.csv`
  * `benchmark_worst.csv`
  * `benchmark_buddy.csv`
* Each CSV includes:

  * step, total\_free, max\_free, fragments, fragmentation\_ratio
* A Python script `plot_benchmark.py` is provided in the **project root** to compare fragmentation ratios:

```bash
python3 plot_benchmark.py
```

This generates `benchmark_comparison.png`, plotting fragmentation ratio vs operations for all strategies. It also prints average fragmentation ratio for each strategy in the console.

## Testing

This project uses [Catch2](https://github.com/catchorg/Catch2), a modern C++ testing framework. After building, you can run tests with:

```bash
ctest --verbose
```

## Future Extensions

* Extended fragmentation analysis (internal + external)
* Advanced allocators (e.g., Slab Allocator, TLSF)
* Interactive/graphical visualizer

---

MIT License © 2025
