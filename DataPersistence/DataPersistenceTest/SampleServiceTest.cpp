#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "SampleService.h"

using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;
using ::testing::SaveArg;

class MockSampleRepository : public SampleRepository {
public:
    MOCK_METHOD(void, save, (const Sample&), (override));
    MOCK_METHOD(std::vector<Sample>, findAll, (), (const, override));
    MOCK_METHOD(std::optional<Sample>, findById, (const std::string&), (const, override));
    MOCK_METHOD(bool, update, (const Sample&), (override));
    MOCK_METHOD(bool, remove, (const std::string&), (override));
    MOCK_METHOD(bool, existsById, (const std::string&), (const, override));
};

static Sample makeSample(const std::string& id = "S-001", int stock = 0) {
    return {id, "Test Wafer", 1.0, 0.9, stock};
}

TEST(SampleServiceTest, RegisterSample_NewId_SavesAndReturnsTrue) {
    MockSampleRepository mockRepo;
    SampleService svc(mockRepo);

    EXPECT_CALL(mockRepo, existsById("S-001")).WillOnce(Return(false));
    EXPECT_CALL(mockRepo, save(_)).Times(1);

    EXPECT_TRUE(svc.registerSample(makeSample("S-001")));
}

TEST(SampleServiceTest, RegisterSample_DuplicateId_ReturnsFalseWithoutSave) {
    MockSampleRepository mockRepo;
    SampleService svc(mockRepo);

    EXPECT_CALL(mockRepo, existsById("S-001")).WillOnce(Return(true));
    EXPECT_CALL(mockRepo, save(_)).Times(0);

    EXPECT_FALSE(svc.registerSample(makeSample("S-001")));
}

TEST(SampleServiceTest, UpdateStock_NegativeValue_ReturnsFalseWithoutQuery) {
    MockSampleRepository mockRepo;
    SampleService svc(mockRepo);

    EXPECT_CALL(mockRepo, findById(_)).Times(0);
    EXPECT_CALL(mockRepo, update(_)).Times(0);

    EXPECT_FALSE(svc.updateStock("S-001", -1));
}

TEST(SampleServiceTest, UpdateStock_ValidValue_UpdatesStockFieldOnly) {
    MockSampleRepository mockRepo;
    SampleService svc(mockRepo);

    Sample existing = makeSample("S-001", 50);
    EXPECT_CALL(mockRepo, findById("S-001")).WillOnce(Return(existing));

    Sample captured;
    EXPECT_CALL(mockRepo, update(_)).WillOnce(DoAll(SaveArg<0>(&captured), Return(true)));

    EXPECT_TRUE(svc.updateStock("S-001", 200));
    EXPECT_EQ(captured.stock, 200);
    EXPECT_EQ(captured.id,    "S-001");
}

TEST(SampleServiceTest, UpdateStock_IdNotFound_ReturnsFalse) {
    MockSampleRepository mockRepo;
    SampleService svc(mockRepo);

    EXPECT_CALL(mockRepo, findById("NOT_EXIST")).WillOnce(Return(std::nullopt));
    EXPECT_CALL(mockRepo, update(_)).Times(0);

    EXPECT_FALSE(svc.updateStock("NOT_EXIST", 100));
}

TEST(SampleServiceTest, UpdateSample_NegativeStock_ReturnsFalseWithoutUpdate) {
    MockSampleRepository mockRepo;
    SampleService svc(mockRepo);

    EXPECT_CALL(mockRepo, update(_)).Times(0);

    Sample s = makeSample("S-001", -5);
    EXPECT_FALSE(svc.updateSample(s));
}
