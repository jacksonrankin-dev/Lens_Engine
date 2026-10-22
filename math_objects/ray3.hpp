#include "vec3.hpp"

#pragma once

class Ray3 {
public:
    Vec3 o_;            // Ray3 origin point
    Vec3 u_;            // Ray3 direction (normalized to 1.0)
    double lambda_;     // Wavelength of the light ray
    double amplitude_;  // Intensity of the light ray

    Ray3() : o_(Vec3()), u_(Vec3()) {}
    Ray3(Vec3 o, Vec3 u, double lambda, double amplitude)
          : o_(o), u_(u.Normalize(1.0)), lambda_(lambda), amplitude_(amplitude) {}

    Vec3 At(const double t) const { return o_ + u_ * t; }
};
