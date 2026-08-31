# FT-ACC-05 — Absence d’effets de bord non spécifiés

## Objet
Valider qu’une écriture Modbus autorisée sur une zone RW ne produit aucun effet qui ne soit prévu par la V1.

La règle de verdict n’est plus « seul le registre ciblé peut changer ». Un changement hors cible est acceptable uniquement s’il est explicitement prescrit par la V1 ou démontré comme évolution autonome indépendante de l’écriture.

## Hiérarchie documentaire
1. spécifications V1 gelées ;
2. mapping unifié GEL-MAP-V1 ;
3. `source/` ;
4. `detaille/` ;
5. `instancie/`.

FT-STR est gelée. FT-ACC-02 établit l’autorisation d’écriture ; FT-ACC-05 contrôle les conséquences de cette écriture.

## Couverture active
- Bloc 2 : préparation de l’heure sans synchronisation implicite ;
- Bloc 4 : configuration préparée, avec effets d’état normatifs autorisés et image active protégée ;
- Bloc 5 : préparation de commande sans déclenchement ;
- Bloc 6 : sélection de campagne avec mise à jour normative de la vue associée.

## Structure
- `source/` : doctrine de validation ;
- `detaille/` : 4 cas génériques ;
- `instancie/` : 35 champs RW instanciés depuis le mapping gelé ;
- `archive_pre_renforcement/` : ancien référentiel, conservé uniquement pour traçabilité.

## Principe de snapshot
Le snapshot du bloc complet reste un moyen de détection utile. Le verdict est obtenu en classant chaque différence observée : cible, effet V1 autorisé, évolution autonome démontrée, ou effet non spécifié.
