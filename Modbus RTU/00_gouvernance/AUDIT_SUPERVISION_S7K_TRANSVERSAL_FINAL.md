# Audit transversal S7-K — Supervision Web de production

## 1. Objet

Ce document clôt l’audit transversal **S7-K** de la supervision Web TR2 après validation de S7-A à S7-J et correction du reliquat S7-K1.

Baseline auditée :

- commit `ad63fd746b8125ae6bbed4465e522b250bf74373`
- message `Supervision: close S7-K1 global acknowledgement exposure`
- validation locale : build et tests verts, **466/466**.

## 2. Résultat de l’audit

Aucun écart bloquant supplémentaire n’a été identifié après S7-K1.

L’audit confirme la cohérence des frontières suivantes :

- un seul runtime physique de supervision ;
- aucun accès Modbus direct depuis le navigateur ;
- aucun second scheduler ou moteur transactionnel B5 côté Web ;
- DTO/projections IHM distincts des types de transport et du mapping brut ;
- B5 soumis uniquement via l’autorité transactionnelle existante ;
- B6 conservé hors du lifecycle transactionnel B5 ;
- sérialisation physique des travaux par bus ;
- sérialisation des soumissions Web par `device_id` ;
- conservation durable des états non terminaux et de `Ambiguous` ;
- aucun replay automatique après incertitude ;
- réconciliation B5 en lecture seule ;
- absence de résolution implicite par reconnexion/B0 ;
- visibilité séparée des problèmes de supervision PC et des défauts TR2 ;
- conservation de la dernière valeur connue après perte de communication ;
- fraîcheur PC distincte de la validité/fraîcheur fournie par le TR2 ;
- configuration B4 Web en lecture seule ;
- sélection B6 distincte des commandes B5 ;
- absence de `RESET_STATISTICS` ;
- absence d’acquittement global exposé par l’IHM ou l’API S7 ;
- absence de Retry / Ignore / Force ;
- absence de vitesse mm/s, FFT, fréquence dominante, seuil ISO ou score santé inventé ;
- RMS et crête B3 présentés comme données reçues du TR2 ;
- aucune règle V1.1 injectée silencieusement.

## 3. Réseau Web

La politique S7-A reste respectée :

- HTTP initial ;
- Web désactivé par défaut ;
- écoute par défaut sur `127.0.0.1` ;
- exposition non-loopback refusée sans `allowRemote=true` explicite ;
- UI et API same-origin ;
- CORS non activé ;
- aucune authentification/ACL/HTTPS ajoutée implicitement.

L’exposition LAN sans authentification reste une décision opérateur explicite et une limite connue, non une configuration implicite de production.

## 4. Cohérence S7-K1

L’audit a détecté puis fermé un reliquat : l’API HTTP acceptait encore `AcknowledgeFault` avec `acknowledgeAll=true` alors que S7-I gelait l’absence d’acquittement global exposé.

S7-K1 corrige uniquement la frontière HTTP :

- `AcknowledgeFault` Web requiert `acknowledgeAll=false` et un `faultCode` ciblé ;
- `acknowledgeAll=true` est rejeté avant le command sink ;
- le runtime/firmware interne n’est pas modifié.

## 5. Concurrence

Les garanties S7-J restent cohérentes après audit :

- `CommandCoordinatorRegistry` protège l’unicité par `device_id` ;
- `BusWorkScheduler` protège files, travaux actifs, work IDs et séquences ;
- les contextes d’un work prioritaire sont publiés avant visibilité dans le scheduler ;
- les lifecycle monitoring/reconciliation empêchent les doublons concurrents ;
- les soumissions HTTP sont sérialisées par device et non globalement ;
- plusieurs TR2 peuvent être sollicités indépendamment tout en respectant l’unicité physique du bus.

## 6. Limites non levées par S7

S7 ne constitue pas une qualification :

- du RS-485 réel ;
- du STM32 réel ;
- des performances sous forte charge HTTP ;
- de la robustesse EMI/EMC ;
- du déploiement Windows Service/installer ;
- d’une politique cybersécurité définitive avec authentification, rôles et TLS ;
- d’une analyse vibratoire avancée hors mapping V1.

Ces sujets nécessitent leurs propres phases et preuves.

## 7. Conclusion

Après correction S7-K1, **aucun écart bloquant supplémentaire n’est retenu** pour le périmètre S7.

S7 peut être gelée comme architecture Web de production pré-matériel, sous réserve de la validation locale du commit documentaire final de gel.
