#include "generation.hpp"

#include "noise.hpp"
#include "raylib.h"

#include "utils/pathUtils.hpp"
#include "utils/raylibUtils.hpp"
#include <algorithm> // for std::clamp
#include <iostream>

using namespace std;


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

float random(float min, float max)
{
   return rand() / static_cast<float>(RAND_MAX) * (max - min) + min;
}

std::vector<glm::vec2> generate2DPositionsPoissonDiskSampling([[maybe_unused]] PointsGenerationParameters const& params) {
    /*
        struct PointsGenerationParameters {
        bool isScaleRandom { true };
        glm::vec3 scaleOffset { 0.f, 0.5f, 0.f };
        bool isRotationRandom { true };
        glm::vec3 rotationOffset { 0.f, 360.f, 0.f };
        glm::vec2 heightTreshold = { 0.5f, 1.f };
        float poissonRadius { 1.0f };
        // TODO(student): add parameters for points generation (ex: poisson disk radius, etc).
        };
    */

    // Choisir un point actif aléatoire
    srand(time(0));

    float r = params.poissonRadius;
    float cellSize = r/sqrt(2);
    int k = 100; // k points candidats autour du point actif
    //int r = 5; // r est la distance minimale

    // Dans la vidéo https://www.youtube.com/watch?v=7WcmyxyFO7o, il y a une variable s'appelant sampleRegionSize.
    // Dans notre cas, nos points varie entre 0 et 1 pour simplifier, de ce fait, on peut remplacer la variable
    // sampleRegionSize par 1

    // int grid[1/cellSize][1/cellSize];
    int grilleLargeur = ceil(1.f / cellSize);
    int grilleHauteur = ceil(1.f / cellSize);

    std::cout
        << "r=" << r
        << " cellSize=" << cellSize
        << " largeur=" << grilleLargeur
        << '\n';

    vector<vector<float>> grille(grilleLargeur, vector<float>(grilleHauteur, 0.0f));


    std::vector<glm::vec2> points {};
    std::vector<glm::vec2> pointsActif {};


    // Choisir un point de départ aléatoire et l'ajouter à une liste de points actifs
    pointsActif.push_back({0.5, 0.5});
    points.push_back({0.5, 0.5});


    // Tant que la liste de points actifs n'est pas vide:
    while (!pointsActif.empty())
    {
        int indexPointActifAleatoire = rand() % pointsActif.size();
        glm::vec2 pointActifAleatoire = pointsActif[indexPointActifAleatoire];


        int nbPointsCandidatValide = 0;

        // Générer jusqu'à k points candidats autour de ce point actif
        for (int i = 0; i < k; ++i)
        {
            // (dans un anneau entre r et 2r, où r est la distance minimale
            // on génère un nombre aléatoire entre -2r et 2r autour du point actif
            // POUR X

            // on prend un point aléatoire entre r et 2r
            float radius = random(r, 2*r);
            float angle = random(0, 2*PI);

            // pour que ça soit plus propre, on va le placer de manière circulaire et non carré
            // et parce qu'on utilise cos et sin, ce point peut être situé tout autour du pointActif
            // ATTENTION x et y N'ONT RIEN À VOIR AVEC UNE POSITION SUR NOTRE GRILLE
            float x = cos(angle);
            float y = sin(angle);

            // on place un point aléatoirement autour du point actif
            glm::vec2 pointCandidat = pointActifAleatoire + radius * glm::vec2{x, y}; // on le place sur l'axe x et y (formule :  x = radius * cos(alpha)   et y = radius * cos(alpha))

            // on s'assure que le point candidat reste bien dans sa zone entre 0 et 1 en x et y.
            // sinon, on ne rentre pas dans le if ce qui nous force à passer au point candidat suivant
            if (x >= 0.f && x <= 1.f && y >= 0.f && y <= 1.f)
            {

                cout << "x: " << x << " y: " << y << endl;


                bool distanceAuMoinsRDeTousLesPoints = true;
                int j = 0;

                // On parcourt tous les points déjà générés
                while (distanceAuMoinsRDeTousLesPoints && j < points.size())
                {
                    glm::vec2 pointDejaGenererAComparer = points[j];
                    float distance = sqrt(pow((pointCandidat.x - pointDejaGenererAComparer.x), 2) + pow((pointCandidat.y - pointDejaGenererAComparer.y), 2));

                    // si le point généré n'est pas dans une distance d'au moins r avec le point candidat
                    if (distance < r)
                    {
                        distanceAuMoinsRDeTousLesPoints = false; // cela devient faux
                    }

                    j++;
                    cout << "distance: " << distance << " --- r: " << r << " --- j: " << j << " --- points.size(): " << points.size() << "--- pointsActif.size(): " << pointsActif.size() << "       AAAAAAAAAAAAAAAAAAAA" << endl;
                }


                // Si un point candidat est à une distance d'au moins r de tous les points déjà générés,
                // l'ajouter à la liste de points actifs et à la liste finale de points.
                // (donc si arrivé ici, distanceAuMoinsRDeTousLesPoints est toujours true)

                if (distanceAuMoinsRDeTousLesPoints)
                {
                    pointsActif.push_back(pointCandidat);
                    points.push_back(pointCandidat);

                    int grilleX = pointCandidat.x / cellSize;
                    int grilleY = pointCandidat.y / cellSize;

                    if (grilleX >= 0.f && grilleX <= 1.f && grilleY >= 0.f && grilleY <= 1.f)
                    {
                        cout << "=== " << grilleX << "(" << pointCandidat.x << " / " << cellSize << ")" << ", " << grilleY << endl;

                        grille[grilleX][grilleY] = points.size();
                        nbPointsCandidatValide++;
                    }
                }
            }
        }

        // Si après k essais (donc après notre boucle aucun point candidat n'est valide retirer le point actif de la liste
        // --> Jules pense que la condition n'est pas nécessaire et qu'on peut directectement retirer le point actif
        //      c'est pas faux que retenter en boucle 100 fois un point déjà étudié, n'a pas vraiment d'intérêt (même si c'est possiblement moins optimisé ainsi)
        pointsActif.erase(pointsActif.begin() + indexPointActifAleatoire);


        cout << pointsActif.size() << endl;

    }

    cout << "totototatatatututu " << points.size() << endl;
    return points;




    // TODO(student): implement Poisson disk sampling to replace the above naive random generation ---> OK
    // points output should be in [0..1] range, where (0,0) is one corner of the terrain and (1,1) is the opposite corner, so they can be easily scaled to terrain size and sampled from heightmap.
    // return positions;
}


void generateObjectsPositions(AppContext& context) {
    // std::vector<glm::vec2> const positions {generate2DPositions(context.pointsGenerationParameters)}; // original
    std::vector<glm::vec2> const positions {generate2DPositionsPoissonDiskSampling(context.pointsGenerationParameters)};


    context.objectPositions.clear();
    context.objectPositions.reserve(positions.size());
    for (glm::vec2 const& p : positions)
    {
        float z = sampleHeightmap(context, p.x, p.y);

        if (
            context.pointsGenerationParameters.heightTreshold[0] < z &&
            z < context.pointsGenerationParameters.heightTreshold[1]
        ) {
            context.objectPositions.emplace_back(
                p.x, // x
                p.y, // y
                z
            );
        }
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

            // return (perlinNoiseSeeded(p * context.imageGenerationParameters.noiseScale, context.imageGenerationParameters.noiseSeed) * 0.5f + 0.5f);
            glm::vec2 position = p * context.imageGenerationParameters.noiseScale;
            return (octaveNoise(context, position, context.imageGenerationParameters.noiseSeed, perlinNoiseSeeded) * 0.5f + 0.5f);

        });


    //NOTE: loaded color map way
    //open color map
    std::filesystem::path colorPath = pathUtils::make_absolute_path(
        context.imageGenerationData.colorMaps[context.imageGenerationParameters.selectedColorMap]
    );
    Image colorMap = LoadImage(colorPath.string().c_str());

    context.image = TransformImage<float, Color>(context.heightmapImage, [&](float const& v, int const, int const) {
        //clamp v + scale to image width like [0, 15]
        float lerpv = std::clamp(v, 0.0f, 1.0f) * (colorMap.width - 1);

        //get near pixels
        int x0 = int(std::floor(lerpv));
        int x1 = int(std::ceil(lerpv));

        //get cols
        Color color1 = GetImageColor(colorMap, x0, 0);
        Color color2 = GetImageColor(colorMap, x1, 0);

        //lerp
        float lerpFactor = lerpv - x0; // [0, 1]

        if (context.imageGenerationParameters.colorMapLerp) {
            return ColorLerp(color1, color2, lerpFactor);
        } else {
            return color2;
        }
        
    }, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    context.texture = LoadTextureFromImage(context.image);
    if (context.model.meshCount > 0) {
        context.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = context.texture;
    }
}