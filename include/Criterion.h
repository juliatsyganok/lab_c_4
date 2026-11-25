#pragma once
#include "Patient.h"

class Criterion {
public:
    virtual bool matches(
        const Patient& patient) const = 0;
    virtual ~Criterion() = default;
};


class PatientCriterion: public Criterion {
private:
    int minAge;
    double maxPressure;
public:
    PatientCriterion(int minAge, double maxPressure): minAge(minAge), maxPressure(maxPressure) {}

    bool matches(const Patient& patient) const override{
        return patient.getAge() >= minAge &&
               patient.getPressure() > maxPressure;
    }
};


class AllCriterion: public Criterion {
public:
    bool matches(const Patient&) const override {
        return true;
    }
};
