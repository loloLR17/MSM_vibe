# FT-RBT — Robustesse protocolaire

## 1. Objet

FT-RBT valide le comportement du protocole TR2 lorsqu'un échange nominal est perturbé, uniquement lorsque la V1 fournit un oracle exploitable sans hypothèse supplémentaire.

## 2. Principe

La famille ne redéfinit pas les oracles des familles gelées. Elle compose ces oracles autour d'une perturbation et vérifie que les garanties explicitement spécifiées restent satisfaites.

## 3. Sous-familles

1. `FT-RBT-01_Requetes_invalides_non_corruption` ;
2. `FT-RBT-02_Perte_reponse_retransmission` ;
3. `FT-RBT-03_Repetitions_solicitations_transactionnelles_degradees` ;
4. `FT-RBT-04_Lectures_transition_solicitations_rapprochees` ;
5. `FT-RBT-05_Trames_degradees_timing_resynchronisation_non_specifies`.

## 4. Frontières

FT-RBT ne reprend pas la propriété de FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-INT, FT-CMD ou FT-SEQ. Les comportements après reboot restent exclusivement FT-PER.

## 5. Synthèse V1

- 36 points de couverture consolidés ;
- 4 tests propriétaires FT-RBT ;
- 1 `COVERED` ;
- 3 `CONDITIONAL` ;
- 12 `DELEGATED` ;
- 1 `TRACE_ONLY` ;
- 19 `NOT_DEFINED`.

La forte proportion de `NOT_DEFINED` reflète l'absence réelle d'oracle V1 pour les politiques de timeout, retry, charge, trames RTU corrompues et résynchronisation ; ces comportements ne sont pas inventés par la validation.

## 6. Statut

Reconstruction et passe croisée finale terminées sur `audit/ft-rbt-v1`. Famille prête pour gel V1 sous réserve de validation explicite et merge dans `main`.
