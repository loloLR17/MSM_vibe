# Résultat FT-STR-07 — GEL-MAP-V1

## Références

- sous-famille : FT-STR-07 ;
- mapping : GEL-MAP-V1 ;
- gouvernance : GEL-GOV ;
- dépendances gelées : FT-STR-03, FT-STR-05, FT-STR-06.

## Résultat documentaire

La reconstruction est **CONFORME DOCUMENTAIREMENT** à la règle V1 de cohérence temporelle, sous réserve que `validate_ft_str_07_structure.py` retourne 0 sur le worktree considéré.

La suite active ne suppose plus qu’un bloc dynamique doive être identique entre deux requêtes successives.

## Corrections apportées

- suppression de l’équivalence erronée « état stable = toutes les valeurs immobiles » ;
- séparation entre stabilité inter-requêtes et cohérence intra-réponse ;
- remplacement du test d’atomicité sur `uint32` immobile par une exigence d’évolution contrôlée ou observable ;
- interdiction de conclure PASS en l’absence d’oracle temporel suffisant ;
- suppression du faux snapshot atomique du Bloc 4 sur 176 registres ;
- maintien de la limite FC03 et des règles de segmentation gelées par FT-STR-06.

## Couverture active

- données statiques justifiées : GEN-001 ;
- tous les `uint32` applicables : GEN-002 ;
- réponses dynamiques multi-registres, avec priorité B1/B2/B3/B6/B7 : GEN-003 ;
- influence du mode et de l’ordre de lecture sur les Blocs 0 à 7 : GEN-004.

## Contrôles terrain restant à exécuter

Aucun résultat runtime n’est fabriqué dans ce document.

Les contrôles d’atomicité et de cohérence dynamique doivent être exécutés sur la cible ou avec une instrumentation adaptée. Lorsque l’interface ne fournit pas d’oracle suffisant, le résultat attendu est :

**NON DÉMONTRABLE PAR L’INTERFACE SEULE / INSTRUMENTATION REQUISE**.

Ce statut n’est ni un PASS ni un FAIL ; il indique qu’une preuve supplémentaire est nécessaire.

## Conclusion

FT-STR-07 est structurellement reconstruite autour de la vraie exigence V1 : une image lisible sans ambiguïté temporelle, sans imposer une immobilité artificielle aux données dynamiques.