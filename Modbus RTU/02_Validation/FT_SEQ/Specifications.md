# FT-SEQ — Spécifications de validation des scénarios séquentiels V1

## 1. Objet

FT-SEQ valide les enchaînements fonctionnels complets qu'une centrale peut exécuter sur l'interface Modbus RTU TR2 V1.

La famille ne redéfinit pas les oracles élémentaires déjà propriétaires des familles gelées. Elle valide la cohérence de leur composition dans une séquence multi-actions ou multi-transactions disposant d'un fondement normatif V1.

## 2. Référentiel normatif

Le référentiel applicable est, par ordre de priorité :

1. `01_Specification_source/bloc0.md` à `bloc7.md` et `charte_typage.md` ;
2. mapping unifié dérivé conforme ;
3. gouvernance et plan maître de validation ;
4. familles gelées FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-INT et FT-CMD.

Les compléments métier explicitement informatifs ne sont jamais utilisés comme oracles normatifs.

## 3. Principe d'admission dans FT-SEQ

Une règle ou un scénario est admis comme propriété FT-SEQ seulement si :

1. il nécessite plusieurs actions, transactions ou phases fonctionnelles successives ;
2. chaque jalon normatif peut être rattaché à une source V1 identifiable ;
3. la propriété recherchée porte sur la réussite, le refus puis la reprise, ou la cohérence de la chaîne complète ;
4. l'oracle n'est pas déjà entièrement propriétaire d'une famille gelée ;
5. aucun ordre, délai, état intermédiaire ou mécanisme de reprise non spécifié n'est inventé.

## 4. Doctrine de propriété

Dans un scénario FT-SEQ, les familles propriétaires restent :

- FT-STR : structure, tailles, encodage, MSW/LSW, snapshots ;
- FT-ACC : droits RO/RW et exceptions d'accès ;
- FT-LIM : domaines simples, limites et valeurs réservées ;
- FT-BLK : invariants intra-bloc ;
- FT-INT : effets et cohérences inter-blocs élémentaires ;
- FT-CMD : soumission, transaction, corrélation, idempotence, concurrence, états/résultats B5 et préconditions propres aux commandes ;
- FT-RBT : défauts de communication, pertes de réponse, répétitions agressives et séquencement hostile ;
- FT-PER : reboot, persistance et reprise après redémarrage.

FT-SEQ peut utiliser ces propriétés comme préconditions ou sous-oracles, mais ne les redéfinit pas.

Le verdict FT-SEQ porte sur la composition séquentielle complète.

## 5. Classifications

Chaque point de couverture utilise exclusivement :

- `COVERED` : oracle V1 explicite et testable dans FT-SEQ ;
- `CONDITIONAL` : oracle V1 explicite mais exécution dépendante d'une condition de banc ou d'un état constructible non garanti ;
- `DELEGATED` : propriété réelle mais appartenant à une autre famille ;
- `TRACE_ONLY` : relation utile à la traçabilité sans oracle FT-SEQ autonome ;
- `NOT_DEFINED` : comportement, ordre, délai ou résultat non défini en V1.

## 6. Règles de construction des scénarios

### 6.1 Pas de duplication

Un scénario FT-SEQ ne repasse pas exhaustivement les tests unitaires des familles amont. Les étapes nécessaires sont exécutées comme préconditions, stimuli ou contrôles de jalons.

### 6.2 États intermédiaires

FT-SEQ ne doit pas exiger l'observation d'un état transitoire si la V1 n'impose ni sa durée d'exposition ni son observabilité entre deux transactions Modbus distinctes.

En particulier, aucune séquence exhaustive des états intermédiaires du moteur de commandes n'est imposée au-delà des oracles FT-CMD gelés.

### 6.3 Temps

Aucune égalité temporelle stricte entre lectures séparées n'est imposée sans tolérance normative. Aucun délai maximal de transition n'est créé par FT-SEQ.

### 6.4 Ordre des opérations

Un ordre n'est normatif que lorsqu'il résulte explicitement de dépendances V1. Une pratique d'exploitation logique ou recommandée ne devient pas une exigence FT-SEQ.

### 6.5 Refus et reprise

FT-SEQ peut couvrir une chaîne `refus → correction de précondition → nouvelle tentative → succès` lorsque :

- le refus initial est explicitement normé ;
- la correction correspond à une précondition V1 identifiable ;
- le scénario ne suppose aucun mécanisme implicite de reset ou de récupération.

Le code de refus initial reste propriétaire FT-CMD.

## 7. Décomposition officielle V1

### FT-SEQ-01 — Qualification initiale et contexte

Composer les informations nécessaires à l'établissement d'un contexte d'exploitation : identification, compatibilité, état système, temps, configuration, inventaire et diagnostic.

Aucun handshake ou ordre exhaustif de lecture n'est imposé s'il n'est pas défini en V1.

### FT-SEQ-02 — Préparation et activation de configuration

Valider la chaîne : préparation B4 → CRC préparé cohérent → commande APPLY CONFIG B5 → succès → configuration active cohérente.

### FT-SEQ-03 — Préparation et synchronisation temporelle

Valider la chaîne : préparation B2 → absence d'effet immédiat → commande SYNC TIME B5 → synchronisation effective → état temporel final cohérent.

### FT-SEQ-04 — Démarrage acquisition et ouverture campagne

Valider la chaîne : contexte admissible → START → acquisition active → ouverture d'une campagne → observables système/campagne cohérents.

### FT-SEQ-05 — Arrêt, clôture et consultation campagne

Valider la chaîne : campagne active → STOP → arrêt propre → campagne clôturée → consultation cohérente de l'inventaire et des métadonnées finales.

### FT-SEQ-06 — Cycle nominal complet de campagne

Valider le scénario système de bout en bout en composant qualification, temps, configuration, démarrage, déroulement observable, arrêt, clôture et consultation finale, sans créer d'exigence supplémentaire.

### FT-SEQ-07 — Refus corrigé et reprise de séquence

Valider les reprises fonctionnelles explicitement constructibles après correction d'une précondition normative, sans couvrir les défauts de communication ou les reboots.

## 8. Exclusions explicites V1

Ne sont pas propriétaires FT-SEQ :

- mécanismes de soumission et corrélation B5 ;
- codes résultat des commandes pris isolément ;
- relations inter-blocs élémentaires prises isolément ;
- validation exhaustive des domaines ;
- accès invalides ;
- pertes de trame/réponse et retries hostiles ;
- persistance après reboot ;
- performances ou délais non normés ;
- arrêt automatique à expiration de `campaign_duration_s` tant qu'aucun oracle V1 explicite ne l'impose ;
- ordre obligatoire de qualification initiale non défini ;
- obligation générale de synchroniser l'heure avant chaque campagne si elle n'est pas explicitement imposée ;
- procédure maintenance/autotest/ACK ordonnée lorsqu'elle repose seulement sur une pratique plausible.

## 9. Convention d'identification

Les tests utilisent la convention du plan maître :

`TT-SEQ-<scope>-<numéro>`

Le scope `SYS` est privilégié pour les scénarios système complets. Un scope composite peut être utilisé lorsqu'il améliore la traçabilité sans ambiguïté.

## 10. Structure documentaire attendue

Chaque sous-famille comporte au minimum :

- un `README.md` de cadrage et couverture ;
- un répertoire `source/` pour les exigences normalisées ;
- un répertoire `detaille/` pour les cas de test détaillés.

À la clôture FT-SEQ seront ajoutés :

- `MATRICE_COUVERTURE_FT_SEQ_V1.md` ;
- `AUDIT_FINAL_FT_SEQ_V1.md` ;
- `EVOLUTIONS_CANDIDATES_V1_1.md`.

## 11. Critères de gel

FT-SEQ V1 ne peut être gelée que si :

- les scénarios séquentiels normatifs pertinents ont été inventoriés ;
- toute exigence `COVERED` possède au moins un test ;
- toutes les délégations sont explicites ;
- aucune règle informative n'a été transformée en exigence ;
- toutes les zones `NOT_DEFINED` restent visibles ;
- aucun oracle FT-CMD, FT-INT ou autre famille gelée n'est dupliqué comme propriété FT-SEQ ;
- un audit croisé final B0 à B7 et inter-familles a été réalisé ;
- le gel reçoit une validation explicite avant merge dans `main`.
