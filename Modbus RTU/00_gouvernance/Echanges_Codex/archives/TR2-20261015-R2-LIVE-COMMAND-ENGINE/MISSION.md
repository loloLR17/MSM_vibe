---
mission_id: TR2-20261015-R2-LIVE-COMMAND-ENGINE
status: READY
base_ref: main
base_sha: f56d0a688e520d556965ae78e94d30203d5dab0f
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R2 — Intégration protocole 2, journal epoch et START_LIVE / STOP_LIVE

## Contexte et références
R1 est reçu comme tranche **logicielle** : backend LiveConfiguration et partition FRAM transactionnelle testés, mais non raccordés à la production, non qualifiés physiquement. Lire les versions courantes de AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md (§10 T1000), CONTRAT_R0_EXTENSION_LIVE_SANS_STOCKAGE.md, LIVE_R0/PROFIL_LIVE.md et MIGRATION_FRAM.md, LIVE_R0/mapping_live.csv et tests_live.csv, 05_Firmware/LIVE_R1.md, roadmap, état, mission et rapport R1. Vérifier HEAD main, état des worktrees et preuves avant action. Archiver intégralement mission/rapport R1 avant remplacement ; conserver les archives A-06/R0.

## Objectif
Intégrer **réellement** le moteur B5 versionné/epoch et les commandes 12 RENEW_TRANSACTION_EPOCH, 13 START_LIVE et 14 STOP_LIVE au firmware, en utilisant R1 pour la configuration/identité persistantes, sans rendre la microSD nécessaire au LIVE. Respecter le découpage R2/R3 : ne pas simuler une chaîne de mesure B3 déjà opérationnelle.

## Travaux autorisés
1. Analyser les points d'entrée réels de la composition STM32, registres B0/B1/B4/B5/B6/B7, commande transactionnelle, journal, FRAM et acquisition ; établir le plan minimal d'intégration sans dégrader V1 ni les gels historiques.
2. Implémenter la machine B5 epoch complète du successeur, avec journal format 4, identités et protections anti-rejeu, transitions RESERVED/STARTED/COMPLETED, erreurs aux barrières, refus/idempotence et recovery après coupure. Le code 12 doit respecter V1.1 ; 13/14 suivent les tables R0 et `decisions_start_stop.csv`. Aucune réussite de commande ne doit être publiée avant preuve de l'effet correspondant.
3. Implémenter le cycle LIVE STARTING/LIVE/STOPPING/READY/FAULT, arrêt sûr, exclusion mutuelle avec les campagnes, aucun campaign_id fictif, aucun appel au DataStore SD, aucun auto-START au reboot. Ne pas démarrer réellement le capteur si la couche d'acquisition requise n'est pas disponible : garder les capacités non annoncées et refuser explicitement plutôt que fabriquer un succès. Préparer l'interface réelle du futur R3.
4. Raccorder de façon cohérente B0/B1/B4/B5/B6/B7 à la composition successeur **uniquement lorsque l'ensemble des invariants requis est tenu** ; préserver le protocole 1 historique et ses oracles. Ne pas publier protocole 2 avec capacités ou valeurs B3 mensongères. Définir le comportement déterministe de sélection/commissioning et de rollback sans modifier la FRAM historique.
5. Tester le vrai moteur et les vrais stores sur transport simulé : 384 contextes R0 START/STOP, duplication/epoch/reboot/power-loss, fautes capteur/FRAM, refus APPLY pendant acquisition, absence SD, identité et compatibilité V1.1. Ajouter tests unitaires, intégration et non-régression complets, logs et preuves ; cross-build ARM. Documenter ressources CPU/RAM/pile et volumes I/O sans inventer de marges physiques.
6. Actualiser état et roadmap factuellement, rapporter les fonctions non raccordées ou non qualifiées et préparer R3 (FIFO/indicateurs B3) et R4 (latences).

## Matériel / T1000
Banc unique T1000 ; HP retiré. `allow_flash: true` et `allow_debug: true` autorisent les interventions STM32 conformes à AGENTS.md, mais ne les imposent pas. Avant tout flash/debug/provisioning : vérifier identité réelle de la carte (TR2-001 ou seconde Nucleo), image et sauvegardes ; inventaire ST-LINK et USB–RS-485 du T1000, ports et propriétaires ; protéger les Option Bytes, le câblage et l'alimentation. Avant toute première écriture FRAM LIVE [122880,245760), prouver la propriété exclusive et le contenu réel de cette zone et disposer d'une sauvegarde vérifiée. Propriété inconnue ou risque de perte : ne pas écrire, poursuivre les tests logiciels et consigner la limite. Aucune opération physique utilisateur déléguée à Codex. Aucun essai SDMMC demandé.

## Critères d'acceptation
- Journal epoch et B5 12/13/14 implémentés ou limites d'intégration honnêtement exposées ; aucun faux PASS, faux démarrage ni annonce protocole/capacité prématurée.
- Invariants R0 sur transactions, exclusion campagne/LIVE, reboot sans auto-START, absence de dépendance SD, erreur/rollback et FRAM préservée ; scénarios R0 applicables exécutés avec oracles indépendants.
- Build/tests hôte et ARM réussis, non-régression V1, preuves et hashes conservés ; distinction explicite des essais logiciels et physiques.
- Révision critique des coûts de commits FRAM (~52426 octets écrits, lectures synchrones), disponibilité STOP et marges FIFO/Modbus à mesurer en R4.

## Publication
Développement, commit et push normaux autorisés sur main ; préserver travaux préexistants et éviter force push. Publier DERNIER_RAPPORT.md, preuves, état/roadmap et SHA ; vérifier publication distante. ChatGPT contrôlera indépendamment avant R3.
