#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include "JsonSampleRepository.h"

namespace fs = std::filesystem;

class JsonSampleRepositoryTest : public ::testing::Test {
protected:
    static constexpr const char* TEST_FILE = "test_samples_tmp.json";

    void SetUp() override {
        repo = std::make_unique<JsonSampleRepository>(TEST_FILE);
    }

    void TearDown() override {
        fs::remove(TEST_FILE);
    }

    std::unique_ptr<JsonSampleRepository> repo;

    static Sample make(const std::string& id,
                       const std::string& name = "테스트 시료",
                       double avgProdTime = 1.0,
                       double yield = 0.9,
                       int stock = 0) {
        return {id, name, avgProdTime, yield, stock};
    }
};

// ── Create ──────────────────────────────────────────────────────────────────

TEST_F(JsonSampleRepositoryTest, SaveStoresNewSample) {
    repo->save(make("S-001"));
    EXPECT_TRUE(repo->existsById("S-001"));
}

TEST_F(JsonSampleRepositoryTest, SavePersistsAcrossInstances) {
    repo->save(make("S-001", "실리콘 웨이퍼"));

    JsonSampleRepository repo2(TEST_FILE);
    auto result = repo2.findById("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->name, "실리콘 웨이퍼");
}

// ── Read ─────────────────────────────────────────────────────────────────────

TEST_F(JsonSampleRepositoryTest, FindAllReturnsEmptyWhenNoData) {
    EXPECT_TRUE(repo->findAll().empty());
}

TEST_F(JsonSampleRepositoryTest, FindAllReturnsAllSavedSamples) {
    repo->save(make("S-001"));
    repo->save(make("S-002"));
    EXPECT_EQ(repo->findAll().size(), 2u);
}

TEST_F(JsonSampleRepositoryTest, FindByIdReturnsCorrectSample) {
    repo->save(make("S-001", "웨이퍼A"));
    repo->save(make("S-002", "웨이퍼B"));

    auto result = repo->findById("S-002");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->name, "웨이퍼B");
}

TEST_F(JsonSampleRepositoryTest, FindByIdReturnsNulloptWhenNotFound) {
    EXPECT_FALSE(repo->findById("NOT_EXIST").has_value());
}

// ── Update ────────────────────────────────────────────────────────────────────

TEST_F(JsonSampleRepositoryTest, UpdateChangesExistingSample) {
    repo->save(make("S-001", "원본"));

    Sample updated = make("S-001", "수정됨");
    updated.stock = 100;
    EXPECT_TRUE(repo->update(updated));

    auto result = repo->findById("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->name, "수정됨");
    EXPECT_EQ(result->stock, 100);
}

TEST_F(JsonSampleRepositoryTest, UpdateReturnsFalseWhenNotFound) {
    EXPECT_FALSE(repo->update(make("NOT_EXIST")));
}

// ── Delete ────────────────────────────────────────────────────────────────────

TEST_F(JsonSampleRepositoryTest, RemoveDeletesExistingSample) {
    repo->save(make("S-001"));
    EXPECT_TRUE(repo->remove("S-001"));
    EXPECT_FALSE(repo->existsById("S-001"));
}

TEST_F(JsonSampleRepositoryTest, RemoveReturnsFalseWhenNotFound) {
    EXPECT_FALSE(repo->remove("NOT_EXIST"));
}

TEST_F(JsonSampleRepositoryTest, RemoveDoesNotAffectOtherSamples) {
    repo->save(make("S-001"));
    repo->save(make("S-002"));
    repo->remove("S-001");

    EXPECT_TRUE(repo->existsById("S-002"));
    EXPECT_EQ(repo->findAll().size(), 1u);
}

// ── ExistsById ────────────────────────────────────────────────────────────────

TEST_F(JsonSampleRepositoryTest, ExistsByIdReturnsTrueForSavedId) {
    repo->save(make("S-001"));
    EXPECT_TRUE(repo->existsById("S-001"));
}

TEST_F(JsonSampleRepositoryTest, ExistsByIdReturnsFalseForMissingId) {
    EXPECT_FALSE(repo->existsById("S-001"));
}
