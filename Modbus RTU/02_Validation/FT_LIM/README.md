# FT-LIM — Validation des limites et domaines de valeurs

## 1. Objet

La famille **FT-LIM** est dédiée à la validation des valeurs aux frontières et hors domaine définis par la spécification Modbus RTU V1 gelée.

Elle couvre notamment, lorsque ces éléments sont explicitement définis :

- bornes minimales et maximales ;
- valeurs immédiatement adjacentes aux bornes ;
- formats et représentations ;
- domaines autorisés ;
- valeurs réservées ;
- enums hors domaine ;
- sentinelles explicitement définies ;
- combinaisons de paramètres invalides définies par les règles fonctionnelles.

## 2. Référentiel

La hiérarchie applicable est :

```text
Spécification Modbus RTU V1 gelée
        ↓
mapping_unifie dérivé
        ↓
FT-LIM source
        ↓
tests détaillés
        ↓
tests instanciés
```

Le mapping unifié est la source opérationnelle d’instanciation des tests, mais reste un artefact dérivé.

En cas de divergence avec la spécification V1 gelée, la spécification fait foi et la divergence doit être remontée comme anomalie documentaire.

## 3. Règle de non-invention

FT-LIM ne doit créer ni supposer aucune :

- borne ;
- valeur réservée ;
- sentinelle ;
- enum ;
- valeur par défaut ;
- exception ;
- unité ;
- règle de rejet.

Toute information nécessaire non définie dans le référentiel normatif doit être classée :

**NON DÉFINI / À ARBITRER**.

## 4. Accès Modbus et invalidité métier

Une valeur métier hors domaine écrite dans un registre explicitement `RW` ne constitue pas, à elle seule, un accès Modbus invalide.

FT-LIM doit donc distinguer systématiquement :

- **accès Modbus invalide** : adresse ou opération non autorisée → traitement selon la doctrine des accès invalides ;
- **valeur métier invalide sur accès RW valide** : traitement selon les règles fonctionnelles normatives du bloc concerné.

Aucune exception Modbus ne doit être inventée pour une valeur métier invalide lorsque la spécification ne la définit pas.

## 5. État de la famille

FT-LIM est actuellement en cours de consolidation après gel de la spécification Modbus RTU V1.

Les fiches existantes doivent être auditées contre la V1 avant poursuite de la génération ou de l’instanciation massive.
