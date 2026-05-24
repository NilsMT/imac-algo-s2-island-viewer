#pragma once

#include <glm/glm.hpp>
#include <functional>

#include "app.hpp"

float perlinNoise(glm::vec2 const& position);
float perlinNoiseSeeded(glm::vec2 const& position, int seed);
float simplexNoise(glm::vec2 const& position);
float simplexNoiseSeeded(glm::vec2 const& position, int seed);
float octaveNoise(AppContext& context,glm::vec2 const& position, int seed, std::function<float(glm::vec2 const&, int)> noiseFunction);