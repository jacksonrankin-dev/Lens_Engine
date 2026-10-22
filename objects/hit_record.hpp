#pragma once

#include <memory>
#include "material.hpp"
#include "vec3.hpp"

#pragma once

class HitRecord {
public:
    double distance_;
    Vec3 position_;
    Vec3 normal_;
    bool front_face_;
    std::shared_ptr<Material> mat_;

    HitRecord() = default;

};
