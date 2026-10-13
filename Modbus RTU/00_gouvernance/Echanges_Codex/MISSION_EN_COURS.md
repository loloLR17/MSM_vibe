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

Aucun nouvel essai pendant l'analyse M015-T, sauf essai conditionnel 8MHz ci-dessous expressément autorisé par l'utilisateur. Ne pas modifier firmware primaire M014-H, FRAM SPI1, SD, Modbus, HP, supervision, ni changer les sources expérimentales hors corrections strictement nécessaires aux analyses hors ligne. Ne pas entreprendre de DMA. Essai 8MHz autorisé seulement sous les conditions suivantes. Préserver tous les bruts et l'historique.

Livrer un rapport final DERNIER_RAPPORT.md avec statut correspondant aux preuves, matrice S/T, mesures de marges et implications pour un éventuel 8MHz (recommandation argumentée sans essai), preuves reproductibles, mise à jour ETAT_COURANT_TR2 et roadmap. Publier sur main par commit/push normal, vérifier intégralement la relecture distante, et conserver les archives et worktrees/stashes. En cas de preuves absentes, BLOCKED motivé, sans prétendre à une qualification 4096. Aucune validation humaine intermédiaire nécessaire pour ce travail documentaire et analytique.


## Autorisation conditionnelle SPI 8 MHz — décision utilisateur

Après vérification des preuves de M015-T et calcul des marges à CPU160MHz/SPI4MHz, Codex peut enchaîner **sans demander une validation supplémentaire** sur un unique essai physique à **CPU160MHz/SPI3 8MHz** si un gain de marge utile pour FFT, stockage et autres traitements est justifié et si l'intégrité M015-T est établie. Sinon, documenter le refus d'essai. SCK8MHz doit respecter la documentation IIS3DWB (maximum 10MHz), les limites STM32 et les garde-fous électriques du banc. Garder le même capteur, ODR, mode3, contrôles et traitement ; vérifier source d'horloge/diviseur et fréquences calculées (pas de prétention de mesure oscilloscope). Compiler, vérifier identité/image/empreinte/flash readback, une seule acquisition 4096 XYZ avec logs DWT, service complet, niveaux FIFO, flags, erreurs SPI, XYZ/TS, intégrité brute et cleanup. Stop immédiat sur OVR, timeout, corruption, overflow ou incertitude ; pas de répétition ni DMA. Seconde NUCLEO uniquement SN 0032001F3235510C37333439 UID 001B00463842501820333157 ; primaire strictement exclu. Pas de mass erase, Option Bytes/RDP, geste physique ni accès FRAM/SD/Modbus. allow_flash:true et allow_debug:true applicables à cet essai conditionnel dans AGENTS.md. Ajouter les résultats 8MHz au rapport final, même en cas d'échec ; distinguer fenêtre bornée et qualification industrielle.
