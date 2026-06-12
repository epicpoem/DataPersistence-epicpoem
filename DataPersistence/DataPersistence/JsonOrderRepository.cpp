#include "JsonOrderRepository.h"
#include "nlohmann/json.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>

using json = nlohmann::json;
namespace fs = std::filesystem;

JsonOrderRepository::JsonOrderRepository(const std::string& filePath)
    : filePath_(filePath) {
    fs::path p(filePath_);
    if (p.has_parent_path()) {
        fs::create_directories(p.parent_path());
    }
}

std::vector<Order> JsonOrderRepository::loadFromFile() const {
    std::vector<Order> orders;
    if (!fs::exists(filePath_)) return orders;

    std::ifstream file(filePath_);
    if (!file.is_open()) return orders;

    try {
        json j;
        file >> j;
        for (const auto& item : j) {
            Order o;
            o.id                      = item.at("id").get<std::string>();
            o.sampleId                = item.at("sampleId").get<std::string>();
            o.customerName            = item.at("customerName").get<std::string>();
            o.quantity                = item.at("quantity").get<int>();
            o.status                  = statusFromString(item.at("status").get<std::string>());
            o.actualProductionQuantity = item.at("actualProductionQuantity").get<int>();
            o.productionStartTime     = item.at("productionStartTime").get<std::string>();
            o.totalProductionTime     = item.at("totalProductionTime").get<double>();
            orders.push_back(o);
        }
    } catch (...) {}

    return orders;
}

void JsonOrderRepository::saveToFile(const std::vector<Order>& orders) const {
    json j = json::array();
    for (const auto& o : orders) {
        j.push_back({
            {"id",                       o.id},
            {"sampleId",                 o.sampleId},
            {"customerName",             o.customerName},
            {"quantity",                 o.quantity},
            {"status",                   statusToString(o.status)},
            {"actualProductionQuantity", o.actualProductionQuantity},
            {"productionStartTime",      o.productionStartTime},
            {"totalProductionTime",      o.totalProductionTime}
        });
    }
    std::ofstream file(filePath_);
    file << j.dump(2);
}

void JsonOrderRepository::save(const Order& order) {
    auto orders = loadFromFile();
    orders.push_back(order);
    saveToFile(orders);
}

std::vector<Order> JsonOrderRepository::findAll() const {
    return loadFromFile();
}

std::optional<Order> JsonOrderRepository::findById(const std::string& id) const {
    auto orders = loadFromFile();
    auto it = std::find_if(orders.begin(), orders.end(),
        [&id](const Order& o) { return o.id == id; });
    if (it != orders.end()) return *it;
    return std::nullopt;
}

bool JsonOrderRepository::update(const Order& order) {
    auto orders = loadFromFile();
    auto it = std::find_if(orders.begin(), orders.end(),
        [&order](const Order& o) { return o.id == order.id; });
    if (it == orders.end()) return false;
    *it = order;
    saveToFile(orders);
    return true;
}

bool JsonOrderRepository::remove(const std::string& id) {
    auto orders = loadFromFile();
    auto it = std::find_if(orders.begin(), orders.end(),
        [&id](const Order& o) { return o.id == id; });
    if (it == orders.end()) return false;
    orders.erase(it);
    saveToFile(orders);
    return true;
}

bool JsonOrderRepository::existsById(const std::string& id) const {
    return findById(id).has_value();
}
