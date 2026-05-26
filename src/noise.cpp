
#include "noise.hpp"
#include "utils/randFloat.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <cstdint>
#include <functional>
#include <algorithm> 


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
    return glm::perlin(position) * 0.5f + 0.5f;;
}

//perlin

float perlinNoiseSeeded(glm::vec2 const& position, int seed) {
    // Cache computed offset because the same seed is used for many samples per frame.
    static int cachedSeed {};
    static glm::vec2 cachedOffset {};

    if (seed != cachedSeed) {
        cachedSeed = seed;
        cachedOffset = seedToOffset2D(seed);
    }

    return perlinNoise(position + cachedOffset);
}


float octaveNoise(int nbOctave, glm::vec2 const& position, int seed, std::function<float(glm::vec2 const&, int)> noiseFunction) {
    // TODO(student): Implement octave/fractal noise accumulation.

    // noiseFunction est la fonction que l'on passe en entrée à notre fonction
    // en fait, permet de dire quel fonction de bruit on veut utiliser si on en a plusieurs
    // par exemple : perlinNoise

    // Initial values
    float value = 0.0;
    float amplitude = .5;
    glm::vec2 pos = position;


    // Loop of octaves
    for (int i = 0; i < nbOctave; i++) {
        value += amplitude * noiseFunction(pos + seedToOffset2D(i), seed);
        pos.x *= 2.;
        pos.y *= 2.;
        amplitude *= .5;
    }

    return value;
}



//simplex from https://www.researchgate.net/publication/216813608_Simplex_noise_demystified

static const glm::vec2 grad2[12] = {
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    {1, 0}, {-1, 0}, {1, 0},  {-1, 0},
    {0, 1}, {0, -1}, {0, 1},  {0, -1}
};

static const int p[256] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,
    125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
    105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,
    200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,
    126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,
    70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,
    246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,
    181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
    128,195,78,66,215,61,156,180
};

int perm[512];
constexpr void initPerm() {
    for (int i = 0; i < 512; i++) {
        perm[i] = p[i & 255];
    }
}

float dot(const glm::vec2& g, float x, float y) {
    return g.x * x + g.y * y;
}

float simplexNoise(glm::vec2 const& position) {
    const float F2 = 0.5f * (sqrtf(3.0f) - 1.0f);
    const float G2 = (3.0f - sqrtf(3.0f)) / 6.0f;

    float s = (position.x + position.y) * F2;
    int i = (int)floorf(position.x + s);
    int j = (int)floorf(position.y + s);

    float t = (i + j) * G2;
    float x1 = position.x - (i - t);
    float y1 = position.y - (j - t);

    int i1 = (x1 > y1) ? 1 : 0;
    int j1 = (x1 > y1) ? 0 : 1;

    float x2 = x1 - i1 + G2;
    float y2 = y1 - j1 + G2;
    float x3 = x1 - 1.0f + 2.0f * G2;
    float y3 = y1 - 1.0f + 2.0f * G2;

    int ii = i & 255;
    int jj = j & 255;
    int gi0 = perm[ii +     perm[jj    ]] % 12;
    int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
    int gi2 = perm[ii + 1  + perm[jj + 1 ]] % 12;

    float n0 = 0.0f, n1 = 0.0f, n2 = 0.0f;

    float t0 = 0.5f - x1*x1 - y1*y1;
    if (t0 >= 0.0f) { t0 *= t0; n0 = t0 * t0 * dot(grad2[gi0], x1, y1); }

    float t1 = 0.5f - x2*x2 - y2*y2;
    if (t1 >= 0.0f) { t1 *= t1; n1 = t1 * t1 * dot(grad2[gi1], x2, y2); }

    float t2 = 0.5f - x3*x3 - y3*y3;
    if (t2 >= 0.0f) { t2 *= t2; n2 = t2 * t2 * dot(grad2[gi2], x3, y3); }

    return (70.0f * (n0 + n1 + n2)) * 0.5f + 0.5f;
}

float simplexNoiseSeeded(glm::vec2 const& position, int seed) {
    // Cache computed offset because the same seed is used for many samples per frame.
    static int cachedSeed {};
    static glm::vec2 cachedOffset {};

    if (seed != cachedSeed) {
        cachedSeed = seed;
        cachedOffset = seedToOffset2D(seed);
    }

    return simplexNoise(position + cachedOffset);
}


////////////////////////////////////////////////
////////////////////////////////////////////////

// ATTROCIOUS DIAMOND SQUARE INTEGRATION ATTEMPTS
// NOT DONE

// https://github.com/NilsMT/imac-wk-prog-algo-1/blob/main/EXOS.md#-diamond-square

////////////////////////////////////////////////
////////////////////////////////////////////////

//config (you can change these)
const int n = 8;
const float hMin = 1.0f;
const float hMax = 8.0f;

//globals (don't touch)
float mapMin = 0;
float mapMax = 0;
const size_t mapSize = pow(2, n) + 1;
using arrayT = std::array<std::array<float, mapSize>, mapSize>;

void fillArr(arrayT& arr, float value) {
    for (auto& row : arr) {
        for (auto& item : row) {
            item = value;
        }
    }
}

void updateMapMinMax(float value) {
    if (value < mapMin) {
        mapMin = value;
    }
    if (value > mapMax) {
        mapMax = value;
    }
}

/////////////////////////////////////

void square_step(arrayT& arr, int size, int half, float r) {
    for (int iy = 0; iy < mapSize - 1; iy += size) {
        for (int ix = 0; ix < mapSize - 1; ix += size) {
            float x = arr[iy][ix]; //TL
            updateMapMinMax(x);
            float y = arr[iy][ix + size]; //TR
            updateMapMinMax(y);
            float z = arr[iy + size][ix]; //BL
            updateMapMinMax(z);
            float w = arr[iy + size][ix + size]; //BR
            updateMapMinMax(w);
            float c = (x + y + z + w) / 4; //C
            c += randF(-r, r);
            updateMapMinMax(c);
            arr[iy + half][ix + half] = c;
        }
    }
}

void diamond_step(arrayT& arr, int size, int half, float r) {
    for (int iy = 0; iy < mapSize; iy += half) {
        for (int ix = (iy + half) % size; ix < mapSize; ix += size) {
            int count = 0;
            float sum = 0;

            //T
            if (iy - half >= 0) {
                sum += arr[iy - half][ix];
                count++;
            }
            //L
            if (ix - half >= 0) {
                sum += arr[iy][ix - half];
                count++;
            }
            //R
            if (ix + half < mapSize) {
                sum += arr[iy][ix + half];
                count++;
            }
            //B
            if (iy + half < mapSize) {
                sum += arr[iy + half][ix];
                count++;
            }

            float c = sum / count;
            c += randF(-r, r);
            updateMapMinMax(c);
            arr[iy][ix] = c;
        }
    }
}

Image diamondSquare(float r) {
    // reset globals
    mapMin = std::numeric_limits<float>::max();
    mapMax = std::numeric_limits<float>::lowest();

    arrayT height_map = {};
    fillArr(height_map, 0);

    height_map[0][0]                       = randF(hMin, hMax);
    height_map[0][mapSize - 1]             = randF(hMin, hMax);
    height_map[mapSize - 1][0]             = randF(hMin, hMax);
    height_map[mapSize - 1][mapSize - 1]   = randF(hMin, hMax);

    int chunk_size = mapSize - 1;
    while (chunk_size > 1) {
        int half = chunk_size / 2;
        square_step(height_map, chunk_size, half, r);
        diamond_step(height_map, chunk_size, half, r);
        chunk_size /= 2;
        r /= 2;
    }

    // normalize into a flat float array then wrap in a Raylib Image
    float* data = new float[mapSize * mapSize];
    for (int y = 0; y < mapSize; ++y) {
        for (int x = 0; x < mapSize; ++x) {
            float v = (height_map[y][x] - mapMin) / (mapMax - mapMin);
            data[y * mapSize + x] = std::clamp(v, 0.0f, 1.0f);
        }
    }

    Image img;
    img.data    = data;
    img.width   = static_cast<int>(mapSize);
    img.height  = static_cast<int>(mapSize);
    img.mipmaps = 1;
    img.format  = PIXELFORMAT_UNCOMPRESSED_R32;
    return img;
}
