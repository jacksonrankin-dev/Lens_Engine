#pragma once

#include "sphere.hpp"
#include "vec3.hpp"

class AABB{
public:
    Vec3 min_;
    Vec3 max_;

    AABB(Vec3 min, Vec3 max) : min_(min), max_(max) {}
    AABB(Sphere s) : min_(s.center_ - s.radius_ * Vec3(1, 1, 1)), max_(s.center_ + s.radius_ * Vec3(1, 1, 1)) {}

    bool Hit(const Ray3 &ray, HitRecord &hit) { return 1.0; }

};
