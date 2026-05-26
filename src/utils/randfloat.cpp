#include "randFloat.hpp"
#include <cstdlib>

float randF() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float randF(float a, float b) {
    return a + (b - a) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}