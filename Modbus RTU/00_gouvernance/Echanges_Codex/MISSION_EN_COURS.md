---
mission_id: TR2-20261012-M015-J-CONTINUOUS-FIFO
status: READY
base_ref: main
base_sha: d25aa42a3d2c4850f4cedeeaca545da63aad0c29
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-J — Acquisition FIFO continue, fenêtre 4096 XYZ en RAM

Lire AGENTS.md V2-B3, gouvernance courante, rapport M015-I DONE, preuves, état courant et roadmap sur main réel. Archiver mission et rapport précédents ; préserver worktrees, stashes et preuves. M015-I a validé physiquement deux bursts de 128 mots, 256 XYZ réels, contrôles SPI groupés, zéro erreur détectée et restauration M014-H. Cette preuve bornée ne valide pas le continu V1.

## Objectif
Construire et qualifier une acquisition FIFO réellement continue à ODR configurée 26667 Hz, pendant la vidange FIFO, et produire au moins une fenêtre complète de 4096 XYZ en RAM, sans changer le mapping ni les fenêtres V1 gelées 4096/8192/16384/32768. Les autres tailles, endurance et absence absolue de perte restent à qualifier séparément.

## Travaux
1. Vérifier le code courant, les ressources RAM, pile, BSS, les interfaces, tests et ownership des buffers. Dimensionner XYZ int16 x3, tags, métadonnées, marges, état de validité, timestamps et backpressure. Si 32768 ne tient pas en RAM, proposer une architecture segmentée pour stockage futur sans modifier V1 ni écrire sur média.
2. Réutiliser les contrôles groupés M015-I, WHO 7B, CTRL1 A8, IF_INC, FIFO controls, HAL_OK, ErrorCode, compteurs de transfert et CS. Aucune lecture simple sur admission, aucun vote, fallback ou assouplissement. Maintenir fail-closed au premier écart. Vérifier tags, parité, niveaux, overflow, comptage et budget temporel ; le compteur modulo4 ne suffit pas à prouver aucune perte.
3. Implémenter la plus petite tranche de service FIFO continu : bursts maximum128 mots sous limite SPI3 1024 octets, FIFO restant active pendant lecture, buffer borné pour 4096 XYZ, transitions fenêtre, télémétrie cadence, temps de transfert, erreurs et pertes. Tests hôte nominal et défauts (SPI incomplet, niveau incohérent, overrun, timeout, wrap, limites RAM, arrêt). Cross-build ARM défaut et diagnostic. Checkpoint commit/push et relecture GitHub avant campagne.
4. Si préflights et restauration garantis, réaliser une seule campagne physique courte et bornée visant 4096 XYZ réels pendant acquisition active, avec plafond explicite temps et bursts. Publier données brutes, niveaux FIFO, timings et critères de continuité. Arrêter au premier refus ; pas de répétition opportuniste. Une fenêtre 4096 n'est pas une qualification endurance ni stockage V1.
5. Avant flash : UID 004A003D3842501820333157, STLINK 003500463235510B37333439, sauvegarde fraîche Flash2Mio et RAM utile avec hashes, image M014-H BIN SHA256 b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611. Flash NORMAL verify/reset uniquement, jamais HOTPLUG, mass erase, Option Bytes/RDP. Traces sauvegardées avant reflash ; restaurer M014-H en finally, Flash2Mio byte-identique à la sauvegarde fraîche, reprise HP deux FC03 B0/B1, FRAM gen12, SD OFF, IRQ49/66, fault0, MCU running. Aucune manipulation physique ni écriture FRAM/SD.
6. Si ressources, quota ou sécurité insuffisants, publier état IN_PROGRESS et point de reprise plutôt que flasher. Si échec matériel, BLOCKED documenté, avec restauration contrôlée. Ne pas solliciter de validations répétitives pour développement, commit, push, flash ou debug autorisés.

## Critères de sortie
DONE seulement si 4096 XYZ réels en une fenêtre continue sont acceptés selon contrôles documentés, sans erreur ni perte détectée, et si restauration et Modbus HP sont vérifiés. Publier code, tests, cross-build, preuves et hashes, métriques, limites et statut exact dans DERNIER_RAPPORT.md, ETAT_COURANT et roadmap ; push main et relecture distante. Hors périmètre : FFT, DMA non nécessaire, stockage persistant, Modbus vibrations, supervision Web, calibration, changements V1 et endurance prolongée.
