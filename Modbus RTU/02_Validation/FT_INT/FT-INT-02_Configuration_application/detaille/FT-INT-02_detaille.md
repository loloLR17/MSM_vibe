# FT-INT-02 — Cas de test détaillés

## Préambule

Les cas ci-dessous supposent que le client sait préparer une configuration B4 valide et exécuter correctement la commande B5 `appliquer configuration préparée`. La conformité du moteur transactionnel B5 n'est pas évaluée ici.

Le banc doit conserver un instantané de la configuration active avant application et de la configuration préparée destinée à être appliquée afin de distinguer sans ambiguïté l'avant et l'après.

---

## TT-INT-B04B05-001 — Application effective de la configuration préparée

### Objectif

Vérifier qu'une commande B5 d'application réussie rend effectivement active la configuration préparée dans B4.

### Préconditions

- communication Modbus opérationnelle ;
- configuration préparée complète et valide ;
- `config_state = VALIDE` avant application ;
- configuration préparée distinguable de l'image active existante ;
- conditions d'application compatibles selon B5.

### Procédure

1. Lire et enregistrer l'image active B4 avant application.
2. Lire et enregistrer la configuration préparée destinée à être appliquée.
3. Exécuter la commande B5 `appliquer configuration préparée` selon le protocole B5.
4. Ne poursuivre le verdict FT-INT que si la commande est déclarée réussie ; sinon classer l'exécution comme non exploitable pour FT-INT-02 et renvoyer l'analyse du refus ou de l'échec vers FT-CMD.
5. Relire B4 après succès.
6. Vérifier que les observables actifs B4 reflètent désormais la configuration qui vient d'être appliquée.

### Oracle

`PASS` si la configuration préparée devient effectivement la configuration active après succès B5.

`FAIL` si B5 déclare l'application réussie mais que B4 conserve manifestement l'ancienne configuration active ou expose une configuration active incompatible avec celle appliquée.

---

## TT-INT-B04B05-002 — Mise à jour de l'identité active

### Objectif

Vérifier que `active_config_id` correspond à l'identité de la configuration préparée effectivement appliquée.

### Préconditions

Identiques à `TT-INT-B04B05-001`, avec `prepared_config_id != 0` et choisi de manière distinguable de l'identité active précédente.

### Procédure

1. Lire `prepared_config_id` avant application.
2. Lire `active_config_id` avant application.
3. Exécuter avec succès la commande B5 d'application.
4. Relire `active_config_id` après succès.
5. Comparer l'identité active post-application à l'identité préparée appliquée.

### Oracle

`PASS` si `active_config_id` post-application correspond au `prepared_config_id` de la configuration effectivement appliquée.

`FAIL` si la commande B5 est réussie mais que l'identité active conserve l'ancienne valeur ou correspond à une autre configuration.

---

## TT-INT-B04B05-003 — Transition d'état vers ACTIF

### Objectif

Vérifier la transition normative `VALIDE` + application réussie → `ACTIF`.

### Préconditions

- configuration préparée valide ;
- `config_state = VALIDE` avant application ;
- commande B5 applicable dans le contexte courant.

### Procédure

1. Lire et confirmer `config_state = VALIDE` avant commande.
2. Exécuter la commande B5 d'application.
3. Ne poursuivre le verdict FT-INT que si la commande termine avec succès.
4. Relire `config_state` après succès.

### Oracle

`PASS` si `config_state = ACTIF` après application réussie.

`FAIL` si B5 déclare l'application réussie mais que `config_state` n'est pas `ACTIF`.

---

## TT-INT-B04B05-004 — Cohérence de l'image active 4E après application

### Objectif

Vérifier que l'image active 4E représente la configuration effectivement appliquée et que son CRC actif reste cohérent avec cette image sans dupliquer l'oracle CRC propriétaire de FT-BLK-04.

### Préconditions

Identiques à `TT-INT-B04B05-001`.

### Procédure

1. Capturer les registres préparés B4 participant à la configuration destinée à être appliquée.
2. Exécuter avec succès la commande B5 d'application.
3. Lire l'image active 4E selon les règles de cohérence/snapshot déjà validées par FT-STR/FT-BLK.
4. Comparer les champs métier actifs avec les valeurs préparées appliquées pour lesquels la V1 définit une correspondance préparé → actif.
5. Lire `active_config_crc`.
6. Utiliser l'oracle CRC défini par FT-BLK-04 pour contrôler la cohérence de `active_config_crc` avec l'image active, sans redéfinir ici l'algorithme ni ses vecteurs.

### Oracle

`PASS` si l'image active 4E correspond à la configuration appliquée et si le contrôle CRC propriétaire FT-BLK-04 confirme la cohérence de l'image active.

`FAIL` si la commande B5 est réussie mais que l'image active diffère de la configuration appliquée sur un champ normativement correspondant, ou si l'image active et `active_config_crc` sont incohérents selon l'oracle FT-BLK-04.

### Limites

- aucune nouvelle règle CRC n'est créée par FT-INT-02 ;
- aucune exigence sur `config_revision_counter` n'est ajoutée ;
- les effets des seuils actifs sur B3 sont hors de ce test et relèvent de FT-INT-03.

---

## Relations tracées sans cas actif

### INT02-R06 — Configuration non VALIDE

Le refus d'une commande d'application lorsque `config_state != VALIDE` appartient à FT-CMD, car le verdict porte sur l'acceptation/refus et le résultat B5.

### INT02-R07 — CRC préparé incorrect

Le calcul et la validité du CRC relèvent de FT-BLK-04 ; le refus de commande correspondant relève de FT-CMD.

### INT02-R08 — Configuration préparée incomplète

Le refus et son code résultat sont délégués à FT-CMD.

### INT02-R09 — Échec réel d'application

La transition `VALIDE` + application échouée → `ERREUR_APPLICATION` reste `CONDITIONAL`. Elle ne doit être activée qu'avec un moyen reproductible de provoquer un véritable échec interne pendant l'application, distinct d'un refus préalable.
