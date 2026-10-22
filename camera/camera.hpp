#include <memory>

#include "optic.hpp"
#include "random.hpp"
#include "ray3.hpp"
#include "vec3.hpp"

#pragma once

class FrontStandard{
public:
    // ******************************************* Front Standard Attributes *******************************************
    Vec3 O_f_;                      // Position of the front standard
    Vec3 O_nominal_f_;              // Nominal position of the front standard

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

    // ****************************************** Rear Standard Constructors *******************************************
    FrontStandard()  // RearStandard default constructor
    {
        optic_ = std::make_shared<Pinhole>(0.0);  // Default is an ideal pinhole
    }

    FrontStandard(std::shared_ptr<Optic> optic) // RearStandard parameterized constructor
    {
        optic_ = optic;

    }

    // ******************************************** Rear Standard Movements ********************************************
    void SetMovements(double rise_fall_r,
                      double tilt_deg_r,
                      double shift_r,
                      double swing_deg_r,
                      double defocus_r)
    {
        // Set the movements
        rise_fall_f_ = rise_fall_r;
        tilt_f_      = Vec3::ConvertDegToRad(tilt_deg_r);
        shift_f_     = shift_r;
        swing_f_     = Vec3::ConvertDegToRad(swing_deg_r);
        defocus_f_   = defocus_r;
    }

    void ApplyMovements()
    {
        // Makes sure ApplyMovements doesn't accumulate position
        O_f_ = O_nominal_f_;

        // Tilt
        up_f_ = up_c_.Rotate(right_c_, -tilt_f_, 0);
        forward_f_ = forward_c_.Rotate(right_c_, -tilt_f_, 0);

        // Shift
        O_f_ = O_f_ + shift_f_* right_c_;

        // Rise/fall
        O_f_ = O_f_ + rise_fall_f_* up_c_;

        // Swing
        right_f_   = right_c_.Rotate(up_c_, swing_f_, 0);
        up_f_      = up_f_.Rotate(up_c_, swing_f_, 0);
        forward_f_ = forward_f_.Rotate(up_c_, swing_f_, 0);

        // Defocus
        O_f_ = O_f_ + defocus_f_ * forward_c_;
    }

    void IncrementMovements(double rise_fall_f, double tilt_deg_f, double shift_f, double swing_deg_f, double defocus_f)
    {
        // Increment the movements
        rise_fall_f_ += rise_fall_f;
        tilt_f_      += Vec3::ConvertDegToRad(tilt_deg_f);
        shift_f_     += shift_f;
        swing_f_     += Vec3::ConvertDegToRad(swing_deg_f);
        defocus_f_   += defocus_f;
    }

    // ********************************************* Front Standard Sample *********************************************
    Vec3 SampleFront()
    {
        // Get the sample
        Vec3 sample_optic_coords = optic_->Sample();

        // Convert to world coordinates
        Vec3 sample_world_coords = O_f_ + sample_optic_coords.x_ * right_f_ + sample_optic_coords.z_ * up_f_;

        // return the sample
        return sample_world_coords;
    }

};

class RearStandard{
public:
    // ******************************************* Rear Standard Attributes ********************************************
    Vec3 O_r_;                  // Position of the rear standard
    Vec3 O_nominal_r_;           // Nominal position of the rear standard

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
        width_ = 36.0e-3;  // 36 mm
        height_ = 24.0e-3; // 24 mm
        res_width_ = 800;  // Default 800 px
        res_height_ = static_cast<int>(res_width_ * height_ / width_);
    }

    RearStandard(double width, double height, int res_width) // RearStandard parameterized constructor
    {
        width_ = width;
        height_ = height;
        res_width_ = res_width;
        res_height_ = static_cast<int>(std::round(res_width_ * height_ / width_));
    }

    // ******************************************** Rear Standard Movements ********************************************
    void SetMovements(double rise_fall_r,
                      double tilt_deg_r,
                      double shift_r,
                      double swing_deg_r,
                      double defocus_r)
    {
        // Set the movements
        rise_fall_r_ = rise_fall_r;
        tilt_r_      = Vec3::ConvertDegToRad(tilt_deg_r);
        shift_r_     = shift_r;
        swing_r_     = Vec3::ConvertDegToRad(swing_deg_r);
        defocus_r_   = defocus_r;
    }

    void ApplyMovements()
    {
        // Makes sure ApplyMovements doesn't accumulate position
        O_r_ = O_nominal_r_;

        // Tilt
        up_r_ = up_c_.Rotate(right_c_, -tilt_r_, 0);
        forward_r_ = forward_c_.Rotate(right_c_, -tilt_r_, 0);

        // Shift
        O_r_ = O_r_ + shift_r_* right_c_;

        // Rise/fall
        O_r_ = O_r_ + rise_fall_r_* up_c_;

        // Swing
        right_r_   = right_c_.Rotate(up_c_, swing_r_, 0);
        up_r_      = up_r_.Rotate(up_c_, swing_r_, 0);
        forward_r_ = forward_r_.Rotate(up_c_, swing_r_, 0);

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

    // ********************************************* Rear Standard Sample **********************************************
    Vec3 SampleRear(int i, int j)
    {
        // Generates two random numebrs
        double x_jitter = RandomDouble();
        double y_jitter = RandomDouble();

        // Pixel coordinates in world space
        Vec3 x = (-width_/2.0 + width_/res_width_ * (j + x_jitter)) * right_r_;
        Vec3 y = (-height_/2.0 + height_/res_height_ * (i + y_jitter)) * up_r_;

        // Pixel position in world space
        Vec3 p = x + y;

        return p + O_r_;
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

        // Set the position of the camera
        front_standard_.O_f_ = O_c_;
        front_standard_.O_nominal_f_ = O_c_;

        rear_standard_.O_r_ = O_c_ - standard_separation_ * forward_c_;
        rear_standard_.O_nominal_r_ = rear_standard_.O_r_;

        front_standard_.ApplyMovements();
        rear_standard_.ApplyMovements();
    }

    // Returns a ray that samples the sensor correctly
    Ray3 SampleRay(int i, int j, double wavelength_nm, double amplitude)
    {
        Vec3 rear_sample = rear_standard_.SampleRear(i,j);
        Vec3 front_sample = front_standard_.SampleFront();

        Vec3 pos = rear_sample;
        Vec3 dir = (front_sample - rear_sample).Normalize();

        return Ray3(pos, dir, wavelength_nm, amplitude);
    }


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
