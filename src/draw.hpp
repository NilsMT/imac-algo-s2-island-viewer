#pragma once

#include "app.hpp"
#include "raylib.h"

void draw3DScene(AppContext& context);
void drawRaylibUI(AppContext& context);
void drawCones(AppContext const& context, Matrix const& terrainCentering);

void drawImGui(AppContext& context);