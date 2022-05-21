# Carcasum

- [Carcasum](#carcasum)
  - [English](#english)
    - [how to build](#how-to-build)
      - [requirements](#requirements)
      - [building for linux](#building-for-linux)
        - [make final build](#make-final-build)
        - [make debug build](#make-debug-build)
      - [building for windows](#building-for-windows)
        - [make final build](#make-final-build-1)
        - [make debug build](#make-debug-build-1)
    - [how to play](#how-to-play)
      - [run the game](#run-the-game)
      - [Controls](#controls)
        - [main menu](#main-menu)
        - [game](#game)
    - [game rules](#game-rules)
      - [basic rules](#basic-rules)
      - [special cases](#special-cases)
    - [credits](#credits)
      - [graphics](#graphics)
      - [music](#music)
      - [code](#code)
        - [game core](#game-core)
        - [game graphics](#game-graphics)
        - [AI code](#ai-code)
  - [Français](#français)
    - [Compilation](#compilation)
      - [pre-requis](#pre-requis)
      - [compilation pour linux](#compilation-pour-linux)
        - [version finale](#version-finale)
        - [version debug](#version-debug)
      - [compilation pour windows](#compilation-pour-windows)
        - [version finale](#version-finale-1)
        - [version debug](#version-debug-1)
    - [comment jouer](#comment-jouer)
      - [lancer le jeu](#lancer-le-jeu)
      - [Contrôles](#contrôles)
        - [menu principal](#menu-principal)
        - [partie](#partie)
    - [règles du jeu](#règles-du-jeu)
      - [règles de base](#règles-de-base)
      - [regles specifiques](#regles-specifiques)
    - [credits](#credits-1)
      - [graphiques](#graphiques)
      - [musique](#musique)
      - [code](#code-1)
        - [partie basique (systemes du jeu)](#partie-basique-systemes-du-jeu)
        - [graphiques](#graphiques-1)
        - [IA](#ia)





## English
### how to build
#### requirements
The requirements to build and play this game are:

    - libsdl2-dev 
    - libsdl2-image-dev 
    - libsdl2-ttf-dev 
    - libsdl2-mixer-dev


to install these requirements you can run this command:

    sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev

Note: if you are using a linux distro that is not Ubuntu or Debian, you may need to install these libraries using your distro's package manager.


For Windows you will need to download the dev libraries files from the websites listed below.

- [libsdl2-dev](https://www.libsdl.org/download-2.0.php)
- [libsdl2-image-dev](https://www.libsdl.org/projects/SDL_image/)
- [libsdl2-ttf-dev](https://github.com/libsdl-org/SDL_ttf)
- [libsdl2-mixer-dev](https://www.libsdl.org/projects/SDL_mixer/)

and place the contents in your compiler's installation folder.

#### building for linux
##### make final build
    make install
##### make debug build
    make

#### building for windows
##### make final build
    make install-win
##### make debug build
    make windows



### how to play
#### run the game
    ./Carcasum
#### Controls
    +/- or m/p keys to change the music volume.
##### main menu
in this menu you can select the number of players as well as the number of players controlled by the AI.

    up/down arrow keys to select the number of players/AI.

    left/right arrow keys to move the selection cursor.

    Enter to start the game.
##### game
The gameplay loop consists of rotating a tile to find the best spot to place it, and then placing it and finally placing a meeple if possible.

The game ends when all the tiles are placed.

    left/right arrow keys to rotate the tile.

    right click to pan the camera.

    left  click to place the tile (only when hovering over a valid spot).

    mouse wheel or z / s keys to zoom in/out.

    escape to go back to the main menu. (!warning! this will reset the game!)


### game rules
#### basic rules
The game is played with a 72 (usually) card deck.

The game ends when all the tiles are placed.

The deck contains cards which represent a specific landscape.

The type of landscapes are the following:

- Town

- Road

- Field

- Abbey

The objective of the game is to gain as many points as possible by the end of the game.

To do so the players must place tiles on the board such that the landscapes of the tiles match their neighbours.

For instance if the right side of a tile is a town, then the right side of the tile to it's left must also be a town.

The player who has the most points at the end of the game wins.

The player only gets points for completing structures (towns and roads) or placing a tile near an abbey.

The player must have a meeple on the structure to claim it's points.

Players start with 7 meeples and can place them right after placing a tile.

The player can only reclaim the meeple when completing the structure it is on.

The player can only place a meeple on a structure if it is not already occupied.



The player gains points for each tile composing the structure when it is completed.

The points gained for each tile depend on the type of structure:
- Town: 1 point
- Road: 2 points


Towns can also have a bonus point per tile if it the tile has castle in the middle. 

The bonus point is added to the total points of the town when it is closed for each tile that has a castle.



When the game is over, points are counted for each incomplete structure.

Each tile in the structure gives one point.



#### special cases

If there are multiple meeples on a structure when it is closed (this can happen when two different structures with a meeple are are connected by a new tile) the points go to the player with the most meeples on the structure. If the number of meeples on the structure is the same for both players, both players get the points. 



### credits

#### graphics

    Arthur Chateauneuf



#### music

Main theme:

    Tavern Loop One by Alexander Nakarada | https://www.serpentsoundstudios.com

    Music promoted by https://www.free-stock-music.com

    Attribution 4.0 International (CC BY 4.0)

    https://creativecommons.org/licenses/by/4.0/



Game sounds:

    https://kenney.nl/assets/rpg-audio 

    License: (CC0 1.0 Universal)

    https://creativecommons.org/publicdomain/zero/1.0/



    Win Sound #2 by remaxim | https://opengameart.org/content/win-sound-2

    License: (CC BY-SA 3.0), GNU GPL 3.0, GNU GPL 2.0

    https://creativecommons.org/licenses/by-sa/3.0/

    http://www.gnu.org/licenses/gpl-3.0.html

    http://www.gnu.org/licenses/gpl-2.0.html



    Win sound effect by Listener | https://opengameart.org/content/win-sound-effect

    License: (CC 0 1.0 Universal)

    https://creativecommons.org/publicdomain/zero/1.0/



#### code

##### game core

    Arthur Bossu

    Arthur Chateauneuf

    Samy Belhadri

##### game graphics

    Arthur Chateauneuf

##### AI code

    Arthur Chateauneuf



## Français

### Compilation

#### pre-requis

Les pré-requis pour compiler et jouer ce jeu sont:

    libsdl2-dev 

    libsdl2-image-dev 

    libsdl2-ttf-dev 

    libsdl2-mixer-dev





Pour installer ces pré-requis vous pouvez exécuter cette commande:

    sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev



#### compilation pour linux

##### version finale

    make install

##### version debug

    make



#### compilation pour windows

##### version finale

    make install-win

##### version debug

    make windows





### comment jouer

#### lancer le jeu

    ./Carcasum

#### Contrôles

    +/- ou m/p pour changer le volume de la musique.

##### menu principal

Dans ce menu vous pouvez choisir le nombre de joueurs et le nombre de joueurs contrôlés par l'IA.

    Touches haut/bas pour choisir le nombre de joueurs/IA.

    Touches gauche/droite pour déplacer le curseur.

    Entrée pour commencer la partie.

##### partie

Le gameplay loop consiste a tourner une tuile pour trouver le meilleur endroit pour la placer, puis la placer et enfin placer un meeple si possible.

La partie se termine quand toutes les tuiles sont placées.

    Touches gauche/droite pour tourner la tuile.
    
    Clic droit pour faire défiler la caméra.
    
    Clic gauche pour placer la tuile (seulement si vous survolez un emplacement valide).
    
    Molette ou Touches z/s pour zoomer.
    
    Echap pour revenir au menu principal. (!Attention! cela réinitialise la partie!)



### règles du jeu

#### règles de base

Le jeu est joué avec un deck de 72 cartes (habituellement).

La partie se termine quand toutes les tuiles sont placées.

Le deck contient des cartes qui représentent un type de terrain.

Les types de terrain sont les suivants:

- Ville
- Route
- Champ
- Abbaye

L'objectif du jeu est d'atteindre le plus de points possible par la fin de la partie.

Pour cela les joueurs doivent placer des tuiles sur le plateau de jeu de sorte que les terrains des tuiles correspondent à leurs voisins.

Par exemple si la partie droite d'une tuile est une ville, alors la partie de gauche de celle-ci doit aussi être une ville.

Le joueur qui a le plus de points à la fin de la partie gagne.

Le joueur gagne des points en complétant des structures (villes et routes) ou placer une tuile à proximité d'une abbaye.

Les joueurs doivent avoir un meeple sur la structure pour obtenir des points.

Les joueurs commencent avec 7 meeples et peuvent les placer juste après avoir placé une tuile.

Les joueurs peuvent récupérer leur meeple quand ils complètent une structure.

Les joueurs ne peuvent placer un meeple sur une structure que si elle n'est pas déjà occupée par un autre meeple.



Le joueur gagne des points pour chaque tuile composant la structure quand elle est complétée.

Les points gagnés par tuile dépendent du type de structure:

- Ville: 1 point
- Route: 2 points



Les villes peuvent aussi donner un point bonus si la tuile contient un château.

Le bonus est ajouté au total des points de la ville quand elle est complétée pour chaque tuile qui contient un château.



Quand la partie est terminée, les joueurs gagnent un point par tuile pour chaque structures non complétées sur lesquels ils ont un meeple.



#### regles specifiques

Si il y a plusieurs meeples sur une seule structure lorsqu'elle est fermée, (Ceci peut arriver lorsque deux structures séparées ayant un meeple sont jointes par une nouvelle tuile) le joueur avec le plus de meeple sur la structure gagne les points. Si il y a égalité, les deux joueurs gagnent les points.



### credits

#### graphiques

    Arthur Chateauneuf



#### musique

Thème principal:

    Tavern Loop One by Alexander Nakarada | https://www.serpentsoundstudios.com

    Music promoted by https://www.free-stock-music.com

    Attribution 4.0 International (CC BY 4.0)

    https://creativecommons.org/licenses/by/4.0/



Sons de jeu:

    https://kenney.nl/assets/rpg-audio 

    License: (CC0 1.0 Universal)

    https://creativecommons.org/publicdomain/zero/1.0/



    Win Sound #2 by remaxim | https://opengameart.org/content/win-sound-2

    License: (CC BY-SA 3.0), GNU GPL 3.0, GNU GPL 2.0

    https://creativecommons.org/licenses/by-sa/3.0/

    http://www.gnu.org/licenses/gpl-3.0.html

    http://www.gnu.org/licenses/gpl-2.0.html



    Win sound effect by Listener | https://opengameart.org/content/win-sound-effect

    License: (CC 0 1.0 Universal)

    https://creativecommons.org/publicdomain/zero/1.0/



#### code

##### partie basique (systemes du jeu)

    Arthur Bossu

    Arthur Chateauneuf

    Samy Belhadri

##### graphiques

    Arthur Chateauneuf

##### IA

    Arthur Bossu
