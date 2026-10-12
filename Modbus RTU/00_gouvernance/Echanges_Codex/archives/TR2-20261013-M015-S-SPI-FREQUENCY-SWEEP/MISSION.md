---
mission_id: TR2-20261013-M015-S-SPI-FREQUENCY-SWEEP
status: READY
base_ref: main
base_sha: 4c0785bf11dfedabe93e996fefecdb58971d0177
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-S — Comparer les fréquences SPI du IIS3DWB et qualifier 4096 XYZ

## Décision et objectif
L'utilisateur autorise explicitement une exploration comparative de plusieurs fréquences SPI et, si nécessaire dans une future intégration, une adaptation de la vitesse du bus FRAM sous réserve de son fonctionnement validé. M015-R a établi un budget de 4,267 ms par segment de 128 mots FIFO contre 4,988 ms observés à CPU16MHz/SPI2MHz, avec overflow sur 4096 XYZ. Le SPI2MHz est la référence physique historique M015-Q ; ne pas le retester inutilement. Objectif : trouver la plus faible fréquence SPI offrant une marge mesurée et une acquisition 4096 XYZ sans débordement, sans supposer qu'une fréquence plus élevée garantit l'intégrité.

## Préparation et gouvernance
Lire AGENTS.md intégralement, protocole et gouvernance courants, ETAT_COURANT_TR2.md, DERNIER_RAPPORT M015-R, preuves et analyse DWT M015-R, rapport Q, sources de l'expérience, datasheet IIS3DWB et limites SPI de la carte/câblage. Vérifier HEAD réel origin/main, git status, worktrees et stashes ; archiver mission et rapport R byte-identiques selon le protocole. Préserver tout travail local. Conserver l'architecture, la configuration capteur, l'ODR, le driver vendor, le parsing, les contrôles, les journaux et le cleanup. Aucune investigation sur la cause historique nappe/Dupont.

## Campagne physique courte et discriminante
1. Vérifier avant essai les limites de fréquence SCK autorisées par le IIS3DWB et les conditions électriques de la carte STEVAL et du câblage. CPU MSI16MHz, SPI3 source PCLK3, mode3 et autres paramètres inchangés ; vérifier clock tree et diviseurs effectifs. La fréquence SCK annoncée est calculée tant qu'elle n'est pas mesurée physiquement : ne pas inventer une mesure oscilloscope.
2. Construire une configuration paramétrée MINIMALE du diviseur SPI3, sans duplication de banc ni refactoring. Tester **4 MHz (÷4)** puis, si compatible et sans défaut critique, **8 MHz (÷2)**. Si 8MHz excède une limite documentaire ou un garde-fou électrique, ne pas l'essayer ; le documenter. Aucune autre fréquence ni changement CPU/DMA dans cette mission. Le 2MHz est une référence Q historique, avec instrumentation identique ou différences explicitement signalées.
3. Au maximum **deux acquisitions physiques nouvelles**, une par fréquence, cible **4096 XYZ** chacune ; aucune répétition inutile. Un premier échec par overflow peut justifier 8MHz seulement si l'échec est un manque de débit propre, sans corruption, erreur HAL, registre ou signal matériel. En cas de corruption, incohérence de tags, fault, erreur SPI ou cleanup : arrêt immédiat de la campagne, pas d'escalade de fréquence. Si 4MHz réussit, exécuter 8MHz seulement si les limites documentaires et les marges justifient la comparaison ; la comparaison demandée par l'utilisateur est un objectif, pas une obligation de prendre un risque.
4. Pour chaque essai : vérifier avant flash cible, image BIN/ELF et empreintes, source, UID, SWD normal/verify/readback. Mesurer DWT durée payload et cadence service complète, statuts FIFO (niveau et flags FULL/OVR), nombre réel de XYZ/timestamps, ratio, TAG_CNT, parité, cohérence, échantillons distincts, erreurs HAL/SPI, compteurs de traces, intégrité mémoire et reconstruction brute byte-identique, registres capteur, cleanup. Conserver bruts y compris préfixe rejeté. Pas de promesse « zéro perte global » à partir d'une fenêtre unique. Sur échec arrêter au premier garde-fou ; ne pas masquer l'échec par le statut du programme.
5. Calculer la marge de débit et de temps réellement observée par fréquence, pas uniquement le débit SPI payload. Tableau comparatif 2/4/8 MHz avec statut physique VALIDÉ/BLOCKED/NON TESTÉ. Recommander la plus faible fréquence satisfaisant l'objectif avec marge raisonnable, sans déclarer le firmware TR2 principal qualifié.

## FRAM et intégration future
Inspecter **en lecture seule** les sources/pinout courants du TR2 principal pour établir le bus de la FRAM (SPI/I2C, instance, CS, partage éventuel avec IIS3DWB), les limites de fréquence et les validations existantes. Documenter séparément faits vérifiés et points non établis. L'utilisateur accepte une modification future de fréquence FRAM si nécessaire, sous condition de conserver sa fiabilité ; **aucun essai ni modification FRAM dans M015-S**. Ne pas inférer que le banc secondaire SPI3 représente automatiquement le câblage du TR2 principal. Toute intégration ultérieure devra préserver ou requalifier la FRAM si son bus change.

## Cible, autorisations, sécurité
allow_flash: true ; allow_debug: true uniquement seconde NUCLEO STLINK SN 0032001F3235510C37333439, UID 001B00463842501820333157, sans FRAM ni microSD. Primaire interdit : SN 003500463235510B37333439, UID 004A003D3842501820333157. Aucun flash/debug principal M014-H, FRAM, SD, Modbus, HP ou supervision. Pas de mass erase, Option Bytes, RDP, backup/restore 2MiB injustifié, intervention physique, manipulation nappe/Dupont ou alimentation. Si identité ou matériel incertain, arrêter. Vérifier après chaque capture XL off, FIFO bypass, timestamps off, faults0, détachement et état inactif. Les interventions physiques éventuelles reviennent à l'utilisateur.

## Efficacité et livrables
Budget réduit : deux fréquences au plus, deux captures au plus, compilation ARM et tests ciblés uniquement, pas de suite complète ni DMA. Préparer les deux variantes en une seule passe, réutiliser l'instrumentation et les outils d'analyse Q/R. Ne pas effectuer de nouvelles campagnes si la première montre un risque ou si les limites empêchent la suite. Publier matrice de résultats, traces et calculs reproductibles, conclusion FRAM lecture seule, état courant, roadmap et DERNIER_RAPPORT.md sur main. Contrôler publication et relecture distante, préserver archives, stashes, worktrees. DONE seulement pour objectifs effectivement prouvés, distinguer qualification 4096 et résultat de mission.
