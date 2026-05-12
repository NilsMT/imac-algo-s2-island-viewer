https://dsmte.github.io/Learn--cpp_programming/Subjects/IMAC2028/S2/sujet

# Répartition des tâches

- Bruit fractal (FBM)
- Générer heightmap + couleur : Nils
- Poisson disk sampling (2D) + placement en 3D : Matthieu
- Interface minimum
    - Regénérer la heightmap
    - Regénérer le mesh 3D
    - Regénérer le poisson disk sampling
    - Paramètres (seed, size, height range)
- Améliorations
    - Colormap builder
    - Algorithmes différents de heightmap
    - Variation placement d'objet (taille, rotation)
    - Placement d'objet avec des conditions (pentes, hauteur)
    - Liste de modèles 3D à placer of 3D models that can be placed
        - Importer un modèle 3D à la liste
    - Biomes (colormap, liste d'objets, influence sur la hauteur)
    - Connecter les îles avec des ponts, ou par la terre

> End product would run on heightmap, biome map, colormap (a gradient, maybe for each biome)

# Informations supplémentaires

> Extension pratique pour naviguer dans des endroits du code sur `VS Code` :<br>
> [Todo Tree](https://marketplace.visualstudio.com/items?itemName=Gruntfuggly.todo-tree)<br>
> Avec `NOTE:` et `SOURCE:` d'ajoutée dans [todo-tree.general.tags](vscode://settings/todo-tree.general.tags)
>
> Il est aussi recommandé d'utiliser l'`Outline` dans le panneau de droite de `VS Code`
