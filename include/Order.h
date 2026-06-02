#pragma once
#include <cstdint>
#include <iostream>

enum class OrderType {
    Buy,
    Sell
};

struct Order {
    uint64_t orderId;
    OrderType type;
    uint32_t price;
    uint32_t quantity;
    uint64_t timestamp; // can represent arrival time or sequence number

    Order(uint64_t id, OrderType t, uint32_t p, uint32_t q, uint64_t ts)
        : orderId(id), type(t), price(p), quantity(q), timestamp(ts) {}
};
