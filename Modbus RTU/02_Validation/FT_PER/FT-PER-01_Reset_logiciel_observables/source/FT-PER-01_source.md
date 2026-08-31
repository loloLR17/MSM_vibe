# FT-PER-01 — Source normative consolidée

## 1. Références normatives

Sources principales :
- `Modbus RTU/01_Specification_source/bloc1.md` V1 ;
- `Modbus RTU/01_Specification_source/bloc5.md` V1 ;
- `Modbus RTU/01_Specification_source/bloc7.md` V1 ;
- `Modbus RTU/02_Validation/FT_CMD/FT-CMD-07_Maintenance_commandes_protegees/` gelée ;
- `Modbus RTU/02_Validation/FT_INT/FT-INT-05_Etat_diagnostic_transversal/` gelée ;
- plan maître de validation Modbus TR2.

Les compléments métier informatifs ne sont jamais utilisés comme oracle.

## 2. Exigences retenues

### PER01-R01 — RESET SOFTWARE provoque un redémarrage logiciel contrôlé

**Classification : `CONDITIONAL`.**

La commande 10, lorsqu'elle est acceptée selon les règles FT-CMD, a comme effet normatif un `redémarrage logiciel contrôlé`.

FT-PER-01 ne reteste pas la clé `0xA55A`, l'acquisition arrêtée, les opérations critiques ni les codes de refus : ces éléments restent propriétaires FT-CMD-07.

Condition d'exécution : le moyen d'essai doit permettre de soumettre la commande puis de reprendre les lectures après le redémarrage.

Test : `TT-PER-B01B05B07-001`.

### PER01-R02 — Cause post-reboot dans B1

**Classification : `COVERED`, exécution `CONDITIONAL`.**

`B1.last_reset_cause` définit explicitement la valeur `2 = Reset logiciel`.

Après un RESET SOFTWARE effectivement accepté et exécuté, la première observation post-reboot exploitable de la cause de dernier redémarrage doit donc identifier un reset logiciel.

Test : `TT-PER-B01B05B07-001`.

### PER01-R03 — Cause post-reboot dans B7

**Classification : `COVERED`, exécution `CONDITIONAL`.**

`B7.reset_cause` définit explicitement la valeur `2 = reset logiciel`.

Après un RESET SOFTWARE effectivement accepté et exécuté, la première observation post-reboot exploitable de la cause de dernier reset doit donc identifier un reset logiciel.

Test : `TT-PER-B01B05B07-001`.

### PER01-R04 — Cohérence B1 ↔ B7 de la cause de reset

**Classification : `DELEGATED`.**

La présence du même concept dans B1 et B7 est déjà traitée comme relation inter-blocs dans FT-INT-05. FT-PER-01 peut relever les deux champs pour établir ses oracles propres à chaque bloc, mais ne crée pas une nouvelle règle générale d'égalité inter-blocs.

### PER01-R05 — Uptime après reset

**Classification : `TRACE_ONLY`.**

B7 définit `uptime_s` comme le temps de fonctionnement depuis le dernier reset et indique qu'il ne doit jamais revenir en arrière sauf reset.

Cette règle confirme que le reset est une frontière légitime de rupture de monotonie, mais la V1 ne définit aucune valeur exacte ou borne temporelle à la première lecture post-reboot.

Le test relève donc l'uptime avant et après reboot sans imposer `uptime=0`, une décroissance obligatoire entre deux lectures arbitraires, ni une tolérance inventée.

Contrôle : `TT-PER-B01B05B07-001`.

### PER01-R06 — Délai de redémarrage

**Classification : `NOT_DEFINED`.**

Aucun délai maximal ou minimal de redémarrage logiciel n'est défini en V1.

Une mesure de durée peut être enregistrée pour caractérisation, mais ne peut pas produire de verdict de conformité temporelle.

### PER01-R07 — Délai avant reprise Modbus

**Classification : `NOT_DEFINED`.**

La V1 ne fixe aucun délai maximal avant première réponse Modbus valide après reboot.

### PER01-R08 — État B5 post-reboot

**Classification : `NOT_DEFINED` / propriété détaillée reportée à FT-PER-04.**

La V1 ne précise pas l'état post-reboot de `cmd_status`, `cmd_active_*`, `cmd_last_*`, des champs de requête ou de la mémoire d'idempotence.

### PER01-R09 — Accusé de prise en compte avant reboot

**Classification : `DELEGATED`.**

L'effet `accusé de prise en compte` fait partie de la sémantique de la commande RESET SOFTWARE et de son moteur transactionnel. La validation de cet accusé reste à FT-CMD. FT-PER ne doit pas imposer une forme supplémentaire ni un délai avant disparition de la communication.

## 3. Frontières de propriété

- FT-CMD-07 : acceptation/refus et mécanique transactionnelle de RESET SOFTWARE ;
- FT-INT-05 : relations générales B1↔B7 ;
- FT-PER-01 : observables normatifs après franchissement du reboot ;
- FT-PER-04 : persistance et état du moteur B5 après reboot ;
- FT-PER-06 : power cycle et reprise Modbus générale.

## 4. Règles anti-fabrication

FT-PER-01 interdit explicitement :
- d'imposer un délai de reboot ou de reprise ;
- d'exiger `uptime_s = 0` à la première lecture ;
- de déduire une tolérance B1/B7 absente de la V1 ;
- de considérer la perte temporaire de réponse comme suffisante, seule, pour prouver le type de reset ;
- d'inventer un état initial du moteur B5 ;
- de transformer une mesure de performance en oracle de conformité.