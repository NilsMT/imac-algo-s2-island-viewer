#pragma once

#include <glm/glm.hpp>
#include <functional>

#include "app.hpp"

float perlinNoise(glm::vec2 const& position);
float perlinNoiseSeeded(glm::vec2 const& position, int seed);

// float octaveNoise(glm::vec2 const& position, std::function<float(glm::vec2 const&)> noiseFunction);
float positionaveNoise(AppContext& context,glm::vec2 const& position, int seed, std::function<float(glm::vec2 const&, int)> noiseFunction);