# TT-SEQ-REC-001 — START refusé sans configuration puis reprise réussie

## Objectif
Valider la chaîne `refus 22 → correction configuration → START réussi` sans dupliquer les oracles FT-CMD.

## Préconditions
- acquisition arrêtée ;
- aucune configuration active valide ;
- support SD exploitable ;
- absence de défaut critique bloquant ;
- possibilité de préparer une configuration V1 valide.

## Procédure
1. Soumettre START conformément à FT-CMD.
2. Vérifier avec FT-CMD-06 le refus normatif code `22`.
3. Préparer puis appliquer une configuration valide conformément à FT-SEQ-02.
4. Vérifier que la configuration active résultante satisfait les préconditions START.
5. Soumettre une nouvelle transaction START conforme à FT-CMD.
6. Vérifier son succès.
7. Vérifier conformément à FT-SEQ-04 que l'acquisition devient active et qu'une nouvelle campagne cohérente est ouverte.

## PASS
Le refus initial est celui attendu, sa cause est corrigée normativement, puis la nouvelle tentative réussit avec les effets attendus.

## Garde-fous
- ne pas imposer le même `transaction_id` aux deux START ;
- ne pas imposer de délai/backoff non normé ;
- ne pas accepter une simple répétition sans correction comme preuve FT-SEQ.