# FREEZE — Firmware P12-H3d2 — Portable Transactional Persistent Media

## Status

**GELÉ — P12-H3d2**

Baseline gelée :

```text
bdfbf33fba48ddffa4e54401f3a624d31ff7b31e
Firmware: qualify transactional generation exhaustion
```

Validation locale de la baseline par l'opérateur :

```text
94/94 tests verts
```

Le présent document gèle le moteur transactionnel portable et sa qualification host. Il ne gèle aucun composant F-RAM, bus SPI, brochage STM32, fréquence, protocole électrique ni budget temporel matériel.

## 1. Objet du gel

P12-H3d2 fournit un backend `PersistentMedia` portable qui préserve exactement la frontière logique :

```text
read()   -> image engagée publiée
write()  -> image candidate RAM uniquement
commit() -> publication atomique d'une image logique complète
```

Une écriture non engagée ne devient pas visible par `read()` et ne devient pas autorité après reboot.

## 2. Taille logique et format

Taille logique TR2 gelée pour cette tranche :

```text
51 818 octets
```

Format physique v1 :
- deux images physiques A/B ;
- header image : 64 octets ;
- deux publications/superblocs A/B ;
- superbloc : 64 octets ;
- magic image `TR2I` ;
- magic média `TR2M` ;
- `physical_format_version = 1` ;
- `logical_layout_version = 1` ;
- génération `uint64`, sans wrap ;
- entiers multi-octets big-endian ;
- CRC32 réfléchi, polynôme `0xEDB88320`, init/final `0xFFFFFFFF`.

Le CRC protège contre les corruptions accidentelles et écritures déchirées ; il n'est pas un mécanisme cryptographique.

## 3. Géométrie

Le moteur ne dépend pas d'une capacité physique fixe. La géométrie est fournie explicitement à l'initialisation et validée avant usage.

Elle contient :
- taille physique ;
- base superbloc A ;
- base superbloc B ;
- base image A ;
- base image B ;
- taille d'une zone image.

Le minimum brut dérivé du protocole courant est :

```text
103 892 octets
```

Le profil 256 KiB et ses offsets historiques constituent uniquement un **profil de qualification**. Ils ne sont pas une exigence architecturale.

Une géométrie alternative 128 KiB a été qualifiée en format/commit/recovery.

## 4. Protocole de commit gelé

Pour une autorité courante G :

1. refuser l'opération avant tout wrap de génération ;
2. choisir l'image physique inactive ;
3. écrire le payload candidat complet dans cette image ;
4. écrire ensuite le header final de l'image avec génération et CRC du payload ;
5. relire et valider l'image complète ;
6. choisir le superbloc opposé au superbloc actuellement/récemment récupéré comme autorité ;
7. écrire le nouveau superbloc de publication ;
8. relire et valider ce superbloc contre l'image ;
9. seulement après succès, basculer l'autorité runtime vers la nouvelle image, génération et publication.

Invariant :

```text
payload complet
-> header image final
-> validation image
-> publication superbloc
-> bascule autorité runtime
```

L'image précédemment publiée n'est pas modifiée pendant la construction de la nouvelle génération.

## 5. Politique de publication A/B

L'autorité de publication est suivie explicitement par `active_superblock`.

La cible d'un commit est :

```text
opposée à l'autorité de publication récupérée/courante
```

Elle n'est jamais déduite de la parité de génération.

Recovery :
- une seule publication valide -> elle est autorité ;
- deux publications valides de générations différentes -> génération valide la plus élevée ;
- même génération et même publication canonique -> accepté, A sélectionné déterministement ;
- même génération mais images contradictoires -> `CORRUPTED`.

Les chemins autorité A -> publication B et autorité B -> publication A sont explicitement qualifiés.

## 6. Recovery gelé

États publics :

```text
EMPTY
VALID
UNSUPPORTED
CORRUPTED
UNAVAILABLE
```

Principes :
- erreur de lecture physique -> `UNAVAILABLE`, non masquée ;
- une autorité valide peut masquer un peer déchiré/corrompu/unsupported ;
- `UNSUPPORTED` est retenu lorsqu'aucune autorité valide ne survit et qu'un format TR2 reconnaissable porte une version non supportée ;
- métadonnées courantes non vides mais sans autorité valide -> `CORRUPTED`;
- média vierge uniforme `0xFF` ou `0x00` sur les en-têtes/publications examinés -> `EMPTY`;
- aucun formatage, réparation ou migration silencieuse au recovery.

Une image logique formatée contenant 51 818 octets à zéro reste une image `VALID` grâce à ses métadonnées TR2 ; elle n'est pas confondue avec un média physique vierge.

## 7. Format explicite

`transactional_image_media_format_empty()` est une opération explicitement destructive.

Ordre gelé :
1. invalider les deux publications ;
2. construire le payload logique zéro ;
3. écrire le payload Image A ;
4. écrire son header final génération 1 ;
5. valider Image A ;
6. publier Superbloc A génération 1 ;
7. valider la publication.

Après invalidation complète des deux anciennes publications, une ancienne génération ne doit jamais redevenir autorité.

Le formatage n'est jamais lancé automatiquement par le recovery.

## 8. Power-loss et ambiguïté

La qualification host couvre notamment :
- coupures sur tous les octets du header image ;
- coupures représentatives et limites dans le payload ;
- coupures sur tous les octets du superbloc ;
- publication physiquement complète mais retour d'erreur ;
- commits successifs ;
- coupures pendant le reformatage ;
- reboot avec candidate non engagée.

Après une erreur de commit, le média passe en `recovery_required`. Aucune mutation suivante n'est admise dans cet état.

Après reboot/recovery, l'autorité doit être une image complète ancienne ou nouvelle. Aucun mélange candidate/committed n'est autorisé.

## 9. Intégrations qualifiées

H3d2 est qualifié avec :
- `PersistentStorageCore` ;
- frontière candidate/committed ;
- reboot avec abandon de candidate non engagée ;
- campagne et checkpoints transactionnels ;
- opérations directes de taille zéro ;
- image logique vide entièrement zéro ;
- géométrie alternative ;
- récupération et publication A/B ;
- publications dupliquées de même génération ;
- épuisement de génération.

L'épuisement est fail-closed avant wrap : le commit est refusé, `recovery_required` est posé et aucune nouvelle mutation n'est admise.

## 10. Frontière avec H3d3

Restent explicitement hors du gel H3d2 et doivent être traités en H3d3 / qualification physique :

- choix et validation officielle du composant F-RAM réel ;
- driver de stockage physique STM32 ;
- SPI instance, pins, chip-select et fréquence ;
- format d'adresse du composant ;
- WREN / status register / protections matérielles ;
- erreurs et timeouts SPI ;
- comportement après reset/power-loss réel ;
- allocation production du buffer candidat de 51 818 octets ;
- budget SRAM/stack/linker STM32 ;
- durée réelle d'un commit complet d'environ 51,8 KiB ;
- impact temps réel et cadence de commit ;
- qualification électrique et matérielle.

Aucune de ces propriétés matérielles n'est implicitement déclarée validée par le gel H3d2.

## 11. Règles de modification après gel

Toute modification ultérieure de l'un des éléments suivants exige une réouverture explicite du gel :
- sémantique `PersistentMedia` ;
- taille logique ;
- format physique v1 ;
- ordre de commit/publication ;
- règles de recovery ;
- politique d'autorité A/B ;
- classification EMPTY/VALID/UNSUPPORTED/CORRUPTED/UNAVAILABLE ;
- règle de génération ;
- géométrie minimale/validation ;
- politique de format explicite.

Une adaptation strictement matérielle qui respecte ces contrats appartient à H3d3 et ne réouvre pas H3d2.

## 12. Conclusion

La tranche P12-H3d2 est considérée **fonctionnellement qualifiée et gelée** sur la baseline indiquée en tête de document, sous réserve de l'ajout du présent document de freeze lui-même.

La prochaine tranche est :

```text
P12-H3d3 — PersistentMedia physique STM32 / F-RAM
```

Son objectif est d'implémenter le contrat physique `read/write` sous le moteur transactionnel gelé, sans modifier ses invariants portables.
