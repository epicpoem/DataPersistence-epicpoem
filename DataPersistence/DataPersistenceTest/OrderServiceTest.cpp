#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "OrderService.h"

using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;
using ::testing::SaveArg;

// ── Fakes & Mocks ──────────────────────────────────────────────────────────

class FakeClock : public IClock {
public:
    FakeClock(std::string t, std::string n) : today_(std::move(t)), now_(std::move(n)) {}
    std::string today() const override { return today_; }
    std::string now()   const override { return now_;   }
private:
    std::string today_, now_;
};

class MockOrderRepository : public OrderRepository {
public:
    MOCK_METHOD(void, save, (const Order&), (override));
    MOCK_METHOD(std::vector<Order>, findAll, (), (const, override));
    MOCK_METHOD(std::optional<Order>, findById, (const std::string&), (const, override));
    MOCK_METHOD(bool, update, (const Order&), (override));
    MOCK_METHOD(bool, remove, (const std::string&), (override));
    MOCK_METHOD(bool, existsById, (const std::string&), (const, override));
};

class MockSampleRepository : public SampleRepository {
public:
    MOCK_METHOD(void, save, (const Sample&), (override));
    MOCK_METHOD(std::vector<Sample>, findAll, (), (const, override));
    MOCK_METHOD(std::optional<Sample>, findById, (const std::string&), (const, override));
    MOCK_METHOD(bool, update, (const Sample&), (override));
    MOCK_METHOD(bool, remove, (const std::string&), (override));
    MOCK_METHOD(bool, existsById, (const std::string&), (const, override));
};

// ── Tests ──────────────────────────────────────────────────────────────────

TEST(OrderServiceTest, PlaceOrder_ValidSample_ReturnsReservedOrder) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    EXPECT_CALL(mockSampleRepo, existsById("S-001")).WillOnce(Return(true));
    EXPECT_CALL(mockOrderRepo,  findAll()).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(mockOrderRepo,  save(_)).Times(1);

    auto result = svc.placeOrder("S-001", "ACME Corp", 100);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->sampleId,     "S-001");
    EXPECT_EQ(result->customerName, "ACME Corp");
    EXPECT_EQ(result->quantity,     100);
    EXPECT_EQ(result->status,       OrderStatus::RESERVED);
}

TEST(OrderServiceTest, PlaceOrder_InvalidSample_ReturnsNullopt) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    EXPECT_CALL(mockSampleRepo, existsById("S-NONE")).WillOnce(Return(false));
    EXPECT_CALL(mockOrderRepo,  save(_)).Times(0);

    EXPECT_FALSE(svc.placeOrder("S-NONE", "ACME Corp", 100).has_value());
}

TEST(OrderServiceTest, PlaceOrder_GeneratesIdWithTodayDate) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    EXPECT_CALL(mockSampleRepo, existsById(_)).WillOnce(Return(true));
    EXPECT_CALL(mockOrderRepo,  findAll()).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(mockOrderRepo,  save(_)).Times(1);

    auto result = svc.placeOrder("S-001", "ACME Corp", 50);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id, "ORD-20260612-0001");
}

TEST(OrderServiceTest, PlaceOrder_SequentialIdWhenOrdersExist) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    Order prior;
    prior.id = "ORD-20260612-0003";
    EXPECT_CALL(mockSampleRepo, existsById(_)).WillOnce(Return(true));
    EXPECT_CALL(mockOrderRepo,  findAll()).WillOnce(Return(std::vector<Order>{prior}));
    EXPECT_CALL(mockOrderRepo,  save(_)).Times(1);

    auto result = svc.placeOrder("S-001", "ACME Corp", 50);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id, "ORD-20260612-0004");
}

TEST(OrderServiceTest, TransitionToProducing_SetsProductionFieldsFromClock) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    Order existing;
    existing.id     = "ORD-20260612-0001";
    existing.status = OrderStatus::RESERVED;
    EXPECT_CALL(mockOrderRepo, findById("ORD-20260612-0001")).WillOnce(Return(existing));

    Order captured;
    EXPECT_CALL(mockOrderRepo, update(_)).WillOnce(DoAll(SaveArg<0>(&captured), Return(true)));

    EXPECT_TRUE(svc.transitionStatus("ORD-20260612-0001", OrderStatus::PRODUCING, 206, 165.0));
    EXPECT_EQ(captured.status,                   OrderStatus::PRODUCING);
    EXPECT_EQ(captured.productionStartTime,      "2026-06-12T10:00:00");
    EXPECT_EQ(captured.actualProductionQuantity, 206);
    EXPECT_DOUBLE_EQ(captured.totalProductionTime, 165.0);
}

TEST(OrderServiceTest, TransitionToNonProducing_DoesNotChangeProductionFields) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    Order existing;
    existing.id                   = "ORD-20260612-0001";
    existing.status               = OrderStatus::RESERVED;
    existing.productionStartTime  = "";
    EXPECT_CALL(mockOrderRepo, findById("ORD-20260612-0001")).WillOnce(Return(existing));

    Order captured;
    EXPECT_CALL(mockOrderRepo, update(_)).WillOnce(DoAll(SaveArg<0>(&captured), Return(true)));

    EXPECT_TRUE(svc.transitionStatus("ORD-20260612-0001", OrderStatus::REJECTED));
    EXPECT_EQ(captured.status,              OrderStatus::REJECTED);
    EXPECT_EQ(captured.productionStartTime, "");
}

TEST(OrderServiceTest, TransitionStatus_InvalidId_ReturnsFalse) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    EXPECT_CALL(mockOrderRepo, findById("NOT_EXIST")).WillOnce(Return(std::nullopt));
    EXPECT_CALL(mockOrderRepo, update(_)).Times(0);

    EXPECT_FALSE(svc.transitionStatus("NOT_EXIST", OrderStatus::REJECTED));
}

// Negative / edge-case

TEST(OrderServiceTest, PlaceOrder_EmptyCustomerName_ReturnsNullopt) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    EXPECT_CALL(mockSampleRepo, existsById("S-001")).WillOnce(Return(true));
    EXPECT_CALL(mockOrderRepo,  save(_)).Times(0);

    EXPECT_FALSE(svc.placeOrder("S-001", "", 100).has_value());
}

TEST(OrderServiceTest, PlaceOrder_ZeroQuantity_ReturnsNullopt) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    EXPECT_CALL(mockSampleRepo, existsById("S-001")).WillOnce(Return(true));
    EXPECT_CALL(mockOrderRepo,  save(_)).Times(0);

    EXPECT_FALSE(svc.placeOrder("S-001", "ACME", 0).has_value());
}

TEST(OrderServiceTest, PlaceOrder_NegativeQuantity_ReturnsNullopt) {
    MockOrderRepository  mockOrderRepo;
    MockSampleRepository mockSampleRepo;
    FakeClock clock("20260612", "2026-06-12T10:00:00");
    OrderService svc(mockOrderRepo, mockSampleRepo, clock);

    EXPECT_CALL(mockSampleRepo, existsById("S-001")).WillOnce(Return(true));
    EXPECT_CALL(mockOrderRepo,  save(_)).Times(0);

    EXPECT_FALSE(svc.placeOrder("S-001", "ACME", -5).has_value());
}
