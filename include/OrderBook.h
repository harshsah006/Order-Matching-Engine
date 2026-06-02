#pragma once
#include "Order.h"
#include <map>
#include <list>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <memory_resource>

class OrderBook {
public:
    OrderBook() = default;

    // Add a new order to the book and attempt to match
    void addOrder(const Order& order);

    // Cancel an order (simplified for now, usually requires an ID map)
    void cancelOrder(uint64_t orderId);

    // Print the current state of the order book
    void printBook() const;

private:
    std::pmr::unsynchronized_pool_resource memPool;

    struct OrderLocation {
        bool isBid;
        uint32_t price;
        std::pmr::list<Order>::iterator orderIt;
    };

    // Bids (Buy orders) - Highest price first
    std::pmr::map<uint32_t, std::pmr::list<Order>, std::greater<uint32_t>> bids{&memPool};

    // Asks (Sell orders) - Lowest price first
    std::pmr::map<uint32_t, std::pmr::list<Order>, std::less<uint32_t>> asks{&memPool};

    std::pmr::unordered_map<uint64_t, OrderLocation> orderMap{&memPool};

    // Optional: Mutex for thread safety if accessed concurrently
    // std::mutex bookMutex;

    // Internal matching logic
    void match();
};
