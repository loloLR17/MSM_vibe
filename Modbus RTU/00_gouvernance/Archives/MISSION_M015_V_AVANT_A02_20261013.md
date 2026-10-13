---
mission_id: TR2-20261013-M015-V-COOPERATIVE-CLOTURE
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-V — Viser la clôture de M015 sur TR2-001

## Références et objectif
Lire AGENTS.md, ETAT_COURANT_TR2.md, ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md, rapport et preuves M015-U, spécifications V1 et règles de sauvegarde. Vérifier le HEAD réel de main, l'état Git, les worktrees/stashes, archiver l'ancien contrat selon la gouvernance. Objectif : intégrer le moteur FIFO IIS3DWB au firmware STM32 principal en **service coopératif**, avec acquisition physique et Modbus B0/B1 **simultanés** sur TR2-001, puis viser la clôture M015 si et seulement si les critères applicables sont démontrés. Ne pas démarrer M016–M020 ni publier prématurément des mesures vibratoires V1.

## Travaux autorisés
Auditer l'architecture actuelle et les propriétaires des ressources (SPI3, SPI1/FRAM, UART/Modbus, SDMMC2, IRQ, horloges, RAM, CPU). Transformer le profil FIFO expérimental exclusif M015-U en chemin de production coopératif à temps borné, sans affaiblir les guards ni casser M014-H. Réutiliser le driver ST et ses adaptations validées. Tester les défauts, transitions, reprise, erreurs HAL/OVR/FULL, timestamps, tags, compteurs, nettoyage et invalidation. Préserver le mapping V1, les médias et la configuration persistante. Pas de DMA, RTOS, refactoring global ou changement d'architecture majeur sans escalade motivée.

**SPI 8 MHz explicitement autorisé si les contraintes temporelles le justifient.** Mesurer d'abord la coexistence à 4 MHz sur TR2-001 (M015-U : service actif moyen 2,056 ms, budget nominal 4,267 ms, borne haute occupée maximale 4,000 ms). Si la marge est insuffisante, Codex peut de façon autonome tester et retenir SPI3 8 MHz, comme sur le second banc M015-T (CPU160, HSI16/div2, service actif moyen 1,119 ms), après contrôle effectif de la source RCC, du diviseur, du mode SPI, de l'intégrité des transactions et des ressources partagées sur TR2-001. La qualification 8 MHz du second banc ne vaut pas qualification automatique sur TR2-001. Ne pas dépasser 8 MHz ni modifier SPI1/FRAM.

## Validation et matériel
Compiler et tester logiciel/ARM ; exécuter la validation complète lorsque les interfaces communes sont touchées. Avant flash/debug, identifier sans ambiguïté TR2-001 et distinguer la seconde NUCLEO ; sauvegarder/vérifier les images et données nécessaires, les empreintes, la méthode ST-LINK et la possibilité de retour M014-H. Autorisations de commit/push/flash/debug normales accordées, sans validation répétitive ; préserver FRAM, microSD, identité, Option Bytes et protections. Aucun mass erase, changement de câblage, écriture persistante imprévue ou opération destructive inhabituelle. L'utilisateur réalise uniquement les gestes physiques indispensables.

Effectuer des essais physiques bornés et instrumentés : au minimum fenêtre 4096 XYZ + timestamps, reconstruction wire/RAM, contrôle des erreurs et marges ; lectures B0/B1 **pendant** l'acquisition, contrôle des latences, de l'état UART et des transitions d'erreur. Vérifier les exigences M015 de la roadmap, dont les fenêtres V1 et la continuité ; une seule fenêtre 4096 ne suffit pas à affirmer une qualification complète si d'autres critères restent ouverts. Consigner PASS/FAIL/NON VERIFIE. Ne pas prétendre à endurance, zéro perte global, FFT, stockage ou calibration non testés.

Si l'intégration et la qualification sont satisfaisantes, **laisser le firmware cumulatif M014+M015 en fonctionnement sur TR2-001** avec sauvegarde M014-H vérifiée ; ne pas restaurer M014-H par automatisme. Si échec critique, retour qualifié à l'image de référence sans perte de données et vérification B0/B1. Ne jamais laisser la carte dans un état indéterminé.

## Livraison et clôture
Publier code, tests, traces et preuves, rapport DERNIER_RAPPORT.md, ETAT_COURANT_TR2.md et roadmap selon les faits ; vérifier les commits distants. M015 DONE seulement si les critères réels sont tous remplis ; sinon M015 IN PROGRESS avec blocages et prochain pas explicites. Ne pas déclarer J3/M016 accomplis. Après clôture M015, prévoir une pause méthodologique sur l'usage des drivers officiels ST et l'audit futur SDMMC 4 bits. ChatGPT contrôlera indépendamment le rapport. Respecter AGENTS.md et préserver travaux et archives préexistants.
