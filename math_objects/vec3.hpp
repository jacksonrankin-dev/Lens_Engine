#include "random.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <ostream>

#pragma once

class Vec3 {
public:
    // Values
    double x_, y_, z_;

    // Constructors
    Vec3() : x_(0.0), y_(0.0), z_(0.0) {}
    Vec3(double x, double y, double z) : x_(x), y_(y), z_(z) {}

    // Vector operators
    Vec3 operator+(const Vec3& v) const { return Vec3(this->x_ + v.x_, this->y_ + v.y_, this->z_ + v.z_); }
    Vec3 operator-(const Vec3& v) const { return Vec3(this->x_ - v.x_, this->y_ - v.y_, this->z_ - v.z_); }
    Vec3 operator-() const { return Vec3(-x_, -y_, -z_); }

    // Scalar operators
    Vec3 operator*(double s) const { return Vec3(s*this->x_, s*this->y_, s*this->z_); }
    friend Vec3 operator*(double s, const Vec3& v) { return Vec3(s * v.x_, s * v.y_, s * v.z_); }
    Vec3 operator/(double s) const { return (*this)*(1/s); }

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Vec3& v)
    {
        return os << v.x_ << ' ' << v.y_ << ' ' << v.z_;
    }

    // Dot and Cross product
    double Dot(const Vec3& other ) const { return this->x_ * other.x_+ this->y_ * other.y_+ this->z_ * other.z_ ; }  // Dot product function
    double operator*(const Vec3& other) const { return this->Dot(other); }  // Dot product shorthand

    Vec3 Cross(const Vec3& v )
    {
        return Vec3(this->y_ * v.z_ - this->z_ * v.y_,   // x-component
                    this->z_ * v.x_ - this->x_ * v.z_,   // y-component
                    this->x_ * v.y_ - this->y_ * v.x_);  // z-component
    }

    // Helpful functions
    double Mag() const { return std::sqrt(x_*x_ + y_*y_ + z_*z_); }  // Find the magnitude of a vector
    double Mag2() const { return x_*x_ + y_*y_ + z_*z_; }            // Find the magnitude squared of a vector

    Vec3 Normalize(double r = 1.0) { return *this*r/Mag(); }  // Normalize the vectors to a magnitude of r

    // Distance betwen two vectors
    double Distance(const Vec3& v) { return (*this - v).Mag(); }    // Distance between two vectors
    double Distance2(const Vec3& v) { return (*this - v).Mag2(); }  // Distance squared

    double AngleBetween(const Vec3& v) const {
        double cos_theta = (*this)*v / (*this).Mag() * v.Mag();
        cos_theta = std::clamp(cos_theta, -1.0, 1.0);  // Prevents floating point errors from messing up arccos
        return std::acos(cos_theta);
    }

    // Angle conversions
    static double ConvertRadToDeg(double rad) {
        return rad * 180.0 / std::numbers::pi;
    }

    static double ConvertDegToRad(double deg) {
        return deg / 180.0 * std::numbers::pi;
    }

    // Rodrigues' rotation formula
    Vec3 Rotate(Vec3 k, double theta, bool using_deg = true)
    {
        if (using_deg)
            theta = ConvertDegToRad(theta);

        k = k.Normalize();

        return (*this) * std::cos(theta) + k.Cross(*this) * std::sin(theta) + k * (k.Dot(*this) * (1.0 - std::cos(theta)));
    }

    static Vec3 Random() {
        return Vec3(RandomDouble(), RandomDouble(), RandomDouble());
    }

    static Vec3 Random(double min, double max) {
        return Vec3(RandomDouble(min,max), RandomDouble(min,max), RandomDouble(min,max));
    }


    static Vec3 RandomUnitVector() {
        while (true) {
            auto p = Vec3::Random(-1,1);
            auto lensq = p.Mag2();
            if (1e-160 < lensq && lensq <= 1)
                return p / sqrt(lensq);
        }
    }
};
