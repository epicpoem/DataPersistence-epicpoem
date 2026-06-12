#pragma once
#include <optional>
#include <string>
#include <vector>
#include "Sample.h"
#include "SampleRepository.h"

class SampleService {
public:
    explicit SampleService(SampleRepository& repo);

    bool registerSample(const Sample& sample);
    std::vector<Sample> listAll() const;
    std::optional<Sample> findById(const std::string& id) const;
    bool updateSample(const Sample& sample);
    bool removeSample(const std::string& id);
    bool updateStock(const std::string& id, int newStock);
    bool existsById(const std::string& id) const;

private:
    SampleRepository& repo_;
};
