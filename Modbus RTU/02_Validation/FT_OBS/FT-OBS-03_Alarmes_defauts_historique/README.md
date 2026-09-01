# FT-OBS-03 — Alarmes, défauts et historique observable

## 1. Objet

FT-OBS-03 vérifie ce qu'une centrale peut réellement distinguer en V1 entre :
- défaut ou alarme **actif** ;
- avertissement courant ;
- information **mémorisée / latched** ;
- **dernier défaut détecté** ;
- absence de défaut connu.

La sous-famille interdit toute généralisation d'un mécanisme local à l'ensemble du protocole.

## 2. Constat structurant

La V1 ne définit pas un modèle transversal unique d'alarme/défaut.

Elle définit au contraire plusieurs mécanismes locaux :

- **B1** : présence de `fault_flags`, `warning_flags`, `error_code`, `warning_code`, mais sans tables normatives détaillées des bits/codes ;
- **B3** : `B3_ALARM_FLAGS` avec bits d'alarmes actives et `ALARM_LATCHED_PRESENT`, plus `B3_ALARM_LATCHED` ;
- **B7** : `system_fault_flags` détaillé, `last_fault_code`, `last_fault_timestamp` ; `last_fault_code = 0` signifie explicitement « aucun défaut connu ».

Une centrale peut donc distinguer plusieurs situations, mais **ne peut pas en déduire un modèle universel d'acquittement ou de mémorisation**.

## 3. Frontières de propriété

FT-OBS-03 possède uniquement la discriminabilité externe.

Sont délégués :
- domaines unitaires, bits réservés, encodage : FT-STR / FT-LIM ;
- apparition/disparition fonctionnelle des défauts : FT-BLK / FT-INT ;
- cohérences entre B1, B3 et B7 : FT-INT ;
- effet d'une éventuelle commande d'effacement/statistiques : FT-CMD / FT-INT ;
- survie après reboot d'une alarme mémorisée ou du dernier défaut : FT-PER.

## 4. Points normativement exploitables

- B3 permet de distinguer une **alarme active** d'une **alarme mémorisée** ;
- B3 expose une sévérité globale courante ;
- B7 permet de connaître les catégories de **défauts système actifs** via `system_fault_flags` ;
- B7 distingue le défaut courant du **dernier défaut détecté** ;
- `last_fault_code = 0` a une sémantique locale explicite : aucun défaut connu.

## 5. Limites V1

Ne sont pas définis de manière normative :
- la signification bit par bit de `B1.fault_flags` ;
- la signification bit par bit de `B1.warning_flags` ;
- les tables détaillées `B1.error_code` et `B1.warning_code` ;
- un modèle global actif / latched / acknowledged ;
- une commande ou un état universel d'acquittement ;
- la valeur obligatoire de `last_fault_timestamp` lorsque `last_fault_code = 0` ;
- le caractère encore actif ou non du défaut désigné par `last_fault_code`.

## 6. Tests actifs

- `TT-OBS-B03-003` — distinction alarme active / alarme mémorisée ;
- `TT-OBS-B07-001` — défauts actifs B7 et dernier défaut connu ;
- `TT-OBS-B01B07-001` — preuve de non-généralisation B1/B7 : les détails B1 incomplets ne doivent pas être reconstruits depuis B7.

## 7. Artefacts

- `source/FT-OBS-03_source.md` ;
- `detaille/FT-OBS-03_detaille.md` ;
- `detaille/FT-OBS-03_matrice_couverture.csv`.

## 8. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.