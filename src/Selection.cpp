#include "Selection.h"
#include <utility>

Selection::Selection(const Selection& s): cnt(s.cnt), cap(s.cap) {
    if (cap > 0) {
        data = std::make_unique<std::weak_ptr<Patient>[]>(cap);
        for (std::size_t i = 0; i < cnt; ++i) {
            data[i] = s.data[i];
        }
    }
}

Selection::Selection(Selection&& s) noexcept: data(std::move(s.data)), cnt(s.cnt), cap(s.cap) {
    s.cnt = 0;
    s.cap = 0;
}

Selection& Selection::operator=(const Selection& s) {
    if (this != &s) {
        Selection tmp(s);
        *this = std::move(tmp);
    }
    return *this;
}

Selection& Selection::operator=(Selection&& s) noexcept {
    if (this != &s) {
        data = std::move(s.data);
        cnt = s.cnt;
        cap = s.cap;
        s.cnt = 0;
        s.cap = 0;
    }
    return *this;
}

void Selection::pushBack(std::weak_ptr<Patient> p) {
    if (cnt == cap) {
        std::size_t newCap = 1;
        if (cap > 0) {
            newCap = cap * 2;
        }
        auto newData = std::make_unique<std::weak_ptr<Patient>[]>(newCap);
        for (std::size_t i = 0; i < cnt; ++i) {
            newData[i] = data[i];
        }
        data = std::move(newData);
        cap = newCap;
    }
    data[cnt] = p;
    ++cnt;
}

std::size_t Selection::size() const {
    return cnt;
}

std::size_t Selection::getCapacity() const {
    return cap;
}

bool Selection::empty() const {
    return cnt == 0;
}

std::weak_ptr<Patient> Selection::operator[](std::size_t i) const {
    if (i >= cnt) {
        return {};
    }
    return data[i];
}