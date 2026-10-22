#pragma once

#include <memory>

#include "hit_record.hpp"
#include "material.hpp"
#include "object.hpp"
#include "srd.hpp"

class Plane : public Object {
public:
    Vec3 origin_, normal_;

    // Plane(Vec3 origin,Vec3 normal, std::shared_ptr<Material> mat) : origin_(origin), normal_(normal), mat_(mat) {}
    Plane(Vec3 origin, Vec3 normal, const SRD& srd, std::shared_ptr<Material> mat)
        : Object(srd, std::move(mat)), origin_(origin), normal_(normal) {}

    // Checks if a ray has hit the Plane object
    bool Hit(const Ray3& ray, HitRecord& hit) override {

        // Margin of error value
        constexpr double epsilon = 1e-12;

        // Calculates the denomator for the distance equation
        double denominator = (ray.u_ * normal_);

        // If the denomonator is close to zero, return false
        if (std::abs(denominator) < epsilon)
        {
            return 0;
        }

        // Calculate the distance
        double t = ((origin_ - ray.o_) * normal_) / denominator;

        // If the distance is less than epsilon, return false
        if (t < epsilon)
        {
            return 0;
        }

        // Calculate the hit posiion
        Vec3 pos  = ray.At(t);

        // Orient normal against incoming ray
        bool front_face = (ray.u_ * normal_) < 0.0;
        Vec3 norm = front_face ? normal_ : -normal_;

        // Updates HitRecord object
        hit.distance_    = t;
        hit.position_    = pos;
        hit.normal_      = norm;
        hit.front_face_  = front_face;
        hit.mat_         = mat_;

        return 1;
    }
};
