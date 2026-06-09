#include "randFloat.hpp"
#include <cstdlib>

float randF() {
    return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f);
}

float randF(float a, float b) {
    return a + (b - a) * randF();
}