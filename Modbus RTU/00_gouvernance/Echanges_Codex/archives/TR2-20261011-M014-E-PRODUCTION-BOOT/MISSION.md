---
mission_id: TR2-20261011-M014-E-PRODUCTION-BOOT
status: READY
base_ref: main
base_sha: a3bd116511f985a92a9abf8b64479b186f34625e
created_at_utc: 2026-10-11T21:14:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# M014-E — Gouvernance SSH-HP et socle de production STM32

## Références et préalable
Partir de l'état distant réel de main ; vérifier HEAD, branche, git status, stashes, AGENTS.md, protocole V2-B3, roadmap, état courant, rapport SSH-HP-001 DONE, rapport M014-D archivé et INTEGRATION_SUPERVISION_M014_D.md (dont décisions d'architecture approuvées). Ne jamais utiliser d'ancien ZIP comme référence source. Préserver les acquis et les gels V1. Un seul pilote sur main ; vérifier que SSH-HP-001 est terminé et publié avant toute modification.

## Objectif
Livrer en une tranche autonome la gouvernance opérationnelle T1000→SSH→HP et faire avancer au maximum le vrai firmware STM32 de production par le provisioning explicite, la composition de ses autorités durables et les tests fail-safe, sans prétendre qualifier physiquement M014.

## Lot 1 — Documentation SSH-HP
Inspecter les scripts réels dans 00_Environnement_developpement/T1000/ssh-hp/ et les rapports. Créer ou compléter la documentation de gouvernance appropriée sur main (éviter doublons) : topologie Windows T1000/WSL Debian, Codex CLI, client SSH Windows et alias tr2-hp, HP Windows/PowerShell, contrôle hôte et clé, privilèges observés et risques, artefacts/hashes, logs, modes OFFLINE, verrou coopératif et limites d'exclusivité COM7, absence de retry, timeout/état distant inconnu, sécurité, retrait/réversibilité. Énoncer explicitement qu'aucune transmission physique n'a été qualifiée par SSH-HP-001. Ne pas publier secrets, IP privée si non nécessaire, clés ou données sensibles. Revue documentaire et vérification distante.

## Lot 2 — Audit normatif et conception de production
Inspecter les spécifications gelées B0/B1, mappings, politiques d'adressage et de version, code SystemRuntime/ProductionApplication, stm32_production_binding_acquire, FRAM geometry_validate/recover, stockage bulk microSD et leurs tests. Déterminer précisément les interfaces/contraintes réelles avant d'écrire. Documenter une matrice de dépendances et une proposition de provisioning compatible V1 :
- identité matérielle STM32 entière, immuable et interne ; identité fonctionnelle unique attribuée explicitement et persistante ; adresse Modbus configurable et distincte ; compatibilité exacte DeviceId/B0 à démontrer sans troncature UID ni nouveau registre ;
- états FRAM ABSENT/INACCESSIBLE, EMPTY, VALID, RECOVERABLE, CORRUPT/UNRECOVERABLE, sans autoformatage ni faux READY ;
- provisioning séparé du boot, transactionnel, idempotent, contrôle identité/version, relecture et recovery ;
- propriété exclusive des allocations FRAM/SD et des écritures harness vs production ; pas de backend fictif ni d'utilisation de microSD comme FRAM ;
- démarrage dégradé sûr, diagnostic lecture seule uniquement si réellement possible et conforme au gel.

## Lot 3 — Développement logiciel autonome
Implémenter les éléments de provisioning/boot et de composition carte qui sont justifiés par les autorités réelles disponibles et les spécifications ; privilégier le moteur et les tests hôte/émulation pour ce qui ne peut être qualifié sur média réel. Couvrir par tests absence de média, EMPTY, VALID, corruption, recovery, panne en cours de transaction, idempotence, mismatch d'identité, version incompatible, adresse invalide, aucune écriture implicite, disponibilité des blocs B0/B1 conditionnée à la vraie source. Conserver le chemin diagnostic existant restaurable ; ne jamais contourner stm32_production_binding_acquire avec fixtures, DeviceId arbitraire ou READY artificiel. Réutiliser les services existants. Faire les compilations ARM et validations firmware/.NET proportionnées et rapporter les sorties réelles. Poursuivre les sous-tâches indépendantes même en cas de blocage matériel.

## Interdictions et escalade
Aucun flash, GDB, reset cible, écriture sur FRAM ou SD physiques, aucune ouverture COM7, émission Modbus ni commande distante HP susceptible de transmettre ; tests SSH OFFLINE non destructifs autorisés si utiles. Pas de mass erase, Option Bytes, changement câblage, formatage, provisionnement implicite, altération des gels V1, force push, reset hard, effacement de données, ou perte des stashes. La permission générale flash/debug d'AGENTS ne s'applique pas : allow_flash/debug false pour cette mission. Si la géométrie physique, l'ownership, la spécification ou le provisioning imposent une décision structurante non couverte par la gouvernance approuvée, documenter les options et le risque sans inventer de valeur ; implémenter le reste. Ne pas demander validations répétitives sur opérations logicielles autorisées.

## Critères et livrables
Documentation SSH-HP publiée et vérifiée ; revue indépendante de M014-D et matrice des autorités/dépendances ; code et tests logiciels réels publiés sur main, validations sans affaiblissement ; rapport honnête distinguant logiciel, simulation, OFFLINE, et qualification physique. M014 reste IN PROGRESS tant que B0/B1 et polling/perte-reprise physiques non démontrés. Actualiser ETAT_COURANT_TR2.md et DERNIER_RAPPORT.md (Codex seul auteur) avec SHAs, résultats, preuves, risques et plan concret de qualification suivant. Vérifier push et relecture distante ; préserver les preuves hors Git. En cas de quota/blocage, publier un point de reprise BLOCKED précis.
