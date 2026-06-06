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


    // TODO(student): implement Poisson disk sampling to replace the above naive random generation ---> OK
    // points output should be in [0..1] range, where (0,0) is one corner of the terrain and (1,1) is the opposite corner, so they can be easily scaled to terrain size and sampled from heightmap.
    // return positions;

    // Choisir un point actif aléatoire
    srand(time(0));

    float r = params.poissonRadius;
    float cellSize = r/sqrt(2);
    int k = 30; // k points candidats autour du point actif
    //int r = 5; // r est la distance minimale

    // Dans la vidéo https://www.youtube.com/watch?v=7WcmyxyFO7o, il y a une variable s'appelant sampleRegionSize.
    // Dans notre cas, nos points varie entre 0 et 1 pour simplifier, de ce fait, on peut remplacer la variable
    // sampleRegionSize par 1


    // ETAPE 0
    int grilleLargeur = ceil(1.f / cellSize);
    int grilleHauteur = ceil(1.f / cellSize);

    std::cout
        << "r=" << r
        << " cellSize=" << cellSize
        << " largeur=" << grilleLargeur
        << '\n';



    vector<vector<int>> grille(grilleLargeur, vector<int>(grilleHauteur, -1));


    std::vector<glm::vec2> points {};
    std::vector<glm::vec2> pointsActif {}; // liste des points avec les quels ont travail

    // ETAPE 1
    // Choisir un point de départ aléatoire et l'ajouter à une liste de points actifs
    float x = (rand() % 100) * 0.01;
    float y = (rand() % 100) * 0.01;
    pointsActif.push_back({x, y});
    points.push_back({x, y});

    // on fait du hachage afin que les valeurs x-y proches tombe dans la même case
    int grilleX = x / cellSize;
    int grilleY = y / cellSize;
    grille[grilleX][grilleY] = 0; // 0 car c'est le premier élément de notre index



    while (!pointsActif.empty())
    {
        int indexPointActifAleatoire = rand() % pointsActif.size();
        glm::vec2 pointActifAleatoire = pointsActif[indexPointActifAleatoire];

        bool candidatValide = false;
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
            if (pointCandidat.x >= 0.f && pointCandidat.x < 1.f && pointCandidat.y >= 0.f && pointCandidat.y < 1.f)
            {
                // cout << pointCandidat.x << " -- " << pointCandidat.y << endl;

                bool distanceAuMoinsRDeTousLesPoints = true;

                // Avec Bridson, pas besoin de tout parcourir, on va juste regarder les cases autour de notre pointCandidat
                // pour voir s'il y a de la place.
                // on va dire qu'on va regarder dans un rayon de deux cases.
                int candidatGrilleX = pointCandidat.x / cellSize;
                int candidatGrilleY = pointCandidat.y / cellSize;

                int rVerif = 2; // rayon verif
                int minCandidatGrilleX = (candidatGrilleX-rVerif >= 0) ? candidatGrilleX-rVerif : 0;
                int maxCandidatGrilleX = (candidatGrilleX+rVerif < grilleLargeur-1) ? candidatGrilleX+rVerif : grilleLargeur-1;
                int minCandidatGrilleY = (candidatGrilleY-rVerif >= 0) ? candidatGrilleY-rVerif : 0;
                int maxCandidatGrilleY = (candidatGrilleY+rVerif < grilleHauteur-1) ? candidatGrilleY+rVerif : grilleHauteur-1;

                for (int a = minCandidatGrilleX; a <= maxCandidatGrilleX; a++) {
                    for (int o = minCandidatGrilleY; o <= maxCandidatGrilleY; o++) {
                        if (grille[a][o] != -1)
                        {
                            // vu que grille[a][o] correspond à l'index du point déjà généré que l'on doit comparer
                            glm::vec2 pointDejaGenererAComparer = points[grille[a][o]];
                            float distance = sqrt(pow((pointCandidat.x - pointDejaGenererAComparer.x), 2) + pow((pointCandidat.y - pointDejaGenererAComparer.y), 2));

                            // si le point généré n'est pas dans une distance d'au moins r avec le point candidat
                            if (distance < r)
                            {
                                distanceAuMoinsRDeTousLesPoints = false; // cela devient faux
                            }

                        }
                    }
                }


                // Si un point candidat n'a pas de voisin trop proche,
                // l'ajouter à la liste de points actifs et à la liste finale de points.
                // (donc si arrivé ici, distanceAuMoinsRDeTousLesPoints est toujours true)

                if (distanceAuMoinsRDeTousLesPoints)
                {
                    pointsActif.push_back(pointCandidat);
                    points.push_back(pointCandidat);

                    int grilleX = pointCandidat.x / cellSize;
                    int grilleY = pointCandidat.y / cellSize;

                    grille[grilleX][grilleY] = points.size() - 1; // -1 car grille contient les indexs de la liste point, donc on indique l'index du point candidat
                    candidatValide = true ;
                    break;

                }

            }



        }
        // Si après k essais (donc après notre boucle aucun point candidat n'est valide retirer le point actif de la liste
        // --> Jules pense que la condition n'est pas nécessaire et qu'on peut directectement retirer le point actif
        //      c'est pas faux que retenter en boucle 100 fois un point déjà étudié, n'a pas vraiment d'intérêt (même si c'est possiblement moins optimisé ainsi)
        // MAIS vu que c'est demandé dans l'énoncé, on va le faire quand même

        if (!candidatValide) {
            pointsActif.erase(pointsActif.begin() + indexPointActifAleatoire);
        }
    }


    return points;
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