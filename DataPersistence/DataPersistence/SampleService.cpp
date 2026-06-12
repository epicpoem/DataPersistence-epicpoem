#include "SampleService.h"

SampleService::SampleService(SampleRepository& repo) : repo_(repo) {}

bool SampleService::registerSample(const Sample& sample) {
    if (sample.id.empty()) return false;
    if (repo_.existsById(sample.id)) return false;
    repo_.save(sample);
    return true;
}

std::vector<Sample> SampleService::listAll() const {
    return repo_.findAll();
}

std::optional<Sample> SampleService::findById(const std::string& id) const {
    return repo_.findById(id);
}

bool SampleService::updateSample(const Sample& sample) {
    if (sample.stock < 0) return false;
    return repo_.update(sample);
}

bool SampleService::removeSample(const std::string& id) {
    return repo_.remove(id);
}

bool SampleService::updateStock(const std::string& id, int newStock) {
    if (newStock < 0) return false;
    auto existing = repo_.findById(id);
    if (!existing) return false;
    existing->stock = newStock;
    return repo_.update(*existing);
}

bool SampleService::existsById(const std::string& id) const {
    return repo_.existsById(id);
}
