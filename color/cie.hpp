#include <fstream>
#include <string>
#include <sstream>
#include <vector>

struct CIE {
    // Data attributes
    static constexpr double lambda_min_ = 360.0;  // Lambda value lower bound in nm
    static constexpr double lambda_max_ = 830.0;  // Lambda value upper bound in nm
    static constexpr double increment_  = 1.0;    // Lambda increment value in nm
    static constexpr int sample_count_  = 471;    // Number of datapoints in this set

    // Open the data
    void LoadData(std::string filename = "color/CIE_xyz_1931_2deg.csv")
    {
        // Try to open the file
        std::ifstream file(filename);

        // Prevents the data from doubling, should LoadData() be called twice
        x_bar_.clear();
        y_bar_.clear();
        z_bar_.clear();

        // Throw a helpful error if the file can't open
        if (!file) {throw std::runtime_error("Could not open CIE CSV file"); }

        // Initialize a string to hold the data on a line
        std::string line;

        // Go line by line
        for(int s = 0; s < sample_count_; s++)
        {
            // Grab a line from the data
            std::getline(file, line);

            //
            std::stringstream stream(line);

            // Desireable data
            std::string lambda;
            std::string x_bar;
            std::string y_bar;
            std::string z_bar;

            // Separate the data
            std::getline(stream, lambda, ',');
            std::getline(stream, x_bar, ',');
            std::getline(stream, y_bar, ',');
            std::getline(stream, z_bar, ',');

            // Accumulate data into the vectors
            x_bar_.push_back(std::stod(x_bar));
            y_bar_.push_back(std::stod(y_bar));
            z_bar_.push_back(std::stod(z_bar));
        }
    }

    // Sample the x bar data with linear interpolation
    double XBar(double lambda) const
    {
        return Interpolate(x_bar_, lambda);
    }
    // Sample the y bar data with linear interpolation
    double YBar(double lambda) const
    {
        return Interpolate(y_bar_, lambda);
    }
    // Sample the z bar data with linear interpolation
    double ZBar(double lambda) const
    {
        return Interpolate(z_bar_, lambda);
    }

private:
    // Holds the CIE 1931 xyz color matching functions
    std::vector<double> x_bar_;
    std::vector<double> y_bar_;
    std::vector<double> z_bar_;

    // Linearly interpolate data from the CIE data based on a specific wavelength
    double Interpolate(const std::vector<double>& color_matching_function, double lambda) const
    {

        // If the sample is somehow outside the data, return the nearest value in the data
        if(lambda <= lambda_min_) { return color_matching_function.front(); }
        if(lambda >= lambda_max_) { return color_matching_function.back(); }

        // Linearly interpolate the result
        double position = (lambda - lambda_min_) / increment_;  // Relative position
        int index = static_cast<int>(position);                 // Round down
        double fraction = position - index;                     // Find how much to interpolate

        // Interpolation
        double slope = (color_matching_function[index + 1] - color_matching_function[index]) / increment_;
        return color_matching_function[index] + fraction * slope;

    }
};
