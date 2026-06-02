#include "OrderBook.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

void OrderBook::addOrder(const Order& order) {
    if (order.type == OrderType::Buy) {
        auto& list = bids[order.price];
        list.push_back(order);
        orderMap[order.orderId] = {true, order.price, std::prev(list.end())};
    } else {
        auto& list = asks[order.price];
        list.push_back(order);
        orderMap[order.orderId] = {false, order.price, std::prev(list.end())};
    }
    
    // Attempt to match after every order
    match();
}

void OrderBook::cancelOrder(uint64_t orderId) {
    auto it = orderMap.find(orderId);
    if (it == orderMap.end()) return;

    OrderLocation loc = it->second;
    if (loc.isBid) {
        auto& list = bids[loc.price];
        list.erase(loc.orderIt);
        if (list.empty()) {
            bids.erase(loc.price);
        }
    } else {
        auto& list = asks[loc.price];
        list.erase(loc.orderIt);
        if (list.empty()) {
            asks.erase(loc.price);
        }
    }
    orderMap.erase(it);
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
                orderMap.erase(bidOrder.orderId);
                bestBid->second.pop_front();
                if (bestBid->second.empty()) {
                    bids.erase(bestBid);
                }
            }
            if (askOrder.quantity == 0) {
                orderMap.erase(askOrder.orderId);
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
