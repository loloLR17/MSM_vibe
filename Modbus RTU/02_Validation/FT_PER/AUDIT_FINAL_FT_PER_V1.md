# FT-PER — Audit final V1

## 1. Objet

Ce document clôt la passe croisée finale de la famille FT-PER du référentiel Modbus RTU TR2 V1.

Périmètre : persistance et reprise après reboot, RESET SOFTWARE, power cycle, identité, configuration, moteur transactionnel, acquisition, campagnes, statistiques, diagnostic et retour de l'interface Modbus.

## 2. Résultat global

La famille FT-PER V1 est cohérente avec les sources normatives actuelles et avec les frontières de propriété établies avec les familles déjà gelées.

Aucune contradiction bloquante n'a été identifiée lors de la passe croisée finale.

La couverture consolidée comprend 69 exigences/classifications et 6 scénarios FT-PER distincts :

- 6 `COVERED` ;
- 1 `CONDITIONAL` ;
- 11 `DELEGATED` ;
- 6 `TRACE_ONLY` ;
- 45 `NOT_DEFINED`.

La proportion élevée de `NOT_DEFINED` est un résultat attendu et assumé : la V1 spécifie peu de politiques de rétention post-reboot et FT-PER n'en invente aucune.

## 3. Correction issue de la passe finale

Une ambiguïté de classification a été corrigée dans FT-PER-05 : la règle « défauts/warnings persistants tant que la condition est présente » mélangeait sémantique nominale et mémoire post-reboot.

Elle a été scindée en :

- règle nominale de condition active : `DELEGATED` ;
- mémoire/restauration non volatile après reboot : `NOT_DEFINED`.

Cette correction ne modifie aucun oracle V1 ; elle rend seulement la matrice univoque.

## 4. Contrôles croisés réalisés

### 4.1 FT-PER / FT-STR

FT-PER ne transforme pas la stabilité en fonctionnement normal en persistance non volatile. Les règles d'encodage, atomicité et cohérence intra-réponse restent à FT-STR.

### 4.2 FT-PER / FT-ACC

Les droits RO/RW et exceptions Modbus restent à FT-ACC. Aucun droit d'accès n'est utilisé comme preuve de persistance.

### 4.3 FT-PER / FT-LIM et FT-BLK

Les domaines simples et invariants intra-bloc restent respectivement à FT-LIM et FT-BLK. FT-PER n'utilise que les valeurs nécessaires aux observables post-reboot explicitement définis.

### 4.4 FT-PER / FT-CMD

FT-CMD reste propriétaire :

- de l'acceptation/refus de RESET SOFTWARE ;
- de la mécanique transactionnelle nominale B5 ;
- de l'idempotence sans reboot ;
- de l'historique nominal de dernière commande ;
- de RESET STATISTICS.

FT-PER ne possède que la question de leur état ou de leur conservation après une frontière réelle de reboot.

### 4.5 FT-PER / FT-INT

FT-INT reste propriétaire des relations inter-blocs nominales, notamment B1/B7 hors frontière de reboot. FT-PER applique séparément la sémantique normative de chaque champ de cause de reset sans inventer une égalité générale nouvelle.

### 4.6 FT-PER / FT-SEQ et FT-RBT

Les séquences nominales multi-actions restent à FT-SEQ. Les scénarios hostiles, pertes de trames et robustesse hors propriété spécifique de reprise restent à FT-RBT.

## 5. Oracles post-reboot confirmés

La passe finale confirme que les seuls oracles discriminants principaux sont :

- RESET SOFTWARE accepté/exécuté provoque un redémarrage logiciel contrôlé ;
- B1 `last_reset_cause = 2` après RESET SOFTWARE ;
- B7 `reset_cause = 2` après RESET SOFTWARE ;
- `device_id` explicitement déclaré unique et persistent, testé à travers RESET SOFTWARE ;
- B1 `last_reset_cause = 1` après power cycle ;
- B7 `reset_cause = 1` après power cycle ;
- distinction normative entre power-on et reset logiciel.

Aucune valeur exacte d'uptime au premier accès n'est imposée.

## 6. Dettes normatives conservées

Restent volontairement non définis dans V1 :

- persistance des configurations active et préparée ;
- politique des IDs, CRC, état et compteur de révision B4 après reboot ;
- mémoire d'idempotence et historique B5 après reboot ;
- reprise d'une transaction interrompue ;
- état de l'acquisition au boot ;
- devenir d'une campagne interrompue et reconstruction B6 ;
- persistance des diagnostics historiques ;
- politique des statistiques/compteurs ;
- valeurs initiales générales ;
- politique de persistance selon watchdog, brown-out, reset externe ou firmware update ;
- temps maximal de boot et reprise Modbus ;
- comportement Modbus pendant l'initialisation.

Ces dettes sont séparées dans `EVOLUTIONS_CANDIDATES_V1_1.md`, document informatif uniquement.

## 7. Contrôle anti-fabrication

La passe finale a vérifié explicitement qu'aucun test n'impose :

- une technologie Flash/EEPROM/RAM ;
- une conservation simplement parce qu'un champ est RO ou « actif » ;
- une persistance simplement parce qu'une donnée est statique en fonctionnement normal ;
- `uptime = 0` à la première réponse ;
- un temps arbitraire de boot/reprise ;
- une équivalence entre RESET SOFTWARE et power cycle ;
- une politique de reprise d'une campagne ou transaction non spécifiée ;
- un complément métier informatif comme oracle normatif.

## 8. Critère de gel

Les critères de gel sont satisfaits sur le plan documentaire :

- chaque `COVERED` est relié à un scénario ou oracle explicite ;
- la condition d'exécutabilité des resets réels est visible ;
- les observations non normatives sont `TRACE_ONLY` ;
- toutes les dettes significatives sont visibles en `NOT_DEFINED` ou dans le backlog V1.1 ;
- les délégations sont explicites ;
- aucune ambiguïté n'a été fermée artificiellement.

Le gel documentaire ne constitue pas un PASS matériel du firmware : les scénarios nécessitant une cible réelle restent à exécuter sur banc lorsque disponible.

## 9. Décision proposée

**FT-PER V1 est déclarée finalisée et gelable.**

Le merge dans `main` reste interdit tant que cette passe finale et cette décision n'ont pas reçu la validation explicite de l'utilisateur.
