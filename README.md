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

L'échec (cuisant) de l'ajout du diamond Square est dispo dans ces fichiers :

- [./src/app.hpp](./src/app.hpp)
- [./src/app.cpp](./src/app.cpp)
- [./src/noise.hpp](./src/noise.hpp)
- [./src/noise.cpp](./src/noise.cpp)
- [./src/generation.cpp](./src/generation.cpp)
