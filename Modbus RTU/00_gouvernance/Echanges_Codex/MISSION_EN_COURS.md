---
mission_id: TR2-20261012-M015-N-ST-FIFO-XYZ-QUALIFICATION
status: READY
base_ref: main
base_sha: 83f9cb80bc292bd24dda878a8e99b85085f1d664
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-N — Qualification ambitieuse du FIFO ST : vrais XYZ, intégrité, robustesse

## Mandat
L'utilisateur autorise la poursuite autonome avec une mission substantielle. Lire AGENTS.md, le HEAD réel de main, M015-M et ses preuves, PHYSICAL_PROTOCOL.md, SOURCES.json, les sources officielles ST, le datasheet IIS3DWB pertinent déjà référencé et la gouvernance courante. Archiver M015-M mission/rapport sans altérer ses preuves. Vérifier état Git, branches, stashes et cible avant action. Périmètre : **uniquement** experiments/iis3dwb_st_reference, ses tests, preuves et gouvernance. Ne pas modifier le firmware permanent TR2, M014-H, FRAM, SD, Modbus, ni le banc principal.

Acquis à préserver : WHO=0x7B, polling64 XYZ conformes ; HAL SPI O0 CPU16MHz / SPI2MHz déborde après 211/896 octets, O2 sur HAL SPI seul reçoit 896/896 deux fois en ~3,63ms, sans OVR. Les 256 entrées FIFO de M015-M étaient toutes tag4 timestamp, **0 tag2 XYZ**, et la fenêtre512 a été interrompue par la saturation des 1024 traces empêchant aussi le cleanup. Ces 256 entrées restent `accepted=false`. Cause de l'absence XYZ inconnue. Aucun résultat FIFO long/continu validé.

## Phase A — Audit causal indépendant avant nouveau flash
1. Reconstituer octet par octet les transactions et états M015-M : registres FIFO_CTRL1..4, CTRL1_XL, CTRL10_C, FIFO_STATUS1/2, tags et horodatages, séquences reset/enable/stream et timing. Vérifier les définitions officielles ST et le datasheet (sens de FIFO batching, BDR, timestamp batching, tag, FIFO ODR, décimation, compteur modulo). Ne **pas** déduire qu'un tag4 prouve une acquisition XYZ. Revoir les getters ST suspects (niveau tronqué, getter BDR) en distinguant bug démontré, différence de version et mauvaise utilisation. Ne pas corriger silencieusement le vendor.
2. Construire une matrice d'hypothèses testables expliquant `256 timestamps / 0 XYZ` : configuration BDR/ODR, mode FIFO, séquence de configuration, interprétation du flux, capture après FIFO plein, et autres mécanismes effectivement étayés par datasheet. Identifier une expérience discriminante par hypothèse et la lecture registre attendue.
3. Examiner les traces brutes et la chaîne de parsing : bytes SPI, longueur 7, parité, tag, valeurs, timestamps, compteurs ; vérifier que les 256 tags ne résultent pas d'une erreur d'offset, de découpage, d'endianness ou de classification.

## Phase B — Harness industriel réutilisable
4. Réserver un **canal de cleanup indépendant** des traces d'acquisition (ou budget réservé démontré), de sorte que la saturation du journal n'empêche jamais la mise au repos. Préserver le STOP et les erreurs explicites. Introduire des traces compactes/agrégées pour polling de niveau, dimensionner les buffers selon un calcul de capacité pour fenêtre512 et marge cleanup, avec indicateurs de troncature et overflow distincts. Ne jamais augmenter une limite au hasard ni supprimer les gardes.
5. Conserver HAL SPI optimisée O2 seule, les hooks pré-close, contrôle exact des 896 octets et hashes. Ajouter un parseur/analyseur hors ligne indépendant, tests synthétiques positifs/négatifs (tags mixtes, uniquement timestamp, erreurs SPI, compteur wrap, FIFO overflow, fenêtre partielle, cleanup impossible), validation du nombre d'échantillons, des pertes et de la cohérence temporelle sans extrapolation. Préserver sources HAL/driver ST officielles intactes ; toutes variantes d'adaptation explicitement documentées.
6. Exécuter tests unitaires host, intégration mocks, cross-builds, analyse statique ciblée si disponible ; vérifier diff et non-régression de la chaîne WHO/polling. Publier un checkpoint logiciel vérifié avant essais physiques.

## Phase C — Campagne physique progressive, autorisée
7. Banc **secondaire seulement** : NUCLEO-U575ZI-Q STLINK SN `0032001F3235510C37333439`, UID `001B00463842501820333157`. Refuser sans exception TR2 principal SN `003500463235510B37333439` / UID `004A003D3842501820333157`. Vérifier identité à chaque flash, source et SHA BIN/ELF, alimentation/état cible, flash NORMAL SWD adresse 0x08000000, verify + readback, GDB qualifié. Aucune sauvegarde/restauration répétée de Flash 2MiB sur **cette carte jetable**, selon décision utilisateur ; aucun mass erase, Option Bytes, RDP, changement de boot, branchement physique par Codex. Préserver TR2 principal et ses médias.
8. Effectuer d'abord **une campagne de référence bornée** sur configuration officielle ST inchangée, avec STATUS10 comme variante de lecture explicitement identifiée, O2 HAL SPI et nouveau harness sûr. Capturer readbacks avant/après, niveaux, tags bruts, timestamps, XYZ, compteurs, HAL status, état FIFO et cleanup. Objectif : fenêtre512 complète si elle est réellement conforme. STOP si anomalie nouvelle, ne pas maquiller les données ni rejouer pour obtenir un résultat favorable.
9. Si l'absence XYZ est reproduite, autoriser **jusqu'à trois expériences diagnostiques distinctes et justifiées par la matrice A**, chacune changeant **un seul paramètre causal** par rapport au témoin ou à un état de référence documenté, avec configuration effective et valeurs exactes du datasheet ; chaque expérience a sa propre image/profil, identité, trigger, preuves et STOP. Ne pas transformer les réglages modifiés en « démo ST strictement identique ». Éviter tout changement simultané de CPU/SPI/ODR/FS/BDR/mode et ne pas masquer les anomalies getter. Si résultat contradictoire, diagnostiquer hors cible avant de décider du prochain essai ; ne pas multiplier les flashs à l'aveugle.
10. Si des XYZ FIFO valides sont obtenus, effectuer une qualification étendue progressive **seulement si les critères précédents sont verts**, avec fenêtres de 512 puis 4096 échantillons au maximum dans cette mission, sous contraintes de RAM, durée, débit, débordement, compteurs, intégrité et cleanup explicitement dimensionnées et testées avant flash. 4096 est un objectif conditionnel, pas une obligation ni une validation 32768. Ne jamais annoncer continuité/perte zéro si les preuves ne le permettent pas. Ne pas solliciter de manipulation mécanique ni d'excitation du capteur sans demande précise à l'utilisateur.
11. À chaque anomalie matérielle, HAL ou configuration non prévue : conserver traces et arrêter la campagne concernée ; une nouvelle expérience ne peut être tentée que si elle a été explicitement prévue, avec risque maîtrisé et cause/hypothèse discriminante. Une saturation de traces ne doit jamais empêcher un cleanup sûr. Si cleanup logiciel échoue, appliquer procédure de mise au repos qualifiée, documenter sans convertir l'échec en succès.

## Critères et livraison
- Minimum : diagnostic rigoureux de l'absence XYZ et correction durable de la capacité de traces/cleanup, avec tests et preuves.
- Succès FIFO : vrais échantillons XYZ prouvés depuis payloads bruts, tags et configuration ; HAL complète, absence d'erreur, cohérence compteur/temps, cleanup réussi ; distinguer démonstration bornée et continuité longue.
- Si succès : proposer un plan de comparaison contrôlée vers le TR2 principal, **sans l'exécuter** dans cette mission.
- Si échec : `BLOCKED`, classer les hypothèses avec expériences restantes ; ne pas déclarer `DONE` artificiellement.
- Publier sources, tests, binaires/hashes, protocole, matrice d'hypothèses, traces brutes, manifestes, journal/roadmap, rapport `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md`. Commits et push normaux sur main autorisés ; relire à distance fichiers et SHAs. Autonomie maximale, aucune validation humaine répétitive ; escalader uniquement risque matériel réel, geste physique ou décision structurante.
