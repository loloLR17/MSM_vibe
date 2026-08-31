# FT-ACC-02 — Écriture autorisée des zones RW

## Objet

Cette sous-famille valide les **droits d'écriture Modbus** des registres et champs explicitement déclarés `RW` par la spécification V1 et instanciés par le mapping GEL-MAP-V1.

Elle vérifie qu'une requête d'écriture conforme au protocole et au périmètre RW est acceptée et que la valeur écrite est observable en relecture lorsque la sémantique du champ le permet.

FT-ACC-02 ne valide ni les domaines métier, ni les transitions fonctionnelles, ni les effets système complexes.

## Hiérarchie documentaire

Ordre de vérité :

1. spécification Modbus RTU V1 gelée (`01_Specification_source/`) ;
2. mapping unifié GEL-MAP-V1, artefact dérivé ;
3. `source/FT-ACC-02.md` ;
4. `detaille/` ;
5. `instancie/`.

FT-STR, notamment FT-STR-06, est considérée gelée en prérequis structurel. FT-ACC-01 est retirée du référentiel actif.

## Doctrine

- une cible nominale FT-ACC-02 doit être entièrement `RW` ;
- une écriture nominale ne doit jamais traverser un registre `RO`, réservé ou non exposé ;
- pour un champ logique multi-registres (`uint32`, `ASCII fixe`), le scénario nominal écrit le champ complet, sauf règle V1 explicite autorisant un accès partiel ;
- une valeur hors domaine métier sur une cible RW ne constitue pas à elle seule un accès Modbus invalide ; son acceptabilité fonctionnelle relève de FT-LIM ou des règles du bloc ;
- les effets secondaires explicitement normatifs ne sont pas des défauts FT-ACC-02 ; leur contrôle relève de FT-ACC-05 ou des familles fonctionnelles ;
- les écritures interdites sur RO/réservés relèvent de FT-ACC-03/04/06.

## Structure

```text
FT-ACC-02/
├── README.md
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

L'archive est informative uniquement.

## Statut

Référentiel reconstruit après audit croisé V1 / GEL-MAP-V1 / GEL-GOV-02 / FT-STR.
