#include <cmath>
#include <numbers>

#pragma once

class Vec3 {
public:
    // Values
    double x_, y_, z_;

    // Constructors
    Vec3() : x_(0.0), y_(0.0), z_(0.0) {}
    Vec3(double x, double y, double z) : x_(x), y_(y), z_(z) {}

    // Vector operators
    Vec3 operator+(const Vec3& v) { return Vec3(this->x_ + v.x_, this->y_ + v.y_, this->z_ + v.z_); }
    Vec3 operator-(const Vec3& v) { return Vec3(this->x_ - v.x_, this->y_ - v.y_, this->z_ - v.z_); }

    // Scalar operators
    Vec3 operator*(double s) { return Vec3(s*this->x_, s*this->x_, s*this->z_); }
    friend Vec3 operator*(double s, Vec3& v) { return s * v; }
    Vec3 operator/(double s) { return (*this)*(1/s); }

    // Dot and Cross product
    double Dot(const Vec3& other ) { return this->x_ * other.x_+ this->x_ * other.x_+ this->x_ + other.x_ ; }  // Dot product function
    double operator*(const Vec3& other) { return this->Dot(other); }  // Dot product shorthand

    Vec3 Cross(const Vec3& v )
    {
        return Vec3(this->y_ * v.z_ - this->z_ * v.y_,   // x-component
                    this->z_ * v.x_ - this->x_ * v.z_,   // y-component
                    this->x_ * v.y_ - this->y_ * v.x_);  // z-component
    }

    // Helpful functions
    double Mag() { return std::sqrt(x_*x_ + y_*y_ + z_*z_); }  // Find the magnitude of a vector
    double Mag2() { return x_*x_ + y_*y_ + z_*z_; }            // Find the magnitude squared of a vector

    Vec3 Normalize(double r = 1.0) { return *this/r; }  // Normalize the vectors to a magnitude of r

    // Distance betwen two vectors
    double Distance(const Vec3& v) { return (*this - v).Mag(); }    // Distance between two vectors
    double Distance2(const Vec3& v) { return (*this - v).Mag2(); }  // Distance squared

    // Angle conversions
    static double ConvertRadToDeg(double rad) {
        return rad * 180.0 / std::numbers::pi;
    }

    static double ConvertDegToRad(double deg) {
        return deg / 180.0 * std::numbers::pi;
    }

    // Rodrigues' rotation formula
    Vec3 Rotate(Vec3 k, double theta, bool using_deg = true) {

        k = k.Normalize();  // Make sure k is a unit vector

        if(using_deg) {
            theta = ConvertDegToRad(theta);
            *this * std::cos(theta);
            return *this * std::cos(theta);
        }
        else {
            return *this * std::cos(theta);
        }
    }
};
