# FT-RBT — Matrice de couverture consolidée V1

## 1. Synthèse globale

| Classification | Nombre |
|---|---:|
| COVERED | 1 |
| CONDITIONAL | 3 |
| DELEGATED | 12 |
| TRACE_ONLY | 1 |
| NOT_DEFINED | 19 |
| **Total** | **36** |

## 2. Tests propriétaires FT-RBT

| Test | Sous-famille | Classification | Objet |
|---|---|---|---|
| TT-RBT-GEN-001 | FT-RBT-01 | COVERED | Requête invalide intercalée sans corruption observable du fonctionnement nominal |
| TT-RBT-B05-001 | FT-RBT-02 | CONDITIONAL | Perte de réponse puis retransmission du même `transaction_id` |
| TT-RBT-B05-002 | FT-RBT-02 | CONDITIONAL | Preuve de non-double-exécution sous perte et retransmission |
| TT-RBT-GEN-020 | FT-RBT-04 | CONDITIONAL | Lectures multi-registres pendant une transition contrôlée |

## 3. Répartition par sous-famille

| Sous-famille | Total | COVERED | CONDITIONAL | DELEGATED | TRACE_ONLY | NOT_DEFINED |
|---|---:|---:|---:|---:|---:|---:|
| FT-RBT-01 | 6 | 1 | 0 | 2 | 0 | 3 |
| FT-RBT-02 | 6 | 0 | 2 | 3 | 0 | 1 |
| FT-RBT-03 | 7 | 0 | 0 | 4 | 0 | 3 |
| FT-RBT-04 | 7 | 0 | 1 | 1 | 1 | 4 |
| FT-RBT-05 | 10 | 0 | 0 | 2 | 0 | 8 |
| **Total** | **36** | **1** | **3** | **12** | **1** | **19** |

## 4. Dette normative principale

Les principaux points explicitement non définis par la V1 sont :
- délai maximal de réponse Modbus ;
- stratégie de retry et backoff ;
- fréquence/cadence maximale des requêtes et débit minimal ;
- profondeur de file ou comportement sous accumulation ;
- nombre de répétitions supportées et durée/profondeur de mémoire d'idempotence ;
- traitement d'une trame RTU à CRC invalide ;
- traitement d'une trame tronquée ou mal formée ;
- politique et délai de resynchronisation après erreur de framing ;
- seuils de rafales d'erreurs et comportement de récupération ;
- priorité entre erreurs transport simultanées ;
- comportement du même `transaction_id` avec un payload différent.

## 5. Délégations structurantes

- FT-ACC : qualification des accès invalides, exception et absence d'effet de bord élémentaire ;
- FT-CMD-02 : idempotence, corrélation, mémoire transactionnelle et dette même-ID/payload différent ;
- FT-CMD-04 : concurrence avec commande active ;
- FT-STR-07 : cohérence interne d'une réponse multi-registres ;
- FT-PER : tout comportement post-reboot ;
- familles fonctionnelles concernées : CRC applicatifs/configuration.

## 6. Conclusion

La couverture FT-RBT V1 est complète au sens documentaire : chaque thème du périmètre est soit couvert par un oracle discriminant, soit conditionné par un moyen d'observation explicite, soit délégué à son propriétaire normatif, soit conservé comme dette `NOT_DEFINED`/`TRACE_ONLY`.
