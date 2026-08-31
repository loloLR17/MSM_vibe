# TT-STR-06-GEN-001 — Lecture unitaire et sous-plage valide

## Objectif

Vérifier qu'une lecture FC03 portant uniquement sur des adresses exposées est acceptée, y compris lorsqu'elle couvre un seul registre d'un champ logique multi-registres.

## Préconditions

- plage testée entièrement exposée par GEL-MAP-V1 ;
- quantité comprise entre 1 et 125 registres.

## Procédure générique

1. Choisir un registre exposé quelconque et le lire seul.
2. Choisir un champ logique de plus d'un registre.
3. Lire une sous-plage stricte de ce champ, par exemple son premier registre seul.
4. Vérifier que la requête n'est pas rejetée du seul fait qu'elle est partielle.

## Résultat attendu

- réponse FC03 normale ;
- quantité retournée égale à la quantité demandée ;
- aucune obligation de lire le champ logique complet.

## Hors périmètre

Le sens de la valeur partielle et sa reconstruction fonctionnelle ne sont pas validés ici.
