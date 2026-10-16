---
mission_id: TR2-20261016-R4S-SERVICE-BUDGET-SAFE-RECOVERY
status: READY
base_ref: main
base_sha: a57cc5bcc84e593a4a99f4100600346b6d990eb4
created_at_utc: 2026-10-16T21:20:16.702Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-S — Qualification du service LIVE et reprise transactionnelle sûre

## Objet et référence

Succède à R4-R `BLOCKED` (rapport archivé sous `archives/TR2-20261016-R4R-PHYSICAL-TIME-B3/`). R4-R a observé 52 894 XYZ/6 612 TS sur 2 s et mis au point un provider temporel ancré sur les tags réels ; l'essai R2/FIFO/B3/RTU s'est arrêté après 118 XYZ, service 2 715 µs pour une borne inchangée de 2 500 µs. Aucune fenêtre B3 valide physique. Journal LIVE final **STARTED indéterminé epoch 4 / tx 5 / run 5**, FRAM intégrale SHA-256 `14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9`. Flash M016 intégrale restaurée SHA-256 `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26` ; cible TR2-001, ST-LINK `003500463235510B37333439`, UID `004A003D/38425018/20333157`. La candidate logicielle `83d4fbd529d0311013889de81145b612701600bd` est compilée/testée mais **jamais essayée physiquement**.

**Objectif :** caractériser puis corriger, si nécessaire, le dépassement de service sur STM32 réel, sans relâcher les seuils ; reprendre de manière causale et traçable l'autorité R2 sans inventer la terminaison du STARTED5 ; démontrer ensuite au moins une fenêtre B3 LIVE réelle et valide en charge si les préconditions sont satisfaites. Priorité à la correction minimale, réutilisation R4-M/R4-D/R4-P, et préservation intégrale des gels V1 et du profil normatif LIVE R0.

## Préflight et phases strictement ordonnées

0. Lire depuis le `main` courant AGENTS.md, gouvernance ChatGPT/Codex, PROTOCOLE_ECHANGE_V2, mission/rapport archivés R4-R, journal, roadmap, norme LIVE R0, LIVE_R3B/R4M/R4D/R4P/R4Q/R4R, preuves et méthodes qualifiées. Vérifier HEAD, environnement T1000 et Git local sans écraser worktrees/stashes. Démontrer préflight cible/Flash/FRAM/capteur, authenticité et récupérabilité des sauvegardes ; si SHA/identité divergent, diagnostiquer sans appliquer les scripts R4-R obsolètes. Archiver le contrat et rapport R4-R déjà archivés sans doublon ni réécriture.

1. **Diagnostic volatil, sans nouvelle admission ni écriture R2/FRAM.** Examiner la candidate `83d4fbd` et les mesures brutes du service fautif ; isoler par instrumentation proportionnée drainage FIFO, datation, métrologie, copie/publication B3, quanta FRAM simulés/readonly et poll RTU. Vérifier qu'aucun nouveau profil ne contourne le budget. Tester sur hôte/oracles/ARM puis flasher, si sûr, un **probe NON-PRODUCTION** identifié et hashé sur TR2-001 avec garde FRAM exacte `14fb...`, accès FRAM strictement readonly. Mesurer réellement work ≤ 1 000 µs, service ≤ 2 500 µs, gap FIFO continu ≤ 5 000 µs, nombre de XYZ/TS, FIFO max, pertes/overrun, coût B3/RTU et marges sous charge représentative. La candidate -O2 n'est pas présumée suffisante. Corriger uniquement une cause démontrée et répéter le test volatil jusqu'à satisfaction ou blocage documenté. Aucun START R2, aucun renouvellement epoch et aucune mutation du journal dans cette phase.

2. **Traitement contrôlé du STARTED5 : étape conditionnelle distincte.** Auditer le snapshot physique complet et la causalité de l'effet, INTENT, root/journal, floors run/epoch et codes du vrai moteur R2. Réutiliser **seulement si prouvée applicable** la procédure de maintenance/retraite R4-M/R4-P : préconditions strictes sur snapshot et identité, contrôle readonly et contre-tests sur copie authentique avec coupures/anti-rejeu, isolation bornée exacte si indispensable, reprise par transitions R2 légitimes, aucun COMMITTED/COMPLETED fictif, aucun reset des compteurs, aucune réutilisation du run5 ou de la clé 4/5. Documenter précisément toute écriture autorisée, état avant/après et preuve de récupération ; ne jamais exposer un pont isolé à un firmware LIVE générique. Si le traitement requiert une nouvelle politique transactionnelle, un effacement arbitraire, un risque matériel non maîtrisé, ou si la causalité reste indécidable sans procédure sûre déjà démontrée, **arrêter à BLOCKED et soumettre la décision structurante à ChatGPT/utilisateur**, en conservant le snapshot intact. Aucun START de production/essai avant résolution démontrée du verrou.

3. **Reprise physique bornée si phases 1–2 PASS.** Sur nouvelle image/procédure qualifiée, vraie acquisition FIFO IIS3DWB et provider des timestamps, R3-B et B3 via RAM/Modbus RTU loopback, R2/FRAM coopératif dans les états autorisés ; vérifier au moins une fenêtre complète **N=4096** et RMS/peak/qualité/âge/UTC absente, contexte run/config/génération, cohérence par ADU, états INVALID/FRESH/HELD et STOP seulement dans la mesure effectivement exercée. Étendre raisonnablement à N supérieurs et durée continue si marge observée suffisante ; ne pas promettre un résultat non mesuré. Mesurer les mêmes budgets sur la charge réellement composée, y compris STOP nominal/sécurité et erreurs observables. **Aucune déclaration de liaison UART/RS-485 électrique**, R5/R6, dérive étalon SI, rollover physique ou endurance sans essais dédiés. Les interruptions GDB durant capture ne valent pas échec capteur sans analyse.

4. Après chaque essai, état sûr et contrôlé : restauration de la Flash M016 initiale si la procédure de banc l'exige, avec relecture intégrale hashée, FRAM vérifiée intégralement sans rollback des identités consommées, partition historique et tail préservées ; sauvegardes originales conservées. Aucune SD, aucun HP, aucune manipulation de câblage, mass erase, Option Bytes/RDP, changement de boot ou intervention physique non autorisée. Stop sûr dès identité inattendue, perte d'autorité, dépassement d'un budget, incohérence transactionnelle ou écriture hors domaine.

## Acceptation, preuve et publication

Livrer séparément : (A) mesures physiques reproductibles et attribution du budget de service avec seuils inchangés ; (B) décision motivée et preuve de l'état transactionnel final / traitement exact du STARTED5 ; (C) si réalisable, B3 physique valide sur fenêtre complète avec oracles numériques et cohérence Modbus ; (D) images exactes BIN/ELF/toolchain, tests/sanitizers/non-régression V1/LIVE proportionnés, logs bruts, audit FRAM/Flash avant/après et restauration ; (E) documentation et mise à jour factuelle d'ETAT_COURANT_TR2/roadmap, sans modifier la gouvernance ni les normes gelées. Aucune suppression/affaiblissement de tests et aucune refonte générale.

Codex est autonome pour diagnostiquer, implémenter, compiler, tester, committer/pusher sur `main`, flasher et déboguer sur TR2-001 **dans ce contrat et les garde-fous d'AGENTS.md**. Séparer rigoureusement logiciel, cible instrumentée et preuve physique. Une phase partielle publiable peut donner un checkpoint `IN_PROGRESS` sous le même ID ; `DONE` uniquement si les critères applicables sont démontrés et relus à distance, sinon `BLOCKED` ou `FAILED` justifié, sans masquer les acquis. Codex publie `DERNIER_RAPPORT.md` et ses preuves, puis vérifie `main` ; ChatGPT contrôle indépendamment après retour utilisateur. Ne pas demander de validations humaines répétitives pour les opérations déjà autorisées.
