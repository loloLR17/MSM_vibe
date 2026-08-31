# FT-BLK-06 — Exigences source normalisées

## 1. Références normatives

- `01_Specification_source/bloc0.md` V1 gelé ;
- `01_Specification_source/bloc5.md` V1 gelé ;
- plan de test Modbus TR2 pour la séparation FT-BLK / FT-CMD / FT-PER / FT-STR / FT-ACC.

Les compléments métier informatifs ne sont pas utilisés comme oracles normatifs.

## 2. Bloc 0 — Identification

### BLK06-B0-001 — Stabilité des informations en fonctionnement normal
Les informations du Bloc 0 doivent être statiques pendant le fonctionnement normal.

**Classification** : COVERED.

### BLK06-B0-002 — Absence de dépendance à l'état dynamique
Aucun champ du Bloc 0 ne doit dépendre d'un état dynamique du capteur.

**Classification** : COVERED, par observation avant/après changements dynamiques maîtrisés sans reboot ni opération de maintenance d'identité.

### BLK06-B0-003 — Unicité de device_id
`device_id` doit être unique.

**Classification** : CONDITIONAL — l'unicité ne peut pas être démontrée avec un DUT unique ; nécessite au minimum deux équipements distincts ou une procédure de contrôle de production adaptée.

### BLK06-B0-004 — Persistance de device_id
`device_id` doit être persistant.

**Classification** : DELEGATED à FT-PER pour les scénarios de reboot, coupure d'alimentation et récupération.

### BLK06-B0-005 — Structure et représentation
Encodage ASCII fixe, MSW/LSW, réservés et cohérence multi-registres.

**Classification** : TRACE_ONLY — FT-STR.

### BLK06-B0-006 — Accès RO
Le Bloc 0 est RO et les écritures invalides sont rejetées sans effet de bord.

**Classification** : TRACE_ONLY — FT-ACC.

## 3. Bloc 5 — Moteur de commandes

### BLK06-B5-001 — Déclenchement explicite
Les écritures dans les autres blocs ne déclenchent aucune action ; le déclenchement effectif passe par le Bloc 5.

**Classification** : DELEGATED à FT-CMD / FT-INT selon l'effet métier observé.

### BLK06-B5-002 — Front montant submit
Une commande n'est évaluée que lors du passage de `submit` de 0 à 1 ; le maintien à 1 ne doit pas provoquer de réexécution répétée.

**Classification** : DELEGATED à FT-CMD.

### BLK06-B5-003 — transaction_id
Le `transaction_id` est obligatoire pour distinguer et corréler les commandes.

**Classification** : DELEGATED à FT-CMD.

### BLK06-B5-004 — Idempotence
Une commande avec un `transaction_id` déjà traité ne doit jamais être exécutée une seconde fois et doit réutiliser le résultat précédent conformément à la V1.

**Classification** : DELEGATED à FT-CMD.

### BLK06-B5-005 — Une seule commande active
Une seule commande active est admise à la fois.

**Classification** : DELEGATED à FT-CMD.

### BLK06-B5-006 — États et résultats
Les statuts `cmd_status`, résultats et détails associés décrivent le traitement d'une commande conformément aux codes V1.

**Classification** : DELEGATED à FT-CMD ; les domaines purs restent FT-LIM.

### BLK06-B5-007 — Historique minimal
Le Bloc 5 mémorise l'historique minimal de la dernière commande terminée.

**Classification** : DELEGATED à FT-CMD pour la mise à jour fonctionnelle ; cohérence structurelle FT-STR.

### BLK06-B5-008 — Commandes protégées
Les commandes protégées requièrent la confirmation prévue par la V1.

**Classification** : DELEGATED à FT-CMD.

### BLK06-B5-009 — Annulation et nettoyage
Les demandes d'annulation et de nettoyage sont gouvernées par les règles fonctionnelles du moteur de commandes.

**Classification** : DELEGATED à FT-CMD.

### BLK06-B5-010 — Effets inter-blocs
Les commandes d'application de configuration, synchronisation d'heure, acquisition, autotest, acquittement et autres actions observables hors B5 créent des relations inter-blocs.

**Classification** : DELEGATED à FT-CMD et FT-INT selon la nature de l'oracle.

## 4. Anti-fabrication

FT-BLK-06 n'utilise pas :
- les valeurs recommandées des compléments métier de B0 comme codes protocole ;
- une preuve d'unicité de `device_id` à partir d'un seul équipement ;
- une simple stabilité en fonctionnement comme preuve de persistance après reboot ;
- des tests B5 locaux qui dupliqueraient FT-CMD.
