# High-Performance Order Matching Engine

A fast and robust Order Matching Engine implemented in C++17. This project simulates the core component of a financial exchange (like NASDAQ or NYSE) by matching "buy" (Bids) and "sell" (Asks) limit orders using strict **price-time priority**.

## Features
- **Limit Order Processing**: Handles both Bids and Asks dynamically.
- **Price-Time Priority**: Ensures fairness by matching the best price first; for identical prices, the earliest order is executed first.
- **Microsecond Latency**: Highly optimized using C++ STL containers for rapid order insertions and continuous matching.
- **Benchmarking Mode**: Built-in latency profiler using `<chrono>`.

## Architecture & Data Structures

Choosing the right data structures is critical for a high-frequency trading engine. This project uses:
- **`std::map<double, std::list<Order>>`**: 
  - **Why `std::map`?** It keeps price levels sorted automatically (O(log N) insertion). Bids use `std::greater<double>` to keep the highest price at the top, while Asks use `std::less<double>` for the lowest price.
  - **Why `std::list`?** Once a price level is found, we need to enforce *time priority*. `std::list` allows for O(1) appending of new orders at the back, and O(1) popping of fulfilled orders from the front.

## Performance Benchmark

The engine includes a benchmark that generates random order flow to test throughput and latency. 

**Test Setup:**
- 100,000 randomized orders (Bids and Asks)
- Randomized prices and quantities

**Results:**
- **Total Execution Time**: ~143 milliseconds
- **Average Latency per Order**: **~1.4 microseconds (1439 ns)**

## Build Instructions

You can build this project using `g++` directly or via CMake.

### Option 1: Direct g++ Compilation (No CMake needed)
```bash
g++ -std=c++17 -I./include src/OrderBook.cpp src/main.cpp -o OrderMatchingEngine
./OrderMatchingEngine
```

### Option 2: Using CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
./OrderMatchingEngine
```

## Example Output
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
```
