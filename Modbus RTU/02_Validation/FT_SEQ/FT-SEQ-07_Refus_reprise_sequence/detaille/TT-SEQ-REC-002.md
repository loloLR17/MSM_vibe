# TT-SEQ-REC-002 — SYNC TIME refusé sans préparation puis reprise réussie

## Objectif
Valider la chaîne `refus 19 → préparation temporelle → SYNC TIME réussi`.

## Préconditions
- absence de temps préparé au sens normatif B2/B5 ;
- horloge disponible ;
- possibilité d'écrire une référence temporelle valide.

## Procédure
1. Soumettre SYNC TIME conformément à FT-CMD.
2. Vérifier avec FT-CMD-05 le refus normatif code `19`.
3. Écrire une référence temporelle préparée valide dans B2.
4. Vérifier avec les oracles propriétaires que la préparation est présente et n'a pas encore appliqué le temps.
5. Soumettre une nouvelle transaction SYNC TIME conforme à FT-CMD.
6. Vérifier son succès.
7. Vérifier conformément à FT-SEQ-03/FT-INT-01 l'application effective et les observables temporels post-synchronisation.

## PASS
Le refus initial est correct, la cause est corrigée, puis la synchronisation réussit et produit les effets normatifs.

## Garde-fous
- aucune égalité temporelle stricte entre lectures séparées ;
- aucun délai de retry inventé ;
- aucune politique de `transaction_id` ajoutée.