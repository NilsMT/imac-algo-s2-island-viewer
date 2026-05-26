https://dsmte.github.io/Learn--cpp_programming/Subjects/IMAC2028/S2/sujet

# Répartition des tâches

> 🚧 : En cours ✅ : Fini ❌ : À faire
> ♒ : Amélioration supplémentaire ☑️ : Amélioration de l'énoncé

| Fait | Catégorie       | Tâche                                                               | Note        | Qui      |
| ---- | --------------- | ------------------------------------------------------------------- | ----------- | -------- |
| ✅   | -               | Bruit fractal (FBM)                                                 |             | Matthieu |
| 🚧   | -               | Générer heightmap + couleur                                         | TODO: mask  | Nils     |
| ❌   | -               | Poisson disk sampling (2D) + placement en 3D                        |             | Matthieu |
| ✅   | 🖥️ IHM          | Regénérer la heightmap                                              |             | Nils     |
| ✅   | 🖥️ IHM          | Regénérer le mesh 3D                                                |             | Nils     |
| ❌   | 🖥️ IHM          | Regénérer le poisson disk sampling                                  |             | -        |
| ✅   | 🖥️ IHM          | Paramètres (seed, size, height range)                               |             | Nils     |
| ❌   | ✨ Amélioration | Colormap builder                                                    |             | -        |
| ❌   | ✨ Amélioration | Coloration avancé (ex: en fonction de la pente, type de biome, etc) |             | -        |
| 🚧   | ✨ Amélioration | Algorithmes différents bruit (Simplex, Worley, etc)                 | fix simplex | -        |
| ✅   | ✨ Amélioration | Pile d'Algorithmes de bruit                                         | fix simplex | -        |
| ✅   | ✨ Amélioration | Variation placement d'objet (taille, rotation)                      |             | -        |
| ✅   | ✨ Amélioration | Placement d'objet avec des conditions (pentes, hauteur)             |             | -        |
| ❌   | ✨ Amélioration | Importer un modèles 3D (libre de droits)à la place du cube          |             | -        |
| ❌   | ✨ Amélioration | Liste de modèles 3D (libre de droits) à placer avec conditions      |             | -        |
| ❌   | ✨ Amélioration | Biomes (colormap, liste d'objets, influence sur la hauteur, bruit)  |             | -        |
| ❌   | ✨ Amélioration | Connecter les îles avec des ponts, ou par la terre                  |             | -        |
| ❌   | ✨ Amélioration | Génération de différentes formes d'îles (au moins 3)                |             | -        |

# Informations supplémentaires

> Extension pratique pour naviguer dans des endroits du code sur `VS Code` :<br>
> [Todo Tree](https://marketplace.visualstudio.com/items?itemName=Gruntfuggly.todo-tree)<br>
> Avec `NOTE:` et `SOURCE:` d'ajoutée dans [todo-tree.general.tags](vscode://settings/todo-tree.general.tags)
>
> Il est aussi recommandé d'utiliser l'`Outline` dans le panneau de droite de `VS Code`

# L'échec de l'ajout du Diamond Square

de base la noise stack c'était ça:

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

J'ai voulu faire en sorte qu'il y ai 2 type de bruits :

- Celui qui se base sur un algo et renvoie une valeur
- Celui qui se base sur une matrice généré par un algo avant et renvoie une valeur

Cependant pour maintenir la cohérence dans la struct `Noise` il fallait que le type de `func` dans la struct `Noise` soit une `std::variant` de plusieurs signatures de fonctions de bruit (celles qui génère une matrice OU revoie une valeur).

Et après il fallait des conditions dans le code pour récupérer la valeur en faisant des comparaisons sur le type de Bruit (matrice ou fonction) et la manière de récupérer la valeur (sampleHeightmap OU noise.func)
