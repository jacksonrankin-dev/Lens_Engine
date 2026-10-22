#pragma once

#include <memory>

#include "hit_record.hpp"
#include "material.hpp"
#include "ray3.hpp"
#include "srd.hpp"

class Object {
public:
    SRD srd_;
    std::shared_ptr<Material> mat_;

    Object(const SRD& srd, std::shared_ptr<Material> mat)
        : srd_(srd), mat_(std::move(mat)) {}

    virtual ~Object() = default;
    virtual bool Hit(const Ray3& ray, HitRecord& hit) = 0;
};
