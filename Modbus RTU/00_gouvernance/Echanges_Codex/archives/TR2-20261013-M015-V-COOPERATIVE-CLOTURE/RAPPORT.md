---
mission_id: TR2-20261013-M015-V-COOPERATIVE-CLOTURE
status: DONE
base_ref: main
base_sha: null
initial_head: 4faf74f42bd313efb9db5d17e0ea8b8ece370cd8
result_sha: ef468539b9943cc9b9b4d832e513a0f8ab05c7ae
created_at_utc: 2026-10-13T11:42:08.197479+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-V — Coexistence et acquisition qualifiées dans la portée bornée

**Résultat technique : PASS. M015 acquise dans la portée d'acquisition physique de la roadmap.** Une acquisition continue de 32 768 XYZ et 4 096 timestamps sur TR2-001 démontre les préfixes V1 4096/8192/16384/32768 et la coexistence avec deux lectures Modbus B0/B1. Le firmware cumulatif M014+M015 à **SPI3 4 MHz** reste chargé, détaché et en fonctionnement. Aucun besoin démontré de passer à 8 MHz. Rapport, preuves, journal et roadmap publiés puis intégralement relus sur `92c0966d5a4f84ee4033490825e3f7cc6dc6737a` : **118 fichiers byte-identiques**. Aucun blocage technique ; clôture DONE dans cette portée, sous contrôle indépendant ChatGPT.

J3 reste **EN COURS**, car M016 n'est pas réalisée. Aucune vibration n'est publiée dans le mapping V1 ; READY/stockage restent indisponibles. La qualification d'acquisition ne qualifie ni le traitement, ni les campagnes stockées, ni l'endurance.

## Identité, Git et autorisations

Mission initialement reçue au HEAD `4faf74f42bd313efb9db5d17e0ea8b8ece370cd8`. Reprise sur `ef468539b9943cc9b9b4d832e513a0f8ab05c7ae`, identique à origin/main, worktree propre `MSM_vibe-M015-V` / `codex/m015-v`. Pushs normaux vers `HEAD:refs/heads/main`, sans merge/rebase/force. Le dépôt principal reste à `82169af` ; tous ses worktrees et les deux stashes historiques sont préservés. Le contrat ne renseigne pas base_sha ni created_at_utc : champ base_sha du rapport null, contrat ChatGPT laissé intact. AGENTS.md et protocole applicables, archive M015-U conservée byte-exacte.

Permissions true de développement, commit, push, flash et debug exercées dans le périmètre. Seul TR2-001 a été accédé : ST-LINK **003500463235510B37333439**, UID **004A003D3842501820333157**. La seconde NUCLEO n'a pas été accédée. Aucun geste physique, Option Bytes, mass erase, changement de câblage, provisionnement FRAM, écriture SD ou modification de configuration persistante.

## Modification et cause du premier échec

Service coopératif réutilisant le driver ST, le transport U et les guards : Modbus pollé entre quanta bornés, un burst de 128 mots maximum, CPU160/HSI16/div4/mode3. Configuration, capture, nettoyage et invalidation séquencés ; IRQ actives, aucune attente HAL_Delay dans la boucle FIFO. RAM statique **602 176 octets**, réserve supérieure à 180 KiB hors allocation future. Les tests prennent en charge les quatre tailles V1.

Première campagne conservée séparément : code `2386654`, FIFO passe de vide à **512/FULL/OVR**, pause observée **141 ms** avant premier burst, 0 XYZ admis, result7 ; cleanup réussi `0C8000`. B0/B1 passent mais B1 survient après invalidation. Aucun OVR SPI STM32 : le refus porte sur le FIFO capteur. La lecture du code localise une reconstruction B5 inutile pour B0/B1 : snapshot du journal FRAM, parcours de 256 slots et deux copies, dans le chemin de préparation Modbus. Cause opérationnelle fortement étayée par le différentiel : la correction seule de cette préparation et un ajustement d'observation RX donnent une capture complète à fréquence inchangée. Ce n'est pas un profilage instrumenté interne des 141 ms ni une preuve électrique.

Correction publiée `ef468539` : utilisation du callback prepare_request existant. Seules les FC03 valides entièrement B0 ou B1 évitent le snapshot B5 non demandé. Fraîcheur B1, retrait de disponibilité, mapping, identité, CRC/adresse et contrôles B5 préservés ; les requêtes B5 gardent le chemin complet, potentiellement bloquant et **non coqualifié**. Tests d'intégration : B0/B1 répondent avec backend B5 indisponible, uptime frais ; B5 refuse sans image périmée puis reprend lorsque son backend revient.

Premier échec : M014-H restauré, Flash 2 MiB entièrement identique, génération12 et deux B0/B1 de retour réussis. Les journaux HP complets sont publiés séparément. Une erreur du collecteur GDB sur tableau de taille0 est conservée ; le collecteur suivant protège explicitement ces tailles. Aucun essai matériel supplémentaire pour cette seule erreur.

## Validations logicielles réutilisées

Validation complète après correction : **125/125 tests hôte, cross-build STM32 par défaut réussi** ; profil cumulatif coopératif SPI4 cross-buildé avec succès. Justification : interface partagée de préparation Modbus modifiée. Tests FIFO : quatre fenêtres, continuité, tags/parité/compteurs, saturation, timestamps non monotones/espacement, FULL/OVR, HAL/I/O, stop, timeout, erreur cleanup et invalidation. Les validations réussies sur code inchangé n'ont pas été répétées. Les avertissements newlib sur appels système non implémentés sont conservés dans les logs ; le build se termine avec code0.

## Essai physique corrigé — tentative n°2

Preuves locales durables : `/home/lolo/dev/msm/reports/TR2-mission015-V-attempt02-20261013T113404Z`. Sources évaluées : `ef468539b9943cc9b9b4d832e513a0f8ab05c7ae`.

BIN169380 octets SHA-256 **8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480** ; ELF SHA-256 **0eb4e3b00a305905a0ac5002b5ef9041ae1ddb56911fba9b5c122c0c5b885f6a**. Build, origine et empreintes conservés. Programmation qualifiée à 0x08000000, -w/-v/-rst, SN explicite ; verify réussi, relecture Flash et queue hors pages nécessaires identique au backup. Reset logiciel après la dernière opération CubeProgrammer, puis GDB seul, sans HOTPLUG. Aucune session GDB pendant le trafic HP. Aucun reset/flash après la capture réussie ; pas de restauration M014-H par automatisme.

| Critère | Observation | Verdict |
|---|---|---|
| Configuration | WHO7B, CPU160, source HSI16/div4, mode3, guards actifs | PASS |
| Fenêtres V1 | Préfixes4096/8192/16384/32768 avec512/1024/2048/4096 TS | PASS borné |
| Continuité | Même parser/capteur, huit frontières4096 sans arrêt/reset ; 0 défaut tag/parité/compteur/saturation | PASS borné |
| Capture complète | 36864 mots,32768 XYZ,4096 TS,32288 triplets distincts ;304 bursts | PASS |
| Timestamps | Pas24 ticks,8XYZ entre TS ; aucune régression ou rupture observée | PASS borné, métrologie non vérifiée |
| SPI/RAM |304 CRC32 de payload égaux aux tranches RAM et memcmp sur cible ; dernière4096 reconstruite intégralement byte-exacte | PASS avec limite ci-dessous |
| FIFO |1270 polls,max158,final106,flags0/result0 ; aucune FULL/OVR signalée | PASS borné |
| Modbus simultané | B0 au tick13699 et B1 au13766, état CAPTURING ; B1 à1479XYZ ; TC observés13704/13771 | PASS |
| UART |RX16,deuxTX,20events consommés,drop/erreur/failsafe0,queue vide | PASS |
| Cleanup et état final |result0/cleanup0,`0C8000`,coopREADY,FRAM verified,faults0,IRQ49/66 actives | PASS |
| Données persistantes |FRAM gen12,candidate actif byte-identique,CRC intégral avant/après égal ;SD non initialisée | PASS pour contrôles réalisés |

Les préfixes V1 sont extraits d'une **seule** capture32768, et non quatre démarrages indépendants. Leur terminaison spécifique est couverte par tests hôte, pas quatre essais physiques répétés. Aucun replay U/T, aucune troisième tentative V, aucun essai8MHz.

## Temps et marges

Capture entre ticks13697 et14944 : **1247 ms**, warm-up et service inclus. Quantum maximal mesuré : **2,9159625 ms**, dont contrôle/comparaison/CRC, contre budget nominal128mots **4,266667 ms** ; marge observée **1,350704 ms**. Ce maximum ne comprend pas toute la boucle Modbus ni la vérification FRAM post-capture. Travail par burst avant son CRC : moyenne2,070/max2,221ms ; payload896 final : moyenne1,811ms. Cadence moyenne entre bursts4,066ms, maximum5,005ms : ce maximum inclut l'attente de remplissage et ne doit pas être confondu avec le quantum CPU.

Niveau maximal158/512 donne une réserve observée354mots, environ11,8ms au débit nominal210ko/s. Ce calcul ne vaut pas garantie de deadline générale. La capture bornée et les deux requêtes ne montrent ni croissance vers FULL ni erreurs ; pas besoin technique démontré d'un essai8MHz. B0/B1 : latences intention→réponse décodée **54,006/31,815 ms** côté HP, service→TC observé **5/5 ms** côté STM32 (tick1ms et observation au poll suivant, pas mesure électrique DE/dernierstopbit).

HP : unique run `physical-m015-v-cooperative4-fixed`, COM7/1152008E1/unit1, deux FC03 seulement, retry0, retour0/passedtrue. **Journal entier récupéré et lu**, SHA-256 recalculé **dcdc9d0d7a7f0da133388247e7f3b27dbf00b007cb36a74280e0d1f3e6073c5a**, égal au journalHash distant. B1 reste statutNOMINAL(1)/AcquisitionState0/StorageStatus0 ; le READY RAM du diagnostic ne devient pas le READY métier V1.

## Préservation, limites et suite

Sauvegardes M014-H déjà vérifiées réutilisées, pas de sauvegarde Flash complète répétée ni restauration inutile après succès. Image M014-H BIN SHA **b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611** ; backup Flash2MiB SHA **556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0**. FRAM complète avant capture SHA **d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce** ; lecture complète post-cleanup CRC32 **3022062602**, égal avant/après. Pas de SHA complet après capture revendiqué. Génération12 et domaine actif byte-identiques ; Option Bytes inchangés par procédure. MicroSD jamais initialisée/accédée, aucune preuve nouvelle de contenu de partitionLBA65536, aucune écriture.

Toute RAM32768 est conservée ; les bruts SPI intégraux sont conservés pour la **dernière4096 seulement**. Pour les précédentes : comparaison byte-exacte sur cible et CRC par burst, sans capture externe wire ni journal SPI intégral. Tags modulo4 et TS ne prouvent pas une absence globale de perte. Cadence/ODR nominal26k7 configurée, pas calibration ou métrologie indépendante ; continuité démontrée sur1,247s, pas endurance. Défauts/transitions largement couverts en hôte ; le premier FULL réel et sa restauration puis la reprise corrigée sont les preuves matérielles d'erreur/reprise, pas une injection physique de toutes les erreurs HAL.

**M015 est clôturée dans le périmètre acquisition physique bornée prévu par la roadmap ; M016–M020 restent à faire, J3 en cours.** Firmware cumulatif conservé, sensor au repos après cleanup, B0/B1 disponibles. Acquisition réarmable seulement par le contrôle RAM de qualification après boot ; aucune campagne autonome V1 ni acquisition indéfinie qualifiée. L'activation métier, les indicateurs/FFT, le stockage, l'endurance, l'excitation/calibration et la coexistence des autres blocs nécessitent des missions dédiées. Pause méthodologique recommandée sur l'usage du driver officiel ST, le coût de HAL et des I/O persistantes dans les callbacks ; audit futur SDMMC4bits à contractualiser, non lancé.

Journal partagé et roadmap actualisés pour distinguer cet acquis des missions futures. Revue indépendante ChatGPT attendue. [Audit reproductible et preuves](preuves/TR2-20261013-M015-V/attempt02-independent-audit.json), [manifest des preuves](preuves/TR2-20261013-M015-V/MANIFEST.sha256), [stratégie de service](../INTEGRATION_COOPERATIVE_M015_V.md). Les binaires publics sont en hex lossless ; les logs Windows normalisés conservent leurs octets originaux lorsque nécessaire. Sauvegardes Flash/FRAM/RAM complètes et ELF restent au chemin local durable, leurs empreintes étant publiées. Aucun secret ni donnée persistante brute ajouté au dépôt.

## Publication

Correction logicielle publiée et intégralement relue sur `ef468539`. Cette tranche ne change plus le code évalué ; elle publie le rapport, les preuves, le journal et la roadmap. État local propre avant préparation ; diff indexé et manifest contrôlés, documentation/preuves uniquement, aucun code modifié après sa qualification. Tranche physique publiée par commit `92c0966d5a4f84ee4033490825e3f7cc6dc6737a` (« M015-V: publish physical cooperative FIFO qualification at 4 MHz »), push normal réussi. Après fetch, relecture intégrale des118fichiers de la mission depuis origin/main et comparaison byte-exacte réussie. Cette mise à jour DONE consigne cette preuve ; son SHA final distant est fourni dans la réponse Codex après vérification. Worktree de mission propre après commits ; dépôt principal et deux stashes historiques inchangés. Le journal a été actualisé pour résultat physique, firmware conservé, limites et prochaines décisions ST/SDMMC4bits ; aucun engagement M016. Le contrôle indépendant ChatGPT reste attendu.
