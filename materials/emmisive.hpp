#include "hit_record.hpp"
#include "material.hpp"


#pragma once

class Emmisive : public Material {
public:
    double I_;  // Intensity

    Emmisive(double I) :  I_(I) {}

    Ray3 Interaction(HitRecord &hit, Ray3 incident_ray) override {
        Vec3 direction = hit.normal_ + Vec3::RandomUnitVector();

        return Ray3(
            hit.position_,
            direction,
            incident_ray.lambda_,
            I_
        );
    };
};
