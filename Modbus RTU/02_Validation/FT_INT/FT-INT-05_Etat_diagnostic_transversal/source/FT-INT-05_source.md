# FT-INT-05 — Référentiel source

## 1. Objet

Ce document inventorie les relations V1 relevant de FT-INT-05 et leur statut de validation.

## 2. Règles

### INT05-R01 — Publication de l'autotest dans B7

**Statut : COVERED**  
**Test : `TT-INT-B05B07-001`, `TT-INT-B05B07-002`**

La commande B5 SELFTEST déclenche l'autotest ; B7 constitue l'interface normative de publication de son état et de son résultat via `selftest_status`, `selftest_result_code` et `selftest_detail`.

FT-INT vérifie l'effet inter-blocs après une exécution B5 reconnue comme réussie par le moteur de commandes. La mécanique transactionnelle reste FT-CMD.

### INT05-R02 — Résultat d'autotest observable dans B7

**Statut : COVERED, exécution conditionnelle au moyen d'injection**  
**Tests : `TT-INT-B05B07-001`, `TT-INT-B05B07-002`**

Lorsque le moyen d'essai permet de provoquer déterministement une issue OK ou échec, B7 doit publier un état final compatible avec cette issue.

FT-INT-05 ne crée aucune table supplémentaire entre `selftest_result_code`, `selftest_detail` et des causes internes non normées.

### INT05-R03 — Acquittement sans disparition de la cause

**Statut : CONDITIONAL**  
**Test : `TT-INT-B01B05B07-001`**

L'acquittement d'un défaut par B5 ne doit pas supprimer la cause tant que celle-ci reste réellement présente. Le test exige donc une injection de défaut contrôlée et maintenue.

Le mécanisme de soumission, les préconditions et les codes de résultat ACK restent FT-CMD.

### INT05-R04 — Base temporelle de `last_fault_timestamp`

**Statut : COVERED**  
**Test : `TT-INT-B02B07-001`**

`B7.last_fault_timestamp` utilise la même base temporelle que le Bloc 2.

L'oracle porte sur la base temporelle et la cohérence de fenêtre, pas sur une égalité bit-à-bit avec une lecture B2 effectuée à un autre instant.

### INT05-R05 — B1 fault flags ↔ B7 system fault flags

**Statut : NOT_DEFINED**

La V1 ne fournit pas de table normative exhaustive établissant une égalité ou une bijection entre les bitfields défaut de B1 et B7. Aucune comparaison exhaustive PASS/FAIL n'est créée.

### INT05-R06 — B1 system status ↔ B7 system health status

**Statut : NOT_DEFINED**

La V1 ne définit pas de fonction normative exhaustive de conversion entre l'état global B1 et `B7.system_health_status`. Toute table logique déduite serait une invention.

### INT05-R07 — Uptime dupliqué B1/B7

**Statut : TRACE_ONLY**  
**Contrôle : `TT-INT-B01B07-001`**

B1 et B7 exposent un uptime. Leur proximité peut être contrôlée et tracée, mais FT-INT-05 n'impose pas une égalité stricte entre deux lectures Modbus séparées ni une tolérance non spécifiée.

La monotonie propre à chaque champ reste du ressort de FT-BLK.

### INT05-R08 — Cause de reset dupliquée B1/B7

**Statut : TRACE_ONLY**  
**Contrôle : `TT-INT-B01B07-001`**

Les deux blocs exposent une cause de reset. La comparaison est enregistrée comme observation croisée ; en l'absence d'une règle inter-blocs explicite suffisante, elle n'est pas promue en oracle PASS/FAIL FT-INT.

Les comportements de persistance et de redémarrage appartiennent à FT-PER.

### INT05-R09 — Température interne dupliquée B1/B7

**Statut : TRACE_ONLY**  
**Contrôle : `TT-INT-B01B07-001`**

B1 et B7 exposent une température interne. FT-INT peut enregistrer les deux valeurs lors d'une même campagne de lecture, sans inventer une égalité stricte ni une tolérance.

### INT05-R10 — État stockage B6 ↔ diagnostic B7

**Statut : NOT_DEFINED**

Aucune correspondance normative exhaustive n'est définie entre les états/indicateurs de stockage B6 et `B7.system_health_status` ou `B7.system_fault_flags`.

## 3. Anti-fabrication

FT-INT-05 interdit explicitement :

- l'utilisation des compléments métier informatifs B7 comme spécification protocolaire ;
- une table inventée B1↔B7 ;
- une table inventée B6↔B7 ;
- une tolérance temporelle non spécifiée ;
- la duplication des essais du moteur B5 ;
- l'assimilation d'un ACK à une suppression de la cause physique ou logique du défaut.

## 4. Critère d'exploitation des commandes B5

Pour les tests SELFTEST et ACK, le verdict FT-INT n'est établi que si l'exécution B5 nécessaire au stimulus est reconnue comme exploitable. Un refus ou un échec transactionnel relève d'abord de FT-CMD et ne constitue pas à lui seul un échec de la relation inter-blocs.