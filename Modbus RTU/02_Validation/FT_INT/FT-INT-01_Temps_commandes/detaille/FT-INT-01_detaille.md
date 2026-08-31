# FT-INT-01 — Cas de test détaillés

## Préambule

Les cas ci-dessous supposent que le client sait préparer les registres B2 et exécuter correctement une commande B5. La conformité du protocole transactionnel B5 n'est pas évaluée ici.

Les acquisitions temporelles doivent conserver les valeurs brutes et les instants de lecture côté banc afin de distinguer une progression légitime de l'horloge d'une incohérence fonctionnelle.

---

## TT-INT-B02B05-001 — Application effective du temps préparé

### Objectif

Vérifier qu'une synchronisation B5 réussie applique effectivement le temps préparé dans B2.

### Préconditions

- communication Modbus opérationnelle ;
- B2 et B5 lisibles conformément à leurs droits d'accès ;
- possibilité d'exécuter une synchronisation B5 réussie ;
- valeur temporelle préparée choisie de façon à être distinguable de l'horloge courante.

### Procédure

1. Lire et enregistrer `current_time` B2 avant préparation.
2. Préparer dans B2 une valeur temporelle valide et distincte de l'horloge courante.
3. Vérifier, si nécessaire pour la traçabilité du banc, que la préparation seule n'a pas réglé l'horloge ; ce point n'entre pas dans le verdict FT-INT-01 car il appartient à FT-BLK-02.
4. Exécuter la commande B5 de synchronisation selon le protocole défini pour B5.
5. Ne poursuivre le verdict FT-INT que si la commande est déclarée réussie ; sinon classer l'exécution du cas comme non exploitable pour FT-INT et renvoyer l'analyse du refus vers FT-CMD.
6. Lire immédiatement `current_time` B2 après succès et enregistrer l'instant de lecture côté banc.
7. Comparer la valeur observée à la valeur préparée en tenant compte uniquement de la progression temporelle légitime entre application et lecture.

### Oracle

`PASS` si l'horloge B2 observée après succès est cohérente avec l'application du temps préparé et sa progression naturelle.

`FAIL` si la commande est réussie mais que B2 reste manifestement sur l'ancienne référence temporelle ou adopte une référence incompatible avec le temps préparé.

Aucune tolérance chiffrée arbitraire ni égalité stricte entre transactions séparées n'est imposée.

---

## TT-INT-B02B05-002 — Mise à jour de last_sync_time

### Objectif

Vérifier qu'une synchronisation effective met à jour `last_sync_time` B2.

### Préconditions

Identiques à `TT-INT-B02B05-001`.

### Procédure

1. Lire et enregistrer `last_sync_time` avant la nouvelle synchronisation.
2. Préparer une nouvelle référence temporelle valide et distinguable.
3. Exécuter la commande B5 de synchronisation.
4. Ne poursuivre le verdict FT-INT que si la commande est réussie.
5. Lire `last_sync_time` et `current_time` B2 après succès.
6. Vérifier que `last_sync_time` a été mis à jour et qu'il est cohérent avec la nouvelle synchronisation matérialisée.

### Oracle

`PASS` si `last_sync_time` reflète la nouvelle synchronisation effective.

`FAIL` si la commande B5 est réussie mais que `last_sync_time` conserve sa valeur antérieure ou représente manifestement une autre référence temporelle.

La stabilité de `last_sync_time` en absence de synchronisation n'est pas retestée ici.

---

## TT-INT-B02B05-003 — Cohérence minimale de l'état temporel après synchronisation

### Objectif

Vérifier qu'après une synchronisation réussie, l'état temporel exposé par B2 n'est pas contradictoire avec l'événement effectivement réalisé.

### Préconditions

Identiques à `TT-INT-B02B05-001`.

### Procédure

1. Enregistrer les observables d'état temporel B2 pertinents avant synchronisation.
2. Préparer une référence temporelle valide.
3. Exécuter avec succès la commande B5 de synchronisation.
4. Lire immédiatement après succès les observables B2 décrivant l'état de synchronisation ainsi que `last_sync_time`.
5. Vérifier la cohérence minimale entre la synchronisation matérialisée et l'état exposé.

### Oracle

`PASS` si B2 matérialise la synchronisation et ne présente pas simultanément un état explicitement incompatible avec une synchronisation réussie selon les significations normatives définies par B2.

`FAIL` uniquement lorsqu'une contradiction avec une signification explicitement normative de B2 peut être démontrée.

`INCONCLUSIVE` si le verdict nécessiterait d'inventer une combinaison exacte de `time_status`, `time_flags` ou `prepared_time_status` que la V1 ne définit pas.

### Interdictions d'oracle

Le test ne doit pas imposer de son propre chef un triplet numérique exact de statuts/flags après synchronisation. Il ne doit pas non plus assimiler automatiquement un flag B5 à un flag B2 en l'absence d'une équivalence normative explicite.

---

## Relations tracées sans cas actif

### INT01-R04 — cmd_last_timestamp

Tracer que B5 utilise l'Epoch TR2 de B2. Aucun PASS/FAIL numérique n'est construit sans tolérance normative.

### INT01-R05 — indicateurs de temps préparé B2/B5

Conserver `NOT_DEFINED` tant qu'aucune équivalence normative explicite n'est introduite dans une version future de la spécification.

### INT01-R06 — refus sans temps préparé

Déléguer à FT-CMD, qui possède l'oracle du moteur transactionnel et des codes résultat.
