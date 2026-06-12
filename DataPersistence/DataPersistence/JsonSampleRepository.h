#pragma once
#include "SampleRepository.h"
#include <string>

class JsonSampleRepository : public SampleRepository {
public:
    explicit JsonSampleRepository(const std::string& filePath);

    void save(const Sample& sample) override;
    std::vector<Sample> findAll() const override;
    std::optional<Sample> findById(const std::string& id) const override;
    bool update(const Sample& sample) override;
    bool remove(const std::string& id) override;
    bool existsById(const std::string& id) const override;

private:
    std::string filePath_;

    std::vector<Sample> loadFromFile() const;
    void saveToFile(const std::vector<Sample>& samples) const;
};
