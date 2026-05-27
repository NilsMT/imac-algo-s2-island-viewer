https://dsmte.github.io/Learn--cpp_programming/Subjects/IMAC2028/S2/sujet

- [Répartition des tâches](#répartition-des-tâches)
- [Informations supplémentaires](#informations-supplémentaires)
- [L'échec de l'ajout du Diamond Square](#léchec-de-lajout-du-diamond-square)
    - [Avant](#avant)
    - [Le problème](#le-problème)
    - [Ce qu'on a fait](#ce-quon-a-fait)
    - [Pourquoi c'est pas idéal](#pourquoi-cest-pas-idéal)

# Répartition des tâches

> 🚧 : En cours ✅ : Fini ❌ : À faire
> ♒ : Amélioration supplémentaire ☑️ : Amélioration de l'énoncé

| Fait | Catégorie | En + | Tâche                                                               | Note        | Qui      |
| ---- | --------- | ---- | ------------------------------------------------------------------- | ----------- | -------- |
| ✅   | -         |      | Bruit fractal (FBM)                                                 |             | Matthieu |
| 🚧   | -         |      | Générer heightmap + couleur                                         | TODO: mask  | Nils     |
| ❌   | -         |      | Poisson disk sampling (2D) + placement en 3D                        |             | Matthieu |
| ✅   | 🖥️ IHM    |      | Regénérer la heightmap                                              |             | Nils     |
| ✅   | 🖥️ IHM    |      | Regénérer le mesh 3D                                                |             | Nils     |
| ❌   | 🖥️ IHM    |      | Regénérer le poisson disk sampling                                  |             | -        |
| ✅   | 🖥️ IHM    |      | Paramètres (seed, size, height range)                               |             | Nils     |
| ❌   | -         | ☑️   | Colormap builder                                                    |             | -        |
| ❌   | -         | ☑️   | Coloration avancé (ex: en fonction de la pente, type de biome, etc) |             | -        |
| 🚧   | -         | ☑️   | Algorithmes différents bruit (Simplex, Worley, etc)                 | fix simplex | -        |
| ✅   | -         | ☑️   | Pile d'Algorithmes de bruit                                         | fix simplex | -        |
| ✅   | -         | ☑️   | Variation placement d'objet (taille, rotation)                      |             | -        |
| ✅   | -         | ☑️   | Placement d'objet avec des conditions (pentes, hauteur)             |             | -        |
| ❌   | -         | ☑️   | Importer un modèles 3D (libre de droits)à la place du cube          |             | -        |
| ❌   | -         | ☑️   | Liste de modèles 3D (libre de droits) à placer avec conditions      |             | -        |
| ❌   | -         | ☑️   | Biomes (colormap, liste d'objets, influence sur la hauteur, bruit)  |             | -        |
| ❌   | -         | ☑️   | Connecter les îles avec des ponts, ou par la terre                  |             | -        |
| ❌   | -         | ☑️   | Génération de différentes formes d'îles (au moins 3)                |             | -        |

# Informations supplémentaires

> Extension pratique pour naviguer dans des endroits du code sur `VS Code` :<br>
> [Todo Tree](https://marketplace.visualstudio.com/items?itemName=Gruntfuggly.todo-tree)<br>
> Avec `NOTE:` et `SOURCE:` d'ajoutée dans [todo-tree.general.tags](vscode://settings/todo-tree.general.tags)
>
> Il est aussi recommandé d'utiliser l'`Outline` dans le panneau de droite de `VS Code`

# L'échec de l'ajout du Diamond Square

Le code de l'échec est dans la branche [diamond-square-attempt](https://github.com/NilsMT/imac-algo-s2-island-viewer/tree/diamond-square-attempt)

## Avant

La noise stack était simple : chaque bruit est une fonction qui prend une position et une seed, et renvoie un float.

```cpp
struct Noise {
    std::function<float(glm::vec2 const&, int)> func;
    int nbOctave {6};
    float scale {5.f};
};

struct ImageGenerationData {
    ...
    static std::function<float(glm::vec2 const&, int)> noiseFunctions[2];
};

struct ImageGenerationParameters {
    ...
    std::vector<Noise> noiseStack {};
};
```

## Le problème

Le Diamond Square est un algorithme **matriciel** : il génère une heightmap entière (`Image`) d'un coup, plutôt que de renvoyer une valeur par position comme `Perlin` et `Simplex`. Il fallait donc supporter deux types de bruits dans la même stack :

- **Fonction** : `float(glm::vec2, int)`, point par point (Simplex, Perlin...)
- **Matrice** : `Image( params de l'algo )`, qu'on sample ensuite (mais chaque algo matriciel peut avoir une signature entrante différente)

## Ce qu'on a fait

Pour garder une struct `Noise` unique, on a utilisé un `std::variant` pour le champ `func`, avec une enum `NoiseType` pour savoir lequel des deux types est actif.

```cpp
enum NoiseType {
    FUNCTION,
    MATRIX,
};

using NoiseFunction = std::variant
    std::function<float(glm::vec2 const&, int)>, // PERLIN + SIMPLEX
    std::function<Image(float)>                  // DS
    //les autres algo matrice de type Image( leurs paramètres )
>;

struct Noise {
    NoiseType type;
    NoiseFunction func;
    int nbOctave {2};
    float scale {5.f};
};

struct ImageGenerationData {
    ...
    static std::variant
        std::function<float(glm::vec2 const&, int)>,
        std::function<Image(float)>
    > noiseFunctions[3];
    const NoiseType noiseFunctionsTypes[2] = {
        NoiseType::FUNCTION,
        NoiseType::MATRIX
    };
};

struct ImageGenerationParameters {
    ...
    std::vector<Noise> noiseStack {};
};

struct App {
    ...
    std::vector<Image> noiseMatrixStack {}; // résultats des algos matriciels
    ...
};
```

## Pourquoi c'est pas idéal

- Le variant ne peut pas couvrir toutes les signatures possibles des algos matriciels : il faudrait ajouter à la main chaque nouvelle signature dans `NoiseFunction`
- Il faut jongler avec `std::get` / `std::get_if` partout où on utilise un bruit
- L'interface ImGui doit s'adapter manuellement à chaque type de bruit (ou alors on s'adapte à la signature de la fonction de bruit matriciel)
- Une `noiseMatrixStack` séparée dans `AppContext` stocke les résultats des algos matriciels, ce qui crée une désynchronisation potentielle avec la `noiseStack`
    > une option plus simple aurait été de générer la matrice pour chaque position au lieu de tout stocker une fois, mais niveau performance c'est catastrophique
