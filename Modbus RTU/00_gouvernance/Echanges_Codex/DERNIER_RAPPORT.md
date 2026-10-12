---
mission_id: TR2-20261012-M015-E-ACQUISITION-DECISION
status: DONE
base_ref: main
base_sha: dbfcec4027200c5d5c989da52000166311fa6954
initial_head: 82d38bf02925d05ce6191ae5e2f38bff4bbe1846
result_sha: null
created_at_utc: 2026-10-12T14:40:26.018428+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-E — Décision et protocole publiés et vérifiés

**DONE pour M015-E documentaire uniquement. M015 reste IN PROGRESS ; B/C/D restent BLOCKED.** Aucun défaut physique n’est déclaré résolu.

## Décision

La [note](../DECISION_ACQUISITION_M015_E.md) conserve le chemin direct A comme démonstrateur RAM borné. La voie V1 proposée est FIFO en bursts de128 mots, puis DMA/service si nécessaire, après preuve de fiabilité et de latence. À625kHz, le débit ne suffit pas : FIFO128+statut exige au minimum1,500019Mbit/s. À2,5MHz candidat, occupationfil60% hors overhead. SPI3 limité impose des transactions sous sa borne constructeur1024 ;128mots donnent897octets. Aucune vitesse supérieure ou acquisition continue n’est qualifiée.

La [fiche de mesure](../MESURE_LOGIQUE_M015_E.md) définit une comparaison fil/RAM88/87/BA pendant alimentation puis étatfigé : instrument4entrées compatibles3,3V, points versionnés à confirmer, masse logique, cadence/profondeur/trigger et critères de décision. L’instrument n’est pas supposé disponible ; aucun câblage demandé. La résolution de décodage ne certifie pas les timings5/20ns.

V1 conserve26667Hz, fenêtres4096..32768, campagne1, stockage store-and-forward et capacité owned. M014 conserve son état historique configurationUNAVAILABLE/READY0/stockage0. Pas de backend fictif ou de remplacement silencieux des lectures fausses par des bursts.

## Preuves et vérification

Les468records D reconstruisent exactement37440octets du dumpRAM conservé. CTRL2 est écrit seulement aux indices15=81 et453=00 :26/28 divergences single/burst actives et0/32 figées restent établies. Cela exclut un write applicatif durant comparaison, sans prouver la vraie valeur sur le fil ni isoler la cause physique. Code cible historiqueD :ca00da976de487ff6fd52fbd3973af105f07876a ; aucun code cible évalué enE (`result_sha: null`).

Inspection des audits et CSV A-D, code du driver/acquisition/clock/ISR/linker, spécification bloc4§8, documentation ST et calculs débit/mémoire. Lecture hors cible de l’ELF permanent par `arm-none-eabi-size` :bss62912/data24. Analyse et calculs reproductibles dans `preuves/TR2-20261012-M015-E/`, entrées hachées dans `inputs.json` ; preuve locale complète : **/home/lolo/dev/msm/reports/TR2-mission015-E-20261012T143539Z**.

Validation proportionnée : relecture documentaire, diff, hachages, reconstruction brute et calculs. Aucun test inchangé, build, cross-build ou accès matériel : cette mission ne change aucun firmware. Recherche initiale de chemins absents corrigée par inventaire du dépôt ; aucun fichier supposé créé.

## Préservation et publication

HEAD initial **82d38bf02925d05ce6191ae5e2f38bff4bbe1846**, base contratdbfcec4. WorktreeE isolé/codex/m015-e, push normal explicitement versmain. Mission active inchangée ; archivesD exactes depuisdbfcec4. Original82169af/H762c459/Aeab07d4/Bcfb9918/C37b97f7/Ddbfcec4 et deux stashes préservés, sans merge/rebase/reset/forcepush.

Permissions exercées : modifications documentaires, commit et push. Les permissionsflash/debug=true ne sont pas exercées car le contrat interdit ces opérations dans cette tranche. Aucun GDB, flash, reset, HP/COM7, FRAM/SD ou branchement. M014 BIN SHA b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611, FRAMgen12 etSDOFF restent les dernières observationsD, pas un nouveau relevéE.

Publication au **10bcdeaf5e492dd5b87535ca8d536b42218dfc5f**, commit `M015-E document acquisition choice, SPI3 limits and wire-to-RAM protocol` : **11livrables relus intégralement depuis la référence récupérée de GitHub, UTF8 et byte-identiques**. Preuve `publication-readback-10bcdea.json`. Journal et roadmap actualisés pour décision préparée/vérifiée et limites ; aucun statut de qualification physique relevé. La mise à jourDONE est contrôlée après push ; son SHA final est donné dans la réponse Codex, sans hash autoréférent ici.

## Suite à ChatGPT

Prochain contrat court recommandé : vérifier la disponibilité d’un instrument adapté, puis une mesure discriminante fil/RAM à625kHz sans rejouer les campagnes A-D. Correctif seulement après défaut démontré ; sinon conserver le blocage. Après lectures fiables, qualifier un seul profilRAM FIFO128 à une fréquence choisie ; DMA/serviceV1 et stockage sont des lots suivants distincts. En attente d’instrument, travailler horsligne sur les donnéesA et les contrats, sans prétendre valider un flux ou avancer READY.
