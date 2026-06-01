#include "OrderBook.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

void OrderBook::addOrder(const Order& order) {
    if (order.type == OrderType::Buy) {
        bids[order.price].push_back(order);
    } else {
        asks[order.price].push_back(order);
    }
    
    // Attempt to match after every order
    match();
}

void OrderBook::cancelOrder(uint64_t orderId) {
    // In a real system, you would maintain an unordered_map<uint64_t, std::list<Order>::iterator>
    // for O(1) cancellations. For simplicity, we just iterate.
    for (auto& [price, orderList] : bids) {
        for (auto it = orderList.begin(); it != orderList.end(); ++it) {
            if (it->orderId == orderId) {
                orderList.erase(it);
                if (orderList.empty()) {
                    bids.erase(price);
                }
                return;
            }
        }
    }
    
    for (auto& [price, orderList] : asks) {
        for (auto it = orderList.begin(); it != orderList.end(); ++it) {
            if (it->orderId == orderId) {
                orderList.erase(it);
                if (orderList.empty()) {
                    asks.erase(price);
                }
                return;
            }
        }
    }
}

void OrderBook::match() {
    while (!bids.empty() && !asks.empty()) {
        auto bestBid = bids.begin();
        auto bestAsk = asks.begin();

        if (bestBid->first >= bestAsk->first) {
            // Match is possible
            auto& bidOrder = bestBid->second.front();
            auto& askOrder = bestAsk->second.front();

            uint32_t tradeQuantity = std::min(bidOrder.quantity, askOrder.quantity);
            double tradePrice = askOrder.price; // Usually, maker's price is used

            // Uncomment the below line for debugging small numbers of orders
            // std::cout << "TRADE: " << tradeQuantity << " @ $" << tradePrice 
            //          << " (Bid ID: " << bidOrder.orderId << ", Ask ID: " << askOrder.orderId << ")\n";

            bidOrder.quantity -= tradeQuantity;
            askOrder.quantity -= tradeQuantity;

            // Remove fulfilled orders
            if (bidOrder.quantity == 0) {
                bestBid->second.pop_front();
                if (bestBid->second.empty()) {
                    bids.erase(bestBid);
                }
            }
            if (askOrder.quantity == 0) {
                bestAsk->second.pop_front();
                if (bestAsk->second.empty()) {
                    asks.erase(bestAsk);
                }
            }
        } else {
            // No match possible
            break;
        }
    }
}

void OrderBook::printBook() const {
    std::cout << "--- Order Book ---\n";
    std::cout << "ASKS:\n";
    for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
        uint32_t totalVolume = 0;
        for (const auto& order : it->second) totalVolume += order.quantity;
        std::cout << "  $" << std::fixed << std::setprecision(2) << it->first << " | Vol: " << totalVolume << "\n";
    }
    
    std::cout << "BIDS:\n";
    for (const auto& [price, orderList] : bids) {
        uint32_t totalVolume = 0;
        for (const auto& order : orderList) totalVolume += order.quantity;
        std::cout << "  $" << std::fixed << std::setprecision(2) << price << " | Vol: " << totalVolume << "\n";
    }
    std::cout << "------------------\n";
}
