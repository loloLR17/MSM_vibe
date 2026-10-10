# État courant TR2 — référence reconstruite

Mise à jour : 10 octobre 2026. Mission `TR2-20261010-DEV-RTU-PRODUCTION-005`.

**Ce document est un nouvel état de référence reconstruit à partir de preuves vérifiables. Il n'est pas une restitution du journal historique HP.** Sa création a été explicitement autorisée par l'utilisateur pour la mission `T1000-20261010-CLOTURE-001`, après escalade AGENTS.md §18. Aucun original n'a été retrouvé dans l'historique Git, les répertoires Linux locaux ou les documents Windows accessibles. Les archives d'échanges du 8 octobre mentionnent un journal local HP, sans en fournir le contenu. Aucun état historique inconnu n'est inventé.

## Référence et gouvernance de la reconstruction T1000 (historique)

- Référence des sources au début de mission : `main`, `75eba6b431725b858db3ac5380884934026a1475`.
- AGENTS.md V2-B3 et `Echanges_Codex/PROTOCOLE_ECHANGE_V2.md` applicables ; les spécifications V1 et freezes restent inchangés.
- Autorisations de la présente mission : `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Elles expirent à la clôture. Aucun nouveau développement fonctionnel TR2.
- `Echanges_Codex/MISSION_EN_COURS.md` reste un document historique rédigé par ChatGPT ; le contrat courant est la demande utilisateur du 10 octobre et sa confirmation explicite de dérogation/reconstruction/permissions, pas cette ancienne mission.

## Résultats effectivement établis avant migration

- **VALIDÉ logiciel** : correctif autorisé du setup de `test_time_history_store.c`, `PersistentStorageCore storage = {0};`, sans changement de source de production ou d'assertion. Validation précédente : 102/102 tests firmware et cross-build ; supervision .NET restore/build et 466/466 tests.
- **VALIDÉ physiquement pour le poste** : mission distincte du 10 octobre, flash vérifié, démarrage, GDB, breakpoints matériels, reprise et reset reproductible sur TR2/ST-LINK CN1 `003500463235510B37333439`, STM32U575, Device ID `0x482`, Flash 2 Mio. MicroSD absente, selon confirmation utilisateur. Aucun mass erase/Option Bytes.
- Artefact réellement programmé : BIN 74 412 octets à `0x08000000`, SHA256 `5bb8d47bf370c99596b8e782611375f1ef086b121e11652618274925f376c0f1`. Référence de source ci-dessus. Rapport physique et sauvegardes Flash conservés hors Git dans le dossier de preuves T1000 du 10 octobre.
- **NON VÉRIFIÉ par ces missions** : qualification fonctionnelle complète TR2, stockage microSD/E4 power-loss, communication RS-485 réelle et debug source ligne par ligne. Les séquences E1/E4 peuvent écrire sur microSD au démarrage : toute mission physique future doit recontrôler les prérequis et autorisations.

## Environnement courant et mission de clôture

Clone actif migré dans Linux : `/home/lolo/dev/msm/projects/MSM_vibe`. SDK officiel CubeU5 v1.9.0 : `/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0`. Scripts et procédures T1000 dans `00_Environnement_developpement/T1000/`.

L'historique Git et les modifications locales ont été sauvegardés puis conservés à l'identique lors du déplacement ; comparaison des empreintes de 3 408 fichiers et `git fsck` réussis. Aucun nouveau flash/debug n'est nécessaire pour cette migration.

**VALIDÉ depuis la nouvelle architecture** : 102/102 tests firmware et cross-build ARM ; .NET restore/build sans erreur ni warning et 466/466 tests. Après arrêt/redémarrage WSL : 102/102 et 466/466 à nouveau, outils et authentifications opérationnels, binfmt actif et aucun service en échec. Codex natif charge réellement AGENTS.md, compte ChatGPT, défaut read-only/on-request inchangé. Git/gh utilisent un jeton vérifié dans le coffre Windows, retiré du fichier en clair ; helper Git local, aucun paramètre global modifié.

BIN et ELF identiques à la qualification physique précédente ; seule la MAP change avec les chemins. Aucun nouveau flash/debug. Copies anciennes archivées et vérifiées avant nettoyage ; dossiers personnels et preuves préservés. Réserves complémentaires non bloquantes selon décision utilisateur : contrôles Windows administrateur/sécurité séparés ; extraction installateur Windows encore verrouillée, conservée sans arrêt forcé.

**VALIDÉ publication** : correctif de test `dc6dac73bbbdd19ecf921f84808065f816a5bc9c`, première publication globale `61b1217fd8d0a337fdbc07445c056061a7e93c1c` sur main, push normal puis relecture Codex de 39 fichiers et contrôle API du rapport/journal. Git propre. Les commits locaux initialement refusés pour email privé restent sauvegardés hors publication ; identité locale passée à noreply sans désactiver la protection GitHub. Le rapport de clôture est mis à jour après cette preuve ; son SHA final sera communiqué après vérification de la seconde publication.

## Prochaine étape

Clôture technique T1000 : VALIDÉ AVEC RÉSERVES complémentaires non bloquantes. Le contrôle indépendant du rapport distant par ChatGPT n'est pas établi par la présente mission. Si l'original HP est retrouvé ultérieurement : relever sa provenance et son empreinte, comparer les faits et décisions à ce nouvel état, puis intégrer uniquement les éléments vérifiés sous mission autorisée ; ne pas remplacer silencieusement ce journal.

## Développement réception RTU — 10 octobre 2026

Mission précédente : `TR2-20261010-DEV-RS485-002`, sur `main`. HEAD évalué : `4591e13a37e99724c35e88fc03105e5e082c90b8` avec modifications locales non commitées. Contrat versionné `Echanges_Codex/MISSION_EN_COURS.md` ; commit/push/flash/debug interdits. Session Linux Debian dans le clone canonique, arbre initial propre ; mise à jour fast-forward depuis `36c33b7981c61376dae45586509327b899e5f3b8`.

- **Fait observé par lecture du code** : après démarrage UART, le harness ne consommait aucun événement série et ne faisait que clignoter avec `HAL_Delay(250U)`.
- **PRÉPARÉ** : `platform/stm32/main.c` consomme maintenant les événements dans la boucle principale, alimente le récepteur RTU portable existant, décode les ADU terminées et expose les compteurs `tr2_rtu_rx_*`. Les erreurs réinitialisent le récepteur. Le clignotement utilise `HAL_GetTick()` sans attente bloquante. Aucun traitement PDU, réponse, filtre d'adresse ou pilotage DE//RE n'est ajouté.
- **VALIDÉ cross-build uniquement** : `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only`, sortie réussie. Choix proportionné : modification limitée au harness, cœur et interfaces inchangés. Invocation directe refusée car script non exécutable ; usage `bash` documenté dans le script. Avertissements newlib au linkage concernant `_close`, `_lseek`, `_read`, `_write`. Pas de nouvelle campagne hôte.
- **NON VÉRIFIÉ** : exécution de cette nouvelle boucle sur cible, réception UART réelle, précision temporelle et liaison RS-485. Les compteurs de trames valides prouvent seulement le décodage ADU/CRC lorsqu'ils seront observés ; ils ne qualifient pas une requête PDU. Aucune opération matérielle effectuée.
- **Dette ouverte** : E4 microSD/power-loss inchangée. Prochaine action : revue de cette tranche locale par ChatGPT puis mission autorisée de qualification de réception ; la composition du serveur PDU de production et le pilotage du transceiver restent à traiter avec les prérequis documentés.

## Finalisation et publication — mission 003

Contrat de 003 (clôturé) : `TR2-20261010-DEV-RS485-003`, `target_branch: main`, commit/push autorisés, flash/debug interdits. HEAD initial `4591e13a37e99724c35e88fc03105e5e082c90b8`, mission distante récupérée au SHA `23e6de29ad4baa229987a9c972ef522b088a6791`. Les deux modifications locales de 002 ont été sauvegardées dans un stash conservé, puis réappliquées après fast-forward sur arbre propre ; comparaison au stash sans différence. Aucun code refait ou changement de spécification.

**VALIDÉ cross-build** : même commande `bash ./tr2_validate.sh --cross-build-only` avec le SDK Linux, relancée le 10 octobre 2026 en mission 003, code retour 0. Quatre avertissements newlib `_close`, `_lseek`, `_read`, `_write`, sans erreur de compilation/linkage. Diff inspecté et contrôle whitespace réussi. BIN SHA256 `dfc08e8c1e0bcd0da1cae0921215215e02289f17c2e0af5149656af70173ed60`. Pas de tests hôte relancés : seul le harness est modifié, sans modification du cœur ni des interfaces.

Publication et relecture distante consignées dans `Echanges_Codex/DERNIER_RAPPORT.md`, seul rapport de clôture de 003. **NON VÉRIFIÉ matériel** : réception, timing, bus RS-485 et réponses Modbus ; aucun flash/debug effectué. E4 reste ouverte. Prochaine action : contrôle indépendant du rapport publié par ChatGPT, puis mission autorisée pour la qualification de réception et la suite de l'intégration de production.

## Intégration applicative RTU — mission 004, 10 octobre 2026

Contrat 004 clôturé `TR2-20261010-DEV-RTU-INTEGRATION-004`, sur main ; commit/push autorisés, flash/debug interdits. HEAD initial propre `3f5706780b76707a3be339ac4fb424997d7e794b` ; mission récupérée par fast-forward au SHA `d04e580f8113b3a76f9122a64eb0106a4b1112c6`. Le stash historique 002 est conservé. Aucun changement de freeze, matériel ou travaux E4.

- **VALIDÉ logiciel hôte** : composition portable `ModbusSystemServer` reliant les autorités d'un SystemRuntime déjà booté au serveur RTU/PDU existant ; actualisation après acceptation CRC/adresse, B0 optionnel provisionné, projections runtime et staging B2/B4, inventaire B6, port explicite de soumission B5. Sans autorité d'écriture, exception 04 ; une anomalie préexistante renvoyant 02 à tort pour une autorité absente a été corrigée et couverte pour B2/B4/B5/B6.
- **Résultats réellement observés** : validation complète firmware finale via `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh` : 103/103 tests hôte et cross-build réussis. Test d'intégration nouveau : lectures, staging, invalidation workflow, soumission jusqu'à REFRESH_INDICATORS réel, CRC/adresse/broadcast, exception, défauts RX/TX sans replay automatique, readiness. Pas de tests supervision relancés. Quatre avertissements newlib de linkage conservés.
- **PRÉPARÉ STM32** : raccordement `main.c` → hook `stm32_modbus_application_bind` → init/start/poll du serveur applicatif. Le hook par défaut renvoie NOT_AVAILABLE, observable ; le firmware livré reste en diagnostic RX et n'émet pas de réponse applicative. Le runtime STM32 durable et le transport DE//RE qualifié doivent être fournis avant activation. Les instances temporaires du harness de qualification ne sont pas réutilisées comme runtime production. Dispatcher universel B5 non livré : port de soumission explicite, écritures indisponibles sans gestionnaire ; refus fonctionnel ID 0/code 14 et annulation relèvent de ce gestionnaire, pas d'une réussite simulée.
- **PRÉPARÉ essais** : `PLAN_ESSAI_RTU_INTEGRATION_004.md` précise les prérequis et commandes. Outil host offline `tr2_rtu_request` construit, génération/décodage et rejets contrôlés sans ouverture série. PG7/PG8 documentés ; PG5/PG4 seulement candidats DE//RE, domaine VDDIO2 à qualifier, transceiver réel à identifier (références historiques ADM2587E vs plan Click ADM2867E).
- **NON VÉRIFIÉ physique** : toute exécution de cette nouvelle composition sur STM32, réponse RS-485, timing, turnaround, débit. BIN SHA256 `4479d4e257273308b9dbe0a82da912e93a7d91918b5ec165d06f2a57541c3c77`. E4 microSD/power-loss reste ouverte. Publication et SHA du code relu : rapport Codex 004.

Prochaine étape à transmettre à ChatGPT : contrôler le rapport distant, puis traiter le raccordement runtime durable/autorités B5 et la décision matérielle DE//RE/VDDIO2/adresse/transceiver ; qualifier la réception/réponse sous une mission physique explicitement autorisée. Le périmètre portable utile est livré, pas un firmware industriel intégralement qualifié.


## Inventaire matériel RS-485 — correction confirmée par l'utilisateur, 10 octobre 2026

**RECTIFICATIF :** l'inventaire initial de ce jour inversait les références du composant installé sur MIKROE-3863. L'utilisateur confirme, après signalement de l'incohérence par Codex pendant la mission 005, que **le composant réellement monté sur MIKROE-3863 et installé sur le TR2 de test est ADM2867E, et NON ADM2587E**. Cette correction remplace l'affirmation erronée du précédent inventaire ; ne pas la traiter comme une nouvelle variante.

- **TR2 de test actuel :** ADM2867E sur carte MIKROE-3863, déclaré par l'utilisateur ; câblage et caractéristiques électriques encore à vérifier sur le montage réel.
- **Stock déclaré :** deux exemplaires ADM2867E au total (dont celui du TR2 actuel) et un exemplaire ADM2587E ; le second TR2 n'est pas encore qualifié. **Aucune carte support n'est attribuée ici à l'ADM2587E.**
- **Adaptateur USB–RS485 :** reçu mais jamais branché au PC ni au TR2 à la date de déclaration ; modèle, pilote et bornage non qualifiés.
- **Conséquence pour mission 005 déjà lancée :** privilégier les preuves constructeur et l'identification physique du couple ADM2867E / MIKROE-3863 ; l'ancienne mention ADM2587E / MIKROE-3863 dans la mission publiée est erronée. La mission READY n'est pas modifiée pendant son exécution ; Codex ayant identifié l'incohérence, le présent rectificatif fait foi pour les travaux suivants.
- **Limites :** inventaire déclaré, pas validation électrique. Ne rien déduire sans documentation sur DE, /RE, VDDIO2, niveaux, alimentation, isolement, terminaison, polarisation et bornes A/B. Aucune connexion ni flash implicite.


## Production RTU — mission 005, 10 octobre 2026

`TR2-20261010-DEV-RTU-PRODUCTION-005`, main ; commit/push autorisés, flash/debug interdits. HEAD initial propre `966737ee122fb4fb04489496691667f659607a23`, fast-forward de mission vers `ee95b8e3c0359f79cd6d208aa2ce0e9fae3d5b2b` ; base `ff19cf40a017233b967b95fea3365bad0f631e11`. Correction documentaire distante `463d69a8a6a2a36fe8a88f2905c6cad3ff5c3fac` intégrée après sauvegarde 005, sans merge/rebase ; code restauré octet pour octet. Stash 002 conservé.

- **VALIDÉ hôte** : propriétaire durable `ProductionApplication` (runtime et descripteurs, dispatcher, staging/workflow/adapter de configuration optionnels), boot unique/readiness, vrais services/journal borné. B5 raccordé aux exécuteurs 1/2 et runtime 3..10, validation/admission/refus persistants/retry/collision ; ID0 → code14 sans identité persistante, annulation consommée → code15 sans effet métier, last durable préservé. SELFTEST/RESET sans capacités et RAZ statistiques sans autorité → 04 avant réservation. Configuration CRC invalide → code4 ; source SYNC Modbus = 1. Aucun succès fictif.
- **VALIDÉ hôte / COMPILÉ STM32** : validation complète finale `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh`, code retour 0, 104/104 tests et cross-build. Nouvel essai d'intégration : RTU/PDU réels, configuration activée dans ConfigurationStore, reboot/restitution de refus, CRC invalide, ID0, annulation/concurrence, capacités absentes et injections de défauts RESERVED/COMPLETED. Journal en recovery_required après défaut : refus de publier une réussite, pas de retry automatique. Quatre warnings newlib usuels. Pas de tests .NET.
- **PRÉPARÉ STM32** : propriétaire statique indépendant du harness, adapters FRAM transactionnel/IIS3DWB/horloges/reset/continuité et boot réel compilés. Pas de format implicite ; EMPTY refuse. `stm32_production_binding_acquire` refuse par défaut avant préparation : firmware toujours diagnostic RX. Transport DE//RE qualifié, backend campagnes microSD durable/allocation, géométrie provisionnée, adresse/B0 et métadonnées de configuration restent prérequis de carte. Pas de recyclage des fixtures E3/E4.
- **Écart documentaire établi, matériel NON VÉRIFIÉ** : [MIKROE-3863 constructeur](https://www.mikroe.com/rs485-isolator-2-click) et schéma v102 désignent ADM2867E, contrairement à l'inventaire utilisateur ADM2587E. Écart levé par le rectificatif utilisateur publié au SHA `463d69a` : ADM2867E / MIKROE-3863 actuel, deux ADM2867E au total et un ADM2587E sans carte support identifiée. Vérifier néanmoins révision, U1 et câblage réels avant connexion. PG4/PG5 restent candidats, VDDIO2 et tous niveaux/jumpers/masses/terminaison/bias/A-B non qualifiés. Aucun pilotage direction activé.
- **PRÉPARÉ essais** : `PREPARATION_RTU_PRODUCTION_005.md` fournit matrice de preuves, identification/ouverture sans émission du câble sur PC seul sous mission future, jalon électrique séparé puis futur essai deux TR2. Câble et second TR2 non qualifiés ; aucun geste physique en 005. E4 microSD/power-loss reste ouverte.

À transmettre à ChatGPT : contrôler le rapport distant 005 ; contrôler révision et câblage après le rectificatif d’identité ; qualifier câble sur PC seul, puis montage électrique/transport et autorités de production sous missions adaptées. Rapport publié et qualification physique restent distincts.
