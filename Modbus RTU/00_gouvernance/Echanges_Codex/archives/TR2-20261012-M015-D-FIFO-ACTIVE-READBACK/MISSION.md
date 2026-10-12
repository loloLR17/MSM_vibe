---
mission_id: TR2-20261012-M015-D-FIFO-ACTIVE-READBACK
status: READY
base_ref: main
base_sha: 37b97f7fd8ea70c4ae8e377a3042bb3e2b8c5328
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-D — Diagnostic FIFO active, tranche courte

Respecter AGENTS.md, le protocole V2, les gels V1, la roadmap et le rapport M015-C BLOCKED. Vérifier main courant, archiver la mission précédente, préserver les worktrees, stashes, sauvegardes et preuves. Publier les résultats rapidement et communiquer les tranches en cours sans demander de validations répétitives.

Objectif unique : déterminer pourquoi les lectures des registres FIFO deviennent incohérentes pendant son remplissage, alors que 512 lectures avec FIFO vide étaient cohérentes en M015-C. M015-B a observé 171 statuts et 11 reculs de niveau sans lecture de données ; ne pas attribuer ces anomalies au mouvement du voilier sans preuve.

1. Analyser hors ligne les traces M015-B/C et la documentation ST : FIFO_CTRL2, STOP_ON_WTM, FIFO_STATUS, niveaux et bits 7/8/9, transactions SPI single/burst, CS, timing et auto-incrément. Identifier un test discriminant précis.
2. Si nécessaire, effectuer une unique campagne courte sur cible avec FIFO active, sans lecture de FIFO_DATA_OUT ni capture XYZ : journaliser les octets TX/RX bruts, les statuts, CTRL2, les marqueurs CS et les horodatages autour du niveau 127/128. Ne pas multiplier les variantes. Si une mesure avec analyseur logique est indispensable, documenter le geste physique et demander seulement celui-ci.
3. Tests hôte ciblés et cross-build ARM proportionnés. Flash/debug autorisés sous sauvegarde, identification cible, hashes, procédure NORMAL et restauration exacte du firmware permanent M014-H, sans HOTPLUG CubeProgrammer, mass erase, Option Bytes ou RDP. Aucune écriture FRAM ni microSD. Après diagnostic, contrôler si nécessaire B0/B1 sur HP, sans GDB pendant trafic.
4. Publier traces et conclusions distinguant faits et hypothèses, ETAT_COURANT, roadmap et DERNIER_RAPPORT.md. M015-D DONE uniquement si le défaut est identifié ou la fiabilité FIFO active démontrée ; sinon BLOCKED documenté. M015 globale IN PROGRESS.

Exclusions : DMA, acquisition continue, FFT, stockage, calibration, campagnes B5, Modbus vibration, répétition des campagnes M015-A/B/C et requalification M014 sans justification. Aucun force push ni opération destructive. En cas de quota faible, publier les acquis et un point de reprise avant de poursuivre.
