#include "hit_record.hpp"
#include "material.hpp"

#include <cmath>
#include <array>

#pragma once

class OpticalMedium : public Material {
public:
    // Sellmeier dispersion constants
    std::array<double, 6> coeffs{};  // In the form of: B1, B2, B3, C1, C2, C3

    double n1_;
    double n2_;

    // Default constructor, Borosilicate crown optical glass
    OpticalMedium()
        : coeffs{
            1.03961212,
            0.231792344,
            1.01046945,
            6.00069867e-3,
            2.00179144e-2,
            1.03560653e2
          }
    {}

    // Parameterized constructor
    OpticalMedium(
        double B1, double B2, double B3,
        double C1, double C2, double C3)
         : coeffs{B1, B2, B3, C1, C2, C3}
    {}

    void SetRefractiveIndeces(double n1, double n2) {
        n1_ = n1;
        n2_ = n2;
    }

    // Return the index of refraction
    double RefractiveIndex(double lambda_nm) const
    {
        // Convert wavelength to um
        const double lambda_um = lambda_nm / 1000.0;
        const double lambda_um2 = lambda_um*lambda_um;

        // Calculate the terms of the equation
        const double term1 = coeffs[0] * lambda_um2 / (lambda_um2 - coeffs[3]);
        const double term2 = coeffs[1] * lambda_um2 / (lambda_um2 - coeffs[4]);
        const double term3 = coeffs[2] * lambda_um2 / (lambda_um2 - coeffs[5]);

        // Return the index of refraction
        return std::sqrt(1.0 + term1 + term2 + term3);
    }

    Ray3 Interaction(HitRecord& hit, Ray3 incident_ray) override {
        Vec3 normal = hit.normal_;
        Vec3 direction_i = incident_ray.u_;

        double lambda = incident_ray.lambda_;

        bool outside = hit.front_face_;

        double n1, n2;

        if (outside) {
            n1 = 1.0003;  // Index of refraction in air
            n2 = RefractiveIndex(lambda);  // Index of refraction in the material
        }

        if (!outside) {
            n1 = RefractiveIndex(lambda);  // Index of refraction in the material
            n2 = 1.0003;  // Index of refraction in air
        }

        double cos_theta1 = std::clamp(-(direction_i * normal), 0.0, 1.0);
        double sin_theta1 = std::sqrt(std::max(0.0, 1.0 - cos_theta1 * cos_theta1));

        double eta = n1 / n2;

        double sin_theta2 = eta * sin_theta1;

        Vec3 direction_r;

        if (sin_theta2 >= 1.0) {
            // Total internal reflection
            direction_r = direction_i - 2 * (direction_i * normal) * normal;

        } else {
            // Transmition
            double cos_theta2 = std::sqrt(std::max(0.0, 1.0 - sin_theta2 * sin_theta2));
            direction_r =  n1/n2 * direction_i + (n1/n2*cos_theta1 - cos_theta2) * normal;

            // Calculate the Fresnel reflectance.
            double F = FresnelReflectance(n1, n2, cos_theta1, cos_theta2);

            // Stochastically choose reflectance or transmission.
            if (RandomDouble() < F) {  // Reflectance with a probability F
                direction_r = direction_i - 2.0 * (direction_i * normal) * normal;
            } else {
                direction_r = eta * direction_i + (eta * cos_theta1 - cos_theta2) * normal;
            }
        }

        return Ray3(
            hit.position_,
            direction_r,
            incident_ray.lambda_,
            incident_ray.amplitude_
        );

    }
private:
    double FresnelReflectance(double n1, double n2, double cos_theta1, double cos_theta2) const {
        // Calculate the numerator and denomonator for the reflectance for s-polarized light
        double rs_numerator = n1 * cos_theta1 - n2 * cos_theta2;
        double rs_denomonator = n1 * cos_theta1 + n2 * cos_theta2;

        // Calculate the numerator and denomonator for the reflectance for p-polarized light
        double rp_numerator = n1 * cos_theta2 - n2 * cos_theta1;
        double rp_denomonator = n1 * cos_theta2 + n2 * cos_theta1;

        // Calculate the reflectance of s and p polarized light.
        double Rs = (rs_numerator / rs_denomonator) * (rs_numerator / rs_denomonator);
        double Rp = (rp_numerator / rp_denomonator) * (rp_numerator / rp_denomonator);

        // Return the Fresnel reflectance F = 1/2 (Rs + Rp)
        return 0.5 * (Rs + Rp);
    }
};
