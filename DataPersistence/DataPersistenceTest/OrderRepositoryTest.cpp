#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <fstream>
#include "JsonOrderRepository.h"

namespace fs = std::filesystem;

class JsonOrderRepositoryTest : public ::testing::Test {
protected:
    static constexpr const char* TEST_FILE = "test_orders_tmp.json";

    void SetUp() override {
        repo = std::make_unique<JsonOrderRepository>(TEST_FILE);
    }

    void TearDown() override {
        fs::remove(TEST_FILE);
    }

    std::unique_ptr<JsonOrderRepository> repo;

    static Order make(const std::string& id,
                      const std::string& sampleId = "S-001",
                      const std::string& customerName = "ACME Corp",
                      int quantity = 100,
                      OrderStatus status = OrderStatus::RESERVED) {
        Order o;
        o.id           = id;
        o.sampleId     = sampleId;
        o.customerName = customerName;
        o.quantity     = quantity;
        o.status       = status;
        return o;
    }
};

// Create

TEST_F(JsonOrderRepositoryTest, SaveStoresNewOrder) {
    repo->save(make("ORD-20260612-0001"));
    EXPECT_TRUE(repo->existsById("ORD-20260612-0001"));
}

TEST_F(JsonOrderRepositoryTest, SavePersistsAcrossInstances) {
    repo->save(make("ORD-20260612-0001", "S-002", "Samsung Foundry"));

    JsonOrderRepository repo2(TEST_FILE);
    auto result = repo2.findById("ORD-20260612-0001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->customerName, "Samsung Foundry");
    EXPECT_EQ(result->sampleId, "S-002");
}

// Read

TEST_F(JsonOrderRepositoryTest, FindAllReturnsEmptyWhenNoData) {
    EXPECT_TRUE(repo->findAll().empty());
}

TEST_F(JsonOrderRepositoryTest, FindAllReturnsAllSavedOrders) {
    repo->save(make("ORD-20260612-0001"));
    repo->save(make("ORD-20260612-0002"));
    EXPECT_EQ(repo->findAll().size(), 2u);
}

TEST_F(JsonOrderRepositoryTest, FindByIdReturnsCorrectOrder) {
    repo->save(make("ORD-20260612-0001", "S-001", "Client A"));
    repo->save(make("ORD-20260612-0002", "S-002", "Client B"));

    auto result = repo->findById("ORD-20260612-0002");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->customerName, "Client B");
}

TEST_F(JsonOrderRepositoryTest, FindByIdReturnsNulloptWhenNotFound) {
    EXPECT_FALSE(repo->findById("NOT_EXIST").has_value());
}

// Update

TEST_F(JsonOrderRepositoryTest, UpdateChangesExistingOrder) {
    repo->save(make("ORD-20260612-0001"));

    Order updated = make("ORD-20260612-0001");
    updated.status                   = OrderStatus::PRODUCING;
    updated.actualProductionQuantity = 206;
    updated.productionStartTime      = "2026-06-12T10:00:00";
    updated.totalProductionTime      = 165.0;

    EXPECT_TRUE(repo->update(updated));

    auto result = repo->findById("ORD-20260612-0001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->status,                   OrderStatus::PRODUCING);
    EXPECT_EQ(result->actualProductionQuantity, 206);
    EXPECT_EQ(result->productionStartTime,      "2026-06-12T10:00:00");
    EXPECT_DOUBLE_EQ(result->totalProductionTime, 165.0);
}

TEST_F(JsonOrderRepositoryTest, UpdateReturnsFalseWhenNotFound) {
    EXPECT_FALSE(repo->update(make("NOT_EXIST")));
}

// Delete

TEST_F(JsonOrderRepositoryTest, RemoveDeletesExistingOrder) {
    repo->save(make("ORD-20260612-0001"));
    EXPECT_TRUE(repo->remove("ORD-20260612-0001"));
    EXPECT_FALSE(repo->existsById("ORD-20260612-0001"));
}

TEST_F(JsonOrderRepositoryTest, RemoveReturnsFalseWhenNotFound) {
    EXPECT_FALSE(repo->remove("NOT_EXIST"));
}

TEST_F(JsonOrderRepositoryTest, RemoveDoesNotAffectOtherOrders) {
    repo->save(make("ORD-20260612-0001"));
    repo->save(make("ORD-20260612-0002"));
    repo->remove("ORD-20260612-0001");

    EXPECT_TRUE(repo->existsById("ORD-20260612-0002"));
    EXPECT_EQ(repo->findAll().size(), 1u);
}

// ExistsById

TEST_F(JsonOrderRepositoryTest, ExistsByIdReturnsTrueForSavedId) {
    repo->save(make("ORD-20260612-0001"));
    EXPECT_TRUE(repo->existsById("ORD-20260612-0001"));
}

TEST_F(JsonOrderRepositoryTest, ExistsByIdReturnsFalseForMissingId) {
    EXPECT_FALSE(repo->existsById("ORD-20260612-0001"));
}

// Status persistence

// Negative / edge-case

TEST_F(JsonOrderRepositoryTest, LoadFromCorruptedFile_ReturnsEmpty) {
    { std::ofstream f(TEST_FILE); f << "[ bad json !!!"; }
    JsonOrderRepository repo2(TEST_FILE);
    EXPECT_TRUE(repo2.findAll().empty());
}

TEST_F(JsonOrderRepositoryTest, LoadWithUnknownStatus_DefaultsToReserved) {
    {
        std::ofstream f(TEST_FILE);
        f << R"([{"id":"ORD-1","sampleId":"S-1","customerName":"X","quantity":1,)"
          << R"("status":"UNKNOWN","actualProductionQuantity":0,)"
          << R"("productionStartTime":"","totalProductionTime":0.0}])";
    }
    JsonOrderRepository repo2(TEST_FILE);
    auto all = repo2.findAll();
    ASSERT_EQ(all.size(), 1u);
    EXPECT_EQ(all[0].status, OrderStatus::RESERVED);
}

// Status persistence

TEST_F(JsonOrderRepositoryTest, StatusRoundTripPersistsAllValues) {
    const std::vector<OrderStatus> statuses = {
        OrderStatus::RESERVED, OrderStatus::REJECTED,
        OrderStatus::PRODUCING, OrderStatus::CONFIRMED, OrderStatus::RELEASE
    };

    for (int i = 0; i < static_cast<int>(statuses.size()); ++i) {
        std::string id = "ORD-20260612-000" + std::to_string(i + 1);
        repo->save(make(id, "S-001", "Client", 10, statuses[i]));
    }

    JsonOrderRepository repo2(TEST_FILE);
    auto all = repo2.findAll();
    ASSERT_EQ(all.size(), statuses.size());
    for (int i = 0; i < static_cast<int>(statuses.size()); ++i) {
        EXPECT_EQ(all[i].status, statuses[i]);
    }
}
