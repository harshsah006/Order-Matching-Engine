# High-Performance Order Matching Engine

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/c%2B%2B-17-blue.svg)
![Latency](https://img.shields.io/badge/Latency-Sub--200ns-brightgreen)

A blazingly fast, ultra-low latency Order Matching Engine implemented in modern C++17. This project simulates the core component of a financial exchange (e.g., NASDAQ, NYSE, Crypto Exchanges) by matching incoming "Buy" (Bids) and "Sell" (Asks) limit orders using strict **Price-Time Priority**.

Following extensive algorithmic refactoring, this engine is capable of processing orders with **~169 nanosecond latency**, allowing for a throughput of nearly 6 million orders per second on a single thread.

---

## 📑 Table of Contents
1. [Key Features](#-key-features)
2. [Architecture & Design](#-architecture--design)
3. [Performance Benchmarks](#-performance-benchmarks)
4. [System Requirements](#-system-requirements)
5. [Build Instructions](#-build-instructions)
6. [Code Structure](#-code-structure)
7. [Example Output](#-example-output)
8. [Future Work](#-future-work)

---

## 🚀 Key Features

*   **Microsecond Latency**: Built specifically for High-Frequency Trading (HFT) performance constraints.
*   **$O(1)$ Order Cancellation**: Utilizes an internal `unordered_map` hash index pointing directly to linked-list nodes for instantaneous order teardown, eliminating $O(N)$ linear scans.
*   **Zero-Allocation Memory Pools**: Leverages C++17 `std::pmr::unsynchronized_pool_resource` to completely eliminate dynamic heap allocation (`new`/`delete`) overhead during runtime. Memory is pre-allocated in contiguous chunks to maximize CPU cache hits.
*   **Integer Pricing (Fixed-Point Math)**: All prices are evaluated as discrete integer ticks (e.g., cents). This guarantees absolute precision, eliminates floating-point drift, and accelerates map lookups and hashes.
*   **Allocator-Aware Containers**: Automatically passes memory pools down to dynamically generated structures (e.g., price level lists).

---

## 🧠 Architecture & Design

Achieving sub-200ns latency requires carefully selected data structures to enforce Price-Time Priority efficiently.

### 1. Price Priority (`std::pmr::map`)
The order book is split into `bids` and `asks`. Keys are sorted (bids highest to lowest via `std::greater`, asks lowest to highest via `std::less`). 
Using integer ticks as keys ensures extremely fast Red-Black Tree traversals, yielding an $O(\log P)$ lookup time where $P$ is the number of active price levels.

### 2. Time Priority (`std::pmr::list`)
Once a price level is located, matching is strictly First-In-First-Out (FIFO). `std::list` allows for $O(1)$ appending of new orders at the back and $O(1)$ popping of fulfilled orders from the front.

### 3. $O(1)$ Cancellation (`std::pmr::unordered_map`)
A hash map tracks the exact `OrderLocation` (isBid, price level, and list iterator) for every active `orderId`. This allows cancellation requests to jump directly to the exact memory location of the order and remove it in $O(1)$ time.

---

## ⏱️ Performance Benchmarks

The engine includes a benchmark suite that generates randomized order flow to test peak throughput and latency.

**Test Setup:**
- 100,000 randomized orders (Bids and Asks).
- Randomized integer prices and quantities.
- Compiled with `-O3` maximum optimization.

**Results:**
| Metric | Result |
| :--- | :--- |
| **Total Execution Time** | ~16 milliseconds |
| **Average Latency per Order** | **~169 ns** |
| **Estimated Throughput** | ~5.9 million orders/sec |

---

## 💻 System Requirements

- A compiler supporting **C++17** (e.g., GCC 9+, Clang 10+, MSVC 19.14+).
- *Note: C++17 is strictly required for `<memory_resource>` (`std::pmr`) support.*
- CMake 3.10+ (optional, for build generation).

---

## 🛠️ Build Instructions

You can build this project using `g++` directly or via CMake.

### Option 1: Direct Compilation (Recommended for Benchmarking)
```bash
# Compile with maximum optimization (-O3)
g++ -std=c++17 -O3 -I./include src/OrderBook.cpp src/main.cpp -o OrderMatchingEngine

# Run the engine
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

---

## 📁 Code Structure

```text
Order-Matching-Engine/
├── include/
│   ├── Order.h         # Defines the Order struct (ID, type, price, quantity)
│   └── OrderBook.h     # Defines the PMR data structures and OrderLocation index
├── src/
│   ├── OrderBook.cpp   # Implements matching logic and O(1) cancellations
│   └── main.cpp        # Contains execution examples and the Latency Benchmark
├── CMakeLists.txt      # Build configuration
└── README.md
```

---

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

---

## 🔮 Future Work

While the engine operates at microsecond scales, it can be extended for full institutional HFT deployment:
1. **Flat Order Books**: Replacing `std::pmr::map` with an array of lists indexed by price tick offsets to achieve $O(1)$ price lookups.
2. **Lock-Free Concurrency**: Implementing ring buffers (LMAX Disruptor pattern) to allow parallel order ingestion without mutex locks.
3. **Advanced Order Types**: Adding support for Market Orders, Immediate-Or-Cancel (IOC), and Fill-Or-Kill (FOK).
