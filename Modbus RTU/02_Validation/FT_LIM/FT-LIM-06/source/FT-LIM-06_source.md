# FT-LIM-06 — Source normative

## Référence principale
V1 Bloc 6 — Inventaire campagnes.

## Exigences

### LIM06-RQ-001 — État campagne
`campaign_state` appartient à {0,1,2,3,4,5} : vide, en préparation, en cours, terminée, erreur, partiellement corrompue.

### LIM06-RQ-002 — Intégrité données
`data_integrity_status` appartient à {0,1,2,3} : inconnue, OK, corrompue, partielle.

### LIM06-RQ-003 — Santé stockage
`storage_health_status` appartient à {0,1,2,3} : OK, warning, dégradé, critique.

### LIM06-RQ-004 — Identifiant campagne
Pour une campagne valide, `campaign_id` ne doit jamais être 0.

### LIM06-RQ-005 — Campagne en cours
`end_timestamp = 0` si la campagne est en cours (`campaign_state=2`).

### LIM06-RQ-006 — Durée
`duration_s` doit être cohérent avec les timestamps et peut être recalculé par le firmware.

La V1 ne définit pas ici une formule exhaustive, une tolérance, un arrondi ni la sémantique numérique exacte de `duration_s` pendant une campagne en cours. FT-LIM-06 n’en invente pas. En conséquence, cette exigence est tracée comme règle normative à compléter mais ne produit pas de verdict PASS/FAIL autonome tant qu’un oracle objectif supplémentaire n’est pas défini par le référentiel.

## Relations non définies

Ne pas déduire :
- terminée => intégrité OK ;
- erreur => taille de données non nulle ;
- état stockage => intégrité d’une campagne donnée ;
- `duration_s = end_timestamp-start_timestamp` comme oracle exact tant qu’aucune règle normative plus précise ne l’impose.
