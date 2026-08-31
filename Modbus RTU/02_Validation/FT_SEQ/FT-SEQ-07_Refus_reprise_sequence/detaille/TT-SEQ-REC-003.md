# TT-SEQ-REC-003 — STOP refusé à l'arrêt puis reprise dans un contexte valide

## Objectif
Valider la chaîne `STOP refusé code 21 → établissement d'une acquisition active → STOP réussi`.

## Préconditions
- acquisition initialement arrêtée ;
- contexte permettant ensuite un START nominal (configuration active valide, SD exploitable, absence de défaut critique bloquant).

## Procédure
1. Soumettre STOP alors que l'acquisition est arrêtée.
2. Vérifier avec FT-CMD-06 le refus normatif code `21`.
3. Soumettre START dans un contexte satisfaisant ses préconditions.
4. Vérifier conformément à FT-SEQ-04 que START réussit, que l'acquisition devient active et qu'une campagne est ouverte.
5. Soumettre une nouvelle transaction STOP conforme à FT-CMD.
6. Vérifier son succès.
7. Vérifier conformément à FT-SEQ-05 que l'acquisition est arrêtée et la campagne mise en cohérence/clôturée.

## PASS
Le STOP initial est correctement refusé, un contexte où STOP est applicable est établi normativement, puis STOP réussit avec ses effets attendus.

## Garde-fous
- ne pas qualifier START de « réparation » interne du premier STOP ; il établit simplement la précondition manquante ;
- pas de retry automatique ni de temporisation inventée ;
- refus et code restent propriétaires FT-CMD.