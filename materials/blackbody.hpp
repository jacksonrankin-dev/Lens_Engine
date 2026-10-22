#pragma once

#include "material.hpp"
#include "ray3.hpp"

class Blackbody : public Material{
public:
    double T_;

    Blackbody(double T) : T_(T) {}

    Ray3 Interaction(HitRecord& hit, Ray3 incident_ray) {

        return Ray3();
    }
};
