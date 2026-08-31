# TT-SEQ-REC-004 — APPLY CONFIG refusé en acquisition puis reprise après STOP

## Objectif
Valider la chaîne `APPLY CONFIG refusé code 5 → STOP → nouvelle application réussie`.

## Préconditions
- acquisition active ;
- campagne en cours ;
- configuration préparée complète, valide et applicable, avec CRC conforme ;
- le seul obstacle intentionnel à APPLY CONFIG est l'acquisition en cours.

## Procédure
1. Soumettre APPLY CONFIG pendant l'acquisition active.
2. Vérifier avec FT-CMD-05 le refus normatif code `5`.
3. Vérifier que ce refus n'est pas utilisé comme preuve d'un échec interne d'application.
4. Exécuter STOP conformément à FT-SEQ-05 et vérifier que l'acquisition devient arrêtée.
5. Vérifier que les autres préconditions normatives d'APPLY CONFIG restent/sont satisfaites ; si la préparation doit être réétablie selon les observables V1, le faire conformément à FT-SEQ-02 sans inventer de persistance implicite.
6. Soumettre une nouvelle transaction APPLY CONFIG conforme à FT-CMD.
7. Vérifier son succès.
8. Vérifier conformément à FT-SEQ-02/FT-INT-02 que la configuration devient active et que l'image active est cohérente.

## PASS
Le refus initial code 5 est correct, l'acquisition est arrêtée normativement, les préconditions d'application sont établies puis la nouvelle application réussit avec ses effets attendus.

## Garde-fous
- ne pas supposer silencieusement la persistance de l'image préparée si les observables du scénario ne permettent pas de l'établir ;
- pas de délai de retry inventé ;
- pas de séquence `cmd_status` imposée ;
- pas de confusion entre refus pour acquisition active et erreur interne d'application.