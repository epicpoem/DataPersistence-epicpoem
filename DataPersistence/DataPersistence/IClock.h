#pragma once
#include <string>

class IClock {
public:
    virtual ~IClock() = default;
    virtual std::string today() const = 0;  // YYYYMMDD
    virtual std::string now() const = 0;    // YYYY-MM-DDTHH:MM:SS
};
