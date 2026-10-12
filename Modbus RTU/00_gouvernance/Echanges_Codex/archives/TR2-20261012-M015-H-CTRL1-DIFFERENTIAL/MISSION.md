---
mission_id: TR2-20261012-M015-H-CTRL1-DIFFERENTIAL
status: READY
base_ref: main
base_sha: 6357b667a45b0979e5edaf394cb273595295414d
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-H — Diagnostic différentiel SPI CTRL1_XL, sans oscilloscope

Appliquer AGENTS.md et gouvernance V2-B3 ; vérifier HEAD réel main, rapport M015-G DONE, archives, état courant et gels V1. Archiver exactement mission et rapport G. Préserver les worktrees/stashes, image M014-H, Flash/FRAM V2 génération12, microSD OFF et partition LBA65536. L'utilisateur NE souhaite PAS acheter d'oscilloscope. Il dispose d'un Brymen BM869s et d'un second breakout IIS3DWB identique, mais aucune manipulation physique ni remplacement n'est demandé dans H. Ne pas présumer les caractéristiques ou modes de mesure du multimètre sans documentation constructeur.

## Objet et critères
Caractériser de façon discriminante, avec une campagne physique courte maximum, la divergence G sur CTRL1_XL 0x10 : write 0xA8/HAL_OK puis read simple 0x50 ; lecture groupée FIFO 07..0A conforme. Déterminer si le résultat dépend de la méthode de lecture, du délai ou de la transition en mode actif, sans attribuer une cause non démontrée. Vérifier le précédent M013-C/D (CubeProgrammer HOTPLUG altérait NVIC ; reset GDB qualifié) et distinguer ce risque du SPI bloquant. Ne pas masquer une configuration incorrecte ni traiter HAL_OK comme preuve d'intégrité des octets.

## Travail autonome
1. Auditer code exact G, séquences SPI, CS, HAL, configuration IIS3DWB et notes constructeur présentes au dépôt ; concevoir un protocole A/B à variables contrôlées et ordre explicite : baseline lecture seule, écriture A8 et relectures CTRL1 simple et en burst contenant 0x10, délai borné documenté, états avant/après activation si sûrs. Capturer TX/RX bruts, HAL status/errors, CS GPIO, clocks, DWT, valeurs, marqueurs de validité, première divergence et état NVIC. Distinguer état capteur et artefact de lecture ; ne pas accepter une valeur non conforme par vote ou fallback. Ne pas inventer d'exigence de signal mesuré au fil.
2. Modifier au minimum le harness diagnostic OFF par défaut, sans toucher cœur, mapping, validator, spécifications gelées. Tests hôte ciblés et cross-build ARM défaut/profil. Une seule campagne matérielle courte si préflight et protection sûrs ; sinon BLOCKED documenté. Arrêt au premier risque, ne pas multiplier variantes ou répétitions pour obtenir un succès. La mission H porte sur la configuration, PAS sur la FIFO/XYZ/production ; si l'anomalie est caractérisée, clôturer H et proposer l'étape suivante.
3. Préparer, uniquement si les résultats le justifient, une fiche de mesures physiques BM869s non intrusive pour l'utilisateur : points exacts à confirmer sur montage réel, référence GND, mesures DC au repos (alimentation, CS, éventuellement lignes accessibles), critères et limites ; aucune mesure ohm/continuité sous tension, aucune sonde sur broches fragiles, aucun changement de câblage autonome. Ne pas mesurer/qualifier le SPI2,5MHz au multimètre ni supposer que MIN/MAX/Hz décodent des bits. Préparer un éventuel protocole A/B de substitution du breakout pour une mission ultérieure, uniquement si discriminant ; aucune intervention physique maintenant.
4. Flash/debug permis : identifier UID/STM32/image/hashes, sauvegarde Flash2Mio et données utiles ; voie flash NORMAL, jamais CubeProgrammer HOTPLUG, mass erase, Option Bytes/RDP. Dernier CLI puis reset/contrôle GDB qualifié, debugger détaché pendant test, restaurer M014-H intégralement byte-identique et vérifier reprise HP B0/B1 sans GDB pendant trafic. Aucune écriture FRAM/SD, aucune partition touchée.
5. Publier preuves brutes, tableau des observations et incertitudes, mesures de timing, conclusion sur hypothèses, ETAT_COURANT/roadmap et DERNIER_RAPPORT.md. Commits/push normaux et vérification distante. H DONE seulement si le diagnostic différentiel apporte une conclusion exploitable ; sinon BLOCKED avec raison exacte. M015 globale IN PROGRESS.

## Invariants
ODR26667Hz, fenêtres4096..32768, continuité, store-and-forward et exigences V1 inchangés ; pas de réduction silencieuse. Pas de DMA, acquisition FIFO, FFT, B5, Modbus vibration, Web, calibration ou nouvelle campagne A-G. Minimiser dépense de quota et préserver acquis.
