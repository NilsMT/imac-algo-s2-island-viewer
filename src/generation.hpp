#pragma once

#include "app.hpp"

std::vector<glm::vec2> generate2DPositions(PointsGenerationParameters const& params);

void generateObjectsPositions(AppContext& context);

float sampleHeightmap(AppContext const& context, float u, float v);

void setupSeed(AppContext& context);

void generateHeightmap(AppContext& context);