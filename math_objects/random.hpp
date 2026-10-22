#pragma once

#pragma once

#include <random>

inline double RandomDouble(double lower_bound = 0.0,
                           double upper_bound = 1.0)
{
    thread_local std::mt19937_64 gen{std::random_device{}()};
    std::uniform_real_distribution<double> dis(lower_bound, upper_bound);

    return dis(gen);
}
