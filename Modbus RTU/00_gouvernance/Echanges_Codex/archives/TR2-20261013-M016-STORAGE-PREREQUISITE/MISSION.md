---
mission_id: TR2-20261013-M016-STORAGE-PREREQUISITE
status: READY
base_ref: main
base_sha: bb299d711d9bd0a76d52eafb201c2670e6cff161
created_at_utc: 2026-10-13T18:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Mission préalable : rendre le stockage V1 réellement disponible

## Décision de pilotage
La mission M016-V1-VIBRATION demeure **BLOCKED**, et n'est pas qualifiée. L'utilisateur retient la **voie A** : conserver intégralement la spécification Modbus RTU V1 gelée et rendre disponible une capacité de stockage de campagne réellement attribuée et qualifiée avant reprise de la publication vibratoire. La présente mission est une tranche préalable distincte ; elle ne clôture pas M016.

## Objectif
Identifier, réaliser et qualifier la solution minimale permettant à l'environnement STM32 de fournir une capacité de stockage de campagne **réellement utilisable**, de valider/reconstituer une ActiveConfiguration autoritative conforme à B4 et d'autoriser le démarrage métier selon les invariants existants. Codex choisit librement l'implémentation proportionnée à l'état réel des composants, sans architecture, système de fichiers, refonte SDMMC ou découpage artificiel imposé.

## Références obligatoires
Depuis le HEAD réel de main : AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, protocole d'échange, roadmap, dette A04, ETAT_COURANT_TR2, spécification V1 B4/B5/B6 et mappings gelés, conception stockage/campaign/recovery, code de la plateforme STM32 et des drivers SDMMC, rapports M015-V/A04-01/A04-02, rapport BLOCKED M016 et preuves `preuves/TR2-20261013-M016/`. Vérifier l'état Git, les travaux existants, l'identité du banc et les contraintes matérielles avant action.

## Périmètre autorisé
- Analyser les ressources physiques et logiques de stockage, leur attribution réelle, les limites de partition et de capacité, les politiques de durabilité/recovery et les prérequis de mise en service.
- Choisir puis implémenter les raccordements logiciels nécessaires pour disposer d'un CampaignDataStore réel et d'une capacité autoritative, en réutilisant prioritairement les composants existants et en préservant la séparation entre stockage campagne, FRAM de configuration et buffers FIFO.
- Vérifier la validation de configuration, le recovery de l'ActiveConfiguration, la cohérence des générations et les refus en cas d'absence, de corruption ou de capacité insuffisante. Ne jamais convertir une configuration candidate inactive en active sans validation effective.
- Si un essai physique est nécessaire et sûr, le réaliser de manière bornée avec flash/debug autorisés, preuves d'identité, de capacité, de lecture/écriture et de reprise, sans dégrader la FIFO/Modbus ni les données existantes.
- Proposer un checkpoint stable et un rapport BLOCKED si une allocation de stockage nécessiterait une décision structurante ou un risque destructif non maîtrisé.

## Invariants et limites impératives
- **V1 inchangée** : storage_mode=0 et storage_limit_mb=0 restent invalides ; pas de capacité fictive, de fallback permissif, de mode supervision hors stockage ni de modification silencieuse du mapping.
- **Données et médias préservés** : aucune repartition, formatage, effacement, écriture dans une zone non explicitement attribuée, altération de FRAM/microSD/identité/Option Bytes/RDP ou écrasement de données préexistantes. Une attribution nouvelle doit être justifiée et non destructive ; si elle ne peut être établie, documenter les options et escalader avant d'écrire.
- Respecter durabilité, idempotence B5, recovery, garanties ActiveConfiguration, architecture M014/M015 et protections A04-01/A04-02. Aucun ACK de succès prématuré.
- Pas de refonte générale, RTOS, changement de driver ST, SDMMC 4-bit imposé ou qualification d'endurance/métrologie non démontrée.
- Le chemin métier FIFO→B3 et les calculs vibratoires restent **hors de cette mission préalable** : les reprendre dans M016 après qualification de la précondition.

## Critères d'acceptation
1. Capacité de stockage exploitable démontrée, physiquement et logiquement attribuée, avec origine et limites documentées ; aucune hypothèse de capacité ne vaut autorité.
2. DataStore réel : comportement lecture/écriture, persistance, erreurs, corruption, saturation et recovery validés selon les exigences V1, avec tests hôte/ARM et essais physiques bornés si sûrs.
3. Validation B4 et recovery de l'ActiveConfiguration sur cette capacité : acceptation conforme lorsque conditions réunies, refus sûr sinon ; cohérence après redémarrage/coupure dans la portée testée.
4. Non-régression des profils sûrs, FRAM, Modbus B0/B1/B5, FIFO et acquis A04-02 ; budgets d'I/O et risques de contention documentés. Distinguer simulation, essais matériels et garanties.
5. Rapport avec commits, SHA, preuves, statut réel, capacité effectivement attribuée, état final du matériel et éventuelles décisions résiduelles. Aucun test affaibli ou preuve inventée.

## Autonomie et garde-fous matériels
`target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true`. Codex analyse, conçoit, implémente, compile, teste, corrige, flashe/debug si nécessaire et sûr, commit/push normalement et publie `DERNIER_RAPPORT.md` avec relecture distante. Pas de validations répétitives. Identifier TR2-001 et l'image avant flash, vérifier la récupérabilité et les sauvegardes pertinentes ; aucune opération destructive inhabituelle, mass erase, changement Option Bytes/RDP ou intervention physique autonome hors périmètre. Préserver stashes/worktrees et l'historique Git. Escalader uniquement décision structurante, risque matériel non maîtrisé ou intervention physique requise.

## Livrables et suite
Rapport et preuves Codex, résultats et limites, éventuellement code/test/qualification physique. ChatGPT vérifiera la cohérence et décidera de la qualification de la précondition, puis préparera la **reprise de M016-V1-VIBRATION** sous référence explicite, sans prétendre que M016 a déjà été achevée.
