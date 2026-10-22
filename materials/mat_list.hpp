// material/mat_list.hpp

/* This file is meant to hold a list of materials one can readily access */

#include "optical_medium.hpp"

#include <memory>

// Data obtained at https://refractiveindex.info/?shelf=popular_glass&book=BAK1&page=SCHOTT

#pragma once

// Optical Glass
inline const auto N_BK7    = std::make_shared<OpticalMedium>(1.03961212,  0.231792344, 1.01046945,  0.00600069867, 0.0200179144, 103.560653);
inline const auto N_BAF10  = std::make_shared<OpticalMedium>(1.5851495,   0.143559385, 1.08521269,  0.00926681282, 0.0424489805, 105.613573);
inline const auto N_BAK1   = std::make_shared<OpticalMedium>(1.12365662,  0.309276848, 0.881511957, 0.00644742752, 0.0222284402, 107.297751);
inline const auto N_FK51A  = std::make_shared<OpticalMedium>(0.971247817, 0.216901417, 0.904651666, 0.00472301995, 0.0153575612, 168.68133);
inline const auto N_LASF9  = std::make_shared<OpticalMedium>(2.00029547,  0.298926886, 1.80691843,  0.0121426017,  0.0538736236, 156.530829);
inline const auto N_SF5    = std::make_shared<OpticalMedium>(1.52481889,  0.187085527, 1.42729015,  0.011254756,   0.0588995392, 129.141675);
inline const auto N_SF10   = std::make_shared<OpticalMedium>(1.62153902,  0.256287842, 1.64447552,  0.0122241457,  0.0595736775, 147.468793);
inline const auto N_SF11   = std::make_shared<OpticalMedium>(1.73759695,  0.313747346, 1.89878101,  0.013188707,   0.0623068142, 155.23629);

// Dummy materials
inline const auto Dispersion_Dummy = std::make_shared<OpticalMedium>(1.0, 0.0, 0.0, 0.12, 0.01, 100.0);
