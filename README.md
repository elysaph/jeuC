# Snake Game en C

Jeu de Snake développé en C avec la bibliothèque [SDL2](https://www.libsdl.org/). Le joueur dirige un serpent sur une grille, collecte des pommes, augmente son score et peut sauvegarder ou reprendre une partie.

Ce document présente l’organisation du projet et ses principaux choix techniques. Il peut servir de base à une présentation du projet.

## 1. Présentation du projet

Le programme ouvre une fenêtre SDL2 de **800 × 600 pixels**. La zone de jeu est découpée en cellules de **20 × 20 pixels**, soit une grille de :

- **40 colonnes** ;
- **30 lignes** ;
- **1 200 cellules au total**.

Le serpent commence au centre de la grille avec une longueur de trois cellules et se déplace automatiquement. Le joueur peut changer sa direction, sa vitesse et sauvegarder l’état courant de la partie.

### Objectifs du jeu

- Diriger le serpent avec les flèches directionnelles.
- Manger les pommes rouges.
- Gagner **10 points par pomme**.
- Éviter les murs et le corps du serpent.
- Faire grandir le serpent après avoir mangé les trois pommes présentes.
- Atteindre le meilleur score possible avant le `Game Over`.

## 2. Structure du projet

```text
.
├── main.c                  # Point d'entrée, initialisation SDL2 et boucle principale
├── game.c                  # Règles du jeu, déplacements, collisions et rendu
├── game.h                  # Constantes, structures et prototypes du module jeu
├── save.c                  # Lecture et écriture d'une sauvegarde
├── save.h                  # Interface du module de sauvegarde
├── Makefile                # Compilation, exécution et nettoyage
├── .vscode/                # Configuration locale de VS Code
├── snake_save.dat          # Sauvegarde binaire générée à l'exécution
├── snake_game.exe          # Binaire Windows généré
├── main.o                  # Objet intermédiaire de compilation
├── game.o                  # Objet intermédiaire de compilation
└── save.o                  # Objet intermédiaire de compilation
```

Les fichiers `.o`, l'exécutable et `snake_save.dat` sont des fichiers générés ou utilisés à l'exécution. Les fichiers `.c` et `.h` constituent le code source du projet.

### Rôle des fichiers source

#### `main.c`

Ce fichier orchestre l'application :

1. initialise SDL2 ;
2. crée la fenêtre et le renderer ;
3. affiche le menu texte de démarrage ;
4. initialise une nouvelle partie ou charge une sauvegarde ;
5. exécute la boucle principale ;
6. libère les ressources SDL2 à la fermeture.

#### `game.h`

Ce fichier centralise les constantes de configuration et les structures de données :

- `Point` représente une position `(x, y)` sur la grille ;
- `Snake` contient le corps du serpent, sa longueur et sa direction ;
- `Apples` contient les positions et l'état actif des trois pommes ;
- `GameState` regroupe tout l'état courant de la partie.

Les constantes principales sont :

| Constante | Valeur | Utilisation |
| --- | ---: | --- |
| `SCREEN_WIDTH` | `800` | Largeur de la fenêtre |
| `SCREEN_HEIGHT` | `600` | Hauteur de la fenêtre |
| `CELL_SIZE` | `20` | Taille d'une cellule en pixels |
| `GRID_WIDTH` | `40` | Nombre de colonnes |
| `GRID_HEIGHT` | `30` | Nombre de lignes |
| `MAX_SNAKE_LENGTH` | `1000` | Longueur maximale du serpent |

#### `game.c`

Ce module contient le fonctionnement du jeu :

- `initGame` prépare une nouvelle partie ;
- `handleInput` traduit les touches SDL en actions ;
- `moveSnake` décale le corps et avance la tête ;
- `checkCollision` détecte les collisions avec les murs et le corps ;
- `checkAppleCollision` gère les pommes, le score et la croissance ;
- `updateGame` applique une étape de simulation ;
- `renderGame` dessine la grille, le serpent et les pommes ;
- `generateApples` place les pommes aléatoirement.

#### `save.c` et `save.h`

Le module de sauvegarde expose deux fonctions :

```c
bool saveGame(GameState *game, const char *filename);
bool loadGame(GameState *game, const char *filename);
```

La sauvegarde est réalisée dans le fichier `snake_save.dat`, au format binaire. L'état complet de `GameState` est écrit ou relu en une seule opération avec `fwrite` et `fread`.

#### `Makefile`

Le Makefile :

- compile avec GCC ;
- active les avertissements `-Wall -Wextra` ;
- active l'optimisation `-O2` ;
- produit `snake_game.exe` sous Windows et `snake_game` sous Linux/macOS ;
- lie SDL2 ;
- fournit les cibles `all`, `run` et `clean`.

## 3. Modèle de données

L'état du jeu est regroupé dans la structure `GameState` :

```text
GameState
├── snake
│   ├── body[1000]       # Tableau de positions du serpent
│   ├── length           # Nombre de cellules utilisées
│   └── direction        # 0 haut, 1 droite, 2 bas, 3 gauche
├── apples
│   ├── positions[3]     # Positions des pommes
│   ├── count             # Nombre configuré de pommes
│   └── active[3]        # Présence de chaque pomme
├── score
├── applesEaten
├── gameOver
└── speed                # 0 lent, 1 rapide
```

L'utilisation d'un tableau de taille fixe pour le corps du serpent évite une allocation dynamique pendant la partie. La longueur réelle utilisée est suivie par `snake.length`.

## 4. Déroulement d'une partie

### Initialisation

Au lancement, SDL2 initialise le sous-système vidéo, puis le programme crée :

- une fenêtre ;
- un renderer accéléré avec synchronisation verticale ;
- un renderer logiciel de secours si le renderer accéléré n'est pas disponible.

Le générateur pseudo-aléatoire est initialisé avec l'heure courante grâce à `srand(time(NULL))`.

Le menu console propose :

```text
1. Nouvelle partie
2. Charger partie
```

Si le chargement échoue, le programme revient automatiquement à une nouvelle partie.

### Boucle principale

Tant que la partie est active, le programme répète les étapes suivantes :

1. récupérer les événements SDL2 ;
2. traiter les touches et la fermeture de la fenêtre ;
3. mettre à jour le jeu selon un intervalle de temps ;
4. dessiner l'état courant ;
5. attendre brièvement pour limiter l'utilisation du processeur.

La vitesse est déterminée par l'intervalle entre deux mises à jour :

- mode lent : environ **150 ms** ;
- mode rapide : environ **80 ms**.

### Mise à jour de la simulation

À chaque étape :

1. le corps est décalé de la queue vers la tête ;
2. la tête avance dans la direction sélectionnée ;
3. les collisions sont vérifiées ;
4. une pomme éventuellement touchée est consommée ;
5. le score et la longueur du serpent sont mis à jour.

Lorsqu'une pomme est mangée, elle devient inactive et le score augmente de 10. Après trois pommes mangées, trois nouvelles pommes sont générées et le serpent grandit de trois cellules, sans dépasser `MAX_SNAKE_LENGTH`.

## 5. Contrôles

| Touche | Action |
| --- | --- |
| Flèche haut | Aller vers le haut |
| Flèche droite | Aller vers la droite |
| Flèche bas | Aller vers le bas |
| Flèche gauche | Aller vers la gauche |
| `S` | Basculer entre vitesse lente et rapide |
| `L` | Sauvegarder la partie |
| Fermeture de la fenêtre | Quitter la partie |

Le demi-tour immédiat est empêché : par exemple, un serpent allant vers la droite ne peut pas passer directement vers la gauche.

## 6. Rendu graphique

Le rendu est volontairement simple et repose uniquement sur les primitives SDL2 :

- fond noir ;
- grille gris foncé ;
- serpent en vert ;
- pommes en rouge ;
- cellules dessinées sous forme de rectangles.

Le score est calculé et conservé dans l'état du jeu, mais il n'est pas encore affiché dans la fenêtre. Le score final est affiché dans la console lorsque la partie se termine.

## 7. Sauvegarde et chargement

La touche `L` déclenche l'écriture de l'état courant dans `snake_save.dat`. Au démarrage, le choix `2` tente de relire ce fichier.

Le format actuel est une copie binaire directe de la structure `GameState`. Cette approche est simple et adaptée à un prototype local, mais elle dépend de la représentation mémoire utilisée par le compilateur et l'architecture. Pour une version plus robuste, une évolution possible serait d'utiliser un format explicitement sérialisé, par exemple un format texte ou un format binaire documenté champ par champ.

## 8. Compilation et exécution

### Prérequis

- GCC ou MinGW ;
- SDL2 et ses fichiers de développement ;
- `make` ;
- sous Windows, les bibliothèques SDL2 accessibles par le compilateur et les DLL SDL2 disponibles lors de l'exécution.

### Compilation

Depuis le dossier du projet :

```bash
make
```

La compilation produit :

- `snake_game.exe` sous Windows ;
- `snake_game` sur les systèmes de type Unix.

### Lancement

```bash
make run
```

Il est également possible de lancer directement l'exécutable généré :

```bash
./snake_game
```

Sous Windows, utiliser :

```powershell
.\snake_game.exe
```

### Nettoyage

```bash
make clean
```

Cette commande supprime les fichiers objets et l'exécutable généré.

## 9. Points techniques à présenter

Pour une présentation, les éléments suivants constituent les principaux points techniques du projet :

1. **Séparation des responsabilités** : `main.c` orchestre l'application, `game.c` contient la logique métier et `save.c` gère la persistance.
2. **Programmation événementielle** : SDL2 fournit les événements clavier et fenêtre via `SDL_PollEvent`.
3. **Boucle de jeu temporisée** : la simulation est indépendante du nombre d'itérations de rendu grâce à `SDL_GetTicks`.
4. **Représentation sur grille** : les positions sont stockées en coordonnées de cellules, puis converties en pixels au rendu.
5. **Gestion mémoire simple** : le corps du serpent utilise un tableau statique borné à 1 000 positions.
6. **Détection de collisions** : les murs et les segments du corps sont contrôlés après chaque déplacement.
7. **Fallback graphique** : le programme tente d'abord un renderer accéléré, puis utilise un renderer logiciel.
8. **Persistance binaire** : l'état complet de la partie peut être enregistré et restauré.
9. **Portabilité de compilation** : le Makefile adapte la commande de suppression, l'extension de l'exécutable et les options de liaison selon le système.

## 10. Limites actuelles et pistes d'amélioration

Les évolutions suivantes pourraient être présentées comme perspectives :

- afficher le score et la vitesse directement dans la fenêtre avec SDL_ttf ;
- ajouter un véritable menu graphique ;
- empêcher le placement d'une pomme sur le serpent ou sur une autre pomme ;
- gérer plusieurs niveaux de difficulté ;
- ajouter un écran de fin de partie et la possibilité de recommencer sans fermer le programme ;
- rendre le format de sauvegarde indépendant de la représentation mémoire ;
- vérifier la cohérence du contenu chargé avant de l'utiliser ;
- ajouter des tests unitaires pour les déplacements, les collisions et le calcul du score ;
- centraliser les paramètres de jeu dans une configuration plus facile à modifier.

## 11. Résumé pour la présentation

Ce projet est un jeu de Snake compact qui illustre plusieurs notions de programmation en C :

- conception modulaire avec fichiers `.c` et `.h` ;
- structures et tableaux ;
- boucle événementielle ;
- rendu 2D avec SDL2 ;
- gestion du temps ;
- génération pseudo-aléatoire ;
- lecture et écriture de fichiers ;
- compilation automatisée avec Make.

Le fonctionnement est volontairement lisible : l'état du jeu est centralisé dans `GameState`, chaque tour met à jour cet état, puis le renderer le représente à l'écran.
