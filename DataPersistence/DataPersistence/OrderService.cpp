#include "OrderService.h"
#include <cstdio>

OrderService::OrderService(OrderRepository& orderRepo, SampleRepository& sampleRepo,
                           const IClock& clock)
    : orderRepo_(orderRepo), sampleRepo_(sampleRepo), clock_(clock) {}

std::optional<Order> OrderService::placeOrder(const std::string& sampleId,
                                               const std::string& customerName,
                                               int quantity) {
    if (!sampleRepo_.existsById(sampleId)) return std::nullopt;
    if (customerName.empty() || quantity < 1) return std::nullopt;

    Order o;
    o.id           = generateNextId();
    o.sampleId     = sampleId;
    o.customerName = customerName;
    o.quantity     = quantity;
    o.status       = OrderStatus::RESERVED;
    orderRepo_.save(o);
    return o;
}

std::vector<Order> OrderService::listAll() const {
    return orderRepo_.findAll();
}

std::optional<Order> OrderService::findById(const std::string& id) const {
    return orderRepo_.findById(id);
}

bool OrderService::transitionStatus(const std::string& id, OrderStatus newStatus,
                                     int actualQty, double totalTime) {
    auto existing = orderRepo_.findById(id);
    if (!existing) return false;

    existing->status = newStatus;
    if (newStatus == OrderStatus::PRODUCING) {
        existing->productionStartTime      = clock_.now();
        existing->actualProductionQuantity = actualQty;
        existing->totalProductionTime      = totalTime;
    }
    return orderRepo_.update(*existing);
}

bool OrderService::removeOrder(const std::string& id) {
    return orderRepo_.remove(id);
}

bool OrderService::existsById(const std::string& id) const {
    return orderRepo_.existsById(id);
}

std::string OrderService::generateNextId() const {
    std::string prefix = "ORD-" + clock_.today() + "-";
    int maxSeq = 0;
    for (const auto& o : orderRepo_.findAll()) {
        if (o.id.rfind(prefix, 0) == 0) {
            try {
                int seq = std::stoi(o.id.substr(prefix.size()));
                if (seq > maxSeq) maxSeq = seq;
            } catch (...) {}
        }
    }
    char seq[5];
    std::snprintf(seq, sizeof(seq), "%04d", maxSeq + 1);
    return prefix + seq;
}
