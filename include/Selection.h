#pragma once
#include "Patient.h"
#include <memory>
#include <cstddef>

class Selection {
private:
    std::unique_ptr<std::weak_ptr<Patient>[]> data;
    std::size_t cnt = 0;
    std::size_t cap = 0;
public:
    Selection() = default;
    Selection(const Selection& s);
    Selection(Selection&& s) noexcept;
    Selection& operator=(const Selection& s);
    Selection& operator=(Selection&& s) noexcept;
    ~Selection() = default;
    void pushBack(std::weak_ptr<Patient> p);
    std::size_t size() const;
    std::size_t getCapacity() const;
    bool empty() const;
    std::weak_ptr<Patient> operator[](std::size_t i) const;
};