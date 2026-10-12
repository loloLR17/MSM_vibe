---
mission_id: TR2-20261013-M015-R-FIFO-DEBIT
status: READY
base_ref: main
base_sha: 7f30bc548938d197f52202454392a4f90c6602ec
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-R — Diagnostic court du débit FIFO

Objectif : identifier la limitation empêchant 4096 XYZ sans présumer une cause CPU. M015-Q a validé 512 XYZ, mais 4096 a échoué à 2275 XYZ et 285 timestamps avec FIFO plein. CPU 16 MHz, SPI 2 MHz ; payload SPI environ 247 ko/s et cadence de service environ 181 ko/s. Quota Codex restant limité : minimiser les tests et arrêter dès qu'une décision technique est possible.

## Préparation
Lire AGENTS.md, gouvernance et protocole, état courant, rapport et preuves Q, sources de main. Vérifier HEAD, Git status, worktrees, stashes. Archiver le contrat et le rapport Q selon le protocole sans altération.

## A — Analyse sans matériel prioritaire
Reconstituer le budget temporel du service FIFO avec les traces Q : durée SPI, intervalle entre segments, statuts, parsing, vérifications et traces. Séparer mesures, estimations et inconnues. Calculer le débit requis à partir de l'ODR effectif, des mots FIFO XYZ/timestamps et de la documentation ST. Vérifier les horloges CPU et SPI effectives, prescalers et couplages. Décider si le SPI 2 MHz possède une marge théorique suffisante. Ne pas lancer d'essai CPU-only si le SPI est déjà limitant.

## B — Test conditionnel
Seulement si A ne suffit pas, préparer une variante instrumentée minimale avec compteurs DWT. Changer au maximum un paramètre de vitesse réellement indépendant, CPU ou SPI selon les mesures. Si les horloges sont couplées, ne pas prétendre les isoler. Garder driver ST, ODR, parser, protocole, vérifications et cleanup inchangés. Maximum deux nouvelles acquisitions sur le banc secondaire, dont au plus une tentative 4096 ; une seule si suffisante. Ne pas rejouer les témoins ni 512 sur code inchangé. Stop immédiat sur overflow ou erreur, conserver tous les octets et traces. Succès 4096 seulement avec compte exact XYZ, tags, timestamps, parité, intégrité brute, absence overflow et cleanup correct. Pas de chantier DMA ni refactoring dans R.

## Cible et garde-fous
Seconde NUCLEO exclusivement : STLINK 0032001F3235510C37333439, UID 001B00463842501820333157. Vérifier identité, image, hash, flash normal avec verify et relecture. Interdit de toucher au principal STLINK 003500463235510B37333439, UID 004A003D3842501820333157, firmware M014-H, FRAM, microSD, Modbus et supervision. Pas de mass erase, Option Bytes, RDP, sauvegarde/restauration Flash 2 MiB injustifiée, ni manipulation physique de nappe, Dupont ou alimentation. Vérifier XL off, FIFO bypass, timestamps off, absence faults et détachement. Arrêter sur incertitude matérielle.

## Livrables
Tests ciblés et compilation ARM seulement si nécessaires. Fournir tableau débit requis/disponible, mesures de coût de service, résultat de l'essai conditionnel et prochaine décision technique. Publier preuves, état courant, roadmap et DERNIER_RAPPORT.md sur main ; vérifier les publications distantes. Préserver archives et travaux locaux. Autonomie complète de développement, commit, push, flash et debug dans ce périmètre, sans demander de validations répétées.
