# Projet MSM — Capteur de vibration TR2

## Arbitrage S7-H3B2 — Raccordement runtime du monitoring B5 post-submit

Date : 2026-09-10

Cette tranche implémente uniquement la partie post-submit décidée en S7-H3A. Elle ne raccorde pas encore la réconciliation après reconnexion/recovery.

## 1. Objet

Lorsque `RuntimeConfiguration.B5` est présent, une commande B5 dont le submit physique se termine avec une transaction `Submitted` déclenche maintenant un travail prioritaire `CommandPostSubmitMonitoring`.

Le runtime utilise :

- le `BusWorkScheduler` existant ;
- le `CommandCoordinator` existant du `device_id` ;
- `B5Reader` ;
- `B5PostSubmitMonitor` ;
- les temporisations explicites de `RuntimeB5LifecyclePolicy`.

Aucune seconde machine transactionnelle n'est introduite.

## 2. Cycle

Après submit réussi :

1. le premier monitoring est planifié à `observedAt + PostSubmitPollInterval` ;
2. le timeout absolu est `observedAt + PostSubmitTimeout` ;
3. chaque travail de monitoring réalise uniquement une lecture B5 ;
4. preuve terminale reconnue par `B5TransactionEvidence` : résolution terminale durable par le `CommandCoordinator` ;
5. preuve non terminale avant timeout : nouveau monitoring planifié ;
6. absence de preuve au timeout : passage durable à `Ambiguous` ;
7. aucune écriture B5 n'est effectuée par le monitoring.

## 3. Identité

Le contexte de monitoring conserve le `device_id` de la transaction.

Avant toute lecture B5, le runtime exige que l'endpoint soit encore :

- `Compatible` ;
- identifié ;
- associé au même `device_id`.

Un autre équipement apparu à la même adresse ne peut donc pas fournir de preuve pour la transaction en cours.

En l'absence de session compatible ou de connexion physique, le monitoring ne lit rien. Il est replanifié avant timeout, ou marque la transaction `Ambiguous` lorsque le timeout est atteint.

## 4. Défaillances transport pendant lecture

Pour `Timeout` et `Io` pendant une lecture de monitoring :

- aucun retry d'écriture B5 n'est réalisé ;
- si la transaction reste `Submitted` et que le timeout de décision n'est pas atteint, une nouvelle observation est planifiée ;
- pour `Io`, la politique S4 de déconnexion/invalidation du bus est appliquée ;
- si `B5PostSubmitMonitor` constate que le timeout est atteint, il persiste `Ambiguous`.

La journalisation détaillée des nouvelles lectures H3 et le cycle de réconciliation sont laissés à la tranche suivante afin de maintenir H3-B2 isolée.

## 5. Activation

Si le bloc `b5` est absent, `PhysicalB5LifecycleWorkRunner` délègue les travaux historiques au `PhysicalPriorityWorkRunner` sans planifier de monitoring post-submit.

Le comportement pré-H3 reste donc compatible.

## 6. Composition

`PhysicalSupervisionRuntimeFactory` utilise désormais `PhysicalB5LifecycleWorkRunner` comme runner prioritaire. Celui-ci enveloppe le `PhysicalPriorityWorkRunner` existant pour tous les travaux historiques et traite uniquement `CommandPostSubmitMonitoring` en plus.

Le `BusWorkScheduler` reste l'unique autorité de sérialisation par bus.

## 7. Validation attendue

Les tests H3-B2 prouvent au minimum :

- submit physique effectué exactement une fois ;
- `Submitted` après les deux écritures B5 ;
- planification d'un travail `CommandPostSubmitMonitoring` ;
- résolution uniquement après preuve terminale B5 ;
- preuve non terminale replanifiée ;
- passage `Submitted -> Ambiguous` au timeout configuré ;
- aucune écriture supplémentaire pendant le monitoring.

## 8. Hors périmètre

Restent pour H3-B3 :

- `TransactionReconciliation` physique ;
- amorçage après nouveau B0 compatible ;
- reprise d'une transaction `Ambiguous` récupérée au startup ;
- replanification périodique de la réconciliation ;
- journalisation spécifique des lectures de monitoring/réconciliation si nécessaire sans nouvelle taxonomie métier.

Aucun `Retry`, `Ignore`, `Force`, replay de submit, nouveau registre, V1.1 ou changement B6 n'est introduit.
