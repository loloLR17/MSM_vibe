# FT-CMD — Moteur de commandes Bloc 5

## 1. Objet

FT-CMD valide le comportement transactionnel normatif du Bloc 5 : soumission, transaction, idempotence, corrélation, concurrence, états, résultats, historique et règles propres aux commandes.

## 2. Référentiel

Source normative principale : `01_Specification_source/bloc5.md` V1, complétée uniquement par les règles normatives explicites des autres blocs et par les délégations des familles gelées.

Les compléments métier explicitement informatifs ne sont pas utilisés comme oracles.

## 3. Décomposition validée

- `FT-CMD-01_Soumission_transaction`
- `FT-CMD-02_Idempotence_correlation`
- `FT-CMD-03_Etats_resultats_historique`
- `FT-CMD-04_Concurrence_controles`
- `FT-CMD-05_Configuration_temps`
- `FT-CMD-06_Acquisition_diagnostic`
- `FT-CMD-07_Maintenance_commandes_protegees`

## 4. Convention des tests

Convention retenue : `TT-CMD-B05-<numéro>` avec numérotation globale à FT-CMD.

## 5. Documents consolidés

- `MATRICE_COUVERTURE_FT_CMD_V1.md` : consolidation des 66 exigences / points de couverture ;
- `AUDIT_FINAL_FT_CMD_V1.md` : passe croisée finale et décision de gel ;
- `EVOLUTIONS_CANDIDATES_V1_1.md` : backlog séparé des clarifications et évolutions souhaitables.

## 6. Statut V1

- FT-CMD-01 à FT-CMD-07 : validées et mergées ;
- matrice consolidée : finalisée ;
- audit croisé final : réalisé ;
- dettes normatives : tracées sans fabrication d'oracle ;
- évolutions souhaitables : sorties du corpus V1 et conservées séparément pour une éventuelle V1.1.

**FT-CMD V1 est finalisée et gelée.**

Toute évolution ultérieure doit passer par une nouvelle version de spécification et un audit de non-régression des familles impactées.
