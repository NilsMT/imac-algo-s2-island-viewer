#include "generation.hpp"

#include "noise.hpp"
#include "raylib.h"

#include "utils/pathUtils.hpp"
#include "utils/raylibUtils.hpp"
#include <algorithm> // for std::clamp


std::vector<glm::vec2> generate2DPositions([[maybe_unused]] PointsGenerationParameters const& params) {
    std::vector<glm::vec2> positions {};

    positions.reserve(1000);
    // Naive random generation
    for (int i {0}; i < 1000; ++i)
    {
        positions.emplace_back(
            static_cast<float>(GetRandomValue(0, INT_MAX)) / static_cast<float>(INT_MAX),
            static_cast<float>(GetRandomValue(0, INT_MAX)) / static_cast<float>(INT_MAX)
        );
    }

    // TODO(student): implement Poisson disk sampling to replace the above naive random generation
    // points output should be in [0..1] range, where (0,0) is one corner of the terrain and (1,1) is the opposite corner, so they can be easily scaled to terrain size and sampled from heightmap.
    return positions;
}

void generateObjectsPositions(AppContext& context) {
    std::vector<glm::vec2> const positions {generate2DPositions(context.pointsGenerationParameters)};

    context.objectPositions.clear();
    context.objectPositions.reserve(positions.size());
    for (glm::vec2 const& p : positions)
    {
        context.objectPositions.emplace_back(
            p.x, // x
            p.y, // y
            // sample height from heightmap for each point (asuming positions are normalized in [0..1] range)
            sampleHeightmap(context, p.x, p.y)
        );
    }
    // TODO(student): extension - filter positions by sampled height range.
}

float sampleHeightmap(AppContext const& context, float u, float v)
{
    if (!context.heightmapImage.data || context.heightmapImage.width <= 0 || context.heightmapImage.height <= 0) return 0.0f;

    int const px = std::clamp(static_cast<int>(u * static_cast<float>(context.heightmapImage.width - 1)), 0, context.heightmapImage.width - 1);
    int const py = std::clamp(static_cast<int>(v * static_cast<float>(context.heightmapImage.height - 1)), 0, context.heightmapImage.height - 1);

    // If the heightmap is in R32 format, we can directly read the height value as a float. 
    if (context.heightmapImage.format == PIXELFORMAT_UNCOMPRESSED_R32)
    {
        float const* heightData = static_cast<float const*>(context.heightmapImage.data);
        int const idx = py * context.heightmapImage.width + px;
        return std::clamp(heightData[idx], 0.0f, 1.0f);
    }

    // Otherwise, we assume it's in a color format and we read the red channel as height (with normalization from [0..255] to [0..1]).
    Color const c = GetImageColor(context.heightmapImage, px, py);
    return static_cast<float>(c.r)/255.0f;
}

void setupSeed(AppContext& context) {
    //random seed or not
    if (context.imageGenerationParameters.isSeedRandom) {
        context.imageGenerationParameters.noiseSeed = rand();
    }
}

void generateHeightmap(AppContext& context) {

    if (context.texture.id > 0) {
        UnloadTexture(context.texture);
        context.texture = {};
    }

    if(context.image.data) {
        UnloadImage(context.image);
        context.image = {};
    }

    if (context.heightmapImage.data) {
        UnloadImage(context.heightmapImage);
        context.heightmapImage = {};
    }

    int const resolution = std::max(1, context.imageGenerationParameters.resolution);

    context.heightmapImage = GenImageFromNoiseFunction<float>(resolution, resolution, PIXELFORMAT_UNCOMPRESSED_R32,
        [&](glm::vec2 const& p)->float {
            // TODO(student): implement stack based noise and island mask

            return (perlinNoiseSeeded(p * context.imageGenerationParameters.noiseScale, context.imageGenerationParameters.noiseSeed) * 0.5f + 0.5f);
        });


    //NOTE: loaded color map way
    //open color map
    std::filesystem::path colorPath = pathUtils::make_absolute_path(
        context.imageGenerationData.colorMaps[context.imageGenerationParameters.selectedColorMap]
    );
    Image colorMap = LoadImage(colorPath.string().c_str());

    context.image = TransformImage<float, Color>(context.heightmapImage, [&](float const& v, int const, int const) {
        //clamp v + scale to image width
        float lerpv = std::clamp(v, 0.0f, 1.0f) * (colorMap.width - 1);

        //clamp lerpv
        lerpv = std::clamp(lerpv, 0.0f, static_cast<float>(colorMap.width - 1));

        //get near pixels
        int x0 = int(std::floor(lerpv));
        int x1 = int(std::ceil(lerpv));

        //clamp idx
        x0 = std::clamp(x0, 0, colorMap.width - 1);
        x1 = std::clamp(x1, 0, colorMap.width - 1);

        //get cols
        Color color1 = GetImageColor(colorMap, x0, 0);
        Color color2 = GetImageColor(colorMap, x1, 0);

        //lerp
        float lerpFactor = lerpv - x0; // [0, 1]
        return ColorLerp(color1, color2, lerpFactor);
    }, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    context.texture = LoadTextureFromImage(context.image);
    if (context.model.meshCount > 0) {
        context.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = context.texture;
    }
}