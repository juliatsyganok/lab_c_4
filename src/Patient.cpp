#include "Patient.h"
#include <utility>

int Patient::alive = 0;

Patient::Patient(int age): Patient("", age, 0) {}

Patient::Patient(const std::string& surname, int age, double pressure): surname(surname), age(age), pressure(pressure) {
    ++alive;
}

Patient::Patient(const Patient& p): surname(p.surname), age(p.age), pressure(p.pressure) {
    ++alive;
}

Patient::Patient(Patient&& p) noexcept: surname(std::move(p.surname)), age(p.age), pressure(p.pressure) {
    ++alive;
}

Patient::~Patient() {
    --alive;
}

const std::string& Patient::getSurname() const {
    return surname;
}

int Patient::getAge() const {
    return age;
}

double Patient::getPressure() const {
    return pressure;
}

int Patient::getAlive() {
    return alive;
}

bool Patient::operator<(const Patient& p) const {
    return age < p.age;
}

bool Patient::operator==(const Patient& p) const {
    return age == p.age;
}

std::ostream& operator<<(std::ostream& out, const Patient& p) {
    return out << p.surname << ' ' << p.age << ' ' << p.pressure;
}