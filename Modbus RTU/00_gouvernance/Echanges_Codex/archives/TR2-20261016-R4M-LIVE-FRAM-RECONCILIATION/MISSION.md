---
mission_id: TR2-20261016-R4M-LIVE-FRAM-RECONCILIATION
status: READY
base_ref: main
base_sha: 0e09805f4ade74ed04553f04efb7a881e8bfb84b
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-M — Maintenance sûre de la trace STARTED et reprise contrôlée R4

## Situation et objectif
R4 a livré la coopération FRAM/FIFO et 136 tests hôte PASS, mais la campagne physique est BLOCKED : la partition FRAM LIVE contient une transaction STARTED dont l'effet causal est indécidable (recovery 9). C'est un verrou de sûreté voulu par R0, **pas** une preuve que START a eu lieu ou non. Firmware initial M016 restauré, historique et tail FRAM intacts, trace conservée. L'objectif est d'établir une voie sûre, auditable et reproductible pour **réconcilier ou isoler** cette trace et reprendre les essais, sans effacement opportuniste, faux reçu ni modification cachée des formats.

## Références obligatoires
Vérifier HEAD main, AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md (§10 T1000), ETAT_COURANT_TR2, roadmap, R0 PROFIL_LIVE/MIGRATION_FRAM, LIVE_R1/R2/R3A/R3B/R4, dernier rapport BLOCKED et preuves R4, backups et hashes. Archiver intégralement mission et rapport R4 avant remplacement, préserver stashes/worktrees et gels V1. Réévaluer état physique au début, ne jamais supposer qu'il est inchangé depuis le rapport.

## Phase A — Analyse et sauvegarde, sans écriture
1. Relire les preuves et analyser les deux copies du journal STARTED, les racines/générations/CRC, la trace de START, l'état capteur et les limites de preuve. Distinguer absence d'effet prouvée, effet prouvé, et indécidabilité persistante ; aucune reconstruction spéculative.
2. Confirmer l'identité TR2-001/UID/ST-LINK, la géométrie FRAM, le contenu LIVE [122880,245760), historique [0,122880) et tail [245760,262144), et les hashes du rapport. Réaliser une nouvelle sauvegarde **intégrale** Flash 2 Mio et FRAM 262144, double lecture et SHA256 ; vérifier les fichiers réellement récupérables, préserver aussi le snapshot initial R4 et l'état STARTED, avant tout flash/debug/écriture. Si l'accès est absent ou l'état diffère, arrêter l'écriture et documenter.
3. Documenter les possibilités compatibles R0 : (a) réconciliation fondée sur preuve causale authentique, si elle existe ; (b) reprovisionnement explicitement borné **uniquement de la partition LIVE** pour un banc de qualification isolé, avec archive vérifiée, reset/arrêt capteur prouvé et journal historique intact. Aucun « COMPLETED » fictif, aucune modification de phase à la main, aucune réutilisation de run_id/epoch qui pourrait faire rejouer une ancienne commande.

## Phase B — Procédure de maintenance bornée
Créer une procédure/outillage de maintenance traçable, indépendante du chemin de commande normal, qui vérifie UID, géométrie, contenu attendu et SHA, état arrêté, sauvegardes, puis prépare une **nouvelle initialisation LIVE propre** pour essais si la réconciliation causale est impossible. Le reprovisionnement est autorisé **pour la seule partition LIVE du banc TR2-001**, dans les limites suivantes :
- préserver hors cible, avec contrôle SHA et procédure de restauration, les 262144 octets FRAM antérieurs, notamment l'état indéterminé ; ne pas détruire la seule preuve ;
- ne jamais effacer/réécrire l'historique [0,122880) ni le tail [245760,262144), ne pas modifier identité historique, UID, Option Bytes, RDP ou la norme R0 ;
- n'effectuer aucun effacement de masse FRAM/Flash ; toute écriture doit être bornée et vérifiée en lecture retour, avec journal d'adresses et comparaison complète des zones protégées ;
- traiter la réinitialisation comme **nouveau contexte de qualification** : ne pas prétendre résoudre rétroactivement START, ne pas inventer d'effet absent, et garantir que l'ancienne identité transactionnelle ne sera pas réadmise/rejouée ; si l'invariant epoch/run_id ne peut être garanti, rester en analyse et escalader ce point structurant avant écriture ;
- utiliser l'outil le moins invasif compatible avec le format et les barrières ; aucune suppression brute de records sans démonstration de cohérence des racines.
Cette autorisation ciblée de maintenance est une décision de gouvernance pour le banc de test uniquement, **pas** une politique de récupération automatique du produit en exploitation.

## Phase C — Reprise ciblée, seulement après réussite A/B
Sur banc T1000 et conditions de sûreté prouvées, reprendre les tests R4 interrompus START/RENEW/STOP/APPLY, coexistence FRAM écriture/FIFO, priorité STOP, récupération après reboot, timing quanta/fautes SPI et provider temps si disponible. Ne pas requalifier comme PASS un run interrompu ou un probe non représentatif ; ne pas refaire inutilement les campagnes déjà qualifiées sans changement de code. Restaurer à la fin le firmware initial depuis backup avec vérification complète et conserver les snapshots FRAM et traces des essais. En cas de nouveau STARTED indéterminé, stopper et conserver la preuve ; ne pas réinitialiser en boucle pour obtenir un PASS.

## Autorisations et limites
allow_flash: true, allow_debug: true, allow_commit: true, allow_push: true. Codex agit de façon autonome dans AGENTS.md. Flash/debug seulement après identification et sauvegardes. Le HP est retiré ; T1000 seul banc. L'utilisateur est sollicité **uniquement** si connexion/alimentation/manipulation physique nécessaire, si une incertitude matérielle non maîtrisée subsiste, ou si un choix structurant non couvert survient. Ne pas lui déléguer des commandes Git ou de maintenance exécutables par Codex. Aucun essai SD ni RTU électrique sans dispositif approprié.

## Critères d'acceptation et sortie
- Preuves archivées avant maintenance et hashes Flash/FRAM/partitions avant/après ; journal STARTED préservé hors cible, état physique de fin explicite.
- Procédure reproductible, sûre, bornée à LIVE, respect de l'anti-rejeu et absence de faux reçu ; sinon arrêt justifié et proposition d'arbitrage.
- Si maintenance possible, essais R4 ciblés avec résultats et latences physiques réellement mesurées ; sinon BLOCKED sans masquer la cause.
- Tests hôte/ARM/sanitizers pertinents et non-régression V1 si code modifié ; pas de revendication protocole2 ou qualification R6.
- Publication sur main de preuves, rapport DERNIER_RAPPORT.md, roadmap/état ; push normal et relecture distante. ChatGPT contrôlera indépendamment les preuves et les commits.
