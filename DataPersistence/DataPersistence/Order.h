#pragma once
#include <string>

enum class OrderStatus {
    RESERVED,
    REJECTED,
    PRODUCING,
    CONFIRMED,
    RELEASE
};

inline std::string statusToString(OrderStatus s) {
    switch (s) {
        case OrderStatus::RESERVED:  return "RESERVED";
        case OrderStatus::REJECTED:  return "REJECTED";
        case OrderStatus::PRODUCING: return "PRODUCING";
        case OrderStatus::CONFIRMED: return "CONFIRMED";
        case OrderStatus::RELEASE:   return "RELEASE";
    }
    return "RESERVED";
}

inline OrderStatus statusFromString(const std::string& s) {
    if (s == "REJECTED")  return OrderStatus::REJECTED;
    if (s == "PRODUCING") return OrderStatus::PRODUCING;
    if (s == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (s == "RELEASE")   return OrderStatus::RELEASE;
    return OrderStatus::RESERVED;
}

inline bool isValidStatusString(const std::string& s) {
    return s == "RESERVED" || s == "REJECTED" || s == "PRODUCING" ||
           s == "CONFIRMED" || s == "RELEASE";
}

struct Order {
    std::string id;
    std::string sampleId;
    std::string customerName;
    int quantity = 0;
    OrderStatus status = OrderStatus::RESERVED;
    int actualProductionQuantity = 0;
    std::string productionStartTime;
    double totalProductionTime = 0.0;
};
