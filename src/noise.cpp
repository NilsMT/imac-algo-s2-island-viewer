
#include "noise.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <cstdint>
#include <functional>
#include <iostream>
using namespace std;

namespace {

    // Fast integer hash function
    std::uint32_t hashU32(std::uint32_t v)
    {
        v ^= v >> 16;
        v *= 0x7feb352du;
        v ^= v >> 15;
        v *= 0x846ca68bu;
        v ^= v >> 16;
        return v;
    }

    glm::vec2 seedToOffset2D(int seed)
    {
        std::uint32_t const base { static_cast<std::uint32_t>(seed) };
        std::uint32_t const hx { hashU32(base ^ 0x9e3779b9u) };
        std::uint32_t const hy { hashU32(base ^ 0x85ebca6bu) };

        float const fx { static_cast<float>(hx & 0x00ffffffu) / 16777216.0f };
        float const fy { static_cast<float>(hy & 0x00ffffffu) / 16777216.0f };

        // Large translation range so seeds land on very different 2D Perlin regions.
        return {
            fx * 4096.0f - 2048.0f,
            fy * 4096.0f - 2048.0f
        };
    }

} // namespace

float perlinNoise(glm::vec2 const& position) {
    return glm::perlin(position);
}

float perlinNoiseSeeded(glm::vec2 const& position, int seed) {
    // Cache computed offset because the same seed is used for many samples per frame.
    static int cachedSeed {};
    static glm::vec2 cachedOffset {};

    if (seed != cachedSeed) {
        cachedSeed = seed;
        cachedOffset = seedToOffset2D(seed);
    }

    return glm::perlin(position + cachedOffset);
}

// Remarque : je me suis fondé sur le code de l'article suivant : https://thebookofshaders.com/13/?lan=fr

float positionaveNoise(AppContext& context, glm::vec2 const& position, int seed, std::function<float(glm::vec2 const&, int)> noiseFunction) {
    // TODO(student): Implement octave/fractal noise accumulation.
    // Temporary fallback return directly from the provided noise function for testing.

    // noiseFunction est la fonction que l'on passe en entrée à notre fonction
    // en fait, permet de dire quel fonction de bruit on veut utiliser si on en a plusieurs
    // par exemple : perlinNoise

    // Initial values
    float value = 0.0;
    float amplitude = .5;
    glm::vec2 positionCopier = position;


    // Loop of octaves
    for (int i = 0; i < context.octaves; i++) {
        value += amplitude * noiseFunction(positionCopier + seedToOffset2D(i), seed);
        positionCopier.x *= 2.;
        positionCopier.y *= 2.;
        amplitude *= .5;
    }

    return value;
}
