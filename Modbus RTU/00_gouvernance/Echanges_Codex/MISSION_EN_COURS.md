---
mission_id: TR2-20261011-M014-A-SUPERVISION-DIAGNOSTIC
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# M014-A — Supervision .NET : chemin diagnostic hors port sur HP

Lire AGENTS.md, protocole V2, état courant, rapport M013-J, proposition de clôture M013/M014, architecture et gels de Supervision TR2 sur main. Vérifier HEAD main, propreté, préserver stashes et preuves.

Contexte confirmé : HP Laptop 15-db1xxx = unique maître Modbus, convertisseur USB-RS485 COM7, 115200 8E1, cible TR2 diagnostic unité1. Le Dell Latitude 7320 T1000 est poste de développement Codex/GitHub/compilation ; dock USB T1000 attendu dans deux jours, non bloquant. Campagne physique M013-I unique terminée avec 12/12 cas conformes, corroborée par 17 deltas STM32 M013-J. Ne jamais relancer cette campagne.

Objectif : préparer et implémenter une voie explicitement DIAGNOSTIC, sûre et isolée, pour interroger FC03 registre1000 quantité1 et lire UNKNOWN=0 depuis la supervision .NET existante. Réutiliser au maximum transport/factory/configuration/test infrastructure actuels sans altérer les gels ni les contrats V1.

1. Auditer les composants .NET réels (transport NModbus, session factory, B0/B1 readers, runtime, configuration, CLI/IHM, tests) et le déploiement HP Windows. Documenter l'écart connu : B1Reader standard lit 20 registres, la cible diagnostic n'expose que 1000/1 ; ne jamais présenter la cible comme READY/production ni inventer DeviceId.
2. Implémenter la plus petite tranche testable hors COM7 : chemin diagnostic opt-in isolé du runtime de production, sans polling automatique, B5, écriture, découverte ni accès concurrent. Réponse attendue unité1 FC03 1000/1 UNKNOWN0 ; paramètres port/baud/parité explicitement configurables, pas de trafic au démarrage par défaut, timeout borné et retry=0.
3. Ajouter tests hors matériel : réponse exacte, mauvaise taille/CRC/exception/timeout/port occupé, non-régression et invariants de non-écriture/absence de trafic sans activation. Si certaines responsabilités CRC sont déléguées à NModbus, tester aux couches appropriées et l'indiquer.
4. Compiler/tester .NET et tests ciblés sous T1000. Préparer procédure d'installation/exécution HP sans la lancer : préflight .NET/COM7 non intrusif, mode diagnostic explicitement activé pour futur essai unique sous mission séparée. Aucune transmission physique dans M014-A.
5. Mettre à jour documentation, DERNIER_RAPPORT.md et ETAT_COURANT_TR2.md, commit/push normal main, puis relecture distante. Signaler clairement ce qui reste à tester sur HP. Préserver freezes et baseline V1 ; aucune modification firmware.

Autonomie complète de développement logiciel et Git selon AGENTS.md ; aucune demande de validation répétitive. Ne pas attendre le dock T1000. Si une incompatibilité majeure impose une décision structurante, documenter options et arrêter seulement cette partie.
