# TT-ACC-06-GEN-015 — Écriture multi-bloc

## 1. Objectif
Valider le traitement d’une requête Modbus invalide pour le scénario `Écriture multi-bloc`.

## 2. Références
- FT-ACC-06 — Accès hors plage, partiels ou non autorisés
- Mapping Modbus unifié
- Doctrine : exception explicite obligatoire

## 3. Préconditions
- FT-STR validée
- FT-ACC-01 à FT-ACC-05 validées
- simulateur stable
- snapshot mémoire disponible si écriture tentée

## 4. Étapes
1. Construire une requête violant une frontière de validité.
2. Exécuter la requête.
3. Contrôler la présence d’une exception Modbus explicite.
4. Si écriture tentée, comparer l’image mémoire avant/après.
5. Vérifier l’absence d’exécution partielle.

## 5. Résultat attendu
- exception Modbus explicite ;
- aucune modification mémoire ;
- comportement déterministe.

## 6. Critères d’acceptation
- refus clair ;
- absence d’effet mémoire ;
- cohérence mapping ↔ comportement.

## 7. Criticité
P0
