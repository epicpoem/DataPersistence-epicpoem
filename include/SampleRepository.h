#pragma once
#include <optional>
#include <string>
#include <vector>
#include "Sample.h"

class SampleRepository {
public:
    virtual ~SampleRepository() = default;
    virtual void save(const Sample& sample) = 0;
    virtual std::vector<Sample> findAll() const = 0;
    virtual std::optional<Sample> findById(const std::string& id) const = 0;
    virtual bool update(const Sample& sample) = 0;
    virtual bool remove(const std::string& id) = 0;
    virtual bool existsById(const std::string& id) const = 0;
};
