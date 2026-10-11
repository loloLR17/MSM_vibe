---
mission_id: TR2-20261011-M014-G-COMMISSIONING
status: READY
base_ref: main
base_sha: 7f9e57b87527a0864a6e4b0eae468b6c940a95c7
created_at_utc: 2026-10-11T22:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-G — Mise en service du TR2 n°1 et qualification M014

## Références et autonomie
Appliquer AGENTS.md, protocole V2-B3, roadmap, ETAT_COURANT, rapport M014-F et spécification IDENTITE_PERSISTANTE_M014_F.md sur HEAD réel main. Vérifier branche, HEAD, stashes, état Git et archives ; archiver mission et rapport F conformément au protocole sans écrasement. Travailler en autonomie maximale, commit/push/flash/debug permis, sans demandes de validations répétitives. Objectif prioritaire : clôturer M014 dès que les critères réels sont démontrés ; sinon documenter uniquement les blocages concrets et poursuivre tous travaux indépendants.

## Attribution décidée
Premier appareil : DeviceId uint32 **1**, adresse Modbus RTU **1**, serial ASCII **TR2-000001**, fabricant ASCII **MSM**, nom interne **MSM-TR2-000001**. Ce sont des valeurs attribuées par décision projet, pas des mesures de carte. Vérifier les spécifications B0 gelées et collisions avec tout registre existant avant attribution ; si collision avérée, choisir prochain DeviceId positif libre et documenter. Créer un registre d'attribution versionné, statut RESERVED puis ASSIGNED après lecture et association UID96 réel. UID96 doit être lu sur la carte via ST-LINK/GDB sûr, jamais inventé, tronqué ou dérivé en DeviceId. Versions firmware/hardware/protocole/capacités conformes aux sources et capacités effectivement qualifiées ; ne pas fabriquer d'identité ou de READY.

## Lots enchaînés
1. Identifier ST-LINK et STM32U575 réel, lire UID96 trois mots et état firmware par méthode GDB validée, sans CubeProgrammer HOTPLUG (incident NVIC). Préserver BIN/ELF diagnostic E et procédure de restauration. Produire preuves et registre d'attribution.
2. Auditer composants, branchements et vrais handles FRAM SPI/CS, capteur, SDMMC/microSD, géométrie et ownership. Distinguer absence physique, interface absente et média EMPTY/inaccessible. Ne jamais inventer présence SD. Avant toute écriture FRAM/SD, lecture et sauvegarde intégrale vérifiée avec hash, plan des plages et restauration ; si impossibles, ne pas écrire et continuer le reste.
3. Construire manifeste de commissioning traçable : UID, DeviceId, B0, adresse, versions, configuration métier valide, géométrie FRAM et SD, allocations, provenance, sauvegardes, images/SHAs, générations et rollback. Déterminer les paramètres manquants à partir des sources et du banc réel ; décider les valeurs ordinaires permises par les normes sans microvalidation. Implémenter inspecteur métier validate_prefix, autorité registre et strong binding STM32 avec contexts durables/exclusifs et vraies dépendances transport, FRAM, capteur, SD ; aucune fixture harness, aucun faux READY.
4. Tester/fault-inject, compiler ARM et .NET selon impact, produire image production identifiée et restaurable. Flash et debug STM32 autonomes autorisés si cible, sauvegardes et procédure sûre établies. Aucun mass erase, Option Bytes, RDP, HOTPLUG, reformat implicite ou destruction de données. Provisioning EMPTY/migration explicite seulement après contrôle d'ownership, sauvegardes/restauration, UID réel, préfixe métier valide et plan de transaction. Pas de downgrade risquant une ancienne V1.
5. Dès que B0/B1 réels sont opérationnels, exécuter via SSH-HP des essais Modbus COM7 bornés, 115200 8E1 slave1, exclusivité maître, logs/hash, aucun retry, sans rejouer M013-I/M014-C. Aucun GDB pendant trafic. Qualifier B0/B1, polling et perte/reprise physiques ; ne pas déduire disponibilité des seuls tests hôte.
6. Actualiser ETAT_COURANT et la roadmap ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md : état M013 historique exact, M014 DONE uniquement après qualification physique complète, sinon IN PROGRESS avec preuves, réserves et prochaine action. Documenter la suite M015 uniquement si M014 réellement close.

## Garde-fous et livrables
Les permissions allow_commit/push/flash/debug sont explicites et permanentes pour cette mission. Escalader seulement décision structurante non couverte, opération destructive inhabituelle, risque matériel non maîtrisé ou geste physique utilisateur. Ne pas demander de validation pour l'attribution déjà décidée, logiciel, flash/debug standards qualifiés. Publier code, registre, manifeste, validations, commits et DERNIER_RAPPORT.md avec résultats observés, SHA, état physique vs logiciel et reprise exploitable. Vérifier relecture distante des livrables. Ne pas annoncer DONE sans preuve des critères M014 ; viser la clôture dès ce soir sans sacrifier la rigueur industrielle.
