---
mission_id: TR2-20261013-M015-T-RESUME-PUBLICATION
status: READY
base_ref: main
base_sha: 7d762176b9fdc14d3c0cdce32bd6b3ce9d135d35
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Reprise et clôture de M015-T après interruption du quota Codex

Il s'agit de la CONTINUATION de M015-T, et non d'une nouvelle campagne physique. Lire AGENTS.md et la gouvernance actuelle, vérifier HEAD origin/main et état Git/worktrees/stashes sans écraser les travaux. Préserver et archiver les contrats et rapports antérieurs selon protocole.

Faits à réconcilier : GitHub main contient le commit 7d762176b9fdc14d3c0cdce32bd6b3ce9d135d35 de préparation CPU160MHz/SPI3 HSI16 div4=4MHz, mais DERNIER_RAPPORT.md demeure un checkpoint provisoire BLOCKED avant essai. Le terminal Codex du 13 octobre annonce ensuite une acquisition physique unique réussie : 4096 XYZ, 512 timestamps, 4082 triplets distincts, zéro erreur de parité/compteur/transport/overflow observée, niveau FIFO maximal 187, cleanup réussi. Ces données sont un compte rendu d'écran, PAS encore une preuve publiée ou auditée. Codex a été interrompu par la limite d'utilisation avant calcul des marges et publication.

MISSION : reprendre le worktree et les artefacts locaux existants M015-T, notamment /home/lolo/dev/msm/reports/TR2-mission015-T-20261012T233912Z et les fichiers de sortie de l'unique acquisition. Identifier les preuves réellement disponibles (BIN/ELF et hashes, UID/SN, logs bruts, dump, snapshots RCC/clock, DWT, états FIFO, buffers, parser, statut cleanup) ; ne jamais fabriquer ni extrapoler des preuves manquantes. Rejouer UNIQUEMENT les analyses hors ligne nécessaires, vérifier reconstruction byte-identique, tags/compteurs/timestamps, flags, débit et marges temporelles sur la fenêtre 4096. Comparer rigoureusement M015-S CPU16/SPI4 et M015-T CPU160/SPI4, distinguer calculs et mesures, et noter que la source SPI a changé MSI vers HSI donc isolement causal imparfait. La fréquence physique SCK n'a pas été mesurée à l'oscilloscope.

NE PAS réaliser de nouvelle acquisition, flash, reset, debug matériel ou autre opération physique pour cette reprise ; autorisations allow_flash/allow_debug restent explicites mais ne sont pas à exercer sans nouvelle mission. Ne pas modifier firmware primaire M014-H, FRAM SPI1, SD, Modbus, HP, supervision, ni changer les sources expérimentales hors corrections strictement nécessaires aux analyses hors ligne. Ne pas commencer essai 8MHz ni DMA. Préserver tous les bruts et l'historique.

Livrer un rapport final DERNIER_RAPPORT.md avec statut correspondant aux preuves, matrice S/T, mesures de marges et implications pour un éventuel 8MHz (recommandation argumentée sans essai), preuves reproductibles, mise à jour ETAT_COURANT_TR2 et roadmap. Publier sur main par commit/push normal, vérifier intégralement la relecture distante, et conserver les archives et worktrees/stashes. En cas de preuves absentes, BLOCKED motivé, sans prétendre à une qualification 4096. Aucune validation humaine intermédiaire nécessaire pour ce travail documentaire et analytique.
