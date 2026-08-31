# FT-CMD — Audit final V1

## 1. Objet

Ce document clôt l'audit de la famille FT-CMD du référentiel Modbus RTU TR2 V1.

Périmètre : moteur transactionnel du Bloc 5, règles de soumission, corrélation, idempotence, concurrence, états/résultats/historique et préconditions transactionnelles des commandes 1 à 11.

## 2. Résultat global

La famille FT-CMD V1 est cohérente avec les sources normatives actuelles et avec les frontières de propriété établies avec les familles déjà gelées.

Aucune contradiction bloquante n'a été identifiée lors de la passe croisée finale.

La couverture consolidée comprend 66 exigences / points de couverture et 53 tests identifiés :
- 40 `COVERED` ;
- 9 `CONDITIONAL` ;
- 10 `NOT_DEFINED` ;
- 4 `TRACE_ONLY` ;
- 3 `DELEGATED`.

## 3. Contrôles croisés réalisés

### 3.1 FT-CMD / FT-STR

FT-CMD n'empiète pas sur :
- encodage MSW/LSW ;
- atomicité des champs multi-registres ;
- structure du Bloc 5 ;
- structure de l'historique.

### 3.2 FT-CMD / FT-ACC

Les règles d'accès RO/RW et exceptions Modbus restent à FT-ACC. Les bits réservés placés dans un registre RW sont traités par FT-CMD uniquement lorsqu'ils deviennent une valeur fonctionnelle invalide soumise au moteur.

### 3.3 FT-CMD / FT-LIM

Les domaines simples restent à FT-LIM. FT-CMD utilise seulement les valeurs nécessaires à la décision transactionnelle et aux codes résultat explicitement associés.

### 3.4 FT-CMD / FT-BLK

Les invariants strictement intra-bloc restent à FT-BLK. Le moteur transactionnel B5, l'idempotence, la concurrence, l'historique fonctionnel et les contrôles de commande sont bien propriétaires FT-CMD.

### 3.5 FT-CMD / FT-INT

La frontière est respectée :
- FT-CMD valide l'acceptation, le refus, l'échec et le résultat B5 ;
- FT-INT valide les effets réussis ou non-effets observables dans les autres blocs.

Cela concerne notamment : application de configuration, synchronisation temporelle, démarrage/arrêt d'acquisition, publication SELFTEST, effets ACK, REFRESH, mode maintenance et non-effets RESET STATISTICS.

### 3.6 FT-CMD / FT-PER et FT-RBT

Le comportement après reboot reste à FT-PER. Les scénarios agressifs de perte/répétition/timing dépassant l'oracle transactionnel direct restent à FT-RBT.

## 4. Points normatifs confirmés

La passe finale confirme notamment :
- déclenchement sur front montant de `submit` ;
- remise automatique de `submit` à 0 après prise en compte ;
- `transaction_id` obligatoire ;
- rejeu d'un ID déjà traité sans seconde exécution et avec réutilisation du résultat ;
- corrélation stricte par `cmd_active_transaction_id` en contexte nominal ;
- une seule commande active ;
- refus concurrent code 13 ;
- états et résultats B5 définis ;
- historique de la dernière commande terminée quel que soit le résultat ;
- protection limitée aux commandes 10 et 11 avec clé `0xA55A` ;
- préconditions/refus propres aux commandes APPLY CONFIG, SYNC TIME, START, STOP, SELFTEST, ACK, RESET SOFTWARE ;
- absence de transformation des politiques recommandées ou compléments métier informatifs en exigences.

## 5. Dettes normatives non bloquantes

Les zones suivantes restent volontairement non figées dans la V1 :
- domaine numérique exact des `transaction_id` invalides ;
- profondeur et durée de mémorisation d'idempotence ;
- rejeu du même ID avec un payload différent ;
- représentation exacte de `cmd_active_*` lors d'un refus concurrent ;
- liste des commandes annulables et cycle de succès d'annulation ;
- effet précis de `clear_request_fields` ;
- table exhaustive de `cmd_result_detail` ;
- priorité entre causes simultanées de refus START ;
- détails des extensions SELFTEST et REFRESH ;
- opérations critiques bloquant RESET SOFTWARE et code associé ;
- portée exacte de RESET STATISTICS et de son futur masque.

Ces points ne compromettent pas la cohérence de la V1 tant qu'aucun test ni implémentation ne leur attribue une sémantique non spécifiée.

## 6. Critère de gel

Les critères définis dans `Specifications.md` sont satisfaits :
- toute exigence `COVERED` possède un test ;
- les tests conditionnels indiquent leur condition d'exécutabilité ;
- toutes les dettes normatives restent visibles ;
- les délégations inter-familles sont explicites ;
- aucun complément métier informatif n'est utilisé comme oracle ;
- aucune exigence nouvelle n'a été inventée pour fermer artificiellement une ambiguïté.

## 7. Décision

**FT-CMD V1 est déclarée finalisée et gelable.**

Les évolutions souhaitables sont séparées dans `EVOLUTIONS_CANDIDATES_V1_1.md`. Elles constituent un backlog de spécification et ne modifient pas le référentiel V1 gelé.
