#include "vec3.hpp"

#include "random.hpp"
#include <cmath>
#include <numbers>

#pragma once

class Optic {
public:
    virtual ~Optic() = default;
    virtual Vec3 Sample() = 0;
};

class Pinhole : public Optic {
public:

    explicit Pinhole(double r) : r_(r) {}

    // Samples across a circle, returns the point in optic-space
    Vec3 Sample() override {
        // Generate two random numbers [0.0, 1.0]
        double u1 = RandomDouble();
        double u2 = RandomDouble();

        // Calculate the radial portion
        double R = r_ * std::sqrt(u1);

        // Calculate the polar portion
        double theta = 2 * std::numbers::pi * u2;

        // Convert to cartesian
        double right = R * std::cos(theta);
        double up    = R * std::sin(theta);

        // Return the coordinate in Optic-space
        return Vec3(right, 0.0, up);
    }

private:
    double r_;  // Radius of the pinhole
};

class Lens : public Optic {
public:
    Vec3 Sample() override {
        // Generate two random numbers [0.0, 1.0]
        double u1 = RandomDouble();
        double u2 = RandomDouble();

        // Calculate the radial portion
        double R = r_ * std::sqrt(u1);

        // Calculate the polar portion
        double theta = 2 * std::numbers::pi * u2;

        // Convert to cartesian
        double right = R * std::cos(theta);
        double up    = R * std::sin(theta);

        // Return the coordinate in Optic-space
        return Vec3(right, 0.0, up);
    }
    double r_;
};

struct Surface {

};

struct LensGroup {

};
