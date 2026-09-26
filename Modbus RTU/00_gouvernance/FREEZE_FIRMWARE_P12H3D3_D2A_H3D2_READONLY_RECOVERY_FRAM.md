# Freeze P12-H3d3-D2-A — H3d2 read-only recovery on physical FRAM

## Statut

**GELÉ — validation matérielle réelle**

Baseline :
- P12-H3d3-D1 FRAM physical read/write gelé : `9646da02ae5deda10aa48c033aa64f09474261c3`
- intégration D2-A read-only : `a2824bb89bbc1996301bc17c77cbdf3d1082da72`
- correction portabilité 32 bits de la validation de géométrie : `7ecc6c0336fd6c5d800747c532c61a76743c0753`

## Objet

D2-A raccorde le moteur transactionnel H3d2 gelé au backend FRAM STM32 physiquement qualifié, mais limite volontairement la qualification à un recovery en lecture seule.

Aucun appel à `transactional_image_media_format_empty()` ou au commit du `PersistentMedia` n'est effectué dans cette tranche.

## Composition qualifiée

Au démarrage :
1. initialisation du backend `Stm32FramStorage` ;
2. création du `TransactionalImagePhysicalStorage` ;
3. obtention de la géométrie `transactional_image_geometry_qualification_profile()` ;
4. validation de la géométrie ;
5. initialisation de `TransactionalImageMedia` avec un buffer candidat réel de 51 818 octets ;
6. appel unique à `transactional_image_media_recover()`.

Le harnais destructif D1 n'est plus exécuté au démarrage.

## Correction de portabilité découverte pendant D2-A

La première exécution physique a donné :
```text
storage_init = 0x0
geometry     = 0x1
media_init   = 0x9
recover      = 0x9
status       = 0x4
generation   = 0x0
active_image = 0xff
```

Le recovery n'avait donc pas été exécuté.

Cause : la borne de `transactional_image_geometry_validate()` destinée à autoriser une taille physique maximale de 2^32 octets utilisait une expression `UINT32_MAX + 1` convertie en `size_t`. Sur la cible STM32 32 bits, `size_t` ne peut pas représenter 2^32 ; l'expression débordait et conduisait au rejet de toute géométrie physique non vide.

Correction :
- la borne supérieure supplémentaire n'est appliquée que lorsque `SIZE_MAX > UINT32_MAX` ;
- sur une cible 32 bits, toute valeur représentable par `size_t` est déjà <= UINT32_MAX et les contrôles de fin de régions restent appliqués.

Cette correction ne modifie ni le format média, ni les offsets, ni le protocole de commit, ni les règles de recovery H3d2.

Après correction :
- validation host complète : verte ;
- cross-build STM32 : vert.

## Budget mémoire observé

Cross-build D2-A :
```text
text   46476
data      48
bss    55264
dec   101788
```

Le buffer candidat H3d2 de 51 818 octets est donc effectivement présent dans le binaire STM32. Cette observation ne constitue pas encore une validation du budget SRAM final du produit complet.

## Résultat matériel final

Après programmation du binaire corrigé et reset :
- LD1 continue de fonctionner normalement ;
- le runtime atteint sa boucle principale.

Sondes GDB :
```text
tr2_fram_d2_storage_init_result = 0x0
tr2_fram_d2_geometry_result     = 0x0
tr2_fram_d2_media_init_result   = 0x0
tr2_fram_d2_recover_result      = 0x0
tr2_fram_d2_recovery_status     = 0x0
tr2_fram_d2_generation          = 0x0
tr2_fram_d2_active_image        = 0x0
```

Dans l'API H3d2 gelée :
`TRANSACTIONAL_IMAGE_RECOVERY_EMPTY = 0`.

Le résultat démontre donc que :
- le backend FRAM réel est accepté ;
- la géométrie physique réelle est acceptée sur STM32 32 bits ;
- `TransactionalImageMedia` est initialisé ;
- le recovery H3d2 réel s'exécute sans erreur d'I/O ;
- H3d2 classe le média physique courant comme `EMPTY`.

Pour un statut EMPTY, `generation` et `active_image` ne constituent pas une autorité active : ils restent à leurs valeurs initialisées dans le résultat. Ils ne doivent pas être interprétés comme génération 0 / image A publiée.

## Limites du gel

D2-A ne valide pas encore :
- le formatage physique H3d2 ;
- une image H3d2 valide sur FRAM ;
- une publication par superblock ;
- un recovery VALID sur FRAM ;
- un commit réel ;
- le basculement image A/B ou superblock A/B ;
- le recovery après coupure d'alimentation ;
- les scénarios de corruption physique ;
- le budget SRAM final du firmware complet.

## Conclusion

P12-H3d3-D2-A est validée et gelée comme qualification du recovery H3d2 en lecture seule sur la FRAM physique.

La prochaine tranche peut effectuer, de manière explicitement destructive et contrôlée, le premier formatage H3d2 réel du média puis vérifier un recovery VALID après reset, avant tout essai de commit transactionnel applicatif.
