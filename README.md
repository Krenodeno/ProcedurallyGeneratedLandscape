# Projet de modélisation "Jeux-vidéos"
Le projet à pour but de nous familiariser avec différentes techniques de génération procédurale et de simulation d'environnement naturel.
Le terrain est généré à l'aide d'une somme de bruits de Perlin "ridgés". Ensuite une érosion est appliquée pour creuser le terrain et par la même occasion créer une couche de sédiments.
On peut ensuite générer de la végétation sur différents critères comme l'épaisseur de sédiments disponibles et la pente. Une simulation du cycle de vie de la végétation peut ensuite être appliquée, mettant en compétition les individus.

## Exemple d'exécution

Le programme se lance en ligne de commande:
`./ANewWorld`
Par défaut, le programme se lance comme si on lui passait ces paramètres:
`./ANewWorld 70 100 40 7 100 "out.obj" -e 10 -v 1`
Les paramètres sont les suivants dans l'ordre:
- Longueur d'onde de base
- La taille des côté du terrain
- La hauteur max du terrain
- Nombre d'octaves du Perlin Noise
- Nombre de cellules par côté (soit n*n cellules au total)
- Le nom du fichier exporté de base avant érosion
- `-e` suivie du nombre d'itérations d'érosion
- `-v` suivie du nombre d'itérations de simulation de végétation

## How to build
Le projet a été réalisé sous Linux, il est possible que le projet ne compile pas sous Windows.
Pour compiler, il faut générer les projets pour votre environnement de développement. Pour cela nous utilisons premake5.
Pour générer un Makefile:
`./premake5 gmake2`
Puis compiler avec make:
`make -C build`
Les binaires se trouvent dans `build/bin/`.

## Dépendances
Nous utilisons la librairie FastNoiseSIMD pour la génération du bruit de Perlin. Elle est intégrée dans le dépôt.
Nous utilisons également SDL2 pour lire des images et FreeImage pour les sauvegarder depuis/vers le disque.
Il vous faut installer SDL2 et SDL2image sur votre système. Reportez-vous au dépôts de votre distribution Linux.
