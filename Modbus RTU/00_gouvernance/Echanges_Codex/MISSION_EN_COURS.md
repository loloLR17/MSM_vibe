---
mission_id: TR2-20261010-DEV-RS485-001
status: READY
base_ref: main
base_sha: 5a1b1a89070f73feebbe513bde1ec0d18ca2a52c
created_at_utc: 2026-10-10T14:15:00Z
author: ChatGPT
target_branch: main
allow_commit: false
allow_push: false
allow_flash: false
allow_debug: false
---

# TR2 — Reprise effective du développement : liaison RS-485 / Modbus RTU

## Objectif
**Développer, pas auditer.** Sur le T1000, reprendre l'intégration du firmware STM32 et de la liaison RS-485/Modbus RTU au niveau réellement atteint, en produisant une avancée logicielle concrète et testable. Délégation de l'inspection ciblée, du choix technique conforme à l'architecture gelée, de l'implémentation et des corrections à Codex.

## Contexte
Dépôt local : `/home/lolo/dev/msm/projects/MSM_vibe`. Référence : `main` et son HEAD réel (le SHA ci-dessus est un repère, pas une présomption). Lire `AGENTS.md`, `Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md`, les spécifications gelées et les sources/tests directement concernés. La qualification T1000 est acquise, **pas** la qualification fonctionnelle TR2. Dette E4 microSD/power-loss ouverte, à conserver et non traiter par opportunisme dans cette mission.

## Exécution
1. Contrôler brièvement branche, HEAD et état Git ; préserver toute modification préexistante. Lire le code STM32/UART/RS-485/Modbus et ses tests ainsi que les décisions documentées correspondantes. **Pas d'audit général, pas de campagne de requalification du poste.**
2. Déterminer la **prochaine lacune d'implémentation concrète** de la chaîne RS-485/Modbus RTU en tenant compte des diagnostics/harness déjà présents. Ne pas réécrire ce qui fonctionne, ne pas inventer de pinout ni de comportement matériel.
3. Implémenter la tranche logicielle utile la plus proche de l'exécution réelle (transport, orchestration, réception/émission, tests associés, selon ce que le code démontre). Corriger de façon autonome les problèmes de build/test dans ce périmètre. Respecter strictement le mapping et les spécifications V1 gelées.
4. Valider de façon **proportionnée** par les builds/tests ciblés appropriés. Les validations T1000 antérieures restent acquises ; ne pas les répéter sans raison liée aux modifications.
5. Mettre à jour `ETAT_COURANT_TR2.md` uniquement si un progrès technique significatif le justifie. Distinguer implémenté, testé logiciellement et non validé physiquement. Conserver explicitement la dette E4.

## Limites
Aucune opération physique, flash, debug, changement de câblage, microSD ou paramètre permanent du T1000. Pas de refonte architecturale, modification de spécification gelée ni refactoring annexe. **Pas de commit ni push** dans cette mission : les permissions correspondantes n'ont pas été accordées explicitement ; préparer les changements dans le dépôt local sans écraser les travaux préexistants. Ne pas bloquer sur une vérification déjà qualifiée ; signaler uniquement une vraie décision ou une dépendance matérielle indispensable.

## Livrable
Répondre directement dans la session Codex avec : fichiers modifiés, fonctionnalité effectivement développée, résultats de tests exécutés, état Git, limite éventuelle nécessitant l'adaptateur USB-RS485 ou une décision utilisateur, et prochaine action précise. Si le périmètre matériel empêche toute implémentation utile, l'expliquer avec les éléments du code, sans lancer un audit de substitution.
