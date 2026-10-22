#pragma once
#include <iostream>
#include <memory>
#include <object.hpp>
#include <thread>
#include <vector>

#include "camera.hpp"
#include "cie.hpp"
#include "color.hpp"
#include "hit_record.hpp"
#include "random.hpp"
#include "srd.hpp"

class Scene {
public:
    // Scene and rendering attributes
    Camera cam_;
    std::vector<std::shared_ptr<Object>> objects_;

    CIE cie_;

    double illuminant_norm_;     // D65 Illuminant

    // Sampling attributes
    int ray_depth_;              // How many times a ray can interact with an object
    int samples_per_pixel_;      // How many samples per pixel there are
    double lambda_increment_;    // How much the samples increment by each sample

    double lambda_min_ = 380.0;  // Wavelength in nm
    double lambda_max_ = 780.0;  // Wavelength in nm

    Scene(
        Camera cam,
        std::vector<std::shared_ptr<Object>> objects,
        int samples_per_pixel,
        int ray_depth,
        int spectrum_samples_per_ray)
           : cam_(cam),
             objects_(objects),
             ray_depth_(ray_depth),
             samples_per_pixel_(samples_per_pixel)
    {
        lambda_increment_ = (lambda_max_ - lambda_min_) / spectrum_samples_per_ray;

        // Normalization so that a perfect reflector under D65 has Y = 1
        illuminant_norm_ = 0.0;
        for (double l = lambda_min_; l <= lambda_max_; l += 1.0)
            illuminant_norm_ += D65.Sample(l) * cie_.YBar(l);
    }

    Color RenderPixel(int i, int j) {

        std::vector<double> lambda_samples =
            StratifiedSamples(static_cast<int>(std::round((lambda_max_ - lambda_min_) / lambda_increment_)));
        std::vector<double> spectral_power(lambda_samples.size(), 0.0);

        for (std::size_t k = 0; k < lambda_samples.size(); ++k) {
            double lambda = lambda_samples[k];

            for (int s = 0; s < samples_per_pixel_; ++s)
            {
                Ray3 ray = cam_.SampleRay(i, j, lambda, 1.0);

                spectral_power[k] += TraceRay(ray);
            }

            spectral_power[k] /= samples_per_pixel_;
        }

        Color pixel = IntegrateSpectrum(lambda_samples, spectral_power);

        return pixel;
    }

    // Single-threaded rendering
    void RenderImage(std::string filepath)
    {
        // Grab image dimensions
        int pixels_wide = cam_.rear_standard_.res_width_;
        int pixels_tall = cam_.rear_standard_.res_height_;

        // Make a vector to store the image
        std::vector<Color> image;
        image.resize(pixels_tall*pixels_wide);

        // Render loop
        for (int i = 0; i < pixels_tall; i++) {
            for (int j = 0; j < pixels_wide; j++) {

                int index = i * pixels_wide + j;

                Color pixel = RenderPixel(i, j);

                image[index] = pixel;
            }

            double progress = (i + 1) * 100.0 / pixels_tall;

            std::cout << "\r\033[KRender Progress: "
                      << std::fixed << std::setprecision(1)
                      << progress << "%"
                      << std::flush;
        }
        Color::SavePFM(filepath, image, pixels_wide, pixels_tall);
        std::cout << "\nImage saved to " << filepath << "\n";
    }

    // Multi-threaded rendering
    void RenderImage(std::string filepath, int num_threads)
    {
        // Check the available threads in the computer
        auto n = AvailableThreads();

        // Handles if the user requests an invalid number of threads
        if (num_threads > n) {  // If the request is more than the computer can spare
            std::cout << "The requested number of threads is more than available";
            num_threads = 4;  // Using 4 because most computers have 4
        }
        if (num_threads <= 0) {  // If the request is less than or equal to 0
            std::cout << "The requested number of threads is invalid";
            num_threads = 4;  // Using 4 because most computers have 4
        }

        // Tell the user the image is rendering:
        std::cout << "Rendering image with " << num_threads << " threads.";

        // Grab image dimensions
        int pixels_wide = cam_.rear_standard_.res_width_;
        int pixels_tall = cam_.rear_standard_.res_height_;

        // Make a vector to store the image
        std::vector<Color> image;
        image.resize(pixels_tall*pixels_wide);

        // Keep track of the rows completed
        std::atomic<int> rows_completed{0};

        // Reserve memory for the threads
        std::vector<std::thread> workers;
        workers.reserve(num_threads);

        // Calculate the number of rows that each thread needs to render
        int rows_per_thread = pixels_tall / num_threads;

        // Render the image
        for (int n = 0; n < num_threads; n++)
        {
            int start_i = n * rows_per_thread;
            int end_i   = (n == num_threads - 1) ? pixels_tall : start_i + rows_per_thread;

            workers.emplace_back(
                &Scene::RenderChunk,
                this,
                start_i,
                end_i,
                pixels_wide,
                std::ref(image),
                std::ref(rows_completed)
            );
        }

        // Progress bar: only the main thread writes to the console.
        constexpr int bar_width = 40;

        while (true) {
            const int completed =
                rows_completed.load(std::memory_order_relaxed);

            const double progress =
                100.0 * completed / pixels_tall;

            const int filled =
                bar_width * completed / pixels_tall;

            std::cout << '\r' << '['
                      << std::string(filled, '=')
                      << std::string(bar_width - filled, ' ')
                      << "] "
                      << std::fixed << std::setprecision(1)
                      << progress << "% "
                      << completed << "/" << pixels_tall << " rows"
                      << std::flush;

            if (completed >= pixels_tall) {
                break;
            }

            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
        }

        // Wait for all threads to finish their work
        for (auto& t : workers) {
            if (t.joinable()) {
                t.join();
            }
        }

        Color::SavePPM(filepath, image, pixels_wide, pixels_tall);
        std::cout << "\nImage saved to " << filepath << "\n";
    }

private:
    double TraceRay(
        Ray3 ray,
        double t_min = 1e-6,
        double t_max = std::numeric_limits<double>::infinity())
    {
        double throughput = ray.amplitude_;

        for (int depth = 0; depth < ray_depth_; ++depth)
        {
            HitRecord closest_hit;
            double closest_dist = t_max;
            std::shared_ptr<Object> closest_object = nullptr;

            // Find the nearest valid intersection.
            for (const auto& obj : objects_)
            {
                HitRecord temp_hit;

                if (obj->Hit(ray, temp_hit) &&
                    temp_hit.distance_ > t_min &&
                    temp_hit.distance_ < closest_dist)
                {
                    closest_dist = temp_hit.distance_;
                    closest_hit = temp_hit;
                    closest_object = obj;
                }
            }

            // The ray escaped or no object was within t_max.
            if (!closest_object) {
                constexpr double skyIntensity = 2.0;
                return throughput * skyIntensity * BlueSky.Sample(ray.lambda_);
            }
            // Preserve the incoming wavelength for elastic surface reflection.
            const double lambda = ray.lambda_;

            // Interact with the surface.
            ray = closest_hit.mat_->Interaction(closest_hit, ray);

            // Propagate the amplitude produced by the material interaction.
            throughput = ray.amplitude_ * closest_object->srd_.Sample(lambda);

            ray.amplitude_ = throughput;
        }

        return 0.0;
    }

    Color IntegrateSpectrum(const std::vector<double>& lambda_samples, const std::vector<double>& spectral_power)
    {
        double X = 0.0;
        double Y = 0.0;
        double Z = 0.0;

        const double scale = lambda_increment_ / illuminant_norm_;

        for (std::size_t i = 0; i < lambda_samples.size(); ++i)
        {
            const double lambda = lambda_samples[i];
            const double S = spectral_power[i] * D65.Sample(lambda);  // reflectance * illuminant

            X += S * cie_.XBar(lambda) * scale;
            Y += S * cie_.YBar(lambda) * scale;
            Z += S * cie_.ZBar(lambda) * scale;
        }

        // XYZ (D65 white) -> linear sRGB
        double R =  3.2404542 * X - 1.5371385 * Y - 0.4985314 * Z;
        double G = -0.9692660 * X + 1.8760108 * Y + 0.0415560 * Z;
        double B =  0.0556434 * X - 0.2040259 * Y + 1.0572252 * Z;

        return Color(LinearToSRGB(R), LinearToSRGB(G), LinearToSRGB(B));
    }

    double LinearToSRGB(double c)
    {

        if (c <= 0.0031308)
            return 12.92 * c;
        return 1.055 * std::pow(c, 1.0 / 2.4) - 0.055;
    }

    std::vector<double> StratifiedSamples(int count)
    {
        std::vector<double> samples;
        samples.reserve(count);

        for (int i = 0; i < count; ++i)
        {
            double lambda = RandomDouble(
                lambda_min_ + i * lambda_increment_,
                lambda_min_ + (i + 1) * lambda_increment_
            );

            samples.push_back(lambda);
        }

        return samples;
    }

    unsigned int AvailableThreads() {
        auto n = std::thread::hardware_concurrency();
        return n;
    }

    void RenderChunk(int start_i, int end_i, int width, std::vector<Color>& image, std::atomic<int>& rows_completed) {
        for (int i = start_i; i < end_i; i++) {
            for (int j = 0; j < width; j++) {
                int index = i * width + j;
                image[index] = RenderPixel(i, j);
            }

            // This row is finished.
            rows_completed.fetch_add(1, std::memory_order_relaxed);
        }
    }
};
