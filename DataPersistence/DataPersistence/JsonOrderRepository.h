#pragma once
#include "OrderRepository.h"
#include <string>

class JsonOrderRepository : public OrderRepository {
public:
    explicit JsonOrderRepository(const std::string& filePath);

    void save(const Order& order) override;
    std::vector<Order> findAll() const override;
    std::optional<Order> findById(const std::string& id) const override;
    bool update(const Order& order) override;
    bool remove(const std::string& id) override;
    bool existsById(const std::string& id) const override;

private:
    std::string filePath_;
    std::vector<Order> loadFromFile() const;
    void saveToFile(const std::vector<Order>& orders) const;
};
