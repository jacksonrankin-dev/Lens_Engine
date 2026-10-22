#pragma once

#include "material.hpp"
#include "ray3.hpp"
#include "hit_record.hpp"

class Mirror : public Material {
public:
    double rho_;  // Albedo
    double mu_;   // Roughness

    Mirror(double rho, double mu) : rho_(rho), mu_(mu) {}

    Ray3 Interaction(HitRecord& hit, Ray3 incident_ray) override {
        // Grabs the position of the hit and the surface normal
        Vec3 normal = hit.normal_;
        Vec3 position = hit.position_;

        // Calculates the new direction from the old one
        Vec3 direction_i = incident_ray.u_;
        Vec3 direction_r = direction_i - 2 * (direction_i * normal) * normal + mu_ * Vec3::RandomUnitVector();

        // Calculates the new amplitude
        double t = incident_ray.amplitude_*rho_;

        return Ray3(
            position,
            direction_r,
            incident_ray.lambda_,
            t);
    }
};
