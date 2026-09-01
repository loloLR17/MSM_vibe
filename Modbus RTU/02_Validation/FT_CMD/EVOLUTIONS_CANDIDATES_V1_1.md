# FT-CMD — Évolutions candidates V1.1

## 1. Statut de ce document

Ce document est un **backlog d'évolution de spécification** issu de l'audit FT-CMD V1.

Il n'est pas normatif pour la V1 et ne doit pas être utilisé comme oracle de test tant que les choix correspondants ne sont pas arbitrés puis intégrés explicitement dans une future spécification V1.1.

## 2. Priorité haute — cohérence transactionnelle

### V1.1-CMD-001 — Bouclage et réutilisation de `transaction_id`

La V1 définit désormais `transaction_id = 0` comme invalide à la soumission et `1..65535` comme identifiants valides. Restent à définir pour une éventuelle V1.1 :
- règle de bouclage après `65535` ;
- conditions de réutilisation d'un identifiant ancien ;
- interaction avec la profondeur et la durée de mémoire d'idempotence ;
- comportement lorsque l'identifiant réutilisé est encore présent dans l'historique interne.

### V1.1-CMD-002 — Mémoire d'idempotence

À définir :
- profondeur minimale de mémorisation des transactions terminées ;
- durée minimale de conservation ;
- règle d'éviction ;
- comportement après saturation de l'historique interne ;
- interaction éventuelle avec reboot/persistance.

### V1.1-CMD-003 — Même `transaction_id` avec requête différente

Arbitrer explicitement le cas où un ID déjà traité est rejoué avec :
- un autre `cmd_request_code` ;
- des paramètres différents ;
- une clé de confirmation différente.

Préciser si le résultat précédent est toujours réutilisé, si la requête est rejetée comme incohérente, et quel code résultat doit être exposé.

### V1.1-CMD-004 — Corrélation lors d'un refus concurrent

Définir la représentation lorsque A est active et B est refusée avec code `13` :
- valeur de `cmd_active_code` ;
- valeur de `cmd_active_transaction_id` ;
- méthode permettant à la centrale de corréler sans ambiguïté le refus de B tout en conservant l'identité de A.

## 3. Priorité haute — contrôles et annulation

### V1.1-CMD-005 — Commandes annulables

Définir soit :
- une liste normative des commandes annulables ;
- soit une règle déterministe permettant au firmware de renseigner le bit9.

### V1.1-CMD-006 — Cycle d'annulation

Définir :
- fenêtre pendant laquelle l'annulation est recevable ;
- statut pendant l'annulation ;
- état final d'une annulation réussie ;
- code résultat associé ;
- comportement si l'action a déjà produit des effets partiels ;
- mise à jour de l'historique.

### V1.1-CMD-007 — `clear_request_fields`

Définir précisément :
- liste des registres/champs remis à zéro ;
- traitement de `cmd_request_control` lui-même ;
- moment du nettoyage ;
- interaction avec une commande active ;
- interaction avec `cmd_active_*` et l'historique.

## 4. Priorité moyenne — états, résultats et diagnostic

### V1.1-CMD-008 — Progression de `cmd_status`

Décider si une séquence minimale doit être normative ou si seuls les états finaux sont contractuels. Si une séquence est imposée, préciser les transitions autorisées et si certains états peuvent être transitoires/non observables en Modbus.

### V1.1-CMD-009 — `cmd_result_detail`

Créer, si utile, une table normative par commande / cause :
- sens de `cmd_result_detail` ;
- valeur `0` quand non applicable ;
- index de paramètre fautif ;
- code défaut ACK ;
- sous-test SELFTEST ;
- autres usages futurs.

### V1.1-CMD-010 — Priorité des causes de refus

Définir une priorité lorsque plusieurs préconditions sont simultanément invalides, au minimum pour START ACQUISITION, et éventuellement pour les autres commandes multi-préconditions.

## 5. Priorité moyenne — commandes spécifiques

### V1.1-CMD-011 — Incohérence CRC APPLY CONFIG

Décider si le code générique `4` reste suffisant ou si une distinction explicite « CRC incohérent » est souhaitable. Aligner alors Bloc 4 et Bloc 5.

### V1.1-CMD-012 — SELFTEST étendu

Si le masque `param1` est conservé :
- définir les bits de sous-tests ;
- définir les valeurs réservées ;
- préciser la compatibilité avec `selftest_detail` et les résultats Bloc 7.

### V1.1-CMD-013 — REFRESH indicateurs

Définir la liste normative des indicateurs/états recalculés, ou préciser explicitement que la portée est laissée à l'implémentation et que seul le non-effet sur la configuration est contractuel.

### V1.1-CMD-014 — ENTER MAINTENANCE

Arbitrer si la politique « acquisition arrêtée » doit devenir normative. Si oui, définir le code de refus et les interactions avec les commandes autorisées/interdites en maintenance.

### V1.1-CMD-015 — Codes 17 et 18 liés à la maintenance

Définir quelles commandes exigent le mode maintenance (`17`) et quelles commandes sont interdites lorsque le mode maintenance est actif (`18`). La présence des codes dans la table ne suffit pas à définir leur emploi.

### V1.1-CMD-016 — RESET SOFTWARE et opérations critiques

Définir :
- la liste ou le critère d'« opération critique non terminée » ;
- le code résultat associé ;
- l'ordre de priorité avec acquisition active et absence de confirmation ;
- la frontière exacte avec les règles FT-PER post-reboot.

### V1.1-CMD-017 — RESET STATISTICS

Définir :
- la liste exacte des statistiques remises à zéro ;
- la sémantique de `param1` si un masque est introduit ;
- la valeur par défaut ;
- les bits réservés ;
- le résultat d'un masque invalide ;
- la correspondance avec les non-effets déjà normés.

## 6. Principe de traitement futur

Lors de la préparation d'une V1.1 :
1. arbitrer chaque candidat indépendamment ;
2. modifier d'abord les spécifications source concernées ;
3. mettre à jour le mapping si nécessaire ;
4. mettre à jour les familles de validation impactées ;
5. réaliser un audit de non-régression croisé FT-CMD / FT-INT / FT-PER / FT-RBT ;
6. ne promouvoir un candidat en exigence que lorsque son oracle est explicite et testable.

Ce fichier est volontairement conservé séparément du corpus V1 afin d'éviter toute ambiguïté normative.
