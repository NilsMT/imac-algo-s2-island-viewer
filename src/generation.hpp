#pragma once

#include "app.hpp"

std::vector<glm::vec2> generate2DPositions(PointsGenerationParameters const& params);

void generateObjectsPositions(AppContext& context);

float sampleHeightmap(Image heightmapImage, float u, float v);

void setupSeed(AppContext& context);

void generateHeightmap(AppContext& context);