# FT-PER-01 — Reset logiciel et observables de redémarrage

## 1. Objet

FT-PER-01 valide uniquement les conséquences observables d'un **RESET SOFTWARE** à travers une frontière de redémarrage, lorsque la V1 fournit un oracle discriminant.

La sous-famille ne redéfinit ni les conditions d'acceptation de la commande 10, ni la mécanique transactionnelle B5 : ces propriétés appartiennent à FT-CMD-07.

## 2. Principe de propriété

FT-PER-01 possède la composition suivante :

`RESET SOFTWARE accepté selon FT-CMD → redémarrage logiciel contrôlé → lecture post-reboot des observables normatifs de reset`.

Les observables principaux sont :
- `B1.last_reset_cause` ;
- `B7.reset_cause` ;
- `B7.uptime_s` comme trace du temps de fonctionnement depuis le dernier reset.

## 3. Oracle V1 retenu

La V1 définit explicitement :
- commande 10 = reset logiciel contrôlé ;
- effet = redémarrage logiciel contrôlé ;
- `B1.last_reset_cause = 2` signifie `Reset logiciel` ;
- `B7.reset_cause = 2` signifie `reset logiciel` ;
- `B7.uptime_s` est le temps de fonctionnement depuis le dernier reset et ne doit jamais revenir en arrière sauf reset.

Le verdict propriétaire FT-PER-01 porte donc sur la **cause post-reboot**. La comparaison B1↔B7 en tant que relation inter-blocs reste déléguée à FT-INT ; FT-PER lit les deux champs comme deux observables post-reset définis par leur bloc respectif.

## 4. Limite importante sur l'uptime

La V1 ne définit :
- aucun délai maximal de reboot ;
- aucun délai maximal avant reprise Modbus ;
- aucune valeur exacte d'`uptime_s` à la première réponse ;
- aucune tolérance entre B1 et B7.

Par conséquent, l'uptime est conservé dans FT-PER-01 comme **TRACE_ONLY** pour documenter la rupture de continuité liée au reset, mais il n'est pas utilisé pour imposer `0`, `1` ou une borne temporelle inventée.

## 5. Périmètre actif

- exécution d'un RESET SOFTWARE accepté selon FT-CMD-07 ;
- franchissement effectif d'une frontière de redémarrage ;
- vérification de la cause de reset logiciel dans B1 ;
- vérification de la cause de reset logiciel dans B7 ;
- relevé non bloquant des uptime avant/après ;
- mesure éventuelle du temps d'indisponibilité Modbus comme trace uniquement.

## 6. Hors périmètre / dettes V1

Sont explicitement exclus de tout oracle FT-PER-01 :
- conditions d'acceptation, clé et codes de refus RESET SOFTWARE : `DELEGATED` FT-CMD-07 ;
- définition exhaustive des opérations critiques : `NOT_DEFINED` ;
- délai de reboot : `NOT_DEFINED` ;
- délai de reprise Modbus : `NOT_DEFINED` ;
- état exact des champs B5 après reboot : `NOT_DEFINED` ;
- persistance de `cmd_last_*` et `cmd_active_*` : FT-PER-04 / `NOT_DEFINED` ;
- mémoire d'idempotence après reboot : FT-PER-04 / `NOT_DEFINED` ;
- configuration, campagnes, acquisition, diagnostics historiques après reboot : autres sous-familles FT-PER.

## 7. Test actif

- `TT-PER-B01B05B07-001` — RESET SOFTWARE accepté puis cause de redémarrage logiciel observable dans B1 et B7 (`CONDITIONAL`, moyen de reboot contrôlé requis).

Le relevé de l'uptime et du temps de reprise Modbus est inclus dans ce test comme trace, sans critère temporel PASS/FAIL.

## 8. Artefacts

- `source/FT-PER-01_source.md` ;
- `detaille/FT-PER-01_detaille.md` ;
- `detaille/FT-PER-01_matrice_couverture.csv`.

## 9. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.