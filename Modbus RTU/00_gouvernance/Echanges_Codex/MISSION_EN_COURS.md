---
mission_id: TR2-20261015-R1-LIVE-FRAM-CONFIGURATION
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R1 — Implémenter configuration LIVE, persistance FRAM et recovery

## Décision et références
R0 est reçu pour entrée en R1 après contrôle indépendant documentaire de ChatGPT ; aucune validation firmware ou physique LIVE n'est acquise. Références impératives : AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md §10 (HP retraité, banc unique T1000), CONTRAT_R0_EXTENSION_LIVE_SANS_STOCKAGE.md, PROFIL_LIVE.md, MIGRATION_FRAM.md, mapping_live.csv, tests_live.csv, état/roadmap et dernier rapport R0. Vérifier HEAD main et état Git, préserver worktrees/stashes historiques. Archiver mission et rapport R0 sans perte avant de publier le nouveau rapport.

## Objectif R1
Implémenter de manière incrémentale et testable le **profil de configuration LIVE protocole 2** et son autorité persistante indépendante de la microSD, sans encore réaliser START_LIVE/STOP_LIVE ni le calcul B3 (R2/R3). Préserver le fonctionnement historique V1/V1.1 et les données FRAM anciennes.

## Travail autorisé
1. Examiner la composition firmware, le layout et les stores FRAM réels ; implémenter validation des tuples B4 LIVE, version/capabilités B0/B4, CRC/miroirs, identité et activation transactionnelle, refus et diagnostics nécessaires au seul périmètre R1. Ne pas annoncer une capacité opérationnelle qui n'est pas implémentée.
2. Implémenter une partition LIVE physiquement disjointe et une récupération déterministe selon MIGRATION_FRAM.md ; préserver les banques historiques et la provenance. Aucune conversion silencieuse de configuration campagne, aucun auto-START. Gestion explicite des corruptions, états UNAVAILABLE, conflits de générations, interruptions et rollback compatible.
3. Préparer les interfaces minimales pour l'epoch B5 et la future commande LIVE, mais **ne pas simuler comme implémentées** les commandes 12/13/14 ni le moteur R2. Les opérations de migration ne doivent pas exécuter un effet B5 historique inconnu.
4. Ajouter des tests hôte/ARM ciblés sur vrais stores, vecteurs CRC, migration, atomicité aux points de coupure, identités, erreurs FRAM, compatibilité V1/V1.1, et non-régression complète selon impact. Conserver les sorties, SHA, preuves et limites ; contrôler la charge RAM/pile et le coût de transaction sans annoncer R4 qualifié.
5. Mettre à jour l'état et la roadmap factuellement, sans altérer gels V1 ni réécrire les oracles historiques. Conserver le nouveau corpus R0.

## Banc T1000 et interventions STM32
Le HP Brouette est retiré. Le banc physique cible est **T1000** : ST-LINK + USB–RS-485 + supervision locale, ports à inventorier avant usage. Autorisations permanentes explicites ici : allow_flash=true, allow_debug=true, sous AGENTS.md. **Aucun flash, debug ou écriture FRAM n'est obligatoire pour clore la partie logicielle R1.** Si l'accès matériel est utile et sûr : vérifier identité cible (TR2-001 versus seconde Nucleo), firmware/image et empreintes, état des cartes et câblage, sauvegardes vérifiées et autorité réelle de la zone FRAM [122880,245760) AVANT la première écriture. Zone inconnue ou risque de perte : arrêter cette seule opération, ne jamais formater, effacer massivement, changer Option Bytes/RDP ni écraser des données ; poursuivre les essais hôte/ARM et rapporter la limite. L'utilisateur reste responsable des manipulations physiques. Aucun accès SDMMC ni campagne de stockage demandé.

## Critères d'acceptation
- Profil LIVE B4 conforme R0 et CRC V1/LIVE vérifiés ; anciennes configurations conservées et interprétées selon leur version.
- Autorité FRAM et recovery cohérents sous corruption/coupures simulées et vraies implémentations, avec préservation vérifiable de la partition historique ; aucune écriture sur zone de propriété inconnue.
- Aucun auto-START, aucune campagne créée et aucune fausse capacité B5 LIVE publiée.
- Tests ciblés et régressions pertinents exécutés avec logs, SHA et limites ; preuve physique distincte si effectivement réalisée.
- Si l'epoch/identité requiert une intégration plus large pour la cohérence, décrire et implémenter la plus petite tranche sûre, ou signaler le blocage réel ; ne pas livrer un profil prétendument complet avec commandes absentes.

## Publication
Autonomie pour analyse, code, tests, commits et push sur main conformément à AGENTS.md. Publier DERNIER_RAPPORT.md avec commits, preuves, état matériel, écarts et recommandation R2 ; vérification distante de publication. ChatGPT réalise ensuite le contrôle indépendant, sans tag ni qualification automatique.
