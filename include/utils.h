#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <limits>
#include <memory>
#include <algorithm>
#include <cstdlib>

// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;
using std::min;
using std::max;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

/**
 * @brief Returns a random real in [0,1).
 * 
 * @return double 
 */
inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

/**
 * @brief Returns a random real in [min,max).
 * 
 * @return double 
 */
inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

/**
 * @brief Returns a random integer in [min,max].
 * 
 * @return int 
 */
inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}

/**
 * @brief Linearly interpolates between two values.
 * 
 */
template <typename T> T lerp(const T& a, const T& b, double t) {
    return (1.0 - t) * a + t * b;
}

// Common Headers

#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif