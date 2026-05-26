#pragma once

#include "raylib.h"
#include "glm/glm.hpp"
#include "noise.hpp"
#include <vector>
#include <string>
#include <cmath>

struct Noise {
    std::function<float(glm::vec2 const&, int)> func;
    int nbOctave {6};
    float scale {5.f};
};

struct ImageGenerationData {
        const char* colorMaps[5] = { 
            "resources/cm_island_16.png","resources/cm_moutain_16.png",
            "resources/cm_elevation_16.png","resources/cm_rainbow_16.png",
            "resources/cm_mesa_32.png" 
        };

        static std::function<float(glm::vec2 const&, int)> noiseFunctions[2];
};

struct ImageGenerationParameters {
    int noiseSeed { 0 };
    bool isSeedRandom { true };
    int resolution { 256 };
    int selectedColorMap { 0 };
    bool colorMapLerp { true };
    Image noiseImage {}; //the accumulated noise image
    Image maskImage {}; //the mask
    std::vector<Noise> noiseStack {};
};

struct PointsGenerationParameters {
    bool isScaleRandom { true };
    glm::vec3 scaleOffset { 0.f, 0.5f, 0.f };
    bool isRotationRandom { true };
    glm::vec3 rotationOffset { 0.f, 360.f, 0.f };
    glm::vec2 heightTreshold = { 0.5f, 1.f };
    float poissonRadius { 1.0f };
    // TODO(student): add parameters for points generation (ex: poisson disk radius, etc).
};

struct AppContext {
    Camera camera {};

    // Store the heightmap as a raylib Image, which is easy to sample from CPU side when generating object positions.
    Image heightmapImage {};

    // This is the image we use for texturing the terrain. It can be the same as heightmapImage, but it doesn't have to be (for example, you could use a color image with RGB channels representing different material types instead of height).
    Image image {};

    // The generated texture from the image, stored here so we can easily bind it when generating the model.
    Texture2D texture {};

    glm::vec3 terrainSize { 16.0f, 5.0f, 16.0f };

    // The generated terrain mesh and model.
    Mesh mesh {};
    Model model {};

    std::vector<glm::vec3> objectPositions {};

    // A simple cube mesh and material we use to draw objects on the terrain.
    Mesh cube {};
    Material cubeMaterial {};
    float cubeScale { 0.1f };

    // Parameters for object positions generation
    PointsGenerationParameters pointsGenerationParameters;

    ImageGenerationData imageGenerationData;

    // Parameters for island generation
    ImageGenerationParameters imageGenerationParameters;
};

Matrix getTerrainCenteringMatrix(AppContext const& context);
float sampleHeightmap(Image heightmapImage, float u, float v);
void unload(AppContext& context);
void regenerateMeshFromImage(AppContext& context);