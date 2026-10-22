#pragma once

#include "hit_record.hpp"
#include "object.hpp"

class Lens : public Object {
public:

    bool Hit(const Ray3& ray, HitRecord& hit) override {
        constexpr double t_min = 1e-4;

        return true;
    }
};
