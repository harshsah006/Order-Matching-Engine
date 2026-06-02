# High-Performance Order Matching Engine

A blazingly fast, highly-optimized Order Matching Engine implemented in modern C++17. This project simulates the core component of a financial exchange (like NASDAQ or NYSE) by matching "Buy" (Bids) and "Sell" (Asks) limit orders using strict **Price-Time Priority**.

Following extensive algorithmic refactoring, this engine is capable of processing orders with **sub-200 nanosecond latency**.

## 🚀 Key Features

- **Microsecond Latency**: Built for high-frequency trading performance, handling millions of orders per second.
- **$O(1)$ Order Cancellation**: Utilizes an internal hash-map index pointing directly to linked-list nodes for instantaneous order teardown.
- **Zero-Allocation Memory Pools**: Leverages C++17 `std::pmr::unsynchronized_pool_resource` to entirely eliminate dynamic heap allocation (`new`/`delete`) overhead during runtime.
- **Integer Pricing (Fixed-Point Math)**: All prices are evaluated as discrete integer ticks (cents) to guarantee absolute precision, eliminate floating-point drift, and accelerate map lookups.
- **Benchmarking Mode**: Built-in latency profiler using `<chrono>`.

## 🧠 Architecture Overview

Achieving microsecond latency requires carefully selected data structures:
- **`std::pmr::map`**: Maintains Price Priority. Keys are sorted (bids highest to lowest, asks lowest to highest). Using integer ticks as keys ensures extremely fast Red-Black Tree traversals.
- **`std::pmr::list`**: Maintains Time Priority. Once a price level is found, matching is strictly First-In-First-Out (FIFO). `std::list` allows for $O(1)$ appending and popping of orders.
- **Allocator Awareness**: Because C++ map containers are allocator-aware, the central `pmr` memory pool is automatically passed down to dynamically created `pmr::list` price levels, ensuring completely contiguous, zero-latency memory management.

## ⏱️ Performance Benchmarks

The engine includes a benchmark that generates randomized order flow to test peak throughput and latency.

**Test Setup:**
- 100,000 randomized orders (Bids and Asks)
- Randomized prices and quantities
- Compiled with `-O3` optimization

**Results:**
- **Total Execution Time**: ~16 milliseconds
- **Average Latency per Order**: **~169 ns** *(translates to ~5.9 million orders/sec)*

## 🛠️ Build Instructions

You can build this project using `g++` directly or via CMake.

### Option 1: Direct g++ Compilation (Recommended for Benchmarking)
```bash
g++ -std=c++17 -O3 -I./include src/OrderBook.cpp src/main.cpp -o OrderMatchingEngine
./OrderMatchingEngine
```

### Option 2: Using CMake
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
./OrderMatchingEngine
```

## 📊 Example Output
```text
Initial Book State:
--- Order Book ---
ASKS:
  $150.75 | Vol: 50
  $150.50 | Vol: 100
BIDS:
  $149.00 | Vol: 200
------------------

Incoming aggressive buy order to cross the spread:
--- Order Book ---
ASKS:
  $150.75 | Vol: 50
BIDS:
  $150.60 | Vol: 20
  $149.00 | Vol: 200
------------------

--- Starting Latency Benchmark ---
Processed 100000 orders.
Total Time: 16 ms
Average Latency per Order: 169 ns
```
