# FT-SEQ-04 — Référentiel source : démarrage et établissement d'une campagne

## 1. Objet

Valider la chaîne V1 complète allant d'un contexte autorisant le démarrage à une acquisition effectivement active avec une nouvelle campagne B6 cohérente avec l'état « en cours ».

FT-SEQ-04 porte la continuité du scénario START. Les préconditions et résultats de commande restent FT-CMD ; les effets élémentaires B5 → B1/B6 restent FT-INT ; les invariants internes de campagne restent FT-BLK.

## 2. Sources normatives et délégations

Sources V1 principales :
- `01_Specification_source/bloc1.md` ;
- `01_Specification_source/bloc2.md` ;
- `01_Specification_source/bloc5.md` ;
- `01_Specification_source/bloc6.md`.

Oracles gelés composés :
- `FT_CMD/FT-CMD-06_Acquisition_diagnostic/` : acceptation/refus et résultat START ;
- `FT_INT/FT-INT-04_Acquisition_campagnes/` : acquisition active, ouverture d'une nouvelle campagne, cohérence campagne en cours et base temporelle ;
- `FT_BLK/FT-BLK-05_Inventaire_campagnes/` : invariants internes B6 ;
- FT-STR / FT-ACC / FT-LIM pour structure, accès et domaines.

## 3. Chaîne normative retenue

La V1 établit :
1. START n'est accepté que si une configuration active valide existe, le support SD est exploitable, aucun défaut critique bloquant n'est actif et l'acquisition n'est pas déjà active ;
2. une commande START réussie démarre effectivement l'acquisition ;
3. elle ouvre une nouvelle campagne selon la logique firmware ;
4. l'état système est mis à jour ;
5. B1 définit `acquisition_state = 1` comme acquisition en cours ;
6. B6 définit `campaign_state = 2` comme campagne en cours, `end_timestamp = 0` pour une campagne en cours et `campaign_id != 0` pour une campagne valide ;
7. les timestamps de campagne utilisent la base temporelle B2.

La V1 ne définit pas explicitement l'égalité `B1.active_campaign_id == B6.campaign_id`, ni l'incrément observable immédiat et exact de `total_campaign_count`.

## 4. Exigences FT-SEQ-04

### SEQ04-R01 — START puis établissement d'une campagne en cours

- Classification : `COVERED`.
- Propriétaire : FT-SEQ-04.
- Test : `TT-SEQ-CAMP-001`.
- Exigence : depuis un contexte satisfaisant les préconditions START, une commande START réussie doit conduire, dans une même chaîne fonctionnelle, à une acquisition B1 active et à l'ouverture d'une nouvelle campagne B6 cohérente avec l'état en cours.
- Nature de l'oracle FT-SEQ : continuité du scénario complet.

### SEQ04-R02 — Préconditions et succès transactionnel START

- Classification : `DELEGATED`.
- Propriétaire : FT-CMD-06.

### SEQ04-R03 — START réussi → acquisition B1 active

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-04.

### SEQ04-R04 — START réussi → ouverture d'une nouvelle campagne B6

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-04.

### SEQ04-R05 — Campagne créée cohérente avec l'état en cours

- Classification : `DELEGATED`.
- Propriétaire principal : FT-INT-04 ; invariants internes B6 : FT-BLK-05.

### SEQ04-R06 — Base temporelle des timestamps de campagne

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-04.

### SEQ04-R07 — B1.active_campaign_id == B6.campaign_id

- Classification : `NOT_DEFINED`.
- Justification : la V1 expose les deux champs mais ne formule pas explicitement leur égalité normative. FT-SEQ ne la déduit pas de leur nom.

### SEQ04-R08 — total_campaign_count augmente exactement de 1 dès la première lecture post-START

- Classification : `NOT_DEFINED`.
- Justification : l'ouverture d'une nouvelle campagne est normative, mais ni délai de publication ni incrément strict immédiatement observable ne sont définis.

### SEQ04-R09 — Délai maximal START → acquisition active / campagne visible

- Classification : `NOT_DEFINED`.
- Justification : aucune borne temporelle globale n'est définie par V1.

### SEQ04-R10 — Priorité entre plusieurs causes simultanées de refus START

- Classification : `NOT_DEFINED`.
- Propriétaire de la dette : FT-CMD-06.
- Justification : la V1 définit plusieurs causes de refus mais pas leur priorité lorsqu'elles coexistent.

## 5. Oracle composé de TT-SEQ-CAMP-001

Le scénario vérifie :

`contexte START valide` → `START réussi` → `acquisition_state = en cours` → `nouvelle campagne ouverte` → `campagne cohérente avec l'état en cours` → `timestamps dans la base B2`.

Le verdict FT-SEQ est PASS seulement si les jalons applicables sont satisfaits dans la même exécution du scénario.

L'identification de la campagne créée doit utiliser les moyens normativement disponibles dans B6 et les oracles gelés. Elle ne doit pas reposer sur l'égalité non définie avec `B1.active_campaign_id`, ni uniquement sur `total_campaign_count + 1`.

## 6. Anti-fabrication

Ne pas imposer :
- `B1.active_campaign_id == B6.campaign_id` ;
- `total_campaign_count_après = total_campaign_count_avant + 1` à la première lecture ;
- un délai arbitraire de publication ;
- une égalité exacte entre timestamp de START et `start_timestamp` ;
- une priorité inventée entre causes simultanées de refus ;
- une durée automatique de campagne déduite de `campaign_duration_s` si aucune règle explicite ne l'impose ;
- un comportement après reboot, réservé à FT-PER.

## 7. Frontières

- refus START isolés : FT-CMD-06 ;
- START sans configuration puis correction/reprise : FT-SEQ-07 ;
- arrêt et clôture : FT-SEQ-05 ;
- cycle complet préparation→campagne→arrêt : FT-SEQ-06 ;
- reboot/persistance : FT-PER ;
- perturbations hostiles : FT-RBT.