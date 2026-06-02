#pragma once
#include "Order.h"
#include <map>
#include <list>
#include <mutex>
#include <vector>
#include <unordered_map>

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
    struct OrderLocation {
        bool isBid;
        double price;
        std::list<Order>::iterator orderIt;
    };

    // Bids (Buy orders) - Highest price first
    std::map<double, std::list<Order>, std::greater<double>> bids;

    // Asks (Sell orders) - Lowest price first
    std::map<double, std::list<Order>, std::less<double>> asks;

    std::unordered_map<uint64_t, OrderLocation> orderMap;

    // Optional: Mutex for thread safety if accessed concurrently
    // std::mutex bookMutex;

    // Internal matching logic
    void match();
};
