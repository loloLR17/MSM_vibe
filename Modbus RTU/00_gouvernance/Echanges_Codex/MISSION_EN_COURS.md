---
mission_id: TR2-20261012-M015-G-CONFIG-READBACK
status: READY
base_ref: main
base_sha: e11088772987a50ce729eb4ca1148d657b2cc673
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-G — Localiser le refus de configuration puis qualifier la voie FIFO bornée

Respecter AGENTS.md, gouvernance V2-B3, gels V1, état courant et roadmap sur main. Vérifier HEAD réel et rapport M015-F BLOCKED. Archiver la mission F et son rapport sans écrasement, préserver worktrees, stashes, sauvegardes, FRAM génération12, microSD OFF et image permanente M014-H. L'utilisateur dispose d'un multimètre, pas d'oscilloscope. Ne pas exiger d'instrument externe. Développement/commit/push/flash/debug autorisés avec garde-fous matériels, sans validations répétitives.

## Objectif
Identifier sans ambiguïté le premier écart de configuration qui a empêché M015-F de démarrer (stage10/result9/loss_flags4, zéro poll/burst/XYZ), puis, seulement si les préconditions sont satisfaites, effectuer une unique acquisition FIFO RAM bornée et mesurable. Ne pas rechercher exhaustivement la cause des anomalies M015-B/C/D et ne pas affaiblir les gardes. Aucune réduction implicite des exigences V1.

## Étapes
1. Audit ciblé du code F et de ses traces : trouver exactement le chemin de contrôle et la perte de télémétrie. Instrumenter la vérification de configuration pour conserver AVANT restauration, même en échec, registre/adresse, valeur attendue, valeur lue, méthode de lecture, TX/RX bruts, HAL status/error, état SPI/CS et horodatage. Marquer explicitement les champs non renseignés, sans confondre zéro mémoire et zéro mesuré. Capturer également les valeurs groupées 07..0A et la vitesse effective. Conserver le comportement fail-closed, aucun bypass ni substitution de valeurs.
2. Tests hôte ciblés de l'instrumentation, cas nominal et erreur sur chaque étape critique, plus cross-build ARM défaut/profil. Préparer un diagnostic exclusif, réversible, borné, une activation unique. Un seul essai physique court si preflight satisfaisant. Sur refus de configuration, arrêter immédiatement et publier la cause observable, sans deuxième tentative ni variante. Si le contrôle passe, autoriser dans ce même essai la FIFO figée et deux bursts de128 mots au plus, avec contrôle des tags, niveaux, intégrité, timing, overflow, données XYZ et refus de tout lot ambigu. Ne pas déclarer débit continu V1 à partir d'une capture finie.
3. Avant flash : UID/cible/image et hashes, sauvegarde Flash2Mio et données pertinentes, méthode NORMAL qualifiée, jamais HOTPLUG CubeProgrammer, mass erase, Option Bytes ni RDP. Restaurer intégralement M014-H byte-identique et contrôler la reprise minimale B0/B1 sur HP, sans GDB pendant trafic. Aucune écriture FRAM ou microSD ni accès partition LBA65536. Arrêt immédiat en cas de risque non maîtrisé.
4. Publier traces et analyse du premier écart, statut des bursts et pertes, temps et calculs de faisabilité, ETAT_COURANT, roadmap, DERNIER_RAPPORT.md et preuves ; commits/push normaux avec vérification distante. M015-G DONE si le refus F est caractérisé de façon exploitable ou si une capture bornée fiable est prouvée ; sinon BLOCKED documenté. M015 globale IN PROGRESS.

## Arbitrage V1
Préserver ODR26667Hz, fenêtres4096..32768, continuité et stockage store-and-forward, mapping et configuration validator gelés. Si une adaptation semble nécessaire, chiffrer conséquences sur bande fréquentielle, résolution spectrale, pertes, latence, mémoire et fonctionnalités TR2, et soumettre la décision à l'utilisateur sans la mettre en oeuvre.

Exclusions : DMA, acquisition continue, production, FFT, calibration, B5, Modbus vibrations, Web, écriture FRAM/SD, répétition des campagnes A-F, instrumentation électrique externe présumée. Préserver les acquis et limiter les dépenses de quota.
