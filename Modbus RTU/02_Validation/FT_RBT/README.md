# FT-RBT — Robustesse protocolaire

## 1. Objet

FT-RBT valide le comportement du protocole TR2 lorsqu'un échange nominal est perturbé, uniquement lorsque la V1 fournit un oracle exploitable sans hypothèse supplémentaire.

## 2. Principe

La famille ne redéfinit pas les oracles des familles gelées. Elle compose ces oracles autour d'une perturbation et vérifie que les garanties explicitement spécifiées restent satisfaites.

## 3. Sous-familles

1. `FT-RBT-01_Requetes_invalides_non_corruption` ;
2. `FT-RBT-02_Perte_reponse_retransmission_transactionnelle` ;
3. `FT-RBT-03_Repetitions_solicitations_transactionnelles_degradees` ;
4. `FT-RBT-04_Lectures_transition_solicitations_rapprochees` ;
5. `FT-RBT-05_Trames_degradees_timing_resynchronisation_non_specifies`.

## 4. Frontières

FT-RBT ne reprend pas la propriété de FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-INT, FT-CMD ou FT-SEQ. Les comportements après reboot restent exclusivement FT-PER.

## 5. Statut

Famille en reconstruction sur `audit/ft-rbt-v1`. Gel interdit avant passe croisée finale et validation explicite.
