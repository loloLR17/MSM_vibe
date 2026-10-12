---
mission_id: TR2-20261012-M015-E-ACQUISITION-DECISION
status: READY
base_ref: main
base_sha: dbfcec4027200c5d5c989da52000166311fa6954
created_at_utc: 2026-10-12T14:26:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-E — Décision technique FIFO / acquisition, tranche courte sans essai physique

## Gouvernance et reprise
Appliquer AGENTS.md, protocole V2-B3, gels V1, roadmap et état courant de main. Vérifier HEAD réel et rapport M015-D BLOCKED, lire les audits et traces M015-A à D, archiver mission et rapport D sans écrasement. Préserver worktrees, stashes, backups, firmware permanent M014-H, FRAM génération12 et microSD désactivée. Autorisations permanentes de commit, push, flash et debug confirmées (true) ; **cette tranche n'a pas besoin de flasher ni de debugger**. Ne pas demander de validations répétitives. Informer brièvement des tranches et résultats, sans interrompre le travail.

## Objectif unique
Fournir un arbitrage technique documenté, exploitable et peu coûteux en tokens sur la voie d'acquisition à retenir pour avancer vers M016, en séparant la recherche de cause du défaut SPI/FIFO de la faisabilité d'une acquisition réelle fiable. Aucune nouvelle campagne matérielle M015-A/B/C/D, aucun faux READY, aucune qualification physique présumée.

## Trois lots bornés
1. **Diagnostic et instrumentation** : à partir des 468 transactions D, comparer commandes et réponses des lectures simples CTRL2 et burst pendant remplissage puis arrêt. Identifier les hypothèses encore compatibles avec les observations, le test wire-vers-RAM discriminant le plus court et ses critères de décision. Rédiger une fiche de mesure logique CS/SCK/MOSI/MISO : instrument nécessaire, niveaux 3,3V, points de raccordement à confirmer, masse de référence, fréquence/cadence et traces attendues. Ne pas présumer qu'un analyseur logique est disponible ni demander de câblage tant que la nécessité et la faisabilité ne sont pas établies.
2. **Voie alternative** : auditer, sans développement matériel, les options de capture avec IIS3DWB et SPI3 réel : lecture directe non-FIFO prouvée en A, FIFO en bursts, fréquence SPI admissible d'après documentation et caractéristiques réelles, DMA/interruptions, contraintes de débit ODR26,667kHz, horodatage, pertes, mémoire et traitement. Faire les calculs chiffrés de bande passante et distinguer un démonstrateur vibratoire à cadence bornée d'une acquisition V1 complète. Ne pas modifier la spécification V1 ni déclarer un flux continu qualifié. Proposer une stratégie minimale qui ne masque pas le défaut.
3. **Décision et publication** : livrer une note de décision concise avec tableau comparatif des options, risques, dépendances et critère d'arrêt ; proposer la prochaine tranche la plus efficace (mesure instrumentée, correctif démontré ou acquisition alternative) sans exécuter d'essai physique. Mettre à jour ETAT_COURANT et roadmap seulement avec des statuts factuels ; M015 reste IN PROGRESS. Publier DERNIER_RAPPORT.md et preuves d'analyse, commits/push normaux et relecture distante. Statut DONE pour M015-E si la décision et le protocole sont vérifiables, sans prétendre résoudre M015.

## Limites strictes
Mission documentaire et analyse de code uniquement : pas de flash, debug cible, capture FIFO, modification firmware de production, test HP, écriture FRAM/SD, DMA implémenté, refactoring, FFT ou calibration. Ne pas répéter les tests inchangés. Aucun force push/reset destructif. Publier tôt pour limiter quota. Toute intervention physique future reste soumise à un protocole distinct ; les décisions techniques ordinaires sont déléguées à ChatGPT/Codex.
