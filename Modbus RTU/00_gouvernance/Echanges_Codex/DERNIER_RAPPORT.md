---
mission_id: TR2-20261013-M015-U-INTEGRATION-TR2-001
status: DONE
base_ref: main
base_sha: null
initial_head: c27621ffbc40ae96ec5b67d2263a8a9daa21e610
result_sha: d18cf6a29cea8b8361424cb1b0809eb9bf4571fd
created_at_utc: 2026-10-13T10:00:48.315958+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-U — Qualification FIFO sur TR2-001

## Résultat

L’unique acquisition a reçu **4096 XYZ et 512 timestamps**, soit 4608 mots FIFO réels et 4087 triplets distincts. WHO_AM_I vaut 0x7B. Le résultat, le cleanup et les compteurs de défaut valent zéro ; le profil termine en READY, distinct de l’état B1 de production.

L’audit indépendant reconstruit le buffer RAM officiel depuis les transactions SPI, byte à byte. Aucun défaut de parité, tag, compteur, ordre XYZ/TS, saturation, HAL, OVR, FULL/latch, offset RX, CS ou journal n’est observé. Les 511 intervalles de timestamps valent 24 ticks, avec huit XYZ entre timestamps. Cela qualifie cette fenêtre ; le compteur modulo 4 ne prouve pas une absence globale de pertes.

**M014-H est restauré et fonctionne.** Les deux lectures HP B0/B1 ont réussi, et leur journal complet a été récupéré et vérifié. M015-U est clôturée dans sa portée bornée, après publication et relecture distante. M015 reste IN PROGRESS pour le service coopératif et la coexistence simultanée en production. M016–M020 ne sont pas engagées.

## Code et validations

Le profil `TR2_STM32_ST_FIFO_WINDOW`, désactivé par défaut, est intégré au `main` STM32 principal. Il utilise CPU 160 MHz, SPI3 sur HSI16/div4, mode 3, CS PC9 et PC10–12, FIFO threshold 1. La configuration CPU/PLL/VOS/Flash principale est conservée ; la source SPI est explicitement sélectionnée. Fréquence SCK calculée : 4 MHz, sans mesure électrique indépendante.

Le driver et l’initialisation ST, les adaptations T, le parser, le drain par 128 mots et les snapshots avant HALCloseTransfer sont réutilisés. Les erreurs ignorées par l’exemple ST provoquent un refus dans le port. Le getter FIFO défectueux utilise la correction locale documentée à 10 bits. Aucun affaiblissement des guards, DMA, refactoring général ou changement V1. [Matrice de compatibilité et plan](../INTEGRATION_FIFO_M015_U.md).

Le profil est exclusif des autres propriétaires de ressources. Son entrée noreturn empêche d’atteindre le harness : accès SD, écriture FRAM et transport UART sont absents de l’ELF. SPI1 conserve sa configuration et ne sert qu’aux contrôles FRAM en lecture seule. Les données restent en RAM et ne sont pas publiées comme mesures V1.

Validations réellement exécutées :

- `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh` : **123/123 tests hôte et cross-build défaut réussis**.
- Builds ARM du profil et de production réussis ; quatre profils concurrents refusés ; symboles interdits contrôlés.
- Test supplémentaire ciblé : **1/1**, sur l’adaptateur réel avec HAL simulée. Refus vérifiés : clocks incorrectes, UID secondaire, SPI occupé, mode/diviseur/CS/DWT incorrects, timeout, RX partiel, offset erroné, OVR et erreur HAL ; CS remonte et la réserve cleanup reste utilisable. Tests existants du parser et de l’admission finale couvrent notamment FIFO vide/partielle, configuration, tags, discontinuité, saturation et cleanup invalide.
- Le **BIN de production U est byte-identique au BIN construit depuis main avant U** (`c27621ffbc40ae96ec5b67d2263a8a9daa21e610`). L’écart avec l’image historique M014-H est antérieur à U. Ce build courant n’a pas été flashé ni qualifié physiquement.

Les 123 tests réussis n’ont pas été rejoués après l’ajout du test ciblé ; ce rapport ne revendique pas une nouvelle exécution complète de 124 tests. Les warnings newlib sont conservés dans les logs.

Erreurs rencontrées et conservées : lanceur non exécutable remplacé par bash ; déclaration `bench_result` manquante ; collision de macro/include ; déclaration noreturn manquante détectée par le contrôle des symboles ; paramètre BENCH_DUPLEX manquant dans le mock. Toutes ont été corrigées sans contourner les contrôles.

## Qualification physique et preuves

L’utilisateur a confirmé la reconnexion. Une seule sonde a été inventoriée : **SN003500463235510B37333439**, UID **004A003D3842501820333157**, reconfirmé par GDB avant flash. La seconde NUCLEO n’a jamais été accédée.

Code évalué : `d18cf6a29cea8b8361424cb1b0809eb9bf4571fd`, publié et 27 fichiers relus avant matériel. BIN SHA256 `c00918dab4eb1fbc454f8768a580dce12aa69872f30b61dca78b90a7c8b6410f` ; ELF Debug `beb1e8e2cec41fe1db9a652e47fd7e04c952d01449ce27ec684efe675dfdd6e2`. Programmation normale à 0x08000000, verify et readback exacts, tail Flash conservée ; après CLI, reset GDB et contrôles avant trigger. Aucun HOTPLUG, mass erase, Option Bytes/RDP ou geste physique.

Le premier préflight s’est arrêté **avant toute acquisition** : ELF sans informations DWARF, symbole `bench_ready` non typable par GDB. M014-H a été restauré et vérifié. L’ajout de `-g` a donné un nouveau ELF avec un **BIN identique** ; le préflight corrigé et le seul trigger sont conservés séparément. Aucun essai FIFO réussi n’a été répété.

Observations de la capture : RCC_CR=0x03000505, CCIPR3=0x10, VOSR=0x7C000, FLASH_ACR=4, CFG1/div4 et SystemCoreClock160MHz. 38 bursts, 123 statuts, 311 traces, 622 snapshots et 33132 octets de payload journal ; niveau FIFO maximal 187, final 60. Durée 166 ms incluant contrôles et attentes. Cleanup relu 0C/80/00 : XL OFF, FIFO bypass et timestamps OFF.

Le service actif moyen est **2,055843 ms**, maximum 2,057719 ms, contre un budget nominal de **4,266667 ms** par 896 octets. Payload moyen 1,810696 ms. La cadence avec attentes vaut 4,230434 ms ; 39 attentes demandées. La borne haute occupée maximale est 4,000038 ms, marge nominale 0,266629 ms. La soustraction des attentes minimales n’isole pas le coût CPU ; aucune capacité FFT/stockage n’en est déduite.

Raw FIFO SHA256 : `0c442a98b1c2736cd698b146fff5b8bc5443e721bb2e06028089a372a7a48e23`. [Corpus, bruts, scripts, audits et manifests](preuves/TR2-20261013-M015-U/README.md). Les anciens buffers `bench_result.words/xyz` ne sont pas utilisés comme données de fenêtre : les données réelles sont `official_words`, comparées aux payloads SPI.

## Préservation et état final

Avant flash, M014-H a été reconnu par son BIN SHA256 `b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611`, son ELF `c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de`, une double sauvegarde Flash de 2 Mio identique et la sauvegarde RAM du domaine persistant.

La FRAM complète a été sauvegardée et comparée avant acquisition, puis comparée intégralement après capture : SHA256 `d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce`, identique à la génération 12 connue. Aucune écriture FRAM ni accès microSD. La partition LBA65536 est protégée par absence d’accès ; ses secteurs n’ont pas fait l’objet d’un nouvel audit.

Après capture, les pages de l’image de retour ont été restaurées depuis le backup frais vérifié (147456 octets). **Les 2 Mio de Flash sont byte-identiques à l’état initial**, SHA256 `556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0`. Le domaine persistant actif de 51914 octets reste identique avant/après restauration. La FRAM complète n’a pas été relue une nouvelle fois après boot M014 : la comparaison complète porte sur la capture, et celle du domaine actif sur la restauration.

Le contrôle HP a vérifié 371 fichiers du paquet et réalisé uniquement deux FC03, COM7/115200/8E1, retry0 : B0(0/21) puis B1(1000/20), avec gate récent et contrôles d’exclusivité. Retour0, `passed:true`, DeviceId1/proto1 ; B1 UNCONFIGURED, uptime87s, flags/faults/storage/acquisition à zéro. **Journal complet récupéré, relu et hash recalculé** : `8c9a4b8b05871316c85fc4a5d6f277f42ef99e5a998fcf2e2693c62f9dc8bcdf`. La campagne historique perte/reprise n’a pas été relancée.

Dernière observation GDB sans reset : production stage2/result0/pollerrors0, FRAM VALID/gen12/slotB, identité DeviceId1/TR2-000001/unit1/fw0.14.8/proto1/caps0xC, configuration UNAVAILABLE, UART error0, TX idle/fault0, file20/20, IRQ49/66 actives, CFSR/HFSR0. **M014-H est laissé en exécution**, debugger détaché. La coexistence séquentielle capture → restauration → B0/B1 est démontrée ; acquisition et UART simultanés restent NON VÉRIFIÉS.

## Publication et suite

Archive durable : `/home/lolo/dev/msm/reports/TR2-mission015-U-20261013T100029Z`. Flash/RAM/FRAM/images/logs originaux y sont conservés ; seules les preuves utiles sont publiées. Logs et CSV normalisés pour Git avec originaux byte-exacts en `.original.hex`. Le premier commit de résultats conservait les fins CRLF des CSV : le contrôle whitespace les a signalées, mais la commande groupée a continué jusqu’au push. Ce défaut de séquencement est corrigé ; les CSV sont normalisés sans changement de données et les commandes suivantes utilisent un arrêt au premier échec.

Le dépôt principal historique82169af et les worktrees restent préservés ; les deux stashes sont intacts. L’archive T-reprise est exacte depuis51dff58. Le contrat courant omet base_sha et created_at_utc : cette absence est signalée, sans modifier MISSION_EN_COURS.md ; le HEAD initial complet figure dans ce rapport.

Journal et roadmap actualisés pour le résultat primaire, le retour M014 et les limites. Publication et relecture intégrale vérifiées au commit `78ec2d7ff25bb1a31babaa071c29cb325130385b` : 224 fichiers byte-identiques à la référence récupérée de GitHub. Le commit documentaire de clôture est ensuite vérifié séparément. Prochaine étape : revue indépendante ChatGPT puis contrat pour le service coopératif et les autorités de ressources, sans activation implicite des médias ou de M016. Pas de nouvelle opération matérielle nécessaire ici. Continu, endurance, calibration, 32768, traitement et publication V1 restent hors qualification.
