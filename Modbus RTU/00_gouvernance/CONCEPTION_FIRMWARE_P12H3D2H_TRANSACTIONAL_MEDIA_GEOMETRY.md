# CONCEPTION — Firmware P12-H3d2-H — Géométrie du média transactionnel

## Statut

Tranche de conception avant modification d'API.

Cette tranche sépare explicitement :
- les exigences logiques TR2 ;
- la géométrie du moteur transactionnel ;
- le profil physique d'un composant ou d'une carte prototype.

Aucun composant F-RAM, aucune capacité commerciale et aucun brochage STM32 ne sont gelés ici.

## 1. Problème constaté

L'implémentation H3d2 actuelle expose directement dans `transactional_image_media.h` une capacité physique de 262144 octets et des adresses fixes :

- superblock A : 0x00000 ;
- superblock B : 0x01000 ;
- image A : 0x02000 ;
- image B : 0x10000 ;
- image area : 0x0E000.

Ces valeurs décrivent le profil host/prototype actuellement qualifié. Elles ne constituent pas une exigence intrinsèque du contrat `PersistentMedia`.

Le moteur transactionnel ne doit donc pas signifier implicitement « média de 256 KiB ».

## 2. Exigence logique TR2

La baseline logique post-C4-E occupe exactement :

    51 818 octets

Cette valeur reste une propriété du layout persistant TR2 courant.

Le format physique v1 ajoute par image :

    64 octets de header

Deux images nécessitent donc au minimum :

    2 × (51 818 + 64) = 103 764 octets

Deux records de publication de 64 octets nécessitent au minimum :

    128 octets

Minimum géométrique brut, sans séparation ni réserve :

    103 892 octets

Une géométrie réelle doit en plus garantir des régions non chevauchantes et contenues dans la capacité physique déclarée.

Ce minimum est dérivé du protocole et du payload logique ; il ne dépend d'aucun composant acheté.

## 3. Nouveau contrat de géométrie

Le moteur recevra une structure de géométrie explicite :

    physical_size
    superblock_a_base
    superblock_b_base
    image_a_base
    image_b_base
    image_area_size

Les tailles de record restent des propriétés du format physique v1 :

    superblock_size = 64
    image_header_size = 64

Le payload logique courant reste :

    logical_size = 51 818

La géométrie ne change ni le format des headers/superblocks, ni leur endianness, ni leurs CRC, ni la sémantique candidate/commit.

## 4. Validation obligatoire à l'initialisation

Une géométrie est acceptée uniquement si :

1. `physical_size > 0` et représentable par l'adressage uint32 courant ;
2. chaque superblock de 64 octets est entièrement contenu dans le média ;
3. chaque image area est entièrement contenue dans le média ;
4. `image_area_size >= 64 + 51 818` ;
5. les quatre régions physiques sont deux à deux non chevauchantes ;
6. les additions nécessaires aux bornes ne débordent pas ;
7. A et B sont des régions distinctes.

Une géométrie invalide doit faire échouer `transactional_image_media_init()` avec `TR2_ERROR_INVALID_ARGUMENT`.

Aucune lecture/écriture physique ne doit être tentée pour une géométrie rejetée.

## 5. Profil de qualification host actuel

Les valeurs actuellement qualifiées deviennent un profil explicite, pas le contrat générique :

    physical_size       = 262 144
    superblock_a_base   = 0x00000
    superblock_b_base   = 0x01000
    image_a_base        = 0x02000
    image_b_base        = 0x10000
    image_area_size     = 0x0E000

Ce profil conserve exactement le format média déjà testé H3d2-A..G.

Son maintien permet une migration de code sans modifier les images physiques de test existantes.

## 6. API cible

La direction d'API est :

    transactional_image_media_init(
        media,
        physical,
        geometry,
        candidate,
        candidate_size)

Le `TransactionalImageMedia` conserve une copie de la géométrie validée.

Toutes les fonctions internes qui calculent une adresse doivent utiliser cette copie, jamais une constante de capacité ou d'offset propre au profil prototype.

Un helper de profil peut fournir la géométrie de qualification host, mais le moteur ne doit pas la sélectionner implicitement.

## 7. Compatibilité du format

Cette refactorisation de géométrie ne crée pas une nouvelle version physique :

- magic inchangés ;
- physical_format_version = 1 ;
- logical_layout_version = 1 ;
- records inchangés ;
- ordre payload -> header -> validation -> superblock inchangé ;
- règles de recovery inchangées.

Pour une géométrie identique au profil actuel, les octets physiques produits doivent être identiques.

La géométrie est un paramètre de placement du format v1, pas un champ nouveau du format v1.

## 8. Qualification H3d2-H

La migration devra être atomique et testée par tranches :

H3d2-H1 — contrat + validation de géométrie :
- structure publique ;
- validation pure ;
- tests de minimum, dépassement et chevauchement ;
- aucun changement de comportement du profil actuel.

H3d2-H2 — moteur adressé par géométrie :
- remplacement de toutes les bases/capacités fixes dans l'implémentation ;
- tests H3d2-A..G migrés vers le profil explicite ;
- 92/92 doit rester vert avant ajout de nouveaux cas.

H3d2-H3 — géométrie alternative :
- exécuter format/commit/recovery sur une géométrie valide qui n'utilise pas les offsets 256-KiB du profil actuel ;
- prouver que le moteur n'est plus couplé à ces adresses.

## 9. Hors périmètre

Restent hors H3d2-H :
- choix final du composant F-RAM ;
- SPI STM32 ;
- adresse sur 2 ou 3 octets d'un composant concret ;
- write-enable/status register ;
- fréquence SPI ;
- chip-select ;
- budget temps réel d'un commit ;
- allocation production du buffer candidat.

## 10. Décision proposée

La capacité 256 KiB et les offsets 4-KiB actuels deviennent un **profil de qualification**, non une exigence architecturale.

Le moteur transactionnel est paramétré par une géométrie explicitement validée.

Le minimum brut dérivé du protocole courant est 103 892 octets ; une géométrie concrète peut être plus grande pour fournir séparation, alignement et réserve.
