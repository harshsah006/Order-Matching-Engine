# Order Matching Engine

A high-performance Order Matching Engine implemented in C++17. 
This is the core component of a financial exchange that matches "buy" and "sell" orders using price-time priority.

## Features
- Handles Limit Orders (Bids and Asks).
- Price-time priority matching logic using STL containers (`std::map`, `std::list`).
- Measures and benchmarks matching latency using `<chrono>`.

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
