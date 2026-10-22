// color/color.h

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#pragma once

class Color {
public:
    float r_, g_, b_;

    // Default constructor yields black
    Color() : r_(0.0f), g_(0.0f), b_(0.0) {}

    // Arbitrary color constructor (expects values 0.0 to 1.0)
    Color (float r, float g, float b) : r_(r), g_(g), b_(b) {}

    // Vector operators
    Color operator+(const Color& c) const { return Color(this->r_ + c.r_, this->g_ + c.g_, this->b_ + c.b_); }
    Color operator-(const Color& c) const { return Color(this->r_ - c.r_, this->g_ - c.g_, this->b_ - c.b_); }
    Color operator-() const { return Color(-r_, -g_, -b_); }

    // Scalar operators
    Color operator*(double s) const { return Color(s*this->r_, s*this->g_, s*this->b_); }
    friend Color operator*(double s, const Color& v) { return Color(s * v.r_, s * v.g_, s * v.b_); }
    Color operator/(double s) const { return (*this)*(1/s); }

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Color& c)
    {
        return os << c.r_ << ' ' << c.g_ << ' ' << c.b_;
    }

    // Generate a color gradient
    static std::vector<Color> GenerateGrad(int width, int height) {
        std::vector<Color> img;
        img.resize(width * height);

        for (int i = 0; i < height; i++) {     // Row-by-row
            for (int j = 0; j < width; j++) {  // Column-by-column

                // Find the current index of the pixel
                int index = (i * width) + j;

                // Gradient logic
                float r = static_cast<float>(i) / (height - 1);
                float g = static_cast<float>(j) / (width - 1);
                float b = 0.0f;

                img[index] = Color(r, g, b);

            }
        }
        return img;
    }

    // Saves a P3 ppm image
    static void SavePPM(const std::string& filename, const std::vector<Color>& pixelVals, int width, int height) {
        std::ofstream image(filename);

        // Check if the image isn't open
        if (!image.is_open()) {
            std::cout << "Image failed to write" << std::endl;
        } else {
            // File header
            image << "P3" << std::endl;                    // Denotes the type of PPM image
            image << width << " " << height << std::endl;  // Stores the width and height of the image
            image << "255" << std::endl;                   // Stores the max range of values [0, 255]

            // Pixel values
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {

                    // Finds the index
                    int index = (i * width) + j;
                    Color curColor = pixelVals[index];  // Grabs the current color in the loop

                    // Finds the 8-bit RGB values
                    int r = static_cast<int>(std::clamp(curColor.r_, 0.0f, 1.0f) * 255.0f);
                    int g = static_cast<int>(std::clamp(curColor.g_, 0.0f, 1.0f) * 255.0f);
                    int b = static_cast<int>(std::clamp(curColor.b_, 0.0f, 1.0f) * 255.0f);

                    // Output the rgb values to the file
                    image << r << " " << g << " " << b << std::endl;
                }
            }
        }
    }

    // Saves a PFM image (little-endian)
    // Saves a PFM image (little-endian)
    static void SavePFM(const std::string& filename,
                        const std::vector<Color>& pixelVals,
                        int width,
                        int height) {
        if (width <= 0 || height <= 0 ||
            pixelVals.size() != static_cast<std::size_t>(width) * height) {
            std::cerr << "Invalid image dimensions or pixel data\n";
            return;
        }

        std::ofstream image(filename, std::ios::binary);

        if (!image.is_open()) {
            std::cerr << "Image failed to write: " << filename << '\n';
            return;
        }

        // PFM header: RGB image, dimensions, little-endian scale
        image << "PF\n";
        image << width << ' ' << height << '\n';
        image << "-1.0\n";

        // PFM stores rows bottom-to-top.
        for (int i = height - 1; i >= 0; --i) {
            for (int j = 0; j < width; ++j) {
                const Color& c = pixelVals[i * width + j];

                const float rgb[3] = {c.r_, c.g_, c.b_};
                image.write(
                    reinterpret_cast<const char*>(rgb),
                    sizeof(rgb)
                );
            }
        }

        if (!image) {
            std::cerr << "Error writing PFM image: " << filename << '\n';
        }
    }
};
