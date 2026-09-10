# Projet MSM — Capteur de vibration TR2

## Arbitrage S7-H3B4 — Déclenchement de la réconciliation B5 après B0 compatible

Date : 2026-09-10

Cette tranche complète le raccordement H3 sans modifier les règles V1 ni la politique de recovery S5.

## 1. Objet

Une transaction B5 durablement `Ambiguous` ne peut être réconciliée qu'après réidentification physique d'un endpoint par B0 et confirmation que cet endpoint correspond au même `device_id`.

Le déclenchement est donc attaché à l'observation d'un B0 compatible, et non au simple startup, à l'ouverture du port série ou à une reconnexion transport.

## 2. Recovery startup

Le startup conserve exactement le comportement S5 :

- lecture du journal SQLite ;
- restauration de toute transaction non terminale récupérée sous forme `Ambiguous` ;
- aucune écriture B5 ;
- aucune conclusion métier ;
- aucune réconciliation tant que l'identité physique n'a pas été confirmée.

Après le premier B0 compatible du même `device_id`, le runtime peut planifier `TransactionReconciliation` selon `RuntimeB5LifecyclePolicy.ReconciliationInterval`.

## 3. Reconnexion

Une reconnexion transport ou une lecture B0 seule ne résout jamais l'ambiguïté.

B0 sert uniquement à autoriser la reprise des observations B5 si :

- la session est `Compatible` ;
- le device est identifié ;
- le `device_id` correspond au coordinateur portant la transaction `Ambiguous`.

La résolution reste exclusivement fondée sur les preuves B5 reconnues par `B5ReconciliationService` / `B5TransactionEvidence`.

## 4. Anti-doublon

Pour un couple `(device_id, transaction_id)`, un seul cycle de réconciliation peut être planifié à la fois.

Des B0 compatibles successifs ne doivent donc pas empiler plusieurs travaux `TransactionReconciliation` pour la même transaction.

La clé est libérée lorsque :

- la transaction est résolue terminalement ;
- le contexte n'est plus celui de la transaction active ;
- aucun travail de réconciliation ne subsiste pour ce couple.

## 5. Raccordement polling

`PhysicalPollingWorkRunner` conserve son rôle de lecture B0. Après application d'un `B0Session` compatible, il notifie optionnellement le runner lifecycle.

Cette notification :

- ne lit pas B5 elle-même ;
- n'écrit aucun registre ;
- ne crée pas de seconde autorité transactionnelle ;
- ne modifie pas la cadence de polling.

`PhysicalSupervisionRuntimeFactory` câble le runner lifecycle existant comme observateur de session compatible du polling physique.

Les constructeurs historiques du polling restent compatibles via un callback optionnel.

## 6. Invariants

Restent impératifs :

1. aucun replay automatique du submit ;
2. aucune écriture B5 pendant monitoring ou réconciliation ;
3. reconnect + B0 n'est jamais une preuve terminale ;
4. un autre `device_id` au même endpoint ne peut jamais résoudre la transaction ;
5. `Ambiguous` reste durable tant qu'aucune preuve terminale B5 n'est observée ;
6. le `BusWorkScheduler` reste l'unique sérialisation des accès bus ;
7. B6 reste hors de ce mécanisme ;
8. aucune règle V1.1 n'est introduite.

## 7. Validation attendue

Les tests H3B4 doivent vérifier au minimum :

- une transaction récupérée `Ambiguous` ne déclenche aucune écriture au startup ;
- l'observation du B0 compatible du même device planifie une réconciliation ;
- des observations B0 répétées ne créent pas de doublons ;
- un B0 compatible appartenant à un autre device ne planifie rien pour la transaction récupérée ;
- la résolution reste conditionnée à une preuve B5 terminale ;
- le nombre d'écritures B5 reste nul dans le chemin recovery/réconciliation.

## 8. Hors périmètre

Cette tranche n'ajoute ni bouton opérateur, ni Retry/Ignore/Force, ni nouvelle API Web, ni nouvelle commande/register, ni modification de `RESET_STATISTICS`, ni qualification matérielle.
