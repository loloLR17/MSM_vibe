# TT-SEQ-CONFIG-001 — Préparation puis activation nominale d'une configuration

## 1. Objectif

Valider de bout en bout qu'une centrale peut préparer une configuration B4 conforme, constater que cette préparation n'active pas silencieusement la nouvelle image, puis l'activer par la commande B5 APPLY CONFIG et retrouver une image active correspondant à la préparation.

## 2. Sources

- `01_Specification_source/bloc4.md`
- `01_Specification_source/bloc5.md`
- FT-BLK-04 — cycle configuration
- FT-CMD-05 — APPLY CONFIG
- FT-INT-02 — application B4 ↔ B5

## 3. Préconditions

- acquisition arrêtée ;
- capteur dans un état permettant la préparation et l'application d'une configuration ;
- jeu de configuration de test entièrement conforme aux domaines V1 ;
- valeur de `prepared_config_id` choisie de manière à pouvoir identifier sans ambiguïté la configuration préparée ;
- accès aux observables B4 et B5 nécessaires.

Les préconditions élémentaires sont vérifiées selon leurs familles propriétaires ; ce test ne redéfinit pas leurs domaines ni leurs codes.

## 4. Procédure

1. Lire et enregistrer l'image active B4 pertinente avant préparation, notamment l'identité et les champs actifs nécessaires à la comparaison.
2. Écrire dans la zone préparée B4 un jeu de configuration valide et complet, distinct de l'image active sur au moins un champ fonctionnel observable.
3. Calculer le CRC préparé selon l'oracle FT-BLK-04 et écrire `prepared_config_crc` conformément à la V1.
4. Avant toute commande APPLY CONFIG, relire l'image active et vérifier via l'oracle délégué FT-BLK-04 que la seule préparation n'a pas activé la nouvelle configuration.
5. Soumettre la commande B5 APPLY CONFIG avec une transaction conforme à FT-CMD.
6. Attendre un état terminal selon les règles FT-CMD, sans imposer une séquence intermédiaire de `cmd_status` ni un délai non normé.
7. Vérifier avec l'oracle FT-CMD-05 que la commande est terminée avec succès.
8. Lire B4 après succès.
9. Vérifier avec les oracles FT-INT-02 que :
   - la configuration est active ;
   - l'identité active correspond à la configuration préparée ;
   - l'image active reflète la configuration appliquée ;
   - `active_config_crc` est cohérent avec l'image active selon la délégation FT-BLK-04.

## 5. Verdict FT-SEQ

### PASS

Tous les jalons 1 à 9 sont exécutables et satisfaits dans la même exécution du scénario :
- préparation conforme ;
- absence d'activation par la seule préparation ;
- APPLY CONFIG réussi ;
- activation finale cohérente de la configuration préparée.

### FAIL

Au moins un jalon normatif de la chaîne échoue ou la chaîne ne peut pas atteindre l'état final attendu alors que toutes les préconditions normatives sont satisfaites.

Le rapport d'essai doit identifier le jalon fautif et la famille propriétaire de l'oracle élémentaire correspondant.

## 6. Non-oracles

Ce test ne doit pas conclure FAIL sur la seule base :
- de l'absence d'observation externe d'un état intermédiaire `VALIDE` ;
- d'une durée de traitement jugée longue en l'absence de borne V1 ;
- d'un `config_revision_counter` ne suivant pas une politique inventée ;
- d'une séquence de `cmd_status` différente d'une séquence supposée mais non normée.

## 7. Traçabilité

- Exigence propriétaire : `SEQ02-R01`.
- Oracles composés : FT-BLK-04 + FT-CMD-05 + FT-INT-02.
- Scénarios de refus et reprise : FT-SEQ-07.
- Reboot/persistance : FT-PER.