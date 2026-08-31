# FT-SEQ-05 — Référentiel source : arrêt, clôture et consultation de campagne

## 1. Objet

Valider la chaîne V1 complète allant d'une acquisition active avec campagne en cours à un arrêt propre, une campagne mise en cohérence/clôturée puis consultable via l'inventaire B6.

FT-SEQ-05 porte la continuité de cette chaîne. La transaction STOP reste FT-CMD, ses effets élémentaires restent FT-INT et la navigation/invariants internes B6 restent FT-BLK.

## 2. Sources normatives et délégations

Sources V1 principales :
- `01_Specification_source/bloc1.md` ;
- `01_Specification_source/bloc2.md` ;
- `01_Specification_source/bloc5.md` ;
- `01_Specification_source/bloc6.md`.

Oracles gelés composés :
- `FT_CMD/FT-CMD-06_Acquisition_diagnostic/` : condition, acceptation/refus et résultat STOP ;
- `FT_INT/FT-INT-04_Acquisition_campagnes/` : STOP → acquisition arrêtée, campagne mise en cohérence/clôturée, base temporelle ;
- `FT_BLK/FT-BLK-05_Inventaire_campagnes/` : sélection et invariants internes de l'entrée B6 ;
- FT-STR / FT-ACC / FT-LIM pour structure, accès et domaines.

## 3. Chaîne normative retenue

La V1 établit :
1. STOP exige une acquisition active ;
2. une commande STOP réussie entraîne un arrêt propre de l'acquisition ;
3. les buffers sont vidés et les fichiers fermés ;
4. la campagne en cours est mise en cohérence/clôturée ;
5. B1 expose ensuite l'acquisition arrêtée (`acquisition_state = 0`) ;
6. B6 permet de sélectionner une campagne par `selected_campaign_index` et d'en consulter les métadonnées ;
7. une campagne valide expose `campaign_id != 0` ;
8. les timestamps B6 utilisent la base temporelle B2.

La V1 ne donne pas d'oracle Modbus direct permettant de constater séparément le vidage physique des buffers ou la fermeture des fichiers. Ces effets sont normatifs mais leur preuve observable est bornée à la cohérence finale exposée.

## 4. Exigences FT-SEQ-05

### SEQ05-R01 — STOP puis clôture et consultation de la campagne

- Classification : `COVERED`.
- Propriétaire : FT-SEQ-05.
- Test : `TT-SEQ-CAMP-002`.
- Exigence : depuis une acquisition active avec campagne en cours identifiée par les mécanismes normatifs disponibles, une commande STOP réussie doit conduire dans une même chaîne à une acquisition arrêtée, une campagne mise en cohérence/clôturée puis une entrée B6 consultable correspondant à cette campagne.

### SEQ05-R02 — Condition et succès transactionnel STOP

- Classification : `DELEGATED`.
- Propriétaire : FT-CMD-06.

### SEQ05-R03 — STOP réussi → acquisition B1 arrêtée

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-04.

### SEQ05-R04 — STOP → campagne en cours mise en cohérence / clôturée

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-04.
- Limite : oracle borné aux effets explicitement observables/normés par V1.

### SEQ05-R05 — Consultation de la campagne via sélection B6

- Classification : `DELEGATED`.
- Propriétaire : FT-BLK-05.
- Justification : navigation par index et validité de sélection sont déjà couvertes.

### SEQ05-R06 — Identifiant non nul et invariants internes de l'entrée finale

- Classification : `DELEGATED`.
- Propriétaire : FT-BLK-05.

### SEQ05-R07 — Base temporelle des timestamps finaux

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-04.

### SEQ05-R08 — Observation Modbus directe du flush des buffers

- Classification : `TRACE_ONLY`.
- Justification : le flush est un effet normatif de STOP, mais aucun registre/indicateur V1 spécifique ne fournit un oracle direct et indépendant de « buffer vidé ». La cohérence finale de campagne constitue l'observable fonctionnel disponible.

### SEQ05-R09 — Observation Modbus directe de la fermeture des fichiers

- Classification : `TRACE_ONLY`.
- Justification : même limite d'observabilité ; aucun champ V1 ne constitue un témoin direct de fermeture physique des fichiers.

### SEQ05-R10 — État final obligatoirement campaign_state = 3 dans tous les cas de STOP réussi

- Classification : `NOT_DEFINED` comme oracle universel.
- Justification : B6 définit `3 = terminée`, mais V1 demande plus généralement que STOP mette la campagne en cohérence/clôture. Elle définit également des états d'erreur/corruption. FT-SEQ n'efface pas ces possibilités en imposant universellement 3 sans condition supplémentaire.

### SEQ05-R11 — end_timestamp strictement égal au timestamp de STOP

- Classification : `NOT_DEFINED`.
- Justification : transactions non simultanées, absence de règle d'égalité stricte/tolérance chiffrée.

### SEQ05-R12 — duration_s = end_timestamp - start_timestamp comme égalité universelle

- Classification : `NOT_DEFINED`.
- Justification : FT-BLK-05 conserve déjà la cohérence de durée comme `CONDITIONAL / À FORMALISER` et précise que le firmware peut recalculer la durée.

### SEQ05-R13 — Délai maximal STOP → campagne finale consultable

- Classification : `NOT_DEFINED`.
- Justification : aucune borne temporelle globale V1.

## 5. Oracle composé de TT-SEQ-CAMP-002

Le scénario vérifie :

`acquisition active + campagne en cours` → `STOP réussi` → `acquisition arrêtée` → `campagne mise en cohérence/clôturée` → `campagne sélectionnable et consultable dans B6` → `métadonnées finales cohérentes selon les oracles applicables`.

Le scénario doit conserver une méthode d'identification de la campagne avant/après qui n'utilise pas comme oracle l'égalité non définie `B1.active_campaign_id == B6.campaign_id`.

## 6. Anti-fabrication

Ne pas imposer :
- `campaign_state = 3` universellement si la V1 autorise un état final cohérent différent dans un cas normativement justifié ;
- égalité exacte entre `end_timestamp` et un timestamp de commande ;
- égalité universelle exacte de durée ;
- témoin Modbus inventé pour flush/fermeture de fichiers ;
- délai arbitraire de finalisation ;
- égalité B1/B6 déjà classée `NOT_DEFINED` ;
- comportement après reboot.

## 7. Frontières

- STOP sur acquisition inactive et code 21 : FT-CMD-06 ;
- STOP invalide puis correction/reprise : FT-SEQ-07 ;
- démarrage/ouverture : FT-SEQ-04 ;
- cycle nominal complet : FT-SEQ-06 ;
- reboot/persistance : FT-PER ;
- perturbations hostiles : FT-RBT.