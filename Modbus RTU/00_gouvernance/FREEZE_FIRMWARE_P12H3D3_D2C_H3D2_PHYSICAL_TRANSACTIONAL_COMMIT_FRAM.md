# Gel firmware — P12-H3d3-D2-C — Commit transactionnel H3d2 sur FRAM physique

## 1. Objet

Cette tranche qualifie le premier passage du chemin transactionnel H3d2 gelé sur la FRAM physique du STM32U575, à partir de la baseline D2-B déjà publiée et persistante.

La tranche ne modifie pas le protocole transactionnel H3d2. Elle raccorde le `PersistentMedia` existant au backend FRAM physique déjà qualifié.

## 2. Baseline physique avant D2-C

État D2-B de référence avant la qualification :

- recovery : `VALID`
- génération : `1`
- image active : `A`
- backend : FRAM physique STM32
- géométrie : profil H3d2 de qualification

Le harnais D2-C était gardé par les conditions `VALID + generation == 1 + active_image == A`.

## 3. Mutation de qualification

La mutation imposée était volontairement minimale :

- offset logique : `0`
- valeur : `0xA5`
- écriture par l'interface `PersistentMedia.write`
- commit par l'interface `PersistentMedia.commit`

Le commit H3d2 conserve donc la responsabilité exclusive de la séquence transactionnelle et de la publication. Le driver FRAM physique ne réimplémente aucune sémantique transactionnelle.

Préparation du harnais :

`00a3898020b53b55cf81582fd947573bf33353f1` — Firmware: prepare guarded D2-C physical commit qualification

Armement :

`12e443a4d38ef387f2253abffe91a7f64815d30f` — Firmware: arm D2-C physical commit qualification

## 4. Observation physique

Le boot qui a effectivement fait évoluer le média n'a pas été capturé pendant l'exécution du commit. En conséquence, D2-C **ne revendique pas** une observation directe de `commit_result == TR2_OK`.

Au boot observé ensuite, le recovery physique a donné :

- storage init : `TR2_OK`
- geometry validate : `TR2_OK`
- media init : `TR2_OK`
- recovery : `TR2_OK`
- status : `VALID`
- génération : `2`
- image active : `B`
- octet logique récupéré à l'offset 0 : `0xA5`

Le garde D2-C n'a alors déclenché ni nouvelle écriture ni nouveau commit, puisque l'autorité n'était plus génération 1 / image A.

Cette observation est cohérente avec la transition transactionnelle attendue de la baseline D2-B vers génération 2 / image B contenant la mutation `0xA5`.

## 5. Désarmement et preuve de persistance

Après cette observation, les actions de qualification ont été désarmées :

`3c7eb7faebba8e33185ec08f45a74853f75bab9d` — Firmware: disarm D2-C physical commit qualification

`5a1a07050ec572bd7287bd6ec29e70d05fe91718` — Firmware: disarm D2-B physical format qualification

Les trois portes destructives/transactionnelles de qualification sont alors à zéro :

- reset métadonnées D2-B : désarmé
- format EMPTY D2-B : désarmé
- commit D2-C : désarmé

Le firmware désarmé a été validé localement, cross-compilé STM32, flashé et démarré avec LD1 fonctionnelle.

Sur ce boot explicitement non destructif, la capture GDB finale a donné :

- storage init : `0`
- geometry : `0`
- media init : `0`
- recover : `0`
- recovery status : `1` = `VALID`
- génération : `2`
- image active : `1` = image B
- premier octet du candidate récupéré : `0xA5`

Les symboles des probes D2-B/D2-C placés sous compilation conditionnelle ne sont plus présents dans ce binaire désarmé ; leur absence en GDB est donc attendue et n'est pas interprétée comme un défaut runtime.

Cette seconde observation établit que l'autorité génération 2 / image B et la donnée `0xA5` survivent au reflash/reset et sont récupérées sans nouvelle opération de formatage ou de commit de qualification.

## 6. Conclusion de qualification

P12-H3d3-D2-C est gelée pour le périmètre suivant :

- H3d2 utilise le backend FRAM physique qualifié ;
- une mutation logique a été publiée vers l'autorité suivante ;
- le média récupéré est `VALID`, génération 2, image B ;
- la donnée mutée `0xA5` est récupérée depuis cette autorité ;
- l'état reste récupérable après reflash/reset avec les opérations de qualification désarmées ;
- aucune sémantique transactionnelle n'a été déplacée dans le driver FRAM.

## 7. Limites explicites

Ce gel **ne qualifie pas** :

- une observation instrumentée directe de la valeur de retour du commit qui a créé génération 2 ;
- une coupure d'alimentation réelle pendant les différentes phases du commit ;
- les scénarios de power-loss physique ;
- les fautes SPI/FRAM injectées pendant un commit réel ;
- l'endurance ou les performances temporelles en exploitation.

Ces points restent hors du périmètre D2-C et devront être traités dans les tranches physiques suivantes, notamment la qualification power-loss prévue.

## 8. Invariants conservés

Restent inchangés :

- format logique H3d2 ;
- double superblock ;
- double image ;
- génération monotone ;
- validation CRC ;
- publication logique atomique ;
- recovery H3d2 ;
- absence de retry automatique après résultat physique ambigu ;
- séparation entre logique transactionnelle portable et backend physique STM32/FRAM.
