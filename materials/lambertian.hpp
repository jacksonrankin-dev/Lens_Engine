#include "hit_record.hpp"
#include "material.hpp"
#include "ray3.hpp"

#pragma once

class Lambertian : public Material {
public:
    double rho_;  // Diffuse reflectance [0, 1]
    Lambertian(double rho) : rho_(rho) {}

    Ray3 Interaction(HitRecord& hit, Ray3 incident_ray) override
    {
        Vec3 direction = hit.normal_ + Vec3::RandomUnitVector();

        return Ray3(
            hit.position_,
            direction,
            incident_ray.lambda_,
            incident_ray.amplitude_ * rho_
        );
    }
};
