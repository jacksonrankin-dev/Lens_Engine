#pragma once

#include <cmath>
#include <memory>

#include "lambertian.hpp"
#include "material.hpp"
#include "object.hpp"
#include "srd.hpp"

class Sphere : public Object{
public:
    // Sphere attributes
    Vec3 center_;    // Position of the center of the sphere
    double radius_;  // Radius of sphere

    // Default constructor
    Sphere() : Object(White, std::make_shared<Lambertian>(1.0)), center_(Vec3()), radius_(1.0) {}

    // Parametrized constructor
    Sphere(Vec3 c, double r, const SRD& srd, std::shared_ptr<Material> mat)
            : Object(srd, std::move(mat)), center_(c), radius_(r) {}

    // Check if the ray intersects the sphere
    bool Hit(const Ray3& ray, HitRecord& hit) override {
        constexpr double t_min = 1e-4;

        Vec3 oc = ray.o_ - center_;
        double a      = ray.u_ * ray.u_;
        double half_b = ray.u_ * oc;
        double c      = oc * oc - radius_ * radius_;

        double disc = half_b * half_b - a * c;
        if (disc < 0.0) return false;

        double sqrt_d = std::sqrt(disc);

        // Nearest root first, then the far one (e.g. when starting inside)
        double t = (-half_b - sqrt_d) / a;
        if (t < t_min) {
            t = (-half_b + sqrt_d) / a;
            if (t < t_min) return false;
        }

        Vec3 pos     = ray.At(t);
        Vec3 outward = (pos - center_).Normalize(1.0);

        bool front_face = (ray.u_ * outward) < 0.0;
        Vec3 norm       = front_face ? outward : -outward;

        hit.distance_   = t;
        hit.position_   = pos;
        hit.normal_     = norm;
        hit.front_face_ = front_face;
        hit.mat_        = mat_;

        return true;
    }
};
