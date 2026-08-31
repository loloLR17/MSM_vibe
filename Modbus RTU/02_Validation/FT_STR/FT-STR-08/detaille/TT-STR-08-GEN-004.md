# TT-STR-08-GEN-004 — Traçabilité et absence de règles héritées

## Objectif
Vérifier que les artefacts actifs de validation dérivent de la hiérarchie gelée et ne réintroduisent aucune règle ancienne ou implicite.

## Points de contrôle
- aucune règle active ne prend un ancien test comme référence normative ;
- aucune sentinelle implicite telle que `0 = non renseigné` n'est généralisée ;
- les réservés suivent uniquement la V1 ;
- les conventions d'identification restent traçables ;
- les éléments obsolètes sont archivés et exclus de la validation active.

## Résultat attendu
Chaîne documentaire explicite et sans héritage normatif non validé.
