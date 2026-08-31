# FT-ACC-01 — Lecture des zones exposées — RETIRÉE

## Statut

**Sous-famille retirée du référentiel actif V1.**

FT-ACC-01 avait pour objet historique de vérifier la lecture des zones Modbus exposées. Après audit croisé avec la famille FT-STR gelée, ce périmètre est couvert intégralement et plus précisément par **FT-STR-06 — Accessibilité et découpage de lecture Modbus**.

FT-ACC-01 ne doit donc plus être utilisée pour produire de nouveaux verdicts de validation.

## Motif du retrait

L'ancienne suite FT-ACC-01 dupliquait notamment :

- la lecture d'un registre exposé isolé ;
- les lectures en début, milieu et fin de plage ;
- les sous-plages contiguës ;
- les lectures multi-registres ;
- les lectures traversant plusieurs champs logiques ;
- les lectures de plages de blocs.

Ces contrôles appartiennent désormais à FT-STR-06.

L'ancien référentiel FT-ACC-01 contenait en outre des hypothèses devenues incompatibles avec la doctrine gelée, notamment sur la lecture partielle d'un champ logique multi-registres. FT-STR-06 précise qu'une sous-plage entièrement exposée reste valide même lorsqu'elle ne couvre qu'une partie d'un champ logique.

## Référentiel actif

Pour toute validation de lecture Modbus :

- structure et exposition des adresses : FT-STR ;
- accessibilité, découpage et comportement FC03 : **FT-STR-06** ;
- droits et refus d'écriture : FT-ACC-02 à FT-ACC-06 selon le cas.

## Historique

Les anciens artefacts `source/`, `detaille/` et `instancie/` ont été déplacés sous `archive_pre_renforcement/` afin de conserver la traçabilité historique sans polluer le référentiel actif.

Ils sont **informatifs uniquement** et ne doivent pas être exécutés comme tests V1 actifs.

L'identifiant `FT-ACC-01` est conservé et n'est pas réattribué afin de préserver la traçabilité documentaire et l'historique du projet.
