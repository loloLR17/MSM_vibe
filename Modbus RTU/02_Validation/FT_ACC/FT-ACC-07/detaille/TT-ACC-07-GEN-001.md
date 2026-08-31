# TT-ACC-07-GEN-001 — Audit croisé de couverture mapping ↔ FT-ACC

## Objectif
Vérifier statiquement la complétude, l'unicité et la cohérence de la couverture FT-ACC par rapport au mapping unifié dérivé de la V1.

## Sources
- spécification Modbus RTU V1 gelée ;
- mapping unifié gelé ;
- GEL-GOV-02 ;
- sources et index actifs FT-ACC-02 à FT-ACC-06 ;
- FT-STR-06 gelée pour la couverture structurelle en lecture.

## Préconditions
- artefacts gelés disponibles ;
- FT-ACC-02 à FT-ACC-06 auditées ;
- index actifs disponibles et cohérents avec leurs fiches source.

## Étapes
1. Extraire les cibles logiques du mapping.
2. Classer chaque cible en RW, RO non réservé ou réservé selon la V1 et le mapping dérivé conforme.
3. Vérifier la présence de chaque RW dans l'index actif FT-ACC-02.
4. Vérifier la présence de chaque RO non réservé dans l'index actif FT-ACC-03.
5. Vérifier la présence de chaque réservé dans l'index actif FT-ACC-04.
6. Détecter les cibles absentes, dupliquées ou classées dans plusieurs classes primaires.
7. Vérifier explicitement l'unicité de l'adresse 4017 et le classement réservé de `B3_RESERVED_0`.
8. Vérifier la cohérence des couvertures complémentaires FT-ACC-05 et FT-ACC-06 avec leurs sources.
9. Rechercher dans le chemin actif toute tolérance contraire à GEL-GOV-02, notamment « refusée ou sans effet observable ».
10. Vérifier qu'aucun test FT-ACC actif ne réintroduit une exigence structurelle déjà gelée dans FT-STR.
11. Produire la matrice et le verdict de consolidation.

## Résultats attendus
- 35 RW couverts par FT-ACC-02 ;
- 129 RO non réservés couverts par FT-ACC-03 ;
- 18 réservés couverts par FT-ACC-04 ;
- total : 182 cibles primaires uniques ;
- aucune cible orpheline ;
- aucun conflit de classification ;
- aucun doublon injustifié ;
- aucune doctrine obsolète active ;
- couvertures FT-ACC-05/06 cohérentes avec leur périmètre.

## Critère d'acceptation
PASS si tous les résultats attendus sont satisfaits. Toute divergence produit FAIL et une anomalie traçable ; aucun artefact gelé n'est modifié silencieusement.

## Mode d'exécution
Statique, automatisable.

## Traces à conserver
- matrice de consolidation ;
- liste éventuelle des anomalies ;
- versions/commits des artefacts audités.