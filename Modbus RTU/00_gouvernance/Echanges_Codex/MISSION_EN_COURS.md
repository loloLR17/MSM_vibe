---
mission_id: TR2-20261012-M015-C-FIFO-REGISTER-RELIABILITY
status: READY
base_ref: main
base_sha: cfb9918dfe8e5ac3148d6ceaa79e50acda6ac53f
created_at_utc: 2026-10-12T13:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-C — Fiabilité des lectures SPI des registres FIFO (mission courte)

## Contexte et gouvernance
Appliquer AGENTS.md, PROTOCOLE_ECHANGE_V2.md, rapport M015-B BLOCKED, audit, CSV171statuts et analyse hors ligne, roadmap et ETAT_COURANT sur main. Vérifier HEAD distant réel, Git, worktrees et deux stashes historiques. Archiver M015-B mission/rapport sans écrasement, préserver tous les livrables et preuves hors Git. Le firmware permanent M014-H est restauré (BIN SHA b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611), FRAM V2 génération12, microSD OFF. Ne pas répéter les campagnes M015-A/B ni les tests acquis sur code inchangé.

## Objectif unique, budget serré
**Distinguer la cause des readbacks incohérents des registres FIFO du IIS3DWB** avant toute nouvelle capture XYZ. M015-B a observé 171statuts, 11 reculs du niveau sans consommateur (premier127→0), FIFO_CTRL2 relu0x80 après configuration0x81, et bitoverrun ambigu. Aucune cause démontrée ; ne pas attribuer au mouvement du voilier ni déclarer une défaillance du capteur. Préserver les traces brutes et hypothèses contradictoires.

## Trois tranches
1. **Analyse statique courte** : vérifier selon AN5444, datasheet et driver officiel ST l'adresse/auto-incrément, R/W, dummy cycles, longueur, framing et maintien CS, ordre BDU/lecture des octets de statut, registres FIFO_CTRL2/FIFO_STATUS, flags RX HAL STM32U5 et SPI mode3/625kHz. Comparer octet à octet les transactions du harness FIFO avec celles du driver M015-A (lecture XYZ stable) ; examiner les 171statuts et les motifs bit7/décalage comme hypothèses, pas corrections de données. Publier tableau hypothèses/test discriminant minimal.
2. **Un diagnostic borné, si nécessaire** : réaliser un profil read-only de contrôle de fiabilité SPI : lectures répétées de registres constants/WHO_AM_I, lectures contrôlées FIFO_CTRL2/STATUS et octets RX bruts avec marqueurs de transaction/CS et flags matériels. Aucun démarrage de campagne FIFO/XYZ, aucun watermark, aucun overrun volontaire, aucune écriture FRAM/SD ; ne pas reconfigurer le capteur au-delà du strict nécessaire. Si flash temporaire indispensable, autorisé sous preflight, backup/hash, restauration byte-identique de Flash2Mio et contrôle minimal B0/B1. Jamais HOTPLUG CubeProgrammer, mass erase, Option Bytes, RDP ni changement de câblage. Si la seule observation concluante requiert analyseur logique sur SCK/MOSI/MISO ou intervention physique, préparer un protocole et signaler uniquement ce geste, sans prétendre l'avoir exécuté.
3. **Validation et publication** : tests hôte ciblés/cross-build proportionnés, au maximum **une campagne physique courte** si préconditions établies. Corriger seulement un défaut démontré de framing/readback ; aucune correction heuristique des statuts. Publier mesures brutes, comparaison attendue/observée, conclusions certaines vs hypothèses, ETAT_COURANT, roadmap et DERNIER_RAPPORT.md. M015-C DONE uniquement si fiabilité des readbacks prouvée ou cause précise isolée avec test discriminant ; sinon BLOCKED documenté. M015 globale IN PROGRESS.

## Exclusions et communication
Pas de FIFO capture256XYZ, pas de DMA, flux continu, calibration, FFT, microSD, B5 ni extension Modbus. Pas de répétition de la qualification M014. Annoncer tranches et résultats synthétiques, poursuivre sans microvalidations. Commit/push incrémental et relecture distante avant quota ; autorisations allow_commit/push/flash/debug toutes true. Escalader uniquement geste physique indispensable, risque matériel non maîtrisé, opération destructive inhabituelle ou décision structurante non couverte. L'utilisateur délègue les décisions techniques ordinaires. Ne pas modifier la mission active pendant son exécution.
