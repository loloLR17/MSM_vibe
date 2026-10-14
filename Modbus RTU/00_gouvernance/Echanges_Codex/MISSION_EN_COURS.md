---
mission_id: TR2-20261014-M018-SDMMC1-COMMUNITY-REFERENCE
status: READY
base_ref: main
base_sha: a517e1cd8ce35d6e48bbe92e53f07c7289980ae5
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M018 — Reproduction fidèle du projet communautaire SDMMC1 sur NUCLEO-U575ZI-Q

## Objectif
Reproduire sur la seconde Nucleo le fonctionnement SDMMC1 4 bits du projet public https://github.com/VILLO88/NUCLEOU575 (branche main), **sans modifier son code SDMMC, sa configuration de bus ni ses paramètres pour masquer un défaut**. Compiler, flasher, exécuter et diagnostiquer avec GDB ; vérifier le mode 4 bits effectif, l'intégrité des accès et les limites des performances observées. Établir la provenance exacte du commit tiers, de l'image et de la toolchain. Ne pas porter sur SDMMC2 dans cette mission.

## Préparation et état physique
L'utilisateur a soudé les broches, recâblé l'Adafruit 4682 en SDMMC1, vérifié deux fois le câblage, alimenté la seconde Nucleo, et **retiré la microSD SanDisk Ultra A1 32 Go**. **Ne pas considérer la carte présente** : procéder d'abord à l'identification ST-LINK, à l'inspection non destructive et à la compilation hors cible. L'insertion physique de la 32 Go appartient à l'utilisateur : demander uniquement ce geste lorsqu'il devient nécessaire, avec mise hors tension si exigée par le banc. Ne jamais tenter d'écriture SD avant confirmation de présence et identification CID/capacité. Aucun essai sur la SanDisk Ultra 512 Go de TR2-001.

## Référence et fidélité
Inspecter directement le commit réel du dépôt VILLO88/NUCLEOU575 : README, fichier .ioc, Core/Src/main.c, stm32u5xx_hal_msp.c, USB/TinyUSB/FatFs et configuration du projet. SDMMC1 attendu : CLK PC12, CMD PD2, D0 PC8, D1 PC9, D2 PC10, D3 PC11, 3V3/GND. Le code communautaire initialise en 1 bit puis demande 4 bits, avec repli 1 bit possible : un montage USB Mass Storage réussi ne démontre PAS 4 bits. Contrôler également l'adéquation du câblage et des connexions de la Nucleo au schéma ST, sans demander de soudures supplémentaires sans preuve. Toute adaptation indispensable à l'environnement de build ou au debug doit être minimale, séparée, documentée et comparée au code original ; ne pas réécrire le pilote.

## Exécution et preuves
1. Lire gouvernance courante AGENTS, GOUVERNANCE_CHATGPT_CODEX, dernier rapport M017, état courant et garde-fous ; vérifier HEAD et état local ; préserver les travaux existants.
2. Identifier strictement **seconde** NUCLEO ST-LINK SN 0032001F3235510C37333439 / UID 001B0046/38425018/20333157 ; interdire la principale SN 003500463235510B37333439. Ne pas toucher TR2-001, sa FRAM, ni sa microSD 512 Go. Pas de mass erase, Option Bytes, RDP, déprotection, reconfiguration destructrice.
3. Préparer compilation reproductible du projet tiers original. Vérifier si son firmware nécessite le port USB-C natif distinct du ST-LINK et le signaler précisément. Produire image, hash et logs.
4. Quand le banc et la présence physique de la microSD 32 Go sont confirmés, flasher normalement la seconde Nucleo et diagnostiquer avec GDB : retours HAL_SD_Init / HAL_SD_ConfigWideBusOperation, WIDBUS réel dans SDMMC1_CLKCR, clock réelle, erreurs HAL/SDMMC, identité carte, lectures/écritures et comparaison d'intégrité. GDB autorisé, mais ne pas transformer un test temporel en benchmark avec arrêts intempestifs. Mesurer débit et latences uniquement si méthode valide et sans modifier le comportement SD d'origine.
5. Si le firmware original ne peut être compilé ou exécuté tel quel, documenter exactement le blocage et proposer uniquement la correction minimale nécessaire ; ne pas conclure à l'échec du contrôleur SDMMC1 sur un problème d'environnement.

## Critères et sortie
Rapport factuel dans DERNIER_RAPPORT.md : SHA tiers, diffs éventuels, SHA firmware, identité Nucleo/carte, configuration effective SDMMC1, résultat 4 bits **prouvé ou non**, résultats d'intégrité, limites et comparaison prudente avec M017 SDMMC2. Logs et scripts publiés sur main, commits vérifiés. Distinguer preuve fonctionnelle du PoC et qualification TR2 426672 B/s soutenus. Signaler tout besoin de manipulation physique (insertion SD, connexion USB-C) sans transférer les opérations Git à l'utilisateur. Autonomie normale flash/debug/commit/push conformément à AGENTS.md. Ne pas annoncer de test physique non réalisé.
