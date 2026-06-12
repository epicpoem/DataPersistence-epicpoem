#pragma once
#include <optional>
#include <string>
#include <vector>
#include "IClock.h"
#include "Order.h"
#include "OrderRepository.h"
#include "SampleRepository.h"

class OrderService {
public:
    OrderService(OrderRepository& orderRepo, SampleRepository& sampleRepo, const IClock& clock);

    std::optional<Order> placeOrder(const std::string& sampleId,
                                    const std::string& customerName,
                                    int quantity);
    std::vector<Order> listAll() const;
    std::optional<Order> findById(const std::string& id) const;
    bool transitionStatus(const std::string& id, OrderStatus newStatus,
                          int actualQty = 0, double totalTime = 0.0);
    bool removeOrder(const std::string& id);
    bool existsById(const std::string& id) const;

private:
    OrderRepository&  orderRepo_;
    SampleRepository& sampleRepo_;
    const IClock&     clock_;

    std::string generateNextId() const;
};
