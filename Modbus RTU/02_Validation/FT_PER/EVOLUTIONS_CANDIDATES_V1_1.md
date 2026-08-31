# FT-PER — Évolutions candidates pour une spécification V1.1

## Statut

Ce document recense les dettes normatives et arbitrages souhaitables identifiés lors du cadrage de FT-PER V1.

Il est **informatif** et ne modifie en rien la spécification V1 gelée.

Aucun point listé ici ne doit être utilisé comme oracle V1 tant qu'il n'a pas été intégré explicitement dans une future révision normative.

---

## 1. Persistance de la configuration

À arbitrer explicitement pour :

- la configuration active après reset logiciel ;
- la configuration active après power cycle ;
- la configuration préparée mais non appliquée ;
- l'état `config_state` après reboot ;
- la conservation de `prepared_config_id` / `prepared_config_crc` ;
- la conservation de `active_config_id` / `active_config_crc` ;
- les éventuelles valeurs par défaut lorsqu'aucune configuration persistante n'est disponible.

La V1 actuelle ne permet pas d'imposer une politique Flash/RAM implicite.

---

## 2. Persistance du moteur transactionnel B5

À définir explicitement :

- survie ou non de la mémoire d'idempotence au reboot ;
- profondeur de l'historique des `transaction_id` ;
- durée de conservation de cet historique ;
- comportement du rejeu d'un `transaction_id` traité avant reboot ;
- comportement du même ID avec payload différent ;
- état post-reboot de `cmd_active_*` ;
- état post-reboot de `cmd_last_*` ;
- état des champs de requête B5 après reboot ;
- politique applicable à une commande interrompue par watchdog, brown-out, reset externe ou power cycle.

---

## 3. RESET SOFTWARE

À préciser :

- liste normative des « opérations critiques non terminées » bloquant la commande ;
- code résultat associé à chaque motif de refus si nécessaire ;
- état transactionnel observable juste avant reboot ;
- persistance ou non du résultat de la commande RESET elle-même ;
- comportement attendu du moteur B5 après retour en service.

---

## 4. RESET STATISTICS

La V1 définit la non-destruction des campagnes, de l'identité, de la configuration et des journaux critiques, mais pas le périmètre positif exact de la RAZ.

À définir :

- liste exhaustive des statistiques remises à zéro ;
- distinction statistiques volatiles / persistantes ;
- comportement du futur masque `param1` ;
- éventuels compteurs explicitement exclus ;
- relation avec des compteurs de reboot ou d'erreurs internes futurs.

---

## 5. Acquisition et campagnes après reboot

À arbitrer :

- état d'acquisition après reset logiciel ;
- état d'acquisition après power cycle ;
- reprise automatique interdite, autorisée ou conditionnelle ;
- devenir d'une campagne interrompue ;
- cohérence attendue de `campaign_state` après redémarrage ;
- persistance normative de l'inventaire B6 ;
- comportement de `selected_campaign_index` après reboot.

---

## 6. Diagnostic et alarmes

À préciser :

- persistance des défauts mémorisés au-delà de la persistance de leur cause ;
- politique de redétection après boot ;
- persistance de `last_fault_code` / `last_fault_timestamp` ;
- persistance du résultat d'autotest ;
- existence éventuelle d'un compteur de reboot normatif ;
- comportement des alarmes acquittées/non acquittées après reboot.

---

## 7. Temps et reprise

À définir si nécessaire :

- persistance ou non de l'heure RTC lors d'un reset logiciel ;
- persistance ou non lors d'un power cycle ;
- devenir d'une synchronisation préparée mais non appliquée ;
- état de synchronisation temporelle après reboot.

---

## 8. Reprise Modbus

À arbitrer seulement si une exigence de performance est réellement nécessaire :

- délai maximal de reboot ;
- délai maximal avant première réponse Modbus valide ;
- définition d'un état « boot terminé » observable ;
- comportement des requêtes reçues pendant la phase de démarrage.

En l'absence d'exigence normative, ces mesures doivent rester de la traçabilité et non des critères PASS/FAIL.

---

## 9. Power cycle versus reset logiciel

La V1 distingue déjà les causes de reset dans B1/B7, mais ne définit pas de politiques de persistance distinctes.

Une V1.1 pourrait préciser, propriété par propriété, si les comportements sont :

- identiques ;
- différents ;
- dépendants de la cause de reset.

Aucune équivalence implicite ne doit être introduite.

---

## 10. Initialisation et valeurs par défaut

À définir explicitement si souhaité :

- état initial des zones RW ;
- valeurs par défaut de configuration ;
- états initiaux du moteur de commande ;
- valeurs initiales des diagnostics historiques ;
- distinction premier power-on / reboot courant / retour usine éventuel.

---

## 11. Priorité recommandée pour V1.1

### Priorité haute

1. persistance de la configuration active ;
2. persistance de la configuration préparée ;
3. mémoire d'idempotence et historique B5 après reboot ;
4. état d'acquisition / campagne après reboot ;
5. périmètre exact de RESET STATISTICS.

### Priorité moyenne

6. persistance des diagnostics historiques ;
7. comportement du temps après reboot ;
8. distinction normative power cycle / reset logiciel pour les données persistantes.

### Priorité basse / uniquement si besoin système

9. délais de reboot et de reprise Modbus ;
10. séquence détaillée de boot observable.

---

## 12. Doctrine de révision

Toute évolution V1.1 devra distinguer explicitement :

- **persistant non volatil** ;
- **volatil** ;
- **reconstruit au boot** ;
- **réinitialisé volontairement** ;
- **non défini**.

Cette terminologie évitera d'utiliser le terme « persistant » dans plusieurs sens différents et permettra de construire des oracles FT-PER réellement discriminants.
