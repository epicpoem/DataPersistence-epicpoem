#pragma once
#include <optional>
#include <string>
#include <vector>
#include "Order.h"

class OrderRepository {
public:
    virtual ~OrderRepository() = default;
    virtual void save(const Order& order) = 0;
    virtual std::vector<Order> findAll() const = 0;
    virtual std::optional<Order> findById(const std::string& id) const = 0;
    virtual bool update(const Order& order) = 0;
    virtual bool remove(const std::string& id) = 0;
    virtual bool existsById(const std::string& id) const = 0;
};
