#pragma once
#include <string>
#include <iostream>

class Patient {
private:
    std::string surname;
    int age;
    double pressure;
    static int alive;
public:
    explicit Patient(int age);
    Patient(const std::string& surname,int age, double pressure);
    Patient(const Patient& other);
    Patient(Patient&& other) noexcept;
    Patient& operator=(const Patient&) = delete;
    Patient& operator=(Patient&&) = delete;
    ~Patient();
    const std::string& getSurname() const;
    int getAge() const;
    double getPressure() const;
    static int getAlive();
    bool operator<(const Patient& other) const;
    bool operator==(const Patient& other) const;
    friend std::ostream& operator<<(std::ostream& out, const Patient& patient);
};
