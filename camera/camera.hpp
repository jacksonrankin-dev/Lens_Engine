#include <memory>

#include "optic.hpp"
#include "ray3.hpp"
#include "vec3.hpp"

#pragma once

class FrontStandard{
public:
    // ******************************************* Front Standard Attributes *******************************************
    Vec3 O_f_;                      // Position of the front standard

    std::shared_ptr<Optic> optic_;  // The optic on the front standard

    Vec3 right_f_;                  // Front standard right unit vector
    Vec3 forward_f_;                // Front standard forward unit vector
    Vec3 up_f_;                     // Front standard up unit vector

    Vec3 right_c_;                  // Camera right unit vector
    Vec3 forward_c_;                // Camera forward unit vector
    Vec3 up_c_;                     // Camera up unit vector

    double rise_fall_f_ = 0.0;      // Rise/fall of the front standard (default is 0.0 m)
    double      tilt_f_ = 0.0;      // Tilt of the front standard (default is 0.0 deg)
    double     shift_f_ = 0.0;      // Shift of the front standard (default is 0.0 m)
    double     swing_f_ = 0.0;      // Swing of the front standard (default is 0.0 deg)
    double   defocus_f_ = 0.0;      // Defocus of the front standard (default is 0.0 m)
};

class RearStandard{
public:
    // ******************************************* Rear Standard Attributes ********************************************
    Vec3 O_r_;                  // Position of the rear standard
    Vec3 O_nominal_r;           // Nominal position of the rear standard

    double width_;              // Physical width of the sensor
    double height_;             // Physical height of the sensor
    int res_width_;             // Number of pixels across the width of the sensor
    int res_height_;            // Number of pixels across the height of the sensor

    Vec3 right_r_;              // Rear standard right unit vector
    Vec3 forward_r_;            // Rear standard forward unit vector
    Vec3 up_r_;                 // Rear standard up unit vector

    Vec3 right_c_;              // Camera right unit vector
    Vec3 forward_c_;            // Camera forward unit vector
    Vec3 up_c_;                 // Camera up unit vector

    double rise_fall_r_ = 0.0;  // Rise/fall of the rear standard (default is 0.0 m)
    double      tilt_r_ = 0.0;  // Tilt of the rear standard (default is 0.0 deg)
    double     shift_r_ = 0.0;  // Shift of the rear standard (default is 0.0 m)
    double     swing_r_ = 0.0;  // Swing of the rear standard (default is 0.0 deg)
    double   defocus_r_ = 0.0;  // Defocus of the rear standard (default is 0.0 m)

    // ****************************************** Rear Standard Constructors *******************************************
    RearStandard()  // RearStandard default constructor
    {
        O_r_ = Vec3(0.0, -5.0, 2.0);  // default is 5 meters behind the origin, and 2 meters up
        O_nominal_r = O_r_;
        width_ = 36.0e-3;  // 36 mm
        height_ = 24.0e-3; // 24 mm
        res_width_ = 800;  // Default 800 px
        res_height_ = res_width_ * static_cast<int>(height_ / width_);
    }

    RearStandard(Vec3 O_r, double width, double height, int res_width) // RearStandard parameterized constructor
    {
        O_r_ = O_r;
        width_ = width;
        height_ = height;
        res_width_ = res_width;
        res_height_ = res_width_ * static_cast<int>(height_ / width_);
    }

    // ******************************************** Rear Standard Movements ********************************************
    void SetMovements(double rise_fall_r, double tilt_deg_r, double shift_r, double swing_deg_r, double defocus_r)
    {
        // Set the movements
        rise_fall_r_ = rise_fall_r;
        tilt_r_      = Vec3::ConvertDegToRad(tilt_deg_r);
        shift_r_     = shift_r;
        swing_r_     = Vec3::ConvertDegToRad(swing_deg_r);
        defocus_r_   = defocus_r;
    }

    void ApplyMovements() {

        O_r_ = O_nominal_r;

        // Tilt
        up_r_ = up_c_.Rotate(right_c_, -tilt_r_);
        forward_r_ = forward_c_.Rotate(right_c_, -tilt_r_);

        // Shift
        O_r_ = O_r_ + shift_r_* right_c_;

        // Rise/fall
        O_r_ = O_r_ + rise_fall_r_* up_c_;

        // Swing
        right_r_   = right_c_.Rotate(up_c_, swing_r_);
        up_r_      = up_r_.Rotate(up_c_, swing_r_);
        forward_r_ = forward_r_.Rotate(up_c_, swing_r_);

        // Defocus
        O_r_ = O_r_ + defocus_r_ * forward_c_;
    }

    void IncrementMovements(double rise_fall_r, double tilt_deg_r, double shift_r, double swing_deg_r, double defocus_r)
    {
        // Increment the movements
        rise_fall_r_ += rise_fall_r;
        tilt_r_      += Vec3::ConvertDegToRad(tilt_deg_r);
        shift_r_     += shift_r;
        swing_r_     += Vec3::ConvertDegToRad(swing_deg_r);
        defocus_r_   += defocus_r;
    }
};

class Camera {
public:
    // *********************************************** Camera Attributes ***********************************************
    Vec3 O_c_;                      // Position of the camera

    Vec3 target_;                   // Sets a point for the camera to stare at
    Vec3 scene_up_;                 // Describes which way is "up" in a scene
    double standard_separation_;    // Dictates how far away the nominal positons of the standards are

    FrontStandard front_standard_;  // Holds the front standard
    RearStandard rear_standard_;    // Holds the rear standard

    Vec3 right_c_;                  // Front standard right unit vector
    Vec3 forward_c_;                // Front standard forward unit vector
    Vec3 up_c_;                     // Front standard up unit vector


    // ********************************************** Camera Construction **********************************************
    Camera(
        Vec3 pos,
        Vec3 target,
        Vec3 up,
        FrontStandard front_standard,
        RearStandard rear_standard,
        double standard_separation)
         : O_c_(pos),
           target_(target),
           scene_up_(up),
           front_standard_(front_standard),
           rear_standard_(rear_standard),
           standard_separation_(standard_separation)
    {
        CameraBasis();
        front_standard.O_f_ = O_c_;
        rear_standard.O_r_  = O_c_ - standard_separation_ * forward_c_;
    }

    Ray3 SampleRay() { return Ray3(); }


private:
    void CameraBasis()
    {
        // Make the camera's basis
        forward_c_ = (target_ - O_c_).Normalize();
        right_c_   = (forward_c_.Cross(scene_up_)).Normalize();
        up_c_      = (right_c_.Cross(forward_c_)).Normalize();

        // Tell the front standard what the camera's basis is.
        front_standard_.forward_c_ = forward_c_;
        front_standard_.right_c_   = right_c_;
        front_standard_.up_c_      = up_c_;

        // Tell the rear standard what the camera's basis is.
        rear_standard_.forward_c_ = forward_c_;
        rear_standard_.right_c_   = right_c_;
        rear_standard_.up_c_      = up_c_;
    }

};
