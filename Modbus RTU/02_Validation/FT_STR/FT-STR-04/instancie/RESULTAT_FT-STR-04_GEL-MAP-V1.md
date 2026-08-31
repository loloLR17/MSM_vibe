# Résultat FT-STR-04 — GEL-MAP-V1

## Références

- Mapping : GEL-MAP-V1
- Gel mapping historique : `ff948e5917becceed7637d9c7864ec9b279be0ca`
- Convention normative ASCII : `charte_typage.md`, règle explicitement complétée lors de l’audit FT-STR-04

## Couverture

GEL-MAP-V1 contient **8 champs `ASCII fixe`** :

- Bloc 0 : 2 champs ;
- Bloc 4 : 4 champs ;
- Bloc 6 : 2 champs.

Chaque champ apparaît une fois dans la couverture active.

## Contrôle structurel

Le contrôle mécanique attendu vérifie pour chaque champ ASCII :

- type `ASCII fixe` ;
- nombre de registres > 0 ;
- plage d’adresses de longueur égale au nombre de registres ;
- capacité calculée à deux caractères par registre ;
- unicité de la paire `(bloc, logical_name)`.

Résultat documentaire attendu pour GEL-MAP-V1 : **CONFORME**.

## Contrôle d’implémentation

Les règles suivantes ne peuvent pas être démontrées par le mapping seul :

- caractère 1 dans l’octet haut et caractère 2 dans l’octet bas ;
- padding terminal `0x00` ;
- contenu utile ASCII 7 bits.

Elles sont couvertes par GEN-002 et GEN-003 et restent **À EXÉCUTER sur l’implémentation**.

## Conclusion

FT-STR-04 est reconstruite et cohérente avec la nouvelle convention V1. La structure documentaire est prête ; la conformité du firmware à l’encodage ASCII sera établie lors de l’exécution terrain/simulation.
