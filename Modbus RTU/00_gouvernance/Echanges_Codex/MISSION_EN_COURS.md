---
mission_id: TR2-20261014-P12H3H-SDMMC-DAT-PERMUTATION-REVIEW
status: READY
base_ref: main
base_sha: 240a83a86e0f93e1ef647f945853e396111ba971
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# P12/H3h — Revue contradictoire de l'hypothèse de permutation des lignes SDMMC

## Objectif
Évaluer rigoureusement, **sans aucun nouvel essai matériel**, si une permutation physique des lignes DAT1, DAT2, DAT3 (ou toute autre permutation compatible avec les observations) peut expliquer les anomalies SDMMC historiques. Vérifier notamment la proposition externe selon laquelle une permutation pure conserve le CRC16 par ligne, peut corrompre silencieusement les données et peut être masquée par un aller-retour écriture/relecture sur le même câblage. **Ne pas adopter ces propositions comme acquises** : vérifier leur exactitude, leurs conditions et leurs limites avec la spécification SD, la logique du contrôleur et les preuves existantes.

## Références et historique obligatoires
Lire le HEAD réel de main, AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, ETAT_COURANT_TR2.md, DERNIER_RAPPORT.md et la mission précédente; préserver intégralement cette dernière en archive conforme à la gouvernance avant remplacement (mission et rapport, y compris statut final et contradiction administrative BLOCKED/FAILED, sans réécriture historique). Examiner les missions et rapports des archives M016 SDMMC-REFERENCE-BENCH, REFERENCE-PHYSICAL, 1BIT-LOAD-ONLY, M017 1BIT-REALISTIC-QUALIFICATION, P12H3H SDMMC1 REFERENCE-POC, CABLE10CM-COMPARISON, COLDSTART10CM et ROOTCAUSE-REVIEW, ainsi que les huit dossiers de preuves SDMMC correspondants et FIRSTFAULT. Rechercher expressément le raisonnement antérieur sur les inversions de fils dans l'ensemble des sources pertinentes; le citer s'il existe, signaler honnêtement son absence sinon.

Comparer séparément les bancs **SDMMC2 historique** et **SDMMC1 récent** : leurs pins, montages, code, fréquence et succès/échecs ne sont pas interchangeables. Consulter les schémas ST MB1549/UM2861, la netlist Adafruit 4682 et les références officielles SD/STM32 disponibles. Contrôler le mapping physique CN11/CN12 et la distinction 3V3/GND/5V à partir des documents, sans demander de recâblage.

## Questions à trancher
1. Sur un bus SD 4 bits, expliquer précisément le CRC16 par ligne, le transfert et la réception de l'information, l'effet d'une permutation DAT1–DAT3 sur le CRC matériel, sur les données et sur les états/commandes (dont ACMD13/SSR 64 octets). Préciser les hypothèses nécessaires; vérifier si les séquences de CRC, les start/end bits ou les traitements de contrôleur changent la conclusion.
2. Déterminer par raisonnement formel si une écriture puis relecture via la **même permutation** peut donner un round-trip identique, et dans quels cas cela ne fonctionne pas. Examiner le rôle des données écrites par un hôte externe, de leurs oracles SHA, de la signature 0x55AA, de la lecture 1 bit, des commandes CMD/CLK et des succès ponctuels 4 bits. Éviter les conclusions absolues non justifiées.
3. Confronter cette théorie à chaque **preuve primaire** pertinente, en particulier FIRSTFAULT A PASS 1 bit et B DCRCFAIL sur ACMD13/SSR, M016 4 bits 24/4 MHz et fronts, PoC 4 bits succès intermittents de 128 KiB, et preuves de provenance des données lues. Distinguer incompatibilité logique, indice défavorable et impossibilité de conclure.
4. Établir une matrice des permutations plausibles, leur signature attendue (CRC, corruption, réussite possible), les observations correspondantes et le statut : exclue / non exclue / indéterminée, avec degré de preuve et référence précise. Rechercher tout calcul historique existant pour éviter de refaire une analyse déjà publiée.
5. Identifier **uniquement** les vérifications encore nécessaires si les données existantes ne tranchent pas, en privilégiant l'exploitation hors ligne des captures, buffers, hashes et logs déjà publiés. Aucune répétition d'essais connus ni campagne de nouveaux essais à ce stade.

## Périmètre et sécurité
Analyse documentaire, raisonnement technique et éventuels scripts **hors ligne** sur copies de preuves existantes. Aucune intervention sur STM32 : pas de flash, debug, reset, connexion à une cible, commande SD, modification physique, ni écriture microSD. Ne pas modifier firmware, schémas, gouvernance, spécifications gelées ni archives historiques. Conserver tout travail Git préexistant, pas de force push, merge/rebase/reset destructeur. Publier seulement rapport et éventuelles preuves documentaires nouvelles nécessaires, sur main.

## Critères d'acceptation
- Démonstration technique vérifiable, séparant hypothèses externes, sources normatives, observations et inférences.
- Inventaire des éléments historiques déjà testés, avec citations exactes de fichiers et de traces.
- Conclusion explicite : permutation pure peut-elle expliquer **le premier DCRCFAIL SSR** ? Peut-elle expliquer des succès 4 bits ou masquer une corruption en round-trip ? Avec nuances et incertitudes.
- Aucune expérimentation matérielle ni changement fonctionnel.
- Rapport final dans DERNIER_RAPPORT.md, pièces justificatives proportionnées, commits/push sur main et vérification distante par Codex; statut DONE si revue complète, BLOCKED si sources indispensables inaccessibles. ChatGPT réalisera le contrôle indépendant.

## Autonomie et escalade
Effectuer toute la revue de manière autonome, sans solliciter de validations intermédiaires. Signaler seulement une lacune documentaire bloquante ou une décision structurante. Les autorisations permanentes de développement, commit et push s'appliquent; flash/debug sont explicitement interdits dans cette mission.
