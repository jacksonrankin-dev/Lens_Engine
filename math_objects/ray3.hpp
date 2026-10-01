#include "vec3.hpp"

#pragma once

class Ray3 {
public:
    Vec3 o_;  // Ray3 origin point
    Vec3 u_;  // Ray3 direction (normalized to 1.0)

    Ray3() : o_(Vec3()), u_(Vec3()) {}
    Ray3(Vec3 o, Vec3 u) : o_(o), u_(u.Normalize(1.0)) {}

    Vec3 At(double t) { return o_ + u_ * t; }
};
