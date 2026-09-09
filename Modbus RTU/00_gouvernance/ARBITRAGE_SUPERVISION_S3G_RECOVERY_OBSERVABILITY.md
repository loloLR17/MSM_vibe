# Projet MSM — Capteur de vibration TR2

## Arbitrage S3-G — Recovery/restart intégré et observabilité minimale

Date : 2026-09-09

Cette tranche complète S3-F. Elle ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Objectif

S3-G ferme la phase logicielle S3 avant gel documentaire final.

La tranche ajoute :

- un test intégré de close/reopen SQLite ;
- la reconstruction d'une transaction B5 `Ambiguous` après recréation complète du composition root ;
- la vérification qu'une transaction ambiguë restaurée bloque une nouvelle commande ;
- un snapshot d'observabilité locale sans effet de bord.

## 2. Recovery/restart

Le scénario qualifié est un restart host logique :

1. ouverture/migration SQLite ;
2. création d'une transaction B5 ;
3. persistance des états `Prepared`, `Submitted`, puis `Ambiguous` ;
4. abandon du premier graphe runtime ;
5. recréation complète du composition root avec le même fichier SQLite ;
6. startup S3-C ;
7. reconstruction du `CommandCoordinator` par `device_id` ;
8. restauration du même `transaction_id` en `Ambiguous` ;
9. maintien du blocage d'une nouvelle commande tant que l'ambiguïté n'est pas résolue.

Cette qualification reste une qualification host close/reopen. Elle ne prétend pas qualifier une coupure physique pendant une écriture disque.

## 3. Observabilité minimale

`SupervisionRuntimeStatusReader` expose un snapshot local en lecture seule :

- readiness ;
- nombre d'endpoints configurés ;
- nombre d'endpoints `Compatible` ;
- nombre d'endpoints `Disconnected` ;
- nombre de `CommandCoordinator` reconstruits/créés ;
- nombre de transactions B5 actives `Ambiguous`.

Le reader :

- ne modifie aucun état ;
- n'ouvre aucune nouvelle autorité métier ;
- ne remplace pas les journaux persistants S2 ;
- ne constitue pas une API Web, une métrique Prometheus ou un mécanisme de monitoring externe.

## 4. Classification

- reconstruction B5 durable : architecture S2/S3-C déjà validée ;
- blocage d'une nouvelle transaction tant qu'une active non terminale existe : invariant B5 déjà gelé ;
- snapshot d'état host : **SUPERVISION_POLICY S3-G** ;
- test close/reopen : qualification logicielle host ;
- coupure physique disque, transport RS-485, Windows Service, Web/API : hors S3-G.

## 5. Passe transversale S3

S3-A à S3-G conservent les invariants suivants :

- `main` reste l'unique baseline ;
- un endpoint configuré n'invente aucun `device_id` ;
- SQLite est prête avant ouverture de la readiness ;
- les transactions B5 durables sont restaurées avant travail opérationnel ;
- une transaction ambiguë n'est ni rejouée ni réallouée automatiquement ;
- le lifecycle est one-shot ;
- le polling respecte la sérialisation par bus ;
- les travaux prioritaires passent avant le polling ;
- le polling opérationnel n'est autorisé qu'après identification compatible ;
- aucun transport physique, aucune décision V1.1 et aucun Web/API ne sont introduits.

## 6. Gel

Le gel documentaire S3 ne doit être créé qu'après validation locale verte de cette tranche par build et tests de la solution complète.
