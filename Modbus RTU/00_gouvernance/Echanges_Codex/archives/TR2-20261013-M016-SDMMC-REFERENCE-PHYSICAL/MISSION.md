---
mission_id: TR2-20261013-M016-SDMMC-REFERENCE-PHYSICAL
status: READY
base_ref: main
base_sha: 167b8cbcf1ba93209789ea6890799c77bbc7fc96
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Qualification physique du banc de référence ST SDMMC2 4 bits

## Décision et disponibilité matérielle confirmées par l'utilisateur
L'utilisateur a confirmé : **seconde NUCLEO-U575ZI-Q connectée en ST-LINK**, second breakout **Adafruit 4682 câblé conformément au pinout H3h-B**, carte microSD de **test insérée**, dont **tout le contenu peut être effacé ou remplacé librement**. Cette autorisation porte exclusivement sur la microSD de test du **second banc**, et non sur la microSD du TR2 principal. Aucune nouvelle validation humaine répétitive n'est nécessaire pour les écritures/formatages raisonnables sur ce support après identification certaine.

## Références et reprise
Partir du HEAD réel de main, consulter AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, mission précédente et DERNIER_RAPPORT.md (BLOCKED faute de ST-LINK), preuves et README de `experiments/sdmmc_st_reference`, provenance ST CubeU5 v1.9.0 `STM32U575I-EV/Examples/SD/SD_ReadWrite_DMALinkedList`, pinout H3h-B et archives M016-STORAGE. Préserver worktrees/stashes et acquis. Vérifier que le banc n'est pas utilisé par une autre session avant flash.

## Objectif et exécution autonome
1. Détecter et **identifier formellement la seconde Nucleo** (ST-LINK et UID) ; comparer aux identités documentées. Si seule la carte TR2 principale est visible, si les identités sont ambiguës ou si le câblage/alimentation semblent dangereux : ne pas flasher et remonter le blocage.
2. Reprendre le projet de démonstration **isolé** déjà préparé à partir du code ST, sans le réécrire ni substituer un driver expérimental. Vérifier les écarts HAL/callbacks, clocks, GPIO, pull-ups et bus SDMMC2, avec traçabilité. Ne pas imposer au préalable de modifications arbitraires de paramètres.
3. Effectuer le préflight, puis flash/debug autorisés **sur la seconde Nucleo seulement**. Démarrer en diagnostic lecture seule : identification microSD, CID/capacité, état carte, bus et horloge. Confirmer que la configuration matérielle est effectivement **SDMMC 4 bits**.
4. La microSD du second banc est **sacrifiable** : après confirmation de son identité et de l'absence de confusion avec le TR2 principal, autorisation explicite d'écrire, de réécrire, de formater ou de repartitionner cette carte si cela est utile à la démonstration. Privilégier d'abord l'essai brut borné prévu par le projet ST, avec zone/LBA vérifiés et test de lecture/comparaison octet par octet ; ne pas exiger une confirmation supplémentaire pour l'effacement de la carte de test.
5. Exécuter des essais reproductibles en SDMMC2 **4 bits** : plusieurs cycles écriture/relecture avec motifs différents, comparaison complète, vérification après réinitialisation, erreurs/états HAL et CMD13, mesures de fréquence et débit. Séparer les preuves de fonctionnement de l'interface, de l'intégrité et de la performance ; tester des hypothèses explicites en cas d'échec, sans changements empiriques en cascade.
6. Si le banc réussit, comparer la référence fonctionnelle à l'implémentation SDMMC du TR2 principal et identifier les différences pertinentes et la correction minimale recommandée. **Ne pas flasher, écrire, reformater ou modifier le firmware/média du TR2 principal dans cette mission** ; sa remise en état et l'intégration de production seront pilotées séparément.

## Critères d'acceptation
- Identification indépendante et certaine de la seconde Nucleo et de la microSD de test, version/source du code ST tracées.
- Preuve physique répétable d'écriture et relecture identique en SDMMC2 4 bits, avec configuration réellement appliquée, capacité, CID, métriques et erreurs ; ou statut BLOCKED étayé si la preuve ne peut être obtenue.
- Aucun impact sur TR2 principal, FRAM, campagne historique, spécifications Modbus V1 ou données préservées ; le support test seul peut être effacé.
- Tests et compilations proportionnés, logs/artefacts/commits et push normaux sur main, rapport DERNIER_RAPPORT.md publié avec preuves, limites, état matériel final et prochaines recommandations. Ne jamais qualifier une simple compilation comme réussite physique.

## Autonomie et garde-fous
`target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true`. Autorisation permanente AGENTS.md appliquée sans validations répétitives. Flash/debug exclusivement seconde Nucleo identifiée ; aucun mass erase ni changement Option Bytes/RDP, aucune manipulation physique par Codex, aucune écriture sur la carte principale. La microSD **du second banc seulement** peut être entièrement effacée. En cas de doute d'identité, risque électrique ou accès matériel absent, suspendre l'action dangereuse et publier les preuves et le blocage.

## Sortie et suite
Publier rapport et preuves vérifiables. ChatGPT contrôlera indépendamment la démonstration avant une mission distincte de correction/intégration du stockage V1 du TR2 principal.
