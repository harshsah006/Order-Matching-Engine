#include "OrderBook.h"
#include <iostream>
#include <chrono>
#include <random>
#include <cmath>

int main() {
    OrderBook book;
    
    // Add some initial orders
    book.addOrder(Order(1, OrderType::Sell, 150.50, 100, 1));
    book.addOrder(Order(2, OrderType::Sell, 150.75, 50, 2));
    book.addOrder(Order(3, OrderType::Buy, 149.00, 200, 3));
    
    std::cout << "Initial Book State:\n";
    book.printBook();
    
    std::cout << "\nIncoming aggressive buy order to cross the spread:\n";
    // This order will match with the $150.50 ask, leaving the remainder on the book or matching further
    book.addOrder(Order(4, OrderType::Buy, 150.60, 120, 4));
    
    book.printBook();
    
    // Benchmark Mode
    std::cout << "\n--- Starting Latency Benchmark ---\n";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> priceDist(140.0, 160.0);
    std::uniform_int_distribution<> qtyDist(10, 1000);
    std::uniform_int_distribution<> typeDist(0, 1);
    
    const int NUM_ORDERS = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ORDERS; ++i) {
        OrderType type = typeDist(gen) == 0 ? OrderType::Buy : OrderType::Sell;
        double price = std::round(priceDist(gen) * 100.0) / 100.0;
        uint32_t qty = qtyDist(gen);
        
        book.addOrder(Order(5 + i, type, price, qty, 5 + i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "Processed " << NUM_ORDERS << " orders.\n";
    std::cout << "Total Time: " << duration_ms << " ms\n";
    std::cout << "Average Latency per Order: " << (duration_ns / NUM_ORDERS) << " ns\n";
    
    return 0;
}
