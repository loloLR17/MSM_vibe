---
mission_id: TR2-20261012-M015-A-ACQUISITION-BRINGUP
status: BLOCKED
base_ref: main
base_sha: 762c459839439f7d27b42c11d12e309e65622a7f
initial_head: b147a918df1a699c428b6304f455ba76571798d3
result_sha: 3317535897931589cd2d26e44ce1c9900032b9bf
created_at_utc: 2026-10-12T09:32:34.704527+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-A — Acquisition physique IIS3DWB bornée

**Les critères techniques de M015-A sont satisfaits. Le statut BLOCKED porte uniquement sur la publication et la relecture distante finales exigées par le protocole V2-B3.** Aucun blocage matériel ni validation intermédiaire n'est attendu. M015 globale reste **IN PROGRESS** : les preuves concernent un diagnostic en RAM et une excitation mécanique interne au capteur, pas une acquisition continue de production ni une vibration externe contrôlée.

Le firmware permanent M014 est restauré exactement. Un contrôle physique B0/B1 confirme sa reprise. La FRAM et la partition microSD sont préservées.

## 1. Tranches réalisées

| Tranche | Résultat démontré |
|---|---|
| Reprise et préservation | Worktree isolé depuis b147a918 ; dépôt initial au 82169af, worktree M014-H au 762c459 et deux stashes conservés ; archives H exactes depuis 762c459 |
| Audit | Driver, services et norme B4 inspectés ; dépendance au stockage identifiée ; diagnostic isolé conforme au contrat, sans activation artificielle |
| Logiciel | Lecture brute XYZ et conversion communes avec le driver réel ; transaction SPI duplex unique ; sortie invalidée sur erreur ; capture bornée et restauration des registres volatils |
| Acquisition finale | WHO_AM_I=0x7B, CTRL1_XL=0xA8, CTRL3_C=0x44 ; SPI mode 3 à 625 kHz ; 2 048 échantillons, aucune erreur ni saturation |
| Temps | DWT à 160 MHz vérifié ; timestamps encadrant les lectures ; cadence observée d'environ 6 538 lectures/s |
| Variation | Auto-test mécanique interne positif puis négatif à ±4 g ; réponse sur les trois axes et retour au repos observés |
| Restauration | Après chaque diagnostic, les 2 Mio de Flash sont identiques à la sauvegarde initiale ; boot FRAM V2/génération 12 sain |
| Reprise M014 | Deux FC03 B0/B1 sur HP, deux réponses, retour 0, sans retry ; journal intégral récupéré, analysé et publié |

Le code réellement évalué pour la capture finale est **3317535897931589cd2d26e44ce1c9900032b9bf**, compilé en Debug. La configuration STM32 est relue dans `mode3-capture-result-no-reset.log` : SPI3 maître, deux lignes, 8 bits, CPOL haut/CPHA deuxième front, prescaler 256, FIFO threshold 8 et KeepIO activé ; CFG1=0x70070067, CFG2=0x87400000.

Le pinout existant reste PC10/PC11/PC12 AF6 et CS PC9. Aucun raccordement INT1/INT2 ni IRQ capteur n'est inventé. SPI3 est détenu par le diagnostic synchrone ; aucun DMA ou autre client ne l'utilise. L'alimentation capteur 3V3 est celle du banc documentée historiquement ; elle n'a pas été remesurée. La CLI relève 3,29 V côté MCU.

## 2. Échantillons et excitation physique

Quatre phases de 512 échantillons sont capturées. Chaque changement d'auto-test est suivi de 100 ms de stabilisation et d'une première lecture jetée, conformément à la procédure constructeur. Les 2 048 données enregistrées sont conservées sans filtrage.

| Phase | Moyennes nominales X / Y / Z, en mg | Écarts absolus au repos X / Y / Z, en mg |
|---|---:|---:|
| Repos initial | −157,13 / +52,33 / −970,60 | — |
| Auto-test positif | +1 540,87 / +1 914,13 / +645,16 | 1 698,01 / 1 861,80 / 1 615,76 |
| Auto-test négatif | −1 999,88 / −1 722,89 / −2 727,73 | 1 842,75 / 1 775,22 / 1 757,13 |
| Retour au repos | −158,12 / +51,90 / −970,37 | 0,99 / 0,43 / 0,23 |

La norme moyenne au repos vaut 984,75 mg, puis 984,65 mg au retour. Les écarts-types par axe sont d'environ 10 à 13 mg. Les sensibilités nominales et l'arrondi entier en mg proviennent du driver existant ; à ±4 g, la sensibilité utilisée est 122 µg/LSB. Le brut LSB est conservé, et chaque conversion est recalculée indépendamment depuis le CSV.

Les écarts d'auto-test restent tous entre 800 et 3 200 mg, selon [ST DS12569 Rev 8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) et la [procédure officielle ST](https://raw.githubusercontent.com/STMicroelectronics/STMems_Standard_C_drivers/master/iis3dwb_STdC/examples/iis3dwb_self_test.c), consultées le 12/10/2026. L'excitation électromécanique interne et sa réponse sont démontrées. Aucune excitation externe, calibration métrologique ou amplitude/fréquence vibratoire de référence n'est revendiquée.

La capture finale compte 2 052 polls, dont les quatre lectures jetées, zéro NOT_AVAILABLE, zéro erreur SPI et zéro saturation. Les quatre phases contiennent des XYZ distincts. WHO_AM_I, les registres, les statuts DRDY, les valeurs brutes, les conversions et les timestamps sont publiés.

## 3. Anomalie initiale et portée de la correction

Trois captures de 2 048 échantillons chacune restent publiées intégralement :

| Configuration | Résultat |
|---|---|
| Mode 0, TX/RX séparés, 2,5 MHz | Distribution bimodale proche d'un facteur deux ; norme au repos ≈1,54 g ; une saturation |
| Mode 0, transaction duplex unique, 2,5 MHz | Même anomalie ; norme ≈1,53 g ; trois saturations |
| Mode 0, duplex unique, 625 kHz | Même anomalie ; norme ≈1,53 g ; six saturations |
| Mode 3, duplex unique, 625 kHz | Distribution cohérente ; norme ≈0,985 g ; aucune saturation |

Les trois premières captures ne valident pas la cohérence physique, malgré HAL_OK, DRDY et les registres conformes. L'hypothèse initiale d'un décalage de bit à la frontière TX/RX n'est **pas confirmée** : le HAL ferme SPI entre ces appels, mais la transaction unique n'a pas suffi à résoudre l'anomalie. La baisse de fréquence seule n'a pas suffi non plus.

À 625 kHz, le changement de CPOL/CPHA vers le mode 3 supporté par ST élimine l'anomalie dans cet essai borné. Cette configuration est donc démontrée sur le banc. Sans oscilloscope ou analyseur SCK/MOSI/MISO, la cause électrique précise, le ringing et les marges setup/hold restent inconnus. Ne pas généraliser l'échec du mode 0 ou le succès du mode 3 à tous les montages.

Aucun correctif ne divise les valeurs suspectes ni ne les filtre pour obtenir un résultat attendu. Les essais complémentaires portaient sur des configurations modifiées et répondaient à une anomalie réelle ; aucun essai physique réussi sur code inchangé n'a été répété.

## 4. Cadence, cohérence et limites

DWT CYCCNT progresse et HCLK vaut 160 MHz. Chaque ligne contient le début et la fin de la transaction STATUS + XYZ, d'une durée moyenne d'environ 151 µs. Les intervalles entre fins de lecture sont proches de 152,9 µs ; les cadences des phases vont de 6 537 à 6 540 lectures/s.

Ces timestamps sont des bornes de lecture MCU, pas les instants de conversion du capteur ni une heure UTC. Leur précision absolue n'est pas étalonnée. BDU protège les mots LSB/MSB ; IF_INC permet le burst de six octets. La simultanéité des conversions XYZ n'a pas été mesurée.

L'ODR du capteur est **configuré** à 26,667 kHz. Il n'est pas mesuré indépendamment. Le polling, sans FIFO, DRDY physique ou DMA, peut perdre des échantillons intermédiaires. Il ne démontre pas un flux continu à 26,667 kHz, une bande utile de 6 kHz, une FFT ou une acquisition complète de campagne. Les axes sont ceux du capteur ; l'orientation actuelle du banc n'est pas mesurée. Aucun offset utilisateur ou calibration n'est appliqué ; les registres concernés sont audités.

## 5. Préservation et état final de la cible

La cible et le programmateur sont vérifiés : ST-LINK **003500463235510B37333439**, NUCLEO-U575ZI-Q, STM32U575/U585 ID0x482/Rev U ; UID96 **004A003D 38425018 20333157**. Avant le premier flash, les artefacts permanents H sont rehashés, le préfixe Flash est comparé au BIN, puis Flash 2 Mio et RAM 768 Kio sont sauvegardées.

Le firmware laissé en exécution est celui de production M014-H :

- BIN de 143 108 octets, SHA **b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611** ;
- ELF SHA **c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de** ;
- source **0b2a901e185c9cbf8c02410bf2a29a0a3a75491e**.

Il conserve TX qualifié, observer OFF, silence OFF et SPI mode 0, **sans acquisition active**. Le diagnostic mode 3 est temporaire ; cette chaîne n'est pas activée en production. Aucun nouveau firmware permanent n'est substitué à l'image qualifiée M014.

Quatre flashs de diagnostic effacent les seuls secteurs 0..4 ; les quatre restaurations permanentes concernent les secteurs 0..17. La commande qualifiée réellement utilisée est :

```text
-c port=SWD sn=003500463235510B37333439 freq=8000 -w <BIN vérifié> 0x08000000 -v -rst
```

NORMAL, identité et Download verified sont observés à chaque fois. Chaque CLI est suivie de la relecture GDB et du reset logiciel qualifié de réparation NVIC. Aucun HOTPLUG, mass erase, Option Bytes, RDP ou changement de câblage. Un seul trigger RAM 0x4D303135 par image lance la capture ; aucun debugger n'est présent pendant les lectures.

Après chaque capture, CTRL1/CTRL3/CTRL5 sont restaurés à **0/4/0**, avec ODR et auto-test désactivés. Les résultats et la RAM sont sauvegardés avant la restauration du MCU. Après chacune des quatre restaurations, les **2 Mio de Flash sont entièrement identiques octet par octet** à la sauvegarde initiale, et le boot est sain.

Les profils isolés ne démarrent ni SPI1/FRAM, ni microSD, ni UART. L'absence des symboles d'accès médias et de démarrage du transport est contrôlée. Aucune écriture ou opération de provisioning FRAM n'a lieu. Le boot permanent retrouve V2/génération image 12, identité 1 et configuration UNAVAILABLE. Aucun dump physique complet supplémentaire de FRAM n'est revendiqué : les preuves sont l'absence d'écriture dans les diagnostics et l'état de recovery observé ; le backup complet H reste conservé.

La microSD reste entièrement désactivée : zéro lecture, allocation ou écriture, partition LBA65536 préservée par l'absence d'accès. Le bloc 4 V1 impose storage_mode=1, une limite positive et une capacité disponible. `command_start_acquisition` et `acquisition_service` exigent un snapshot actif. Avec l'environnement indisponible/capacité zéro, aucune configuration active, campagne B5, READY ou STORAGE_AVAILABLE artificiel n'est créé. Le diagnostic isolé est expressément prévu par M015-A.

## 6. Contrôle physique de reprise M014

Ce contrôle vérifie la reprise après les profils temporaires ; il ne répète pas la campagne M014 de polling/perte-reprise. Le paquet HP H inchangé est réutilisé après vérification de ses 371 fichiers. Le nouveau nom `physical-m015-a-restored` autorise uniquement Steady : deux FC03, zéro retry. La baseline GDB fraîche est relevée **sans reset** ; les serveurs sont fermés avant le trafic. Une lecture SSH de l'heure HP permet une borne conservatrice ; la garde 0..30 s reste inchangée. Aucune horloge ou configuration PC/SSH n'est modifiée.

Le 12/10/2026, de **09:22:43.1838771 à 09:22:43.9646036 UTC**, COM7/115200 8E1/unité 1 : FC03 0/21 puis 1000/20 après B0 compatible, deux réponses, retour 0, connexion fermée. B0 complet correspond à l'identité persistée ; B1 porte status=1, flags=0, uptime=73 s, resetcause=5, stockage=0, acquisition=0 et erreurs=0.

Le journal de sept événements est récupéré intégralement par SCP, analysé et comparé au SHA distant **CFBF72C1716288EAA2F845B82D662BA62C4520C8647EC40BF7502EE3A1208E78**. Toutes les lignes sont publiées ; il ne s'agit pas d'une conclusion tirée du seul hash.

GDB APRÈS, sans reset : stage=2/result=0/pollerrors=0, génération 12/configuration UNAVAILABLE, erreur UART=0, TX idle, file head=tail=20, IRQ49/66 actives, CFSR/HFSR=0. MCU détaché en exécution, serveur terminé. Les compteurs observer sont absents du profil permanent et ne sont pas inventés. Aucune autre émission HP en M015-A.

## 7. Validation logicielle et erreurs

Deux validations complètes **114/114 tests et cross-build par défaut** réussissent : après l'ajout du driver/profil et après la modification de la lecture SPI partagée. Depuis la racine du worktree :

```bash
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh
```

Le nouveau test exécute le vrai driver et le diagnostic contre un HAL hôte : quatre échelles, signes, arrondi, axes, saturation, not-ready/timeout/error/busy, relâchement CS, refus de registres/reset/auto-test invalides, framing duplex et rejet du premier octet reçu. Il vérifie également les 2 048 acquisitions bornées, une erreur en cours de capture et la restauration des registres, ainsi que le refus d'une configuration d'axe inconnue. Les 113 tests existants sont conservés, sans affaiblissement.

Après les changements limités au harness de vitesse puis de mode, la validation proportionnée est `tr2_validate.sh --cross-build-only`. Les 114 tests inchangés ne sont pas rejoués. Les quatre profils Debug dédiés compilent avec CMake/Ninja, la toolchain ARM du projet, CubeU5 v1.9.0 et `-DTR2_STM32_ACQUISITION_CAPTURE=ON`. Les warnings Newlib/nosys sont conservés, sans erreur finale. Les 539 tests .NET acquis ne sont pas relancés et le paquet HP n'est pas reconstruit.

Erreurs conservées et corrigées : déclaration initiale du driver dupliquée, puis validation complète réussie ; chemin de script single-single refusé avant tout outil matériel ; CRLF normalisés en LF et sortie PowerShell OEM CP850 convertie en UTF-8 dans les preuves textuelles, originaux locaux conservés. Le téléchargement local du PDF ST a expiré ; les sections utiles ont été consultées via le navigateur, sans prétendre disposer d'un PDF local. La contradiction éditoriale de H3g-C historique est signalée dans l'audit et n'est pas corrigée hors périmètre.

Le replay suivant réussit sans accéder au matériel :

```bash
python3 'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261012-M015-A/verify_evidence.py'
```

Les quatre CSV reconstituent exactement les SHAs des arrays binaires bruts, soit 8 192 échantillons. Conversions, temps et statistiques sont recalculés ; les contrôles finaux passent et les échecs des trois captures initiales sont conservés. Manifest et journal HP sont vérifiés. Ce replay est une vérification de preuves, pas une répétition d'essai physique.

## 8. Publication, Git et suite

Commits incrémentaux publiés normalement sur main : 610f6c2 (driver/profil), 9d47f1f (duplex et première capture), 08e5ba3 (vitesse lente et deuxième capture), 3317535 (mode 3 et troisième capture). Les archives M014-H sont immuables et exactes depuis 762c459 ; la mission active M015-A n'est pas modifiée.

Worktree `MSM_vibe-M015-A`, branche locale `codex/m015-a`, destination explicitement autorisée `origin main`. Dépôt initial main au 82169af et worktree H au 762c459 préservés, avec leurs deux stashes historiques ; contrôle final après publication. Aucun merge, rebase, reset destructif ou force push.

Permissions exercées : commits/push normaux sur main, flash/debug standard qualifiés, triggers RAM bornés et contrôle HP de deux lectures prévu au contrat. Aucun provisioning, écriture NVM arbitraire, geste physique utilisateur, modification des gels V1 ou configuration Codex permanente.

Les preuves sont dans `Echanges_Codex/preuves/TR2-20261012-M015-A/` : quatre CSV et analyses, observations GDB, manifest et replay, SHAs de builds/Flash/restaurations, journal HP complet, analyse et gates. Les preuves complètes locales restent dans **/home/lolo/dev/msm/reports/TR2-mission015-A-20261012T085627Z** : BIN/ELF/MAP, backups, scripts et sorties des outils. Les binaires firmware et dumps mémoire ne sont pas ajoutés au dépôt public.

Le journal est actualisé pour l'acquisition bornée, l'anomalie du mode 0, la configuration mode 3 démontrée, la restauration M014 et les limites. La roadmap conserve M015 IN PROGRESS/J3 en cours ; la revue indépendante de M014 est indiquée par le contrat M015-A. La revue indépendante de M015-A par ChatGPT reste à faire.

Suite proposée : contrat M015-B pour le raccordement production en mode 3, le débit FIFO/DMA, les timestamps de conversion, la détection de pertes et une excitation externe contrôlée. L'allocation du stockage V1 avant campagne demeure un lot distinct, avec sauvegardes et protection de la partition existante. Aucun travail M016/Modbus vibrations/Web/SQLite n'est déclaré terminé. Les réserves M013/M014 sur timing DE/TC, endurance, CEM et qualification industrielle restent ouvertes.

La publication finale est suivie d'une relecture distante intégrale, puis d'une mise à jour DONE enregistrant le SHA relu. Le SHA de cette dernière publication sera communiqué séparément, sans référence circulaire.
