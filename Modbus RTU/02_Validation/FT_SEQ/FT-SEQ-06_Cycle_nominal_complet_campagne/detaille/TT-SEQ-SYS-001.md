# TT-SEQ-SYS-001 — Cycle nominal complet de campagne

## 1. Objectif

Valider en une seule exécution système la continuité fonctionnelle V1 depuis l'établissement d'un contexte exploitable jusqu'à la consultation d'une campagne normalement clôturée, en passant par synchronisation temporelle, activation de configuration, START et STOP.

Ce test est l'essai E2E principal FT-SEQ. Il compose les oracles existants et ne les redéfinit pas.

## 2. Scénarios de référence

- FT-SEQ-01 — qualification initiale / contexte
- `TT-SEQ-TIME-001` — préparation et synchronisation temporelle
- `TT-SEQ-CONFIG-001` — préparation et activation configuration
- `TT-SEQ-CAMP-001` — START et établissement campagne
- `TT-SEQ-CAMP-002` — STOP, clôture et consultation

## 3. Préconditions générales

- capteur accessible par Modbus RTU ;
- acquisition initialement arrêtée ou amenée à un état arrêté par un moyen conforme au référentiel ;
- support SD exploitable ;
- absence de défaut critique bloquant START ;
- jeu de configuration nominal valide et complet ;
- référence temporelle valide pour le test ;
- moyens de lecture B0/B1/B2/B4/B5/B6 et, si utile, B7 ;
- aucune perturbation hostile volontaire dans ce scénario nominal.

## 4. Procédure E2E

### Phase A — Contexte initial

1. Lire les informations nécessaires pour identifier l'équipement, vérifier la compatibilité protocolaire et établir les états utiles B1/B2/B4/B5/B6/B7.
2. Ne pas imposer d'ordre entre ces lectures au titre d'une exigence V1 ; l'ordre choisi par le banc est seulement opératoire.
3. Confirmer que les préconditions nécessaires aux phases suivantes sont satisfaites ou préparer le contexte par des moyens normatifs.

### Phase B — Référence temporelle

4. Exécuter la chaîne nominale de `TT-SEQ-TIME-001` : préparer une référence temporelle B2, constater l'absence d'application immédiate, exécuter SYNC TIME avec succès et confirmer la cohérence temporelle post-synchronisation.
5. Conserver comme contexte la référence temporelle effectivement établie pour l'interprétation des timestamps ultérieurs, sans exiger d'égalité bit-à-bit entre lectures séparées.

### Phase C — Configuration active

6. Exécuter la chaîne nominale de `TT-SEQ-CONFIG-001` : préparer une configuration valide, mettre à jour son CRC, constater l'absence d'activation immédiate, exécuter APPLY CONFIG avec succès et confirmer l'image active cohérente.
7. Conserver comme contexte la configuration active obtenue pour START.

### Phase D — Démarrage et campagne

8. Vérifier les préconditions START via les oracles propriétaires.
9. Exécuter START conformément à `TT-SEQ-CAMP-001`.
10. Confirmer que l'acquisition est en cours et qu'une nouvelle campagne B6 cohérente avec l'état en cours a été ouverte.
11. Enregistrer les informations normativement utilisables permettant de retrouver cette campagne après STOP, sans dépendre de l'égalité non définie `B1.active_campaign_id == B6.campaign_id` ni d'un `+1` immédiat du compteur.

### Phase E — Arrêt et clôture

12. Exécuter STOP conformément à `TT-SEQ-CAMP-002`.
13. Confirmer que l'acquisition est arrêtée.
14. Retrouver la campagne précédemment en cours dans B6 et confirmer sa mise en cohérence/clôture ainsi que les métadonnées finales applicables.

### Phase F — Consultation finale

15. Consulter les états/inventaires/diagnostics utiles pour documenter l'état final du cycle.
16. La consultation B7 est une étape de preuve pratique du scénario ; elle n'est pas transformée en obligation V1 après chaque STOP.
17. Vérifier qu'aucune contradiction n'apparaît entre les jalons normatifs successifs du cycle selon les oracles gelés.

## 5. Verdict FT-SEQ E2E

### PASS

Le scénario complet atteint son état final sans rupture d'une dépendance normative :
- temps préparé puis appliqué ;
- configuration préparée puis activée ;
- START réussi ;
- acquisition et campagne en cours établies ;
- STOP réussi ;
- acquisition arrêtée ;
- campagne clôturée/cohérente et consultable.

Tous les jalons sont réalisés dans une même exécution logique du cycle.

### FAIL

Une étape dont les préconditions sont satisfaites échoue sur un oracle normatif ou l'état produit par une étape est contradictoire avec l'effet normatif attendu et empêche la continuité du cycle.

Le rapport doit identifier :
- la phase ;
- le jalon ;
- l'oracle propriétaire ;
- le test élémentaire de référence lorsque disponible.

## 6. Non-concluable

Un jalon est déclaré non concluable, et non remplacé par une règle inventée, lorsque le banc ne peut établir une propriété requise qu'en utilisant une relation explicitement `NOT_DEFINED` ou une tolérance absente de V1.

Le caractère non concluable d'un jalon requis empêche un PASS global et doit être documenté séparément d'un FAIL produit.

## 7. Garde-fous

Le test ne doit pas conclure FAIL uniquement parce que :
- les lectures de qualification ne suivent pas un ordre supposé ;
- SYNC TIME et APPLY CONFIG sont réalisés dans un autre ordre dans une autre utilisation conforme ;
- les timestamps de transactions distinctes ne sont pas égaux ;
- `active_campaign_id` et `campaign_id` ne sont pas utilisés comme égalité normative ;
- `total_campaign_count` n'expose pas immédiatement un `+1` strict ;
- B7 n'est pas consulté après STOP dans une autre séquence ;
- le cycle dépasse une durée arbitraire non normée.

## 8. Exclusions

Hors de ce test :
- refus volontaires et reprise : FT-SEQ-07 ;
- reboot/persistance : FT-PER ;
- pertes de trames, répétitions agressives, délais hostiles : FT-RBT ;
- variantes combinatoires déjà couvertes par les familles propriétaires.

## 9. Traçabilité

- Exigences propriétaires : `SEQ06-R01`, `SEQ06-R10`.
- Scénarios composés : FT-SEQ-02, 03, 04, 05.
- Qualification : FT-SEQ-01, sans handshake normatif.
- Familles élémentaires : FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-INT, FT-CMD.