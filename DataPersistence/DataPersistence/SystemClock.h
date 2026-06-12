#pragma once
#include "IClock.h"
#include <ctime>

class SystemClock : public IClock {
public:
    std::string today() const override {
        std::time_t t = std::time(nullptr);
        std::tm tm{};
        localtime_s(&tm, &t);
        char buf[9];
        std::strftime(buf, sizeof(buf), "%Y%m%d", &tm);
        return buf;
    }

    std::string now() const override {
        std::time_t t = std::time(nullptr);
        std::tm tm{};
        localtime_s(&tm, &t);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
        return buf;
    }
};
