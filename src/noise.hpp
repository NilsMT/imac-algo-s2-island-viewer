#pragma once

#include <glm/glm.hpp>
#include <functional>

// Forward declaration so noise functions can reference AppContext
struct AppContext;

float perlinNoise(glm::vec2 const& position);
float perlinNoiseSeeded(glm::vec2 const& position, AppContext const& context);

void initPerm();
float simplexNoise(glm::vec2 const& position);
float simplexNoiseSeeded(glm::vec2 const& position, AppContext const& context);

float noneNoise(glm::vec2 const& position, AppContext const& context);

float gaussianNoise(glm::vec2 const& position, AppContext const& context);

float octaveNoise(int nbOctave,glm::vec2 const& position, AppContext const& context, std::function<float(glm::vec2 const&, AppContext const&)> noiseFunction);