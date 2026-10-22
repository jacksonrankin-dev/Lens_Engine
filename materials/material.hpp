#include "ray3.hpp"

class HitRecord;

#pragma once

class Material {
public:
    virtual ~Material() = default;
    virtual Ray3 Interaction(HitRecord& hit, Ray3 incident_ray) = 0;
};
