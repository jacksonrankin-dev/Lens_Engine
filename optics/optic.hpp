#include "vec3.hpp"

#include <random>
#include <cmath>
#include <numbers>

class Optic {
public:
    virtual ~Optic() = default;
    Vec3 Sample();
};

class Pinhole : public Optic{

    double r_;  // Radius of the pinhole

    Pinhole(double r) : r_(r) {}

    // Samples across a circle, returns the point in optic-space
    Vec3 Sample() {
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
    double RandomDouble(double lower_bound = 0.0, double upper_bound = 1.0) {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_real_distribution<double> dis(lower_bound, upper_bound);
        double random_double = dis(gen);

        return random_double;
    }
};
