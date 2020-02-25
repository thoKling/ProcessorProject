# Projet de simulation du comportement de l&#39;algorithme d&#39;ordonnancement.

Thomas Kling et Guillaume Lechantre



## Définition du sujet

Il nous a été demandé de modéliser le fonctionnement d&#39;un algorithme sur le modèle de l&#39;algorithme **Round Robin** avec priorité.

Nous devions donc simuler des processus et les insérer dans des listes d&#39;attentes en fonctions de leur priorité.

Le processus simulant le CPU pioche, à chaque quantum, le processus qu&#39;il va exécuter dans la liste d&#39;attentes correspondant à la priorité actuelle (N).

Une fois le quantum de temps terminé, si le processus est terminé, il est supprimé, sinon, on le remet dans la liste d&#39;attente de priorité N-1.

## Nos choix techniques

### Threads vs Forks

Nous avons choisi d&#39;utiliser des threads et non des forks.

Tout d&#39;abord car la communication entre les threads est plus simple qu&#39;entre les forks puisqu&#39;ils partagent le même contexte, on est donc pas obligé d&#39;utiliser des IPC, par exemple la priorité courante est une variable globale mis-à-jour par la table d&#39;allocation et lu par le CPU.

De plus, contrairement aux forks, les threads sont supprimés automatiquement à la fin de l&#39;exécution du processus qui les a initiés.

Ainsi, si notre programme venait à planter, nos threads sont automatiquement supprimés et on ne risque pas d&#39;avoir un processus zombie qui va lire nos files de messages.

Dans le cas contraire, si nous avions utilisé des forks, nous aurions dû les tuer à la main si notre programme venait à planter.

### Files de messages vs Pipes

Nous avons choisi d&#39;utiliser des files de messages et non des pipes.

Les files de messages se prêtent le mieux à l&#39;algorithme Round Robin puisqu&#39;on a un déjà un système de FIFO et de type pour les messages.

Dans la file de messages processus exécution nous avons donné au message un type correspondant à leur priorité.

### Sémaphores

Pour synchroniser la soumission des processus, leur mise en place puis leur exécution nous avons mis en place deux sémaphores. De plus, elles permettent de s&#39;assurer que la variable globale de priorité courante n&#39;est pas lue et changer « en même temps ».

## La vie d&#39;un processus

Une fois créé, le processus est mis dans la file de message processus entrant par le générateur de processus.

Le processus à ensuite un PID qui lui est attribué et est mis dans la file processus exécution avec comme type sa priorité.

Lorsque que c&#39;est son tour, le CPU le sort de la file de message et l&#39;exécute durant un quantum de temps. S&#39;il est terminé à la fin du quantum, il est supprimé, sinon il est remis dans la file processus exécution avec comme type son ancienne priorité +1. Cela va baisser sa priorité de un.

## Les différents modules

Nous avons choisi de découper le projet en plusieurs modules en fonction de leurs rôles. Tout d&#39;abord, nous avons un module CPU va simuler le processeur et exécuter les processus. Ensuite, la table d&#39;allocation va lire les processus entrants, les ajouter aux files de processus à exécuter et calculer la nouvelle priorité en fonction du pas. 

Ensuite, on a un module GénérateurProcessus qui permet de créer les nouveaux processus au démarrage en lisant un fichier puis de générer aléatoirement des nouveaux processus. 

Le main est réduit à créer les threads, les IPC et lire les paramètres, c&#39;est vraiment l&#39;entrée du programme. Enfin, on a un module utils, qui possède les constantes globales du projet, un module MySemaphore qui reprend les sémaphores qu&#39;on a vu en cours et qui permet une utilisation simple et intuitive partout dans le programme.
